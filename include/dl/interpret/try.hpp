#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/case.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct Try final: Node {
    Nodes body;
    std::vector<Case> excepts;
    Nodes finally;

    Try(Nodes&& body, std::vector<Case>&& excepts, Nodes&& finally, Pos src)
    noexcept:
    Node(src),
    body(std::move(body)),
    excepts(std::move(excepts)),
    finally(std::move(finally)) {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::TRY;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Try&>(that);
        return
            nodes_eq(body, casted.body) &&
            excepts == casted.excepts &&
            nodes_eq(finally, casted.finally);
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << category() << "(" <<
            body << ", " << OutContainerManip(excepts) << ", " << finally <<
        ")";
    }
};

}
