#ifndef AIO_SEARCH_MANAGER_HPP
#define AIO_SEARCH_MANAGER_HPP

#include <AIO/Utils/Barrier.hpp>
#include <AIO/Utils/WaitGroup.hpp>

#include <condition_variable>
#include <mutex>

namespace AIO::Search
{
enum class SearchState
{
    PAUSE,
    SEARCHING,
    TERMINATE
};

class SearchManager final
{
 public:
    explicit SearchManager(std::size_t threadNum);

    void Pause();
    void AckPause();
    void Resume();
    void Terminate();

    //! @return false if terminated, else true
    bool WaitResume();

    SearchState GetState() const;

 private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;

    std::size_t threadNum_;

    SearchState state_{ SearchState::PAUSE };

    Utils::WaitGroup resumeGroup_;
    Utils::WaitGroup pauseGroup_;
};
}  // namespace AIO::Search

#endif  // AIO_SEARCH_MANAGER_HPP
