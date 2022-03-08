// PODstore
// Kyle J Burgess

public class PODstore : IDisposable
{

    // -------------------------------------------
    // PODstore C++ Library Methods and Structs
    // -------------------------------------------

    // Result of PODstore functions
    protected enum           PsResult : uint
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
    protected enum           PsType : uint
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

    // Endianness
    protected enum           PsEndian : uint32_t
    {
        PS_ENDIAN_LITTLE          = 0u,          // Save the file in little endian format
        PS_ENDIAN_BIG             = 1u,          // Save the file in big endian format
        PS_ENDIAN_NATIVE          = 2u,          // Save the file in the endianness of the host
    };

    // Checksum Type
    protected enum           PsChecksum : uint32_t
    {
        PS_CHECKSUM_NONE          = 0u,          // Read/write a file with no checksum
        PS_CHECKSUM_ADLER32       = 1u,          // Read/write a file with an adler32 checksum
        PS_CHECKSUM_CRC32         = 2u,          // Read/write a file with a crc32 checksum
    };

    // Create a serializer
    [DllImport("libPODstore", EntryPoint = "psCreateSerializer", CallingConvention = CallingConvention.Cdecl)]
    protected static extern IntPtr      PsCreateSerializer();

    // Delete a serializer
    [DllImport("libPODstore", EntryPoint = "psDeleteSerializer", CallingConvention = CallingConvention.Cdecl)]
    protected static extern void        PsDeleteSerializer(
        IntPtr                             serializer); // Handle to a valid PsSerializer

    // Load a file into a serializer
    // If checksum is NONE, then checksumValue isn't used.
    // If checksum is not NONE, then checksumValue must be
    // equal to the same checksumValue used to save the file
    [DllImport("libPODstore", EntryPoint = "psLoadFile", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsLoadFile(
        IntPtr                             serializer,      // Handle to a valid PsSerializer
        string                             fileName,        // File name
        PsChecksum                         checksum,        // Checksum type
        uint                               checksumValue);  // Initial checksum value

    // Save a file using data stored in the serializer
    // If checksum is NONE, then checksumValue isn't used.
    // If checksum is not NONE, then checksumValue must be
    // used again to load the file.
    [DllImport("libPODstore", EntryPoint = "psSaveFile", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSaveFile(
        IntPtr                             serializer,      // Handle to a valid PsSerializer
        string                             fileName,        // File name
        PsChecksum                         checksum,        // Checksum type
        uint                               checksumValue,   // Initial checksum value
        PsEndian                           endianness);

    // Get a block of data from the serializer using its key
    // If a block doesn't exist, then it will be created
    // returns nullptr only if the key size exceeds available memory
    [DllImport("libPODstore", EntryPoint = "psGetBlock", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsGetBlock(
        IntPtr                             serializer,      // Handle to a valid PsSerializer
        string                             key);            // Null-terminated key

    // Set the values in a block
    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(
        IntPtr                             block,           // Handle to a valid PsBlock
        IntPtr                             srcValueArray,   // Array of values to set
        uint                               valueCount,      // Number of values in the array
        PsType                             valueType);      // Type of values in the array

    // Count the number of values in a block
    [DllImport("libPODstore", EntryPoint = "psTryCountValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCountValues(
        IntPtr                             block,           // Handle to a valid PsBlock
        out uint                           valueCount);     // Returned number of values in the block

    // Get the data type of a block
    [DllImport("libPODstore", EntryPoint = "psTryGetType", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryGetType(
        IntPtr                             block,           // Handle to a valid PsBlock
        out PsType                         valueType);      // Returned type of values in the block

    // Copy the values from a block into a destination array
    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(
        IntPtr                             block,           // Handle to a valid PsBlock
        IntPtr                             dstValueArray,   // Array to copy values to
        uint                               valueCount,      // Number of values to copy
        PsType                             type);           // The type of the values being copied
}
