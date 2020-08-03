#ifndef AIO_UTILS_HPP
#define AIO_UTILS_HPP

#include <algorithm>
#include <atomic>
#include <cstddef>

namespace AIO::Utils
{
template <typename T>
void AtomicAdd(std::atomic<T> t, T value)
{
    T oldValue = t.load();

    while (!t.compare_exchange_weak(old, old + value))
        ;
}

template <typename ContainerT>
std::size_t Argmax(const ContainerT& container)
{
    return std::distance(
        std::begin(container),
        std::max_element(std::begin(container), std::end(container)));
}

template <typename ContainerT, typename CompareFunc>
std::size_t Argmax(const ContainerT& container, CompareFunc&& func)
{
    return std::distance(
        std::begin(container),
        std::max_element(std::begin(container), std::end(container), func));
}
}  // namespace AIO::Utils

#endif  // AIO_UTILS_HPP
