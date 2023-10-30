// pod-io
// Kyle J Burgess

#ifndef POD_IO_H
#define POD_IO_H

#define POD_API __cdecl

#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#endif

// An item contains a key and its associated POD array
typedef void pod_item_t;

// A container of pod_item(s)
typedef struct pod_container_t pod_container_t;

// Result of pod-io functions
typedef enum pod_result_t : uint32_t
{
    POD_SUCCESS                = 0u,          // Success
    POD_NULL_REFERENCE         = 1u,          // Tried to pass null for an item or container
    POD_TYPE_MISMATCH          = 2u,          // Tried to get a type that does not match the stored type
    POD_OUT_OF_RANGE           = 3u,          // Tried to copy values out of the range of the stored buffer
    POD_FILE_CORRUPT           = 4u,          // Save file is corrupt
    POD_FILE_NOT_FOUND         = 5u,          // Unable to open file for read or write
    POD_ARGUMENT_ERROR         = 6u,          // Provided an incorrect argument to a method
    POD_ZLIB_ERROR             = 7u,          // Error during zlib initialization
} pod_result_t;

// Types of Data
typedef enum pod_type_t : uint32_t
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
} pod_type_t;

// Compression Levels
typedef enum pod_compression_t : uint32_t
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
} pod_compression_t;

// Endianness
typedef enum pod_endian_t : uint32_t
{
    POD_ENDIAN_LITTLE          = 0u,          // Save the file in little endian format
    POD_ENDIAN_BIG             = 1u,          // Save the file in big endian format
    POD_ENDIAN_NATIVE          = 2u,          // Save the file in the endianness of the host
} pod_endian_t;

// Checksum Type
typedef enum pod_checksum_t : uint32_t
{
    POD_CHECKSUM_NONE          = 0u,          // Read/write a file with no checksum
    POD_CHECKSUM_ADLER32       = 1u,          // Read/write a file with an adler32 checksum
    POD_CHECKSUM_CRC32         = 2u,          // Read/write a file with a crc32 checksum
} pod_checksum_t;

// Create a container
pod_container_t* POD_API pod_alloc();

// Delete a container
void POD_API pod_free(pod_container_t* container);

// Load a file into a container
// If checksum is NONE, then checksumValue isn't used.
// If checksum is not NONE, then checksumValue must be
// equal to the same checksumValue used to save the file
pod_result_t POD_API pod_load_file(
    pod_container_t*         container,       // Handle to a valid pod_container_t
    const char*              fileName,        // File name
    pod_checksum_t           checksum,        // Checksum type
    uint32_t                 checksumValue);  // Initial checksum value

// Save a file using data stored in the container
// If checksum is NONE, then checksumValue isn't used.
// If checksum is not NONE, then checksumValue must be
// used again to load the file.
pod_result_t POD_API pod_save_file(
    pod_container_t*         container,       // Handle to a valid pod_container_t
    const char*              fileName,        // File name
    pod_compression_t        compression,     // Compression level
    pod_checksum_t           checksum,        // Checksum type
    uint32_t                 checksumValue,   // Initial checksum value
    pod_endian_t             endianness);

// Get an item from a container
// If the item doesn't exist, then it will be created
// returns nullptr if the key size exceeds available memory,
// or if the container is null
pod_item_t* POD_API pod_get_item(
    pod_container_t*         container,       // Handle to a valid pod_container_t
    const char*              key);            // Null-terminated ASCII key

// Get an item from a container
// If the item doesn't exist, or if the container is nullptr,
// then it will return nullptr
pod_item_t* POD_API pod_try_get_item(
    pod_container_t*         container,       // Handle to a valid pod_container_t
    const char*              key);            // Null-terminated ASCII key

// Remove an item from the container
// does nothing if the key doesn't exist
pod_result_t POD_API pod_remove_item(
    pod_container_t*         container,
    pod_item_t*              item);

// Set the values in a block
pod_result_t POD_API pod_set_values(
    pod_item_t*              item,            // Handle to a valid pod_item_t
    const void*              srcValueArray,   // Array of values to set
    uint32_t                 valueCount,      // Number of values in the array
    pod_type_t               valueType);      // Type of values in the array

// Count the number of values in a block
pod_result_t POD_API pod_try_count_values(
    const pod_item_t*        item,            // Handle to a valid pod_item_t
    uint32_t*                valueCount);     // Returned number of values in the block

// Get the data type of a block
pod_result_t POD_API pod_try_get_type(
    const pod_item_t*           item,            // Handle to a valid pod_item_t
    pod_type_t*                 valueType);      // Returned type of values in the block

// Copy the values from a block into a destination array
pod_result_t POD_API pod_try_copy_values(
    const pod_item_t*        item,            // Handle to a valid pod_item_t
    void*                    dstValueArray,   // Array to copy values to
    uint32_t                 valueCount,      // Number of values to copy
    pod_type_t               type);           // The type of the values being copied

// Count the number of characters in an item's key
pod_result_t POD_API pod_try_count_key_chars(
    const pod_item_t*        item,            // Handle to a valid pod_item_t
    uint32_t*                count);          // The returned number of ASCII characters

// Copies an item's key into a buffer without
// a null terminating character
pod_result_t POD_API pod_try_copy_key(
    const pod_item_t*        item,            // Handle to a valid pod_item_t
    char*                    buffer,          // The buffer to copy the key to
    uint32_t                 charCount);      // The number of characters in the key

// Get the first item in the container's current state
// The order will change if items
// are added or removed from the container.
// Returns nullptr if there are no items in the container
pod_item_t* POD_API pod_get_first_item(
    pod_container_t*         container);      // Handle to a valid pod_container_t

// Get the next item in the container
// The order will change if items
// are added or removed from the container.
// Returns nullptr if there are no more
// item in the container.
pod_item_t* POD_API pod_get_next_item(
    pod_container_t*         container,      // Handle to a valid pod_container_t
    pod_item_t*              item);          // Handle to a valid pod_item_t

#ifdef __cplusplus
}
#endif

#endif
