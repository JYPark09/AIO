#include <AIO/Search/SearchManager.hpp>

namespace AIO::Search
{
SearchManager::SearchManager(std::size_t threadNum) : threadNum_(threadNum)
{
    // Do nothing
}

void SearchManager::Pause()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);

        state_ = SearchState::PAUSE;
    }

    cv_.notify_all();
    pauseGroup_.Wait();
}

void SearchManager::AckPause()
{
    pauseGroup_.Done();
}

void SearchManager::Resume()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (state_ == SearchState::SEARCHING)
            return;

        state_ = SearchState::SEARCHING;

        pauseGroup_.Add(threadNum_);
    }

    cv_.notify_all();
}

void SearchManager::Terminate()
{
    Pause();

    {
        std::lock_guard<std::mutex> lock(mutex_);

        state_ = SearchState::TERMINATE;
    }

    cv_.notify_all();
}

bool SearchManager::WaitResume()
{
    std::unique_lock<std::mutex> lock(mutex_);

    cv_.wait(lock, [this] { return state_ != SearchState::PAUSE; });

    if (state_ == SearchState::TERMINATE)
        return false;

    return true;
}

SearchState SearchManager::GetState() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    return state_;
}

}  // namespace AIO::Search
