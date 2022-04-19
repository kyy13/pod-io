// pod-index
// Kyle J Burgess

#ifndef POD_INDEX_H
#define POD_INDEX_H

#include <cstdint>

extern "C"
{
    // An item contains a key and its associated POD array
    typedef void             PxItem;

    // A container of PxItems
    struct                   PxContainer;

    // Result of pod-index functions
    enum                     PxResult      : uint32_t
    {
        PX_SUCCESS                = 0u,          // Success
        PX_NULL_REFERENCE         = 1u,          // Tried to pass null for an item or container
        PX_TYPE_MISMATCH          = 2u,          // Tried to get a type that does not match the stored type
        PX_OUT_OF_RANGE           = 3u,          // Tried to copy values out of the range of the stored buffer
        PX_FILE_CORRUPT           = 4u,          // Save file is corrupt
        PX_FILE_NOT_FOUND         = 5u,          // Unable to open file for read or write
        PX_ARGUMENT_ERROR         = 6u,          // Provided an incorrect argument to a method
        PX_ZLIB_ERROR             = 7u,          // Error during zlib initialization
    };

    // Types of Data
    enum                     PxType        : uint32_t
    {
        PX_ASCII_CHAR8            = 0x02000001u, // 8-bit ASCII characters
        PX_UTF8_CHAR8             = 0x03000001u, // 8-bit UTF8 bytes
        PX_UINT8                  = 0x00000001u, // 8-bit unsigned integer
        PX_UINT16                 = 0x00000002u, // 16-bit unsigned integer
        PX_UINT32                 = 0x00000004u, // 32-bit unsigned integer
        PX_UINT64                 = 0x00000008u, // 64-bit unsigned integer
        PX_INT8                   = 0x00010001u, // 8-bit signed twos-complement integer
        PX_INT16                  = 0x00010002u, // 16-bit signed twos-complement integer
        PX_INT32                  = 0x00010004u, // 32-bit signed twos-complement integer
        PX_INT64                  = 0x00010008u, // 64-bit signed twos-complement integer
        PX_FLOAT32                = 0x01010004u, // 32-bit IEEE floating point number
        PX_FLOAT64                = 0x01010008u, // 64-bit IEEE floating point number
    };

    // Compression Levels
    enum                     PxCompression : uint32_t
    {
        PX_COMPRESSION_0          = 0u,          // No compression (largest size)
        PX_COMPRESSION_1          = 1u,          // Least compression
        PX_COMPRESSION_2          = 2u,
        PX_COMPRESSION_3          = 3u,
        PX_COMPRESSION_4          = 4u,
        PX_COMPRESSION_5          = 5u,
        PX_COMPRESSION_6          = 6u,
        PX_COMPRESSION_7          = 7u,
        PX_COMPRESSION_8          = 8u,
        PX_COMPRESSION_9          = 9u,          // Best compression (smallest size)
        PX_COMPRESSION_NONE       = PX_COMPRESSION_0,
        PX_COMPRESSION_DEFAULT    = PX_COMPRESSION_6,
        PX_COMPRESSION_BEST       = PX_COMPRESSION_9,
    };

    // Endianness
    enum                     PxEndian      : uint32_t
    {
        PX_ENDIAN_LITTLE          = 0u,          // Save the file in little endian format
        PX_ENDIAN_BIG             = 1u,          // Save the file in big endian format
        PX_ENDIAN_NATIVE          = 2u,          // Save the file in the endianness of the host
    };

    // Checksum Type
    enum                     PxChecksum    : uint32_t
    {
        PX_CHECKSUM_NONE          = 0u,          // Read/write a file with no checksum
        PX_CHECKSUM_ADLER32       = 1u,          // Read/write a file with an adler32 checksum
        PX_CHECKSUM_CRC32         = 2u,          // Read/write a file with a crc32 checksum
    };

    // Create a container
    PxContainer*     __cdecl pxCreateContainer();

    // Delete a container
    void             __cdecl pxDeleteContainer(
        PxContainer*           container);      // Handle to a valid PxContainer

    // Load a file into a container
    // If checksum is NONE, then checksumValue isn't used.
    // If checksum is not NONE, then checksumValue must be
    // equal to the same checksumValue used to save the file
    PxResult         __cdecl pxLoadFile(
        PxContainer*            container,       // Handle to a valid PxContainer
        const char*             fileName,        // File name
        PxChecksum              checksum,        // Checksum type
        uint32_t                checksumValue);  // Initial checksum value

    // Save a file using data stored in the container
    // If checksum is NONE, then checksumValue isn't used.
    // If checksum is not NONE, then checksumValue must be
    // used again to load the file.
    PxResult         __cdecl pxSaveFile(
        PxContainer*            container,       // Handle to a valid PxContainer
        const char*             fileName,        // File name
        PxCompression           compression,     // Compression level
        PxChecksum              checksum,        // Checksum type
        uint32_t                checksumValue,   // Initial checksum value
        PxEndian                endianness);

    // Get an item from a container
    // If the item doesn't exist, then it will be created
    // returns nullptr if the key size exceeds available memory,
    // or if the container is null
    PxItem*          __cdecl pxGetItem(
        PxContainer*            container,       // Handle to a valid PxContainer
        const char*             key);            // Null-terminated ASCII key

    // Get an item from a container
    // If the item doesn't exist, or if the container is nullptr,
    // then it will return nullptr
    PxItem*          __cdecl pxTryGetItem(
        PxContainer*            container,       // Handle to a valid PxContainer
        const char*             key);            // Null-terminated ASCII key

    // Remove an item from the container
    // does nothing if the key doesn't exist
    PxResult         __cdecl pxRemoveItem(
        PxContainer*            container,
        PxItem*                 item);

    // Set the values in a block
    PxResult         __cdecl pxSetValues(
        PxItem*                 item,            // Handle to a valid PxItem
        const void*             srcValueArray,   // Array of values to set
        uint32_t                valueCount,      // Number of values in the array
        PxType                  valueType);      // Type of values in the array

    // Count the number of values in a block
    PxResult         __cdecl pxTryCountValues(
        const PxItem*           item,            // Handle to a valid PxItem
        uint32_t&               valueCount);     // Returned number of values in the block

    // Get the data type of a block
    PxResult         __cdecl pxTryGetType(
        const PxItem*           item,            // Handle to a valid PxItem
        PxType&                 valueType);      // Returned type of values in the block

    // Copy the values from a block into a destination array
    PxResult         __cdecl pxTryCopyValues(
        const PxItem*           item,            // Handle to a valid PxItem
        void*                   dstValueArray,   // Array to copy values to
        uint32_t                valueCount,      // Number of values to copy
        PxType                  type);           // The type of the values being copied

    // Count the number of characters in an item's key
    PxResult         __cdecl pxTryCountKeyChars(
        const PxItem*           item,            // Handle to a valid PxItem
        uint32_t&               count);          // The returned number of ASCII characters

    // Copies an item's key into a buffer without
    // a null terminating character
    PxResult         __cdecl pxTryCopyKey(
        const PxItem*           item,            // Handle to a valid PxItem
        char*                   buffer,          // The buffer to copy the key to
        uint32_t                charCount);      // The number of characters in the key

    // Get the first item in the container's current state
    // The order will change if items
    // are added or removed from the container.
    // Returns nullptr if there are no items in the container
    PxItem*          __cdecl pxGetFirstItem(
        PxContainer*            container);      // Handle to a valid PxContainer

    // Get the next item in the container
    // The order will change if items
    // are added or removed from the container.
    // Returns nullptr if there are no more
    // item in the container.
    PxItem*          __cdecl pxGetNextItem(
        PxContainer*            container,      // Handle to a valid PxContainer
        PxItem*                 item);          // Handle to a valid PxItem
}

#endif
