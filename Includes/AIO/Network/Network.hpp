#ifndef AIO_NETWORK_HPP
#define AIO_NETWORK_HPP

#include <AIO/Network/Preprocess.hpp>

#include <string>

namespace AIO::Network
{
class Network
{
public:
    virtual ~Network() = default;

    virtual void Initialize(const std::string& weightFileName) = 0;

    virtual void Evaluate(const std::vector<Tensor>& inputs,
                          std::vector<Tensor>& policy, Tensor& value) = 0;
};
}  // namespace AIO::Network

#endif  // AIO_NETWORK_HPP
