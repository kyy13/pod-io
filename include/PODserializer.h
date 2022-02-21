// PODserializer
// Kyle J Burgess

#ifndef POD_SERIALIZER_H
#define POD_SERIALIZER_H

#include <cstdint>

extern "C"
{
    class PsBlock;

    struct PsSerializer;

    enum PsResult : uint32_t
    {
        PS_SUCCESS,
        PS_UNASSIGNED_BLOCK,
        PS_TYPE_MISMATCH,
        PS_UNABLE_TO_OPEN,
        PS_FILE_CORRUPT,
        PS_FILE_NOT_FOUND,
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
        PS_LITTLE_ENDIAN,
        PS_BIG_ENDIAN,
        PS_NATIVE_ENDIAN,
    };

    enum PsChecksum : uint32_t
    {
        PS_NO_CHECKSUM,
        PS_CRC,
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

    PsBlock* psGetBlockByKey(
        PsSerializer* s,
        const char* key);

    void psSetValues(
        PsBlock* b,
        const void* values,
        PsType valueType,
        uint32_t valueCount);

    PsResult psTryCountValues(
        const PsBlock* b,
        uint32_t& valueCount);

    PsResult psTryGetType(
        const PsBlock* b,
        PsType& valueType);

    PsResult psTryCopyValues(
        const PsBlock* b,
        void* dst,
        PsType type);
};

#endif
