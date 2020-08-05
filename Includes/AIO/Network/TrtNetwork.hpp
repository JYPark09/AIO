#ifndef AIO_TRT_NETWORK_HPP
#define AIO_TRT_NETWORK_HPP

#ifdef BUILD_WITH_TRT

#include <AIO/Network/Network.hpp>
#include <AIO/Search/SearchOptions.hpp>

#include <NvInfer.h>

namespace AIO::Network
{
class TrtNetwork final : public Network
{
 public:
    TrtNetwork(const Search::SearchOptions& opt, int gpu);
    ~TrtNetwork();

    void Initialize(const std::string& weights) override;

    void Evaluate(const std::vector<Tensor>& states,
                  std::vector<Tensor>& policy, Tensor& value) override;

 private:
    const Search::SearchOptions& option_;

    nvinfer1::IRuntime* runtime_{ nullptr };
    nvinfer1::ICudaEngine* engine_{ nullptr };
    nvinfer1::IExecutionContext* context_{ nullptr };

    std::vector<void*> buffers_;

    int gpu_;
};
}  // namespace AIO::Network

#endif  // BUILD_WITH_TRT
#endif  // AIO_TRT_NETWORK_HPP