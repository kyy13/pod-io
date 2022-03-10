// PODstore
// Kyle J Burgess

#ifndef POD_STORE_H
#define POD_STORE_H

#include <cstdint>

extern "C"
{
    // An item contains a key and its associated POD array
    struct                   PsItem;

    // A container of PsBlocks
    struct                   PsContainer;

    // Result of PODstore functions
    enum                     PsResult      : uint32_t
    {
        PS_SUCCESS                = 0u,          // Success
        PS_NULL_REFERENCE         = 1u,          // Tried to pass null for an item or container
        PS_TYPE_MISMATCH          = 2u,          // Tried to get a type that does not match the stored type
        PS_OUT_OF_RANGE           = 3u,          // Tried to copy values out of the range of the stored buffer
        PS_FILE_CORRUPT           = 4u,          // Save file is corrupt
        PS_FILE_NOT_FOUND         = 5u,          // Unable to open file for read or write
        PS_ARGUMENT_ERROR         = 6u,          // Provided an incorrect argument to a method
        PS_ZLIB_ERROR             = 7u,          // Error during zlib initialization
    };

    // Types of Data
    enum                     PsType        : uint32_t
    {
        PS_ASCII_CHAR8            = 0x02000001u, // 8-bit ASCII characters
        PS_UTF8_CHAR8             = 0x03000001u, // 8-bit UTF8 bytes
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
        PS_COMPRESSION_9          = 9u,          // Best compression (smallest size)
        PS_COMPRESSION_NONE       = PS_COMPRESSION_0,
        PS_COMPRESSION_DEFAULT    = PS_COMPRESSION_6,
        PS_COMPRESSION_BEST       = PS_COMPRESSION_9,
    };

    // Endianness
    enum                     PsEndian      : uint32_t
    {
        PS_ENDIAN_LITTLE          = 0u,          // Save the file in little endian format
        PS_ENDIAN_BIG             = 1u,          // Save the file in big endian format
        PS_ENDIAN_NATIVE          = 2u,          // Save the file in the endianness of the host
    };

    // Checksum Type
    enum                     PsChecksum    : uint32_t
    {
        PS_CHECKSUM_NONE          = 0u,          // Read/write a file with no checksum
        PS_CHECKSUM_ADLER32       = 1u,          // Read/write a file with an adler32 checksum
        PS_CHECKSUM_CRC32         = 2u,          // Read/write a file with a crc32 checksum
    };

    // Create a serializer
    PsContainer*     __cdecl psCreateContainer();

    // Delete a serializer
    void             __cdecl psDeleteContainer(
        PsContainer*           container);      // Handle to a valid PsContainer

    // Load a file into a serializer
    // If checksum is NONE, then checksumValue isn't used.
    // If checksum is not NONE, then checksumValue must be
    // equal to the same checksumValue used to save the file
    PsResult         __cdecl psLoadFile(
        PsContainer*            container,       // Handle to a valid PsContainer
        const char*             fileName,        // File name
        PsChecksum              checksum,        // Checksum type
        uint32_t                checksumValue);  // Initial checksum value

    // Save a file using data stored in the serializer
    // If checksum is NONE, then checksumValue isn't used.
    // If checksum is not NONE, then checksumValue must be
    // used again to load the file.
    PsResult         __cdecl psSaveFile(
        PsContainer*            container,       // Handle to a valid PsContainer
        const char*             fileName,        // File name
        PsCompression           compression,     // Compression level
        PsChecksum              checksum,        // Checksum type
        uint32_t                checksumValue,   // Initial checksum value
        PsEndian                endianness);

    // Get an item from a container
    // If the item doesn't exist, then it will be created
    // returns nullptr if the key size exceeds available memory,
    // or if the container is null
    PsItem*          __cdecl psGetItem(
        PsContainer*            container,       // Handle to a valid PsContainer
        const char*             key);            // Null-terminated ASCII key

    // Get an item from a container
    // If the item doesn't exist, or if the container is nullptr,
    // then it will return nullptr
    PsItem*          __cdecl psTryGetItem(
        PsContainer*            container,       // Handle to a valid PsContainer
        const char*             key);            // Null-terminated ASCII key

    // Remove an item from the container
    // does nothing if the key doesn't exist
    PsResult         __cdecl psRemoveItem(
        PsContainer*            container,
        PsItem*                 item);

    // Set the values in a block
    PsResult         __cdecl psSetValues(
        PsItem*                 item,            // Handle to a valid PsItem
        const void*             srcValueArray,   // Array of values to set
        uint32_t                valueCount,      // Number of values in the array
        PsType                  valueType);      // Type of values in the array

    // Count the number of values in a block
    PsResult         __cdecl psTryCountValues(
        const PsItem*           item,            // Handle to a valid PsItem
        uint32_t&               valueCount);     // Returned number of values in the block

    // Get the data type of a block
    PsResult         __cdecl psTryGetType(
        const PsItem*           item,            // Handle to a valid PsItem
        PsType&                 valueType);      // Returned type of values in the block

    // Copy the values from a block into a destination array
    PsResult         __cdecl psTryCopyValues(
        const PsItem*           item,            // Handle to a valid PsItem
        void*                   dstValueArray,   // Array to copy values to
        uint32_t                valueCount,      // Number of values to copy
        PsType                  type);           // The type of the values being copied

    // Count the number of characters in an item's key
    PsResult         __cdecl psTryCountKeyChars(
        const PsItem*           item,            // Handle to a valid PsItem
        uint32_t&               count);          // The returned number of ASCII characters

    // Copies an item's key into a buffer without
    // a null terminating character
    PsResult         __cdecl psTryCopyKey(
        const PsItem*           item,            // Handle to a valid PsItem
        char*                   buffer,          // The buffer to copy the key to
        uint32_t                charCount);      // The number of characters in the key

    // Get the first item in the container's current state
    // The order will change if items
    // are added or removed from the container.
    // Returns nullptr if there are no items in the container
    PsItem*          __cdecl psGetFirstItem(
        PsContainer*            container);      // Handle to a valid PsContainer

    // Get the next item in the container
    // The order will change if items
    // are added or removed from the container.
    // Returns nullptr if there are no more
    // item in the container.
    PsItem*          __cdecl psGetNextItem(
        PsContainer*            container,      // Handle to a valid PsContainer
        PsItem*                 item);          // Handle to a valid PsItem
}

#endif
