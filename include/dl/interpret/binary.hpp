#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"
#include "dl/util.hpp"

namespace dl {

template<NodeKind KIND>
struct Binary final: Node_<Binary<KIND>> {
    NodePtr lhs;
    NodePtr rhs;

    Binary(NodePtr&& lhs, NodePtr&& rhs, Span src) noexcept:
    Node_<Binary<KIND>>(src), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    virtual Binary copy() const override {
        return Binary(copy_np(lhs), copy_np(rhs), this->src);
    }

    virtual NodeKind kind() const noexcept override {
        return KIND;
    }

    virtual bool equals(const Node& that) const noexcept override {
        const auto& casted = dynamic_cast<const Binary&>(that);
        return npeq(lhs, casted.lhs) && npeq(rhs, casted.rhs);
    }

    virtual std::ostream& out_data(std::ostream& os) const override {
        return out_csv(os, lhs, rhs);
    }

    virtual Span span() const noexcept override {
        return Span(lhs->span(), rhs->span());
    }
};

using Add = Binary<NodeKind::ADD>;
using And = Binary<NodeKind::AND>;
using As = Binary<NodeKind::AS>;
using Assign = Binary<NodeKind::ASSIGN>;
using BitAnd = Binary<NodeKind::BAND>;
using BitOr = Binary<NodeKind::BOR>;
using BitXor = Binary<NodeKind::BXOR>;
using Declare = Binary<NodeKind::DECLARE>;
using Defaulted = Binary<NodeKind::DEFAULTED>;
using Divide = Binary<NodeKind::DIV>;
using Entry = Binary<NodeKind::ENTRY>;
using Equals = Binary<NodeKind::EQ>;
using GetAttr = Binary<NodeKind::GET_ATTR>;
using GreaterThan = Binary<NodeKind::GT>;
using GreaterThanOrEqual = Binary<NodeKind::GTE>;
using IAdd = Binary<NodeKind::IADD>;
using IBitAnd = Binary<NodeKind::IBAND>;
using IBitOr = Binary<NodeKind::IBOR>;
using IBitXor = Binary<NodeKind::IBXOR>;
using IDivide = Binary<NodeKind::IDIV>;
using ILeftShift = Binary<NodeKind::ILSH>;
using IMod = Binary<NodeKind::IMOD>;
using IMultiply = Binary<NodeKind::IMUL>;
using In = Binary<NodeKind::IN>;
using IPower = Binary<NodeKind::IPOW>;
using IRightShift = Binary<NodeKind::IRSH>;
using ISubtract = Binary<NodeKind::ISUB>;
using LeftShift = Binary<NodeKind::LSH>;
using LessThan = Binary<NodeKind::LT>;
using LessThanOrEqual = Binary<NodeKind::LTE>;
using Mod = Binary<NodeKind::MOD>;
using Multiply = Binary<NodeKind::MUL>;
using NotEquals = Binary<NodeKind::NEQ>;
using Or = Binary<NodeKind::OR>;
using Power = Binary<NodeKind::POW>;
using RightShift = Binary<NodeKind::RSH>;
using Subtract = Binary<NodeKind::SUB>;
using TypeMatch = Binary<NodeKind::TYPE_MATCH>;

}
