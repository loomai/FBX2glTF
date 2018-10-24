/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#include "AccessorData.h"
#include "BufferViewData.h"

AccessorData::AccessorData(const BufferViewData &bufferView, GLType type)
    : Holdable(),
      bufferView(bufferView.ix),
      type(std::move(type)),
      byteOffset(0),
      count(0)
{
}

AccessorData::AccessorData(GLType type)
    : Holdable(),
      bufferView(-1),
      type(std::move(type)),
      byteOffset(0),
      count(0)
{
}

json AccessorData::serialize() const
{
    json result {
        { "componentType", type.componentType.glType },
        { "type", type.dataType },
        { "count", count }
    };
    if (bufferView >= 0) {
        result["bufferView"] = bufferView;
        result["byteOffset"] = byteOffset;
    }
    if (!min.empty()) {
        result["min"] = min;
    }
    if (!max.empty()) {
        result["max"] = max;
    }
    return result;
}


// SparseAccessorData
SparseAccessorData::SparseAccessorData(const BufferViewData &idxBufferView,
                                       const BufferViewData &valBufferView,
                                       GLType idxType, GLType valType)
    : Holdable(),
      idxBufferView(idxBufferView.ix),
      valBufferView(valBufferView.ix),
      idxType(std::move(idxType)),
      valType(std::move(valType)),
      idxByteOffset(0),
      valByteOffset(0),
      count(0),
      sparseCount(0)
{
}

SparseAccessorData::SparseAccessorData(GLType idxType, GLType valType)
    : Holdable(),
      idxBufferView(-1),
      valBufferView(-1),
      idxType(std::move(idxType)),
      valType(std::move(valType)),
      idxByteOffset(0),
      valByteOffset(0),
      count(0),
      sparseCount(0)
{
}

json SparseAccessorData::serialize() const
{
    json result {
        { "componentType", valType.componentType.glType },
        { "type", valType.dataType },
        { "count", count },
        { "sparse", { { "count", sparseCount } } }
    };
    if (idxBufferView >= 0) {
        result["sparse"]["indices"]["bufferView"] = idxBufferView;
        result["sparse"]["indices"]["byteOffset"] = idxByteOffset;
        result["sparse"]["indices"]["componentType"] = idxType.componentType.glType;
    }
    if (valBufferView >= 0) {
        result["sparse"]["values"]["bufferView"] = valBufferView;
        result["sparse"]["values"]["byteOffset"] = valByteOffset;
        result["sparse"]["values"]["componentType"] = valType.componentType.glType;
    }
    if (!min.empty()) {
        result["min"] = min;
    }
    if (!max.empty()) {
        result["max"] = max;
    }
    return result;
}
