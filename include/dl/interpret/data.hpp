#pragma once

#include <cstdint>

#include <ostream>
#include <string>
#include <utility>

#include "dl/interpret/node.hpp"
#include "dl/interpret/nodecategory.hpp"
#include "dl/pos.hpp"

namespace dl {

template<NodeCategory CATEGORY, typename DataType>
struct DataNode final: Node {
    using Type = DataType;
    DataType data;

    DataNode(DataType&& data, Pos src)
    noexcept: Node(src), data(std::move(data)) {}

    virtual NodeCategory category() const noexcept override {
        return CATEGORY;
    }

    virtual bool equals(const Node& that) const noexcept override {
        return data == dynamic_cast<
            const DataNode<CATEGORY, DataType>&
        >(that).data;
    }

    std::ostream& out(std::ostream& os) const override {
        return os << CATEGORY << "(" << std::boolalpha << data << ")";
    }
};

using Bool = DataNode<NodeCategory::BOOL, bool>;
using Int8 = DataNode<NodeCategory::INT8, std::int8_t>;
using Int16 = DataNode<NodeCategory::INT16, std::int16_t>;
using Int32 = DataNode<NodeCategory::INT32, std::int32_t>;
using Int64 = DataNode<NodeCategory::INT64, std::int64_t>;
using UInt8 = DataNode<NodeCategory::UINT8, std::uint8_t>;
using UInt16 = DataNode<NodeCategory::UINT16, std::uint16_t>;
using UInt32 = DataNode<NodeCategory::UINT32, std::uint32_t>;
using UInt64 = DataNode<NodeCategory::UINT64, std::uint64_t>;
using Float32 = DataNode<NodeCategory::FLOAT32, float>;
using Float64 = DataNode<NodeCategory::FLOAT64, double>;
using Char = DataNode<NodeCategory::CHAR, std::int32_t>;
using String = DataNode<NodeCategory::STRING, std::string>;
using ID = DataNode<NodeCategory::ID, std::string>;
using NumID = DataNode<NodeCategory::NUM_ID, std::int32_t>;

}
