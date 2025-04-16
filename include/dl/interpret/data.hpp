#pragma once

#include <cstdint>

#include <ostream>
#include <string>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodekind.hpp"
#include "dl/span.hpp"

namespace dl {

template<NodeKind KIND, typename DataType>
struct DataNode final: Node_<DataNode<KIND, DataType>> {
    using Type = DataType;
    DataType data;

    DataNode(DataType&& data, Span src)
    noexcept: Node_<DataNode<KIND, DataType>>(src), data(std::move(data)) {}

    virtual DataNode copy() const override {
        return DataNode(DataType(data), this->src);
    }

    virtual bool equals(const Node& that) const noexcept override {
        return data == dynamic_cast<
            const DataNode<KIND, DataType>&
        >(that).data;
    }

    virtual NodeKind kind() const noexcept override {
        return KIND;
    }

    std::ostream& out_data(std::ostream& os) const override {
        return os << std::boolalpha << data;
    }

    virtual Span span() const noexcept override {
        return this->src;
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
using RawInt8 = DataNode<NodeKind::RAW_INT8, std::int8_t>;
using RawInt16 = DataNode<NodeKind::RAW_INT16, std::int16_t>;
using RawInt32 = DataNode<NodeKind::RAW_INT32, std::int32_t>;
using RawInt64 = DataNode<NodeKind::RAW_INT64, std::int64_t>;
using RawUInt8 = DataNode<NodeKind::RAW_UINT8, std::uint8_t>;
using RawUInt16 = DataNode<NodeKind::RAW_UINT16, std::uint16_t>;
using RawUInt32 = DataNode<NodeKind::RAW_UINT32, std::uint32_t>;
using RawUInt64 = DataNode<NodeKind::RAW_UINT64, std::uint64_t>;
using RawFloat32 = DataNode<NodeKind::RAW_FLOAT32, float>;
using RawFloat64 = DataNode<NodeKind::RAW_FLOAT64, double>;
using Char = DataNode<NodeKind::CHAR, std::int32_t>;
using String = DataNode<NodeKind::STRING, std::string>;
using ID = DataNode<NodeKind::ID, std::string>;
using NumID = DataNode<NodeKind::NUM_ID, std::int32_t>;
using LambdaArgs = DataNode<NodeKind::LAMBDA_ARGS, std::uint32_t>;
using LambdaKeywordArgs = DataNode<NodeKind::LAMBDA_KWARGS, std::uint32_t>;

}
