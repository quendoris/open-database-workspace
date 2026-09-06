#include "core/workspace/Layout.hpp"

#include <iostream>
#include <memory>
#include <string>

namespace {

using odw::workspace::Axis;
using odw::workspace::LayoutNode;
using odw::workspace::Split;
using odw::workspace::TabGroup;
using odw::workspace::WindowLayout;
using odw::workspace::WorkspaceLayout;

std::shared_ptr<LayoutNode> tabs(std::string id,
                                 std::initializer_list<std::string> views,
                                 std::size_t activeIndex = 0) {
    return std::make_shared<LayoutNode>(LayoutNode{
        TabGroup{std::move(id), std::vector<std::string>(views), activeIndex}
    });
}

bool expectValidLayout() {
    auto left = tabs("left", {"databases", "schema"});
    auto right = tabs("right", {"query", "results"}, 1);

    auto root = std::make_shared<LayoutNode>(LayoutNode{
        Split{Axis::Horizontal, {left, right}, {0.3, 0.7}}
    });

    WorkspaceLayout layout{{WindowLayout{"main", root}}};
    const auto errors = odw::workspace::validate(layout);

    if (!errors.empty()) {
        std::cerr << "valid layout was rejected: " << errors.front() << '\n';
        return false;
    }
    return true;
}

bool expectDuplicateViewRejected() {
    auto first = tabs("left", {"query"});
    auto second = tabs("right", {"query"});
    auto root = std::make_shared<LayoutNode>(LayoutNode{
        Split{Axis::Horizontal, {first, second}, {1.0, 1.0}}
    });

    WorkspaceLayout layout{{WindowLayout{"main", root}}};
    const auto errors = odw::workspace::validate(layout);

    if (errors.empty()) {
        std::cerr << "duplicate view id was accepted\n";
        return false;
    }
    return true;
}

bool expectInvalidActiveTabRejected() {
    WorkspaceLayout layout{{WindowLayout{"main", tabs("query-tabs", {"query"}, 4)}}};
    const auto errors = odw::workspace::validate(layout);

    if (errors.empty()) {
        std::cerr << "invalid active tab index was accepted\n";
        return false;
    }
    return true;
}

} // namespace

int main() {
    if (!expectValidLayout()) {
        return 1;
    }
    if (!expectDuplicateViewRejected()) {
        return 1;
    }
    if (!expectInvalidActiveTabRejected()) {
        return 1;
    }
    return 0;
}
