// PODserializer
// Kyle J Burgess

#include "PODserializer.h"

#include <cstring>
#include <vector>
#include <unordered_map>

constexpr bool isBigEndian()
{
    return false;
}

template<class T>
T swapBytes(T val)
{
    if constexpr (sizeof(T) == 1)
    {
        return val;
    }

    T result;

    const auto* vBytes = reinterpret_cast<const uint8_t*>(&val);
    auto* rBytes = reinterpret_cast<uint8_t*>(&result);

    for (size_t i = 0; i != sizeof(T); ++i)
    {
        constexpr size_t end = sizeof(T) - 1;
        rBytes[i] = vBytes[end - i];
    }

    return result;
}

template<class T>
std::vector<uint8_t> createDataField(T* val, size_t count)
{
    size_t size = count * sizeof(T);

    std::vector<uint8_t> data(size);

    if constexpr (isBigEndian())
    {
        auto* pData = reinterpret_cast<T*>(data.data());

        for (size_t i = 0; i != count; ++i)
        {
            pData[i] = swapBytes(val[i]);
        }
    }
    else
    {
        memcpy(data.data(), val, size);
    }

    return data;
}

struct Chunk
{
    ValueType type;
    std::vector<uint8_t> data;
};

struct Serializer
{
    std::unordered_map<std::string, Chunk> chunks;
};

void psSetUInt8(Serializer* s, const char* key, uint8_t data)
{
    s->chunks[key] =
        {
            .type = ValueType::UInt8,
            .data = createDataField(&data, 1),
        };
}

void psSetUInt8Array(
    Serializer* s,
    const char* key,
    const uint8_t* data,
    uint32_t count)
{
    s->chunks[key] =
        {
            .type = ValueType::UInt8Array,
            .data = createDataField(&data, count),
        };
}

void psSetUInt16(
    Serializer* s,
    const char* key,
    uint16_t data)
{

}

void psSetUInt16Array(
    Serializer* s,
    const char* key,
    const uint16_t* data,
    uint32_t count)
{

}

void psSetUInt32(
    Serializer* s,
    const char* key,
    uint32_t data)
{

}

void psSetUInt32Array(
    Serializer* s,
    const char* key,
    const uint32_t* data,
    uint32_t count)
{

}

void psSetUInt64(
    Serializer* s,
    const char* key,
    uint64_t data)
{

}

void psSetUInt64Array(
    Serializer* s,
    const char* key,
    const uint64_t* data,
    uint32_t count)
{

}

void psSetInt8(
    Serializer* s,
    const char* key,
    int8_t data)
{

}

void psSetInt8Array(
    Serializer* s,
    const char* key,
    const int8_t* data,
    uint32_t count)
{

}

void psSetInt16(
    Serializer* s,
    const char* key,
    int16_t data)
{

}

void psSetInt16Array(
    Serializer* s,
    const char* key,
    const int16_t* data,
    uint32_t count)
{

}

void psSetInt32(
    Serializer* s,
    const char* key,
    int32_t data)
{

}

void psSetInt32Array(
    Serializer* s,
    const char* key,
    const int32_t* data,
    uint32_t count)
{

}

void psSetInt64(
    Serializer* s,
    const char* key,
    int64_t data)
{

}

void psSetInt64Array(
    Serializer* s,
    const char* key,
    const int64_t* data,
    uint32_t count)
{

}

void psSetFloat(
    Serializer* s,
    const char* key,
    float data)
{

}

void psSetFloatArray(
    Serializer* s,
    const char* key,
    const float* data,
    uint32_t count)
{

}

void psSetDouble(
    Serializer* s,
    const char* key,
    float data)
{

}

void psSetDoubleArray(
    Serializer* s,
    const char* key,
    const float* data,
    uint32_t count)
{

}

bool psTryGetUInt8(
    Serializer* s,
    const char* key,
    uint8_t& data)
{

}

bool psTryCountUInt8Array(
    Serializer* s,
    const char* key,
    uint32_t& count)
{

}

bool psTryCopyUInt8Array(
    Serializer* s,
    const char* key,
    const uint8_t* data)
{

}

bool psTryGetUInt16(
    Serializer* s,
    const char* key,
    uint16_t& data)
{

}

bool psTryCountUInt16Array(
    Serializer* s,
    const char* key,
    uint32_t& count)
{

}

bool psTryCopyUInt16Array(
    Serializer* s,
    const char* key,
    const uint16_t* data)
{

}

bool psTryGetUInt32(
    Serializer* s,
    const char* key,
    uint32_t& data)
{

}

bool psTryCountUInt32Array(
    Serializer* s,
    const char* key,
    uint32_t& count)
{

}

bool psTryCopyUInt32Array(
    Serializer* s,
    const char* key,
    const uint32_t* data)
{

}

bool psTryGetUInt64(
    Serializer* s,
    const char* key,
    uint32_t& data)
{

}

bool psTryCountUInt64Array(
    Serializer* s,
    const char* key,
    uint32_t& count)
{

}

bool psTryCopyUInt64Array(
    Serializer* s,
    const char* key,
    const uint32_t* data)
{

}

bool psTryGetInt8(
    Serializer* s,
    const char* key,
    int8_t& data)
{

}

bool psTryCountInt8Array(
    Serializer* s,
    const char* key,
    uint32_t& count)
{

}

bool psTryCopyInt8Array(
    Serializer* s,
    const char* key,
    const int8_t* data)
{

}

bool psTryGetInt16(
    Serializer* s,
    const char* key,
    int16_t& data)
{

}

bool psTryCountInt16Array(
    Serializer* s,
    const char* key,
    uint32_t& count)
{

}

bool psTryCopyInt16Array(
    Serializer* s,
    const char* key,
    const int16_t* data)
{

}

bool psTryGetInt32(
    Serializer* s,
    const char* key,
    int32_t& data)
{

}

bool psTryCountInt32Array(
    Serializer* s,
    const char* key,
    uint32_t& count)
{

}

bool psTryCopyInt32Array(
    Serializer* s,
    const char* key,
    const int32_t* data)
{

}

bool psTryGetInt64(
    Serializer* s,
    const char* key,
    int32_t& data)
{

}

bool psTryCountInt64Array(
    Serializer* s,
    const char* key,
    uint32_t& count)
{

}

bool psTryCopyInt64Array(
    Serializer* s,
    const char* key,
    const int32_t* data)
{

}

bool psTryGetFloat(
    Serializer* s,
    const char* key,
    float& data)
{

}

bool psTryCountFloatArray(
    Serializer* s,
    const char* key,
    uint32_t& count)
{

}

bool psTryCopyFloatArray(
    Serializer* s,
    const char* key,
    const float* data)
{

}

bool psTryGetDouble(
    Serializer* s,
    const char* key,
    double& data)
{

}

bool psTryCountDoubleArray(
    Serializer* s,
    const char* key,
    uint32_t& count)
{

}

bool psTryCopyDoubleArray(
    Serializer* s,
    const char* key,
    const double* data)
{

}
