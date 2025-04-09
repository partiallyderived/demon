#pragma once

#include <cstdint>

#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "dl/compose/opinfo.hpp"
#include "dl/compose/opkind.hpp"
#include "dl/compose/precedence.hpp"
#include "dl/data.hpp"
#include "dl/parse/opid.hpp"
#include "dl/span.hpp"
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
    Span src;

    inline ~Comp() noexcept;

    Comp(const Comp& that) = delete;

    Comp(Comp&& that) noexcept: op(that.op), src(that.src) {
        using enum OpKind;
        switch(opinfo(that.op).kind) {
        case UNARY:
            new(&comp) (Comp*)(that.comp);
            break;
        case BINARY:
            new(&bin) (BinaryData*)(that.bin);
            break;
        case DATA:
            new(&data) Data(std::move(that.data));
            break;
        case AGGREGATE:
            new(&comps) std::vector<Comp>(std::move(that.comps));
            break;
        default:;
        }
        that.op = OpID::NOTHING;
        that.src = Span(1, 1);
    }

    // Nullary constructor
    Comp(OpID op, Span src) noexcept: op(op), src(src) {}

    // Unary constructor
    Comp(OpID op, Comp&& comp, Span src):
    op(op), comp(new Comp(std::move(comp))), src(src) {}

    // Binary constructor
    inline Comp(OpID op, Comp&& lhs, Comp&& rhs, Span src);

    // Data constructor
    Comp(OpID op, Data&& data, Span src):
    op(op), data(std::move(data)), src(src) {}

    // Aggregate constructor
    Comp(OpID op, std::vector<Comp>&& comps, Span src) noexcept:
    op(op), comps(std::move(comps)), src(src) {}

    inline bool operator==(const Comp& that) const noexcept;

    inline Comp copy() const noexcept;

    // Gives the full span of this Comp, including its children which are not
    // always included in src (for operators, src is only the span for the
    // operator itself).
    inline Span span() const noexcept;
};

struct BinaryData {
    Comp lhs;
    Comp rhs;

    BinaryData(Comp&& lhs, Comp&& rhs) noexcept:
    lhs(std::move(lhs)), rhs(std::move(rhs)) {}
};

Comp::Comp(OpID op, Comp&& lhs, Comp&& rhs, Span src):
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
        if (op == OpID::ERROR)
            return *std::get<ErrPtr>(data) == *std::get<ErrPtr>(that.data);
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

Comp Comp::copy() const noexcept {
    using enum OpKind;
    switch(opinfo(op).kind) {
    case UNARY:
        return Comp(op, comp->copy(), src);
    case BINARY:
        return Comp(op, bin->lhs.copy(), bin->rhs.copy(), src);
    case DATA:
        return Comp(op, copy_data(data), src);
    case AGGREGATE:
        return Comp(op, deep_copy(comps), src);
    default:
        return Comp(op, src);
    }
}

Span Comp::span() const noexcept {
    using enum OpKind;
    switch(opinfo(op).kind) {
    case UNARY:
        if (op == OpID::ADDR_TYPE)
            // Account for suffix operators by reversing order.
            return Span(comp->span(), src);
        if (opinfo(op).left_precedence == Precedence::START)
            // Start operators have the entire span in src already.
            return src;
        return Span(src, comp->span());
    case BINARY:
        return Span(bin->lhs.span(), bin->rhs.span());
    default:
        return src;
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
