#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/case.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct Try final: Node {
    Nodes body;
    std::vector<MatchCase> excepts;
    Nodes finally;

    Try(
        Nodes&& body,
        std::vector<MatchCase>&& excepts,
        Nodes&& finally,
        Pos src
    )
    noexcept:
    Node(src),
    body(std::move(body)),
    excepts(std::move(excepts)),
    finally(std::move(finally)) {}

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Try&>(that);
        return
            nodes_eq(body, casted.body) &&
            excepts == casted.excepts &&
            nodes_eq(finally, casted.finally);
    }

    virtual NodeKind kind() const noexcept override {
        return NodeKind::TRY;
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return
            os << body << ", " << OutContainerManip(excepts) << ", " << finally;
    }
};

}
