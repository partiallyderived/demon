#pragma once

#include <cstdint>

#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "dl/compose/opinfo.hpp"
#include "dl/compose/opkind.hpp"
#include "dl/data.hpp"
#include "dl/parse/opid.hpp"
#include "dl/pos.hpp"
#include "dl/util.hpp"

namespace dl {

struct BinaryData;

// Composition
struct Comp {
    OpID op;
    union {
        Comp* comp;
        BinaryData* bin;
        Data data;
        std::vector<Comp> comps;
    };
    Pos src;

    inline ~Comp() noexcept;

    Comp(const Comp& that) = delete;

    Comp(Comp&& that) noexcept: op(that.op), src(that.src) {
        using enum OpKind;
        switch(opinfo(that.op).kind) {
        case UNARY:
            new(&comp) (Comp*)(that.comp);
            that.comp = nullptr;
            return;
        case BINARY:
            new(&bin) (BinaryData*)(that.bin);
            that.bin = nullptr;
            return;
        case DATA:
            new(&data) Data(std::move(that.data));
            return;
        case AGGREGATE:
            new(&comps) std::vector<Comp>(std::move(that.comps));
            return;
        default: 
            return;
        }
    }

    // Nullary constructor
    Comp(OpID op, Pos src) noexcept: op(op), src(src) {}

    // Unary constructor
    Comp(OpID op, Comp&& comp, Pos src):
    op(op), comp(new Comp(std::move(comp))), src(src) {}

    // Binary constructor
    inline Comp(OpID op, Comp&& lhs, Comp&& rhs, Pos src);

    // Data constructor
    Comp(OpID op, Data&& data, Pos src):
    op(op), data(std::move(data)), src(src) {}

    // Aggregate constructor
    Comp(OpID op, std::vector<Comp>&& comps, Pos src) noexcept:
    op(op), comps(std::move(comps)), src(src) {}

    inline bool operator==(const Comp& that) const noexcept;
};

struct BinaryData {
    Comp lhs;
    Comp rhs;

    BinaryData(Comp&& lhs, Comp&& rhs) noexcept:
    lhs(std::move(lhs)), rhs(std::move(rhs)) {}
};

Comp::Comp(OpID op, Comp&& lhs, Comp&& rhs, Pos src):
op(op), bin(new BinaryData(std::move(lhs), std::move(rhs))), src(src) {}

bool Comp::operator==(const Comp& that) const noexcept {
    if (op != that.op || src != that.src)
        return false;

    using enum OpKind;
    switch(opinfo(op).kind) {
    case UNARY:
        return *comp == *that.comp;
    case BINARY:
        return bin->lhs == that.bin->lhs && bin->rhs == that.bin->rhs;
    case DATA:
        return data == that.data;
    case AGGREGATE:
        return comps == that.comps;
    default:
        return true;
    }
}

Comp::~Comp() noexcept {
    using enum OpKind;
    switch(opinfo(op).kind) {
    case UNARY:
        delete comp;
        return;
    case BINARY:
        delete bin;
        return;
    case DATA:
        data.~Data();
        return;
    case AGGREGATE:
        comps.~vector();
        return;
    default:;
    }
}

std::ostream& operator<<(std::ostream& os, const Comp& comp) {
    using enum OpKind;

    os << "Comp(" << comp.op;
    switch(opinfo(comp.op).kind) {
    case UNARY:
        os << ", " << *comp.comp;
        break;
    case BINARY:
        os << ", " << comp.bin->lhs << ", " << comp.bin->rhs;
        break;
    case DATA:
        os << ", " << comp.data;
        break;
    case AGGREGATE:
        os << ", " << OutContainerManip(comp.comps);
    default:;
    }
    return os << ", " << comp.src << ")";
}

}
