#ifndef AIO_SEARCH_ENGINE_HPP
#define AIO_SEARCH_ENGINE_HPP

#include <AIO/Game/Board.hpp>
#include <AIO/Network/Network.hpp>
#include <AIO/Search/SearchOptions.hpp>
#include <AIO/Search/SearchManager.hpp>
#include <AIO/Search/TreeNode.hpp>
#include <AIO/Utils/Barrier.hpp>

#include <deque>
#include <future>
#include <memory>
#include <thread>
#include <tuple>

namespace AIO::Search
{
using NetEval = std::tuple<Network::Tensor, float>;

struct EvalTask final
{
    Network::Tensor state;
    std::promise<NetEval> task;
};

class SearchEngine final
{
 public:
    SearchEngine(SearchOptions option);
    ~SearchEngine();

    void Search();
    void Play(Game::Point pt);

    [[nodiscard]] Game::Point GetBestMove() const;
    [[nodiscard]] const TreeNode* GetRoot() const noexcept;

 private:
    [[nodiscard]] TreeNode* getBestNode();
    [[nodiscard]] const TreeNode* getBestNode() const;

    void updateRoot(TreeNode* newNode);
    void initRoot();

    void evaluate(const Game::Board& state, Network::Tensor& policy,
                  float& value);

    void evalThread(int threadId, std::unique_ptr<Network::Network> network);
    void searchThread(int threadId);

 private:
    SearchOptions option_;

    Game::Board mainBoard_;

    TreeNode* root_{ nullptr };

    // threads
    Utils::Barrier networkBarrier_;

    std::deque<EvalTask> evalTasks_;
    std::mutex evalMutex_;
    std::vector<std::thread> evalThreads_;
    std::vector<std::thread> searchThreads_;

    // thread controls
    bool runningEvalThread_{ true };
    SearchManager manager_;
};
}  // namespace AIO::Search

#endif  // AIO_SEARCH_ENGINE_HPP
