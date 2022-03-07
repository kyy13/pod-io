// PODserializer
// Kyle J Burgess

#ifndef POD_SERIALIZER_H
#define POD_SERIALIZER_H

#include <cstdint>

extern "C"
{
    struct PsBlock;

    struct PsSerializer;

    enum PsResult : uint32_t
    {
        PS_SUCCESS,
        PS_UNASSIGNED_BLOCK,
        PS_TYPE_MISMATCH,
        PS_OUT_OF_RANGE,
        PS_UNABLE_TO_OPEN,
        PS_FILE_CORRUPT,
        PS_FILE_NOT_FOUND,
        PS_UNSUPPORTED_ENDIANNESS,
        PS_UNSUPPORTED_CHECKSUM,
        PS_ZLIB_ERROR,
    };

    enum PsType : uint32_t
    {
        PS_CHAR8   = 0x02000001u,
        PS_UINT8   = 0x00000001u,
        PS_UINT16  = 0x00000002u,
        PS_UINT32  = 0x00000004u,
        PS_UINT64  = 0x00000008u,
        PS_INT8    = 0x00010001u,
        PS_INT16   = 0x00010002u,
        PS_INT32   = 0x00010004u,
        PS_INT64   = 0x00010008u,
        PS_FLOAT32 = 0x01010004u,
        PS_FLOAT64 = 0x01010008u,
    };

    enum PsEndian : uint32_t
    {
        PS_ENDIAN_LITTLE = 0u,
        PS_ENDIAN_BIG    = 1u,
        PS_ENDIAN_NATIVE = 2u,
    };

    enum PsChecksum : uint32_t
    {
        PS_CHECKSUM_NONE    = 0u,
        PS_CHECKSUM_ADLER32 = 1u,
        PS_CHECKSUM_CRC32   = 2u,
    };

    PsSerializer* psCreateSerializer();

    void psDeleteSerializer(
        PsSerializer* s);

    PsResult psLoadFile(
        PsSerializer* s,
        const char* fileName);

    PsResult psSaveFile(
        PsSerializer* s,
        const char* fileName,
        PsChecksum,
        PsEndian);

    PsBlock* psGetBlock(
        PsSerializer* s,
        const char* key);

    PsResult psSetValues(
        PsBlock* b,
        const void* values,
        uint32_t valueCount,
        PsType valueType);

    PsResult psTryCountValues(
        const PsBlock* b,
        uint32_t& valueCount);

    PsResult psTryGetType(
        const PsBlock* b,
        PsType& valueType);

    PsResult psTryCopyValues(
        const PsBlock* b,
        void* dst,
        uint32_t valueCount,
        PsType type);
};

#endif
