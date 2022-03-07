// PODstore
// Kyle J Burgess

#ifndef POD_STORE_H
#define POD_STORE_H

#include <cstdint>

extern "C"
{
    struct PsBlock;

    struct PsSerializer;

    enum PsResult : uint32_t
    {
        PS_SUCCESS                = 0u,          // Success
        PS_UNASSIGNED_BLOCK       = 1u,          // Tried to read, copy, or access data from a block with no data
        PS_TYPE_MISMATCH          = 2u,          // Tried to get a type that does not match the stored type
        PS_OUT_OF_RANGE           = 3u,          // Tried to copy values out of the range of the stored buffer
        PS_FILE_CORRUPT           = 4u,          // Save file is corrupt
        PS_FILE_NOT_FOUND         = 5u,          // Unable to open file for read or write
        PS_ARGUMENT_ERROR         = 6u,          // Provided an incorrect argument to a method
        PS_ZLIB_ERROR             = 7u,          // Error during zlib initialization
    };

    enum PsType : uint32_t
    {
        PS_CHAR8                  = 0x02000001u, // 8-bit ASCII character
        PS_UINT8                  = 0x00000001u, // 8-bit unsigned integer
        PS_UINT16                 = 0x00000002u, // 16-bit unsigned integer
        PS_UINT32                 = 0x00000004u, // 32-bit unsigned integer
        PS_UINT64                 = 0x00000008u, // 64-bit unsigned integer
        PS_INT8                   = 0x00010001u, // 8-bit signed twos-complement integer
        PS_INT16                  = 0x00010002u, // 16-bit signed twos-complement integer
        PS_INT32                  = 0x00010004u, // 32-bit signed twos-complement integer
        PS_INT64                  = 0x00010008u, // 64-bit signed twos-complement integer
        PS_FLOAT32                = 0x01010004u, // 32-bit IEEE floating point number
        PS_FLOAT64                = 0x01010008u, // 64-bit IEEE floating point number
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
        const char* fileName,
        PsChecksum checksum,
        uint32_t checksumValue);

    PsResult psSaveFile(
        PsSerializer* s,
        const char* fileName,
        PsChecksum,
        uint32_t checksumValue,
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
