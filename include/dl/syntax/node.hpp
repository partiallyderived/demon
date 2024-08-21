#pragma once

#include <memory>
#include <ostream>
#include <vector>

#include "dl/err.hpp"
#include "dl/syntax/kind.hpp"

namespace dl::syntax {

struct Node;
using NodePtr = std::unique_ptr<Node>;

struct Node {
    virtual bool operator==(const Node&) const noexcept = 0;
    bool operator!=(const Node&) const noexcept = default;

    virtual Kind kind() const noexcept = 0;
    virtual std::ostream& out(std::ostream& os) const = 0;

    // `offer` is overridden by constructs such as `if` in order to
    // consolidate a child block, such as `elif` or `else`, into itself.
    // `true` means accept, `false` means reject.
    virtual bool offer(NodePtr&& node) {
        // Reject by default.
        return false;
    }

    // Determine whether this node may be executed without additional parsing.
    virtual bool ready() const noexcept {
        return true;
    }

    // Given the kind of the next operator to be handled, tells whether the
    // parsing of this node is complete, or whether more operators must be
    // handled first.
    virtual bool ready(Kind kind) const noexcept {
        return true;
    }
    
    virtual ~Node() noexcept {}
};

std::ostream& operator<<(std::ostream& os, const Node& x) {
    return x.out(os);
}

using Nodes = std::vector<NodePtr>;
using NodeRes = Res<NodePtr, ErrPtr>;

// Convenience class implementing kind through template parameter.
template<Kind KIND>
struct Node_: Node {
    virtual Kind kind() const noexcept final override {
        return KIND;
    }
};

}
