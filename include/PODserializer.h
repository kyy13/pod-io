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
        PS_BLOCK_NOT_FOUND,
        PS_TYPE_MISMATCH,
        PS_UNABLE_TO_OPEN,
        PS_FILE_CORRUPT,
        PS_FILE_NOT_FOUND,
    };

    enum PsType : uint32_t
    {
        PS_UINT8,
        PS_UINT16,
        PS_UINT32,
        PS_UINT64,
        PS_INT8,
        PS_INT16,
        PS_INT32,
        PS_INT64,
        PS_FLOAT32,
        PS_FLOAT64,
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

    PsBlock* psGetBlockByIndex(
        PsSerializer* s,
        uint32_t index);

    void psSetValues(
        PsBlock* b,
        const void* values,
        PsType valueType,
        uint32_t valueCount);

    PsResult psTryCountValues(
        const PsBlock* b,
        uint32_t& valueCount);

    PsResult psTryCountKeyChars(
        const PsBlock* b,
        uint32_t& charCount);

    PsResult psTryGetType(
        const PsBlock* b,
        PsType& valueType);

    PsResult psTryCopyValues(
        const PsBlock* b,
        void* dst,
        PsType type);

    PsResult psTryCopyKey(
        const PsBlock* b,
        void* dst);
};

#endif
