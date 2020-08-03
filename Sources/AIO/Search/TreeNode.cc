#include <AIO/Search/TreeNode.hpp>

namespace AIO::Search
{
TreeNode::TreeNode(TreeNode&& other) noexcept
    : action(other.action),
      color(other.color),
      mostLeftChildNode(other.mostLeftChildNode),
      rightSiblingNode(other.rightSiblingNode)
{
    state = other.state.load();

    visits = other.visits.load();
    values = other.values.load();
    virtualLoss = other.virtualLoss.load();
}
}  // namespace AIO::Search
