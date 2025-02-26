#pragma once

#include <ostream>
#include <utility>

#include "dl/interpret/data.hpp"
#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/pos.hpp"

namespace dl {

struct LambdaKeywordVar final: Node {
    ID id;

    LambdaKeywordVar(ID&& id, Pos src) noexcept: Node(src), id(std::move(id)) {}

    virtual NodeCategory category() const noexcept override {
        return NodeCategory::LAMBDA_KEYWORD;
    }

    virtual bool equals(const Node& that) const noexcept override {
        return id == dynamic_cast<const LambdaKeywordVar&>(that).id;
    }

    virtual std::ostream& out(std::ostream& os) const override {
        return os << category() << "(" << id << ")";
    }
};

}
