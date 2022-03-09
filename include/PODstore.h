// PODstore
// Kyle J Burgess

#ifndef POD_STORE_H
#define POD_STORE_H

#include <cstdint>

extern "C"
{
    // A block of data consisting of a single type array of data indexed by a key
    struct                   PsBlock;

    // Serializer used to write/read files, and retrieve/store data
    struct                   PsSerializer;

    // Result of PODstore functions
    enum                     PsResult : uint32_t
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

    // Types of Data
    enum                     PsType : uint32_t
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

    // Compression Levels
    enum                     PsCompression : uint32_t
    {
        PS_COMPRESSION_0          = 0u,          // No compression (largest size)
        PS_COMPRESSION_1          = 1u,          // Least compression
        PS_COMPRESSION_2          = 2u,
        PS_COMPRESSION_3          = 3u,
        PS_COMPRESSION_4          = 4u,
        PS_COMPRESSION_5          = 5u,
        PS_COMPRESSION_6          = 6u,
        PS_COMPRESSION_7          = 7u,
        PS_COMPRESSION_8          = 8u,
        PS_COMPRESSION_9          = 9u,          // Most compression (smallest size)
    };

    // Endianness
    enum                     PsEndian : uint32_t
    {
        PS_ENDIAN_LITTLE          = 0u,          // Save the file in little endian format
        PS_ENDIAN_BIG             = 1u,          // Save the file in big endian format
        PS_ENDIAN_NATIVE          = 2u,          // Save the file in the endianness of the host
    };

    // Checksum Type
    enum                     PsChecksum : uint32_t
    {
        PS_CHECKSUM_NONE          = 0u,          // Read/write a file with no checksum
        PS_CHECKSUM_ADLER32       = 1u,          // Read/write a file with an adler32 checksum
        PS_CHECKSUM_CRC32         = 2u,          // Read/write a file with a crc32 checksum
    };

    // Create a serializer
    PsSerializer*    __cdecl psCreateSerializer();

    // Delete a serializer
    void             __cdecl psDeleteSerializer(
        PsSerializer*           serializer);     // Handle to a valid PsSerializer

    // Load a file into a serializer
    // If checksum is NONE, then checksumValue isn't used.
    // If checksum is not NONE, then checksumValue must be
    // equal to the same checksumValue used to save the file
    PsResult         __cdecl psLoadFile(
        PsSerializer*           serializer,      // Handle to a valid PsSerializer
        const char*             fileName,        // File name
        PsChecksum              checksum,        // Checksum type
        uint32_t                checksumValue);  // Initial checksum value

    // Save a file using data stored in the serializer
    // If checksum is NONE, then checksumValue isn't used.
    // If checksum is not NONE, then checksumValue must be
    // used again to load the file.
    PsResult         __cdecl psSaveFile(
        PsSerializer*           serializer,      // Handle to a valid PsSerializer
        const char*             fileName,        // File name
        PsCompression           compression,     // Compression level
        PsChecksum              checksum,        // Checksum type
        uint32_t                checksumValue,   // Initial checksum value
        PsEndian                endianness);

    // Get a block of data from the serializer using its key
    // If a block doesn't exist, then it will be created
    // returns nullptr only if the key size exceeds available memory
    PsBlock*         __cdecl psGetBlock(
        PsSerializer*           serializer,      // Handle to a valid PsSerializer
        const char*             key);            // Null-terminated key

    // Set the values in a block
    PsResult         __cdecl psSetValues(
        PsBlock*                block,           // Handle to a valid PsBlock
        const void*             srcValueArray,   // Array of values to set
        uint32_t                valueCount,      // Number of values in the array
        PsType                  valueType);      // Type of values in the array

    // Count the number of values in a block
    PsResult         __cdecl psTryCountValues(
        const PsBlock*          block,           // Handle to a valid PsBlock
        uint32_t&               valueCount);     // Returned number of values in the block

    // Get the data type of a block
    PsResult         __cdecl psTryGetType(
        const PsBlock*          block,           // Handle to a valid PsBlock
        PsType&                 valueType);      // Returned type of values in the block

    // Copy the values from a block into a destination array
    PsResult         __cdecl psTryCopyValues(
        const PsBlock*          block,           // Handle to a valid PsBlock
        void*                   dstValueArray,   // Array to copy values to
        uint32_t                valueCount,      // Number of values to copy
        PsType                  type);           // The type of the values being copied
}

#endif
