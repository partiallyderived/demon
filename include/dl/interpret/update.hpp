#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/args.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/pos.hpp"

namespace dl {

struct Update final: Node {
    NodePtr object;
    Args args;
    NodePtr value;

    Update(NodePtr&& object, Args&& args, NodePtr&& value, Pos src) noexcept:
    Node(src),
    object(std::move(object)),
    args(std::move(args)),
    value(std::move(value)) {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::UPDATE;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Update&>(that);
        return
            npeq(object, casted.object) &&
            args == casted.args &&
            npeq(value, casted.value);
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << category() << "(" <<
            object << ", " << args << ", " << value <<
        ")";
    }
};

}
