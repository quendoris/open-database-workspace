#include "Layout.hpp"

#include <cmath>
#include <string_view>
#include <unordered_set>

namespace odw::workspace {
namespace {

struct ValidationContext {
    std::unordered_set<std::string> windowIds;
    std::unordered_set<std::string> groupIds;
    std::unordered_set<std::string> viewIds;
    std::vector<std::string> errors;
};

void requireUniqueNonEmpty(std::string_view kind,
                           const std::string& id,
                           std::unordered_set<std::string>& ids,
                           ValidationContext& context) {
    if (id.empty()) {
        context.errors.emplace_back(std::string(kind) + " id must not be empty");
        return;
    }

    if (!ids.insert(id).second) {
        context.errors.emplace_back(std::string(kind) + " id is duplicated: " + id);
    }
}

void validateNode(const LayoutNodePtr& node, ValidationContext& context) {
    if (!node) {
        context.errors.emplace_back("layout contains a null node");
        return;
    }

    if (const auto* tabs = std::get_if<TabGroup>(&node->value)) {
        requireUniqueNonEmpty("tab group", tabs->id, context.groupIds, context);

        if (tabs->viewIds.empty()) {
            context.errors.emplace_back("tab group " + tabs->id + " has no views");
        } else if (tabs->activeIndex >= tabs->viewIds.size()) {
            context.errors.emplace_back("tab group " + tabs->id + " has an invalid active index");
        }

        for (const auto& viewId : tabs->viewIds) {
            requireUniqueNonEmpty("view", viewId, context.viewIds, context);
        }
        return;
    }

    const auto& split = std::get<Split>(node->value);
    if (split.children.size() < 2) {
        context.errors.emplace_back("split node must have at least two children");
    }

    if (!split.weights.empty() && split.weights.size() != split.children.size()) {
        context.errors.emplace_back("split weights must be empty or match child count");
    }

    for (const double weight : split.weights) {
        if (!std::isfinite(weight) || weight <= 0.0) {
            context.errors.emplace_back("split weights must be finite and greater than zero");
            break;
        }
    }

    for (const auto& child : split.children) {
        validateNode(child, context);
    }
}

} // namespace

std::vector<std::string> validate(const WorkspaceLayout& layout) {
    ValidationContext context;

    if (layout.windows.empty()) {
        context.errors.emplace_back("workspace must contain at least one window");
        return context.errors;
    }

    for (const auto& window : layout.windows) {
        requireUniqueNonEmpty("window", window.id, context.windowIds, context);
        validateNode(window.root, context);
    }

    return context.errors;
}

} // namespace odw::workspace
