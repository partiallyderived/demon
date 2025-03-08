#pragma once

#include <cstdint>

#include <ostream>
#include <string>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/pos.hpp"

namespace dl {

template<NodeKind KIND, typename DataType>
struct DataNode final: Node {
    using Type = DataType;
    DataType data;

    DataNode(DataType&& data, Pos src)
    noexcept: Node(src), data(std::move(data)) {}

    virtual bool equals(const Node& that) const noexcept override {
        return data == dynamic_cast<
            const DataNode<KIND, DataType>&
        >(that).data;
    }

    virtual NodeKind kind() const noexcept override {
        return KIND;
    }

    std::ostream& out_data(std::ostream& os) const override {
        return os << data;
    }
};

using Bool = DataNode<NodeKind::BOOL, bool>;
using Int8 = DataNode<NodeKind::INT8, std::int8_t>;
using Int16 = DataNode<NodeKind::INT16, std::int16_t>;
using Int32 = DataNode<NodeKind::INT32, std::int32_t>;
using Int64 = DataNode<NodeKind::INT64, std::int64_t>;
using UInt8 = DataNode<NodeKind::UINT8, std::uint8_t>;
using UInt16 = DataNode<NodeKind::UINT16, std::uint16_t>;
using UInt32 = DataNode<NodeKind::UINT32, std::uint32_t>;
using UInt64 = DataNode<NodeKind::UINT64, std::uint64_t>;
using Float32 = DataNode<NodeKind::FLOAT32, float>;
using Float64 = DataNode<NodeKind::FLOAT64, double>;
using Char = DataNode<NodeKind::CHAR, std::int32_t>;
using String = DataNode<NodeKind::STRING, std::string>;
using ID = DataNode<NodeKind::ID, std::string>;
using NumID = DataNode<NodeKind::NUM_ID, std::int32_t>;
using LambdaArgs = DataNode<NodeKind::LAMBDA_ARGS, std::uint32_t>;
using LambdaKeywordArgs = DataNode<NodeKind::LAMBDA_KWARGS, std::uint32_t>;

}
