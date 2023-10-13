// pod-io
// Kyle J Burgess

#ifndef POD_IO_H
#define POD_IO_H

#include <cstdint>

extern "C"
{
    // An item contains a key and its associated POD array
    typedef void             PodItem;

    // A container of PodItems
    struct                   PodContainer;

    // Result of pod-io functions
    enum                     PodResult      : uint32_t
    {
        POD_SUCCESS                = 0u,          // Success
        POD_NULL_REFERENCE         = 1u,          // Tried to pass null for an item or container
        POD_TYPE_MISMATCH          = 2u,          // Tried to get a type that does not match the stored type
        POD_OUT_OF_RANGE           = 3u,          // Tried to copy values out of the range of the stored buffer
        POD_FILE_CORRUPT           = 4u,          // Save file is corrupt
        POD_FILE_NOT_FOUND         = 5u,          // Unable to open file for read or write
        POD_ARGUMENT_ERROR         = 6u,          // Provided an incorrect argument to a method
        POD_ZLIB_ERROR             = 7u,          // Error during zlib initialization
    };

    // Types of Data
    enum                     PodType        : uint32_t
    {
        POD_ASCII_CHAR8            = 0x02000001u, // 8-bit ASCII characters
        POD_UTF8_CHAR8             = 0x03000001u, // 8-bit UTF8 bytes
        POD_UINT8                  = 0x00000001u, // 8-bit unsigned integer
        POD_UINT16                 = 0x00000002u, // 16-bit unsigned integer
        POD_UINT32                 = 0x00000004u, // 32-bit unsigned integer
        POD_UINT64                 = 0x00000008u, // 64-bit unsigned integer
        POD_INT8                   = 0x00010001u, // 8-bit signed twos-complement integer
        POD_INT16                  = 0x00010002u, // 16-bit signed twos-complement integer
        POD_INT32                  = 0x00010004u, // 32-bit signed twos-complement integer
        POD_INT64                  = 0x00010008u, // 64-bit signed twos-complement integer
        POD_FLOAT32                = 0x01010004u, // 32-bit IEEE floating point number
        POD_FLOAT64                = 0x01010008u, // 64-bit IEEE floating point number
    };

    // Compression Levels
    enum                     PodCompression : uint32_t
    {
        POD_COMPRESSION_0          = 0u,          // No compression (largest size)
        POD_COMPRESSION_1          = 1u,          // Least compression
        POD_COMPRESSION_2          = 2u,
        POD_COMPRESSION_3          = 3u,
        POD_COMPRESSION_4          = 4u,
        POD_COMPRESSION_5          = 5u,
        POD_COMPRESSION_6          = 6u,
        POD_COMPRESSION_7          = 7u,
        POD_COMPRESSION_8          = 8u,
        POD_COMPRESSION_9          = 9u,          // Best compression (smallest size)
        POD_COMPRESSION_NONE       = POD_COMPRESSION_0,
        POD_COMPRESSION_DEFAULT    = POD_COMPRESSION_6,
        POD_COMPRESSION_BEST       = POD_COMPRESSION_9,
    };

    // Endianness
    enum                     PodEndian      : uint32_t
    {
        POD_ENDIAN_LITTLE          = 0u,          // Save the file in little endian format
        POD_ENDIAN_BIG             = 1u,          // Save the file in big endian format
        POD_ENDIAN_NATIVE          = 2u,          // Save the file in the endianness of the host
    };

    // Checksum Type
    enum                     PodChecksum    : uint32_t
    {
        POD_CHECKSUM_NONE          = 0u,          // Read/write a file with no checksum
        POD_CHECKSUM_ADLER32       = 1u,          // Read/write a file with an adler32 checksum
        POD_CHECKSUM_CRC32         = 2u,          // Read/write a file with a crc32 checksum
    };

    // Create a container
    PodContainer*     __cdecl podCreateContainer();

    // Delete a container
    void             __cdecl podDeleteContainer(
        PodContainer*           container);      // Handle to a valid PodContainer

    // Load a file into a container
    // If checksum is NONE, then checksumValue isn't used.
    // If checksum is not NONE, then checksumValue must be
    // equal to the same checksumValue used to save the file
    PodResult         __cdecl podLoadFile(
        PodContainer*            container,       // Handle to a valid PodContainer
        const char*              fileName,        // File name
        PodChecksum              checksum,        // Checksum type
        uint32_t                 checksumValue);  // Initial checksum value

    // Save a file using data stored in the container
    // If checksum is NONE, then checksumValue isn't used.
    // If checksum is not NONE, then checksumValue must be
    // used again to load the file.
    PodResult         __cdecl podSaveFile(
        PodContainer*            container,       // Handle to a valid PodContainer
        const char*              fileName,        // File name
        PodCompression           compression,     // Compression level
        PodChecksum              checksum,        // Checksum type
        uint32_t                 checksumValue,   // Initial checksum value
        PodEndian                endianness);

    // Get an item from a container
    // If the item doesn't exist, then it will be created
    // returns nullptr if the key size exceeds available memory,
    // or if the container is null
    PodItem*          __cdecl podGetItem(
        PodContainer*            container,       // Handle to a valid PodContainer
        const char*              key);            // Null-terminated ASCII key

    // Get an item from a container
    // If the item doesn't exist, or if the container is nullptr,
    // then it will return nullptr
    PodItem*          __cdecl podTryGetItem(
        PodContainer*            container,       // Handle to a valid PodContainer
        const char*              key);            // Null-terminated ASCII key

    // Remove an item from the container
    // does nothing if the key doesn't exist
    PodResult         __cdecl podRemoveItem(
        PodContainer*            container,
        PodItem*                 item);

    // Set the values in a block
    PodResult         __cdecl podSetValues(
        PodItem*                 item,            // Handle to a valid PodItem
        const void*              srcValueArray,   // Array of values to set
        uint32_t                 valueCount,      // Number of values in the array
        PodType                  valueType);      // Type of values in the array

    // Count the number of values in a block
    PodResult         __cdecl podTryCountValues(
        const PodItem*           item,            // Handle to a valid PodItem
        uint32_t&                valueCount);     // Returned number of values in the block

    // Get the data type of a block
    PodResult         __cdecl podTryGetType(
        const PodItem*           item,            // Handle to a valid PodItem
        PodType&                 valueType);      // Returned type of values in the block

    // Copy the values from a block into a destination array
    PodResult         __cdecl podTryCopyValues(
        const PodItem*           item,            // Handle to a valid PodItem
        void*                    dstValueArray,   // Array to copy values to
        uint32_t                 valueCount,      // Number of values to copy
        PodType                  type);           // The type of the values being copied

    // Count the number of characters in an item's key
    PodResult         __cdecl podTryCountKeyChars(
        const PodItem*           item,            // Handle to a valid PodItem
        uint32_t&                count);          // The returned number of ASCII characters

    // Copies an item's key into a buffer without
    // a null terminating character
    PodResult         __cdecl podTryCopyKey(
        const PodItem*           item,            // Handle to a valid PodItem
        char*                    buffer,          // The buffer to copy the key to
        uint32_t                 charCount);      // The number of characters in the key

    // Get the first item in the container's current state
    // The order will change if items
    // are added or removed from the container.
    // Returns nullptr if there are no items in the container
    PodItem*          __cdecl podGetFirstItem(
        PodContainer*            container);      // Handle to a valid PodContainer

    // Get the next item in the container
    // The order will change if items
    // are added or removed from the container.
    // Returns nullptr if there are no more
    // item in the container.
    PodItem*          __cdecl podGetNextItem(
        PodContainer*            container,      // Handle to a valid PodContainer
        PodItem*                 item);          // Handle to a valid PodItem
}

#endif
