#ifndef AIO_TREE_NODE_HPP
#define AIO_TREE_NODE_HPP

#include <AIO/Game/Board.hpp>
#include <AIO/Network/Preprocess.hpp>
#include <AIO/Search/SearchOptions.hpp>

#include <atomic>

namespace AIO::Search
{
enum class ExpandState
{
    UNEXPANDED,
    EXPANDING,
    EXPANDED
};

struct TreeNode final
{
    TreeNode() noexcept = default;
    TreeNode(TreeNode&& other) noexcept;

    std::atomic<ExpandState> state{ ExpandState::UNEXPANDED };

    Game::Point action{ Game::PASS };
    Game::StoneColor color{ Game::P_INVALID };

    float policy{ 0 };
    std::atomic<int> visits{ 0 };
    std::atomic<float> values{ 0 };
    std::atomic<float> virtualLoss{ 0 };

    TreeNode* parentNode{ nullptr };
    TreeNode* mostLeftChildNode{ nullptr };
    TreeNode* rightSiblingNode{ nullptr };

    TreeNode* Select(const SearchOptions& opt) const;
    void Expand(const Game::Board& state, const Network::Tensor& policy);

    TreeNode* GetMaxVisitedChild() const;
};
}  // namespace AIO::Search

#endif  // AIO_TREE_NODE_HPP
