#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/args.hpp"
#include "dl/interpret/data.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/pos.hpp"

namespace dl {

struct CallAttr final: Node {
    NodePtr object;
    ID attr;
    Args args;
    bool cached;

    CallAttr(
        NodePtr&& object, ID&& attr, Args&& args, bool cached, Pos src
    ) noexcept:
    Node(src),
    object(std::move(object)),
    attr(std::move(attr)),
    args(std::move(args)),
    cached(cached)
    {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::CALL_ATTR;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const CallAttr&>(that);
        return
            npeq(object, casted.object) &&
            attr == casted.attr &&
            args == casted.args &&
            cached == casted.cached;
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << category() << "(" <<
            object << ", " <<
            attr << ", " <<
            args << ", " <<
            std::boolalpha << cached <<
        ")";
    }
};

}
