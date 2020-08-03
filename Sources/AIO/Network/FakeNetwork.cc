#include <AIO/Network/FakeNetwork.hpp>

#include <effolkronium/random.hpp>

namespace AIO::Network
{
void FakeNetwork::Initialize([[maybe_unused]] const std::string& weightFileName)
{
    // Do nothing
}

void FakeNetwork::Evaluate(const std::vector<Tensor>& inputs,
                           std::vector<Tensor>& policy, Tensor& value)
{
    const std::size_t batchSize = inputs.size();

    using Random = effolkronium::random_static;

    float sum = 0;
    for (std::size_t batch = 0; batch < batchSize; ++batch)
    {
        policy[batch].resize(Game::BOARD_SIZE);

        sum = 0;

        for (std::size_t i = 0; i < Game::BOARD_SIZE; ++i)
        {
            policy[batch][i] = Random::get<float>(0, 1);
            sum += policy[batch][i];
        }

        for (auto& p : policy[batch])
            p /= sum;

        value[batch] = Random::get<float>(-1, 1);
    }
}
}  // namespace AIO::Network
