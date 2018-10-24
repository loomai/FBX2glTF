/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef FBX2GLTF_ACCESSORDATA_H
#define FBX2GLTF_ACCESSORDATA_H

#include "Raw2Gltf.h"

struct AccessorData : Holdable
{
    AccessorData(const BufferViewData &bufferView, GLType type);
    explicit AccessorData(GLType type);

    json serialize() const override;

    template<class T>
    void appendAsBinaryArray(const std::vector<T> &in, std::vector<uint8_t> &out)
    {
        const unsigned int stride = type.byteStride();
        const size_t       offset = out.size();
        const size_t       count  = in.size();

        this->count = (unsigned int) count;

        out.resize(offset + count * stride);
        for (int ii = 0; ii < count; ii ++) {
            type.write(&out[offset + ii * stride], in[ii]);
        }
    }

    unsigned int byteLength() const { return type.byteStride() * count; }

    const int          bufferView;
    const GLType       type;

    unsigned int       byteOffset;
    unsigned int       count;
    std::vector<float> min;
    std::vector<float> max;
};

template<class T>
inline bool isZero(const T &val)
{ return val < 1.0e-4; }

template<>
inline bool isZero<Vec3f>(const Vec3f &val)
{ for (int i=0; i<3; i++) { if (std::fabs(val[i]) > 1.0e-4) return false; } return true; }

template<>
inline bool isZero<Vec4f>(const Vec4f &val)
{ for (int i=0; i<4; i++) { if (std::fabs(val[i]) > 1.0e-4) return false; } return true; }



struct SparseAccessorData : Holdable
{
    SparseAccessorData(const BufferViewData &idxBufferView,
                       const BufferViewData &valBufferView,
                       GLType idxType, GLType valType);
    explicit SparseAccessorData(GLType idxType, GLType valType);

    json serialize() const override;

    template<class T>
    void appendAsBinaryArray(const std::vector<T> &in, std::vector<uint8_t> &out)
    {
        this->count = (unsigned int)in.size();

        std::vector<uint8_t> idxOut;
        std::vector<uint8_t> valOut;

        this->sparseCount = 0;
        for (unsigned int ii = 0; ii < this->count; ii ++) {
            if (!isZero(in[ii])) {
                this->sparseCount++;

                const size_t v_offset = valOut.size();
                valOut.resize(v_offset + valType.byteStride());
                valType.write(&valOut[v_offset], in[ii]);

                const size_t i_offset = idxOut.size();
                idxOut.resize(i_offset + idxType.byteStride());
                idxType.write(&idxOut[i_offset], ii);
            }
        }

        //std::cout << this->sparseCount << " / " << this->count << " sparse elements." << std::endl;

        std::copy(idxOut.begin(), idxOut.end(), std::back_inserter(out));

        // Align to 4-byte boundary
        unsigned long bufferSize = out.size();
        if ((bufferSize % 4) > 0) {
            bufferSize += (4 - (bufferSize % 4));
            out.resize(bufferSize);
        }

        std::copy(valOut.begin(), valOut.end(), std::back_inserter(out));
    }

    unsigned int idxByteLength() const { return idxType.byteStride() * sparseCount; }
    unsigned int valByteLength() const { return valType.byteStride() * sparseCount; }

    const int          idxBufferView;
    const int          valBufferView;
    const GLType       idxType;
    const GLType       valType;

    unsigned int       idxByteOffset;
    unsigned int       valByteOffset;
    unsigned int       count;
    unsigned int       sparseCount;
    std::vector<float> min;
    std::vector<float> max;
};

#endif //FBX2GLTF_ACCESSORDATA_H
