#ifndef AIO_TREE_NODE_HPP
#define AIO_TREE_NODE_HPP

#include <AIO/Game/BoardDef.hpp>

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
    std::atomic<ExpandState> state{ ExpandState::UNEXPANDED };

    Game::Point action{ 0 };
    Game::StoneColor color{ Game::P_INVALID };

    std::atomic<int> visits{ 0 };
    std::atomic<float> values{ 0 };
    std::atomic<float> virtualLoss{ 0 };

    TreeNode* parentNode{ nullptr };
    TreeNode* mostLeftChildNode{ nullptr };
    TreeNode* rightSiblingNode{ nullptr };
};
}  // namespace AIO::Search

#endif  // AIO_TREE_NODE_HPP
