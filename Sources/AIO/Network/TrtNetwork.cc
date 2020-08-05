#ifdef BUILD_WITH_TRT

#include <AIO/Network/TrtNetwork.hpp>

#include <AIO/Network/Preprocess.hpp>

#include <cuda_runtime_api.h>
#include <fstream>
#include <sstream>

#include <spdlog/spdlog.h>

using namespace std::string_literals;

class Logger : public nvinfer1::ILogger
{
    void log(Severity severity, const char* msg) override
    {
        switch (severity)
        {
            case Severity::kINTERNAL_ERROR:
            case Severity::kERROR:
                throw std::runtime_error(msg);
            default:
                spdlog::info(msg);
        }
    }
} TRT_LOGGER;

namespace AIO::Network
{
TrtNetwork::TrtNetwork(const Search::SearchOptions& opt, int gpu)
    : option_(opt), gpu_(gpu)
{
    // Do Nothing
}

TrtNetwork::~TrtNetwork()
{
    if (context_)
        context_->destroy();
    if (engine_)
        engine_->destroy();
    if (runtime_)
        runtime_->destroy();

    for (auto buffer : buffers_)
    {
        if (int ret = cudaFree(buffer); ret != 0)
        {
            if (option_.Verbose)
                spdlog::warn("CUDA free error (code: {}", ret);
        }
    }
}

void TrtNetwork::Initialize(const std::string& weights)
{
    cudaSetDevice(gpu_);

    if (option_.Verbose)
        spdlog::info("initialize network with {} (gpu: {})", weights, gpu_);

    std::ostringstream oss(std::ios::binary);
    if (!(oss << std::ifstream(weights, std::ios::binary).rdbuf()))
    {
        throw std::runtime_error("Cannot open weights");
    }
    std::string model = oss.str();

    runtime_ = nvinfer1::createInferRuntime(TRT_LOGGER);
    engine_ =
        runtime_->deserializeCudaEngine(model.c_str(), model.size(), nullptr);
    if (engine_ == nullptr)
    {
        throw std::runtime_error("Cannot create engine");
    }
    if (option_.Verbose)
        spdlog::info("deserialize network (gpu: {})", gpu_);

    context_ = engine_->createExecutionContext();

    const int batchSize = engine_->getMaxBatchSize();
    for (int i = 0; i < engine_->getNbBindings(); ++i)
    {
        const auto dim = engine_->getBindingDimensions(i);

        int size = 1;
        for (int j = 0; j < dim.nbDims; ++j)
        {
            size *= dim.d[j];
        }

        void* buffer;
        if (int ret = cudaMalloc(&buffer, batchSize * size * sizeof(float)))
        {
            throw std::runtime_error("CUDA out of memory (code: "s +
                                     std::to_string(ret) + ")"s);
        }

        buffers_.emplace_back(buffer);
    }
}

void TrtNetwork::Evaluate(const std::vector<Tensor>& states,
                          std::vector<Tensor>& policy, Tensor& value)
{
    const std::size_t batchSize = states.size();
    if (batchSize == 0)
    {
        spdlog::warn("Batch size cannot be zero.");
        return;
    }

    constexpr std::size_t TENSOR_SIZE = TENSOR_DIM * Game::BOARD_SIZE;

    Tensor inTensor(batchSize * TENSOR_SIZE);
    for (std::size_t batch = 0; batch < batchSize; ++batch)
        for (std::size_t i = 0; i < TENSOR_SIZE; ++i)
            inTensor[batch * TENSOR_SIZE + i] = states[batch][i];

    if (int ret =
            cudaMemcpy(buffers_[0], inTensor.data(),
                       inTensor.size() * sizeof(float), cudaMemcpyHostToDevice);
        ret != 0)
    {
        throw std::runtime_error("CUDA memcpy error (code: "s +
                                 std::to_string(ret) + ")"s);
    }

    context_->executeV2(buffers_.data());

    Tensor flatPolicy(batchSize * Game::BOARD_SIZE);
    if (int ret = cudaMemcpy(flatPolicy.data(), buffers_[1],
                             flatPolicy.size() * sizeof(float),
                             cudaMemcpyDeviceToHost);
        ret != 0)
    {
        throw std::runtime_error("CUDA memcpy error (code: "s +
                                 std::to_string(ret) + ")"s);
    }

    for (std::size_t batch = 0; batch < batchSize; ++batch)
    {
        policy[batch].resize(Game::BOARD_SIZE);

        for (std::size_t i = 0; i < Game::BOARD_SIZE; ++i)
        {
            policy[batch][i] = flatPolicy[batch * (Game::BOARD_SIZE) + i];
        }
    }

    if (int ret =
            cudaMemcpy(value.data(), buffers_[2], value.size() * sizeof(float),
                       cudaMemcpyDeviceToHost);
        ret != 0)
    {
        throw std::runtime_error("CUDA memcpy error (code: "s +
                                 std::to_string(ret) + ")"s);
    }
}
}  // namespace AIO::Network

#endif  // BUILD_WITH_TRT