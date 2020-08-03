#ifndef AIO_FAKE_NETWORK_HPP
#define AIO_FAKE_NETWORK_HPP

#include <AIO/Network/Network.hpp>

namespace AIO::Network
{
class FakeNetwork final : public Network
{
	void Initialize(const std::string& weightFileName) override;

	void Evaluate(const std::vector<Tensor>& inputs, std::vector<Tensor>& policy, Tensor& value) override;
};
}  // namespace AIO::Network

#endif  // AIO_FAKE_NETWORK_HPP
