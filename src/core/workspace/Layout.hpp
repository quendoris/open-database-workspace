#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace odw::workspace {

enum class Axis {
    Horizontal,
    Vertical,
};

struct TabGroup {
    std::string id;
    std::vector<std::string> viewIds;
    std::size_t activeIndex = 0;
};

struct LayoutNode;
using LayoutNodePtr = std::shared_ptr<LayoutNode>;

struct Split {
    Axis axis = Axis::Horizontal;
    std::vector<LayoutNodePtr> children;
    std::vector<double> weights;
};

struct LayoutNode {
    std::variant<TabGroup, Split> value;
};

struct WindowLayout {
    std::string id;
    LayoutNodePtr root;
};

struct WorkspaceLayout {
    std::vector<WindowLayout> windows;
};

[[nodiscard]] std::vector<std::string> validate(const WorkspaceLayout& layout);

} // namespace odw::workspace
