#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/data.hpp"
#include "dl/interpret/node.hpp"
#include "dl/pos.hpp"

namespace dl {

struct ArgDef {
    ID id;
    NodePtr label;
    NodePtr dflt;
    bool match;

    ArgDef() noexcept:
    id("", Pos(0, 0)), label(nullptr), dflt(nullptr), match(false) {}

    ArgDef(ID&& id, NodePtr&& label, NodePtr&& dflt, bool match) noexcept:
    id(std::move(id)),
    label(std::move(label)),
    dflt(std::move(dflt)),
    match(match) {}

    bool operator==(const ArgDef& that) const noexcept {
        return
            id == that.id &&
            npeq(label, that.label) &&
            npeq(dflt, that.dflt) &&
            match == that.match;
    }
};

std::ostream& operator<<(std::ostream& os, const ArgDef& x) {
    return os << "ArgDef(" <<
        x.id << ", " <<
        x.label << ", " <<
        x.dflt << ", " <<
        std::boolalpha << x.match <<
    ")";
}

}
