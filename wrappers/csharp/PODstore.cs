// PODstore
// Kyle J Burgess

using System;
using System.Runtime.InteropServices;
using System.Text;

public class PODstore : IDisposable
{
    public PODstore()
    {
        m_podSerializer = PsCreateSerializer();
    }

    public void Dispose()
    {
        TryDispose();
        GC.SuppressFinalize(this);
    }

    public void Load(string fileName, PsChecksum checksum, uint checksumValue = 0)
    {
        PsResult r = PsLoadFile(m_podSerializer, Encoding.ASCII.GetBytes(fileName), checksum, checksumValue);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void Save(string fileName, PsChecksum checksum, uint checksumValue = 0, PsEndian endianness = PsEndian.PS_ENDIAN_NATIVE)
    {
        PsResult r = PsSaveFile(m_podSerializer, Encoding.ASCII.GetBytes(fileName), checksum, checksumValue, endianness);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetAsciiString(string key, string str)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsSetValues(block, Encoding.ASCII.GetBytes(str), (uint)str.Length, PsType.PS_CHAR8);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetUInt8Array(string key, byte[] values)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsSetValues(block, values, (uint)values.Length, PsType.PS_UINT8);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetUInt8(string key, byte value)
    {
        SetUInt8Array(key, new byte[]{ value });
    }

    public void SetUInt16Array(string key, UInt16[] values)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsSetValues(block, values, (uint)values.Length, PsType.PS_UINT16);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetUInt16(string key, UInt16 value)
    {
        SetUInt16Array(key, new UInt16[]{ value });
    }

    public void SetUInt32Array(string key, UInt32[] values)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsSetValues(block, values, (uint)values.Length, PsType.PS_UINT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetUInt32(string key, UInt32 value)
    {
        SetUInt32Array(key, new UInt32[]{ value });
    }

    public void SetUInt64Array(string key, UInt64[] values)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsSetValues(block, values, (uint)values.Length, PsType.PS_UINT64);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetUInt64(string key, UInt64 value)
    {
        SetUInt64Array(key, new UInt64[]{ value });
    }

    public void SetInt8Array(string key, sbyte[] values)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsSetValues(block, values, (uint)values.Length, PsType.PS_INT8);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetInt8(string key, sbyte value)
    {
        SetInt8Array(key, new sbyte[]{ value });
    }

    public void SetInt16Array(string key, Int16[] values)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsSetValues(block, values, (uint)values.Length, PsType.PS_INT16);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetInt16(string key, Int16 value)
    {
        SetInt16Array(key, new Int16[]{ value });
    }

    public void SetInt32Array(string key, Int32[] values)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsSetValues(block, values, (uint)values.Length, PsType.PS_INT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetInt32(string key, Int32 value)
    {
        SetInt32Array(key, new Int32[]{ value });
    }

    public void SetInt64Array(string key, Int64[] values)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsSetValues(block, values, (uint)values.Length, PsType.PS_INT64);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetInt64(string key, Int64 value)
    {
        SetInt64Array(key, new Int64[]{ value });
    }

    public void SetFloat32Array(string key, float[] values)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsSetValues(block, values, (uint)values.Length, PsType.PS_FLOAT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetFloat32(string key, float value)
    {
        SetFloat32Array(key, new float[]{ value });
    }

    public void SetFloat64Array(string key, double[] values)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsSetValues(block, values, (uint)values.Length, PsType.PS_FLOAT64);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetFloat64(string key, double value)
    {
        SetFloat64Array(key, new double[]{ value });
    }

    public string GetAsciiString(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsTryCountValues(block, out uint count);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        byte[] dst = new byte[count];

        r = PsTryCopyValues(block, dst, count, PsType.PS_CHAR8);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return Encoding.ASCII.GetString(dst);
    }

    public byte[] GetUInt8Array(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsTryCountValues(block, out uint count);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        byte[] dst = new byte[count];

        r = PsTryCopyValues(block, dst, count, PsType.PS_UINT8);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public byte GetUInt8(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        byte[] dst = new byte[1];

        PsResult r = PsTryCopyValues(block, dst, 1, PsType.PS_UINT8);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public UInt16[] GetUInt16Array(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsTryCountValues(block, out uint count);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        UInt16[] dst = new UInt16[count];

        r = PsTryCopyValues(block, dst, count, PsType.PS_UINT16);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public UInt16 GetUInt16(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        UInt16[] dst = new UInt16[1];

        PsResult r = PsTryCopyValues(block, dst, 1, PsType.PS_UINT16);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public UInt32[] GetUInt32Array(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsTryCountValues(block, out uint count);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        UInt32[] dst = new UInt32[count];

        r = PsTryCopyValues(block, dst, count, PsType.PS_UINT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public UInt32 GetUInt32(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        UInt32[] dst = new UInt32[1];

        PsResult r = PsTryCopyValues(block, dst, 1, PsType.PS_UINT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public UInt64[] GetUInt64Array(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsTryCountValues(block, out uint count);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        UInt64[] dst = new UInt64[count];

        r = PsTryCopyValues(block, dst, count, PsType.PS_UINT64);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public UInt64 GetUInt64(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        UInt64[] dst = new UInt64[1];

        PsResult r = PsTryCopyValues(block, dst, 1, PsType.PS_UINT64);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public sbyte[] GetInt8Array(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsTryCountValues(block, out uint count);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        sbyte[] dst = new sbyte[count];

        r = PsTryCopyValues(block, dst, count, PsType.PS_INT8);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public sbyte GetInt8(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        sbyte[] dst = new sbyte[1];

        PsResult r = PsTryCopyValues(block, dst, 1, PsType.PS_INT8);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public Int16[] GetInt16Array(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsTryCountValues(block, out uint count);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        Int16[] dst = new Int16[count];

        r = PsTryCopyValues(block, dst, count, PsType.PS_INT16);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public Int16 GetInt16(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        Int16[] dst = new Int16[1];

        PsResult r = PsTryCopyValues(block, dst, 1, PsType.PS_INT16);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public Int32[] GetInt32Array(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsTryCountValues(block, out uint count);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        Int32[] dst = new Int32[count];

        r = PsTryCopyValues(block, dst, count, PsType.PS_INT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public Int32 GetInt32(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        Int32[] dst = new Int32[1];

        PsResult r = PsTryCopyValues(block, dst, 1, PsType.PS_INT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public Int64[] GetInt64Array(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsTryCountValues(block, out uint count);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        Int64[] dst = new Int64[count];

        r = PsTryCopyValues(block, dst, count, PsType.PS_INT64);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public Int64 GetInt64(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        Int64[] dst = new Int64[1];

        PsResult r = PsTryCopyValues(block, dst, 1, PsType.PS_INT64);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public float[] GetFloat32Array(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsTryCountValues(block, out uint count);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        float[] dst = new float[count];

        r = PsTryCopyValues(block, dst, count, PsType.PS_FLOAT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public float GetFloat32(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        float[] dst = new float[1];

        PsResult r = PsTryCopyValues(block, dst, 1, PsType.PS_FLOAT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public double[] GetFloat64Array(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        PsResult r = PsTryCountValues(block, out uint count);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        double[] dst = new double[count];

        r = PsTryCopyValues(block, dst, count, PsType.PS_FLOAT64);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public double GetFloat64(string key)
    {
        IntPtr block = PsGetBlock(m_podSerializer, Encoding.ASCII.GetBytes(key));

        if (block == null)
        {
            throw new NullReferenceException();
        }

        double[] dst = new double[1];

        PsResult r = PsTryCopyValues(block, dst, 1, PsType.PS_FLOAT64);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    ~PODstore()
    {
        TryDispose();
    }

    protected virtual void TryDispose()
    {
        if (!m_disposed)
        {
            PsDeleteSerializer(m_podSerializer);
            m_disposed = true;
        }
    }

    protected IntPtr m_podSerializer;

    protected bool m_disposed = false;

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
    public enum           PsEndian : uint
    {
        PS_ENDIAN_LITTLE          = 0u,          // Save the file in little endian format
        PS_ENDIAN_BIG             = 1u,          // Save the file in big endian format
        PS_ENDIAN_NATIVE          = 2u,          // Save the file in the endianness of the host
    };

    // Checksum Type
    public enum           PsChecksum : uint
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
        byte[]                             fileName,        // File name
        PsChecksum                         checksum,        // Checksum type
        uint                               checksumValue);  // Initial checksum value

    // Save a file using data stored in the serializer
    // If checksum is NONE, then checksumValue isn't used.
    // If checksum is not NONE, then checksumValue must be
    // used again to load the file.
    [DllImport("libPODstore", EntryPoint = "psSaveFile", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSaveFile(
        IntPtr                             serializer,      // Handle to a valid PsSerializer
        byte[]                             fileName,        // File name
        PsChecksum                         checksum,        // Checksum type
        uint                               checksumValue,   // Initial checksum value
        PsEndian                           endianness);

    // Get a block of data from the serializer using its key
    // If a block doesn't exist, then it will be created
    // returns nullptr only if the key size exceeds available memory
    [DllImport("libPODstore", EntryPoint = "psGetBlock", CallingConvention = CallingConvention.Cdecl)]
    protected static extern IntPtr      PsGetBlock(
        IntPtr                             serializer,      // Handle to a valid PsSerializer
        byte[]                             key);            // Null-terminated key

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

    // Set the values in a block (uint8)
    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr block, byte[] srcValueArray, uint valueCount, PsType  valueType);

    // Set the values in a block (uint16)
    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr block, UInt16[] srcValueArray, uint valueCount, PsType  valueType);

    // Set the values in a block (uint32)
    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr block, UInt32[] srcValueArray, uint valueCount, PsType  valueType);

    // Set the values in a block (uint64)
    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr block, UInt64[] srcValueArray, uint valueCount, PsType  valueType);

    // Set the values in a block (int8)
    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr block, sbyte[] srcValueArray, uint valueCount, PsType  valueType);

    // Set the values in a block (int16)
    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr block, Int16[] srcValueArray, uint valueCount, PsType  valueType);

    // Set the values in a block (int32)
    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr block, Int32[] srcValueArray, uint valueCount, PsType  valueType);

    // Set the values in a block (int64)
    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr block, Int64[] srcValueArray, uint valueCount, PsType  valueType);

    // Set the values in a block (float32)
    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr block, float[] srcValueArray, uint valueCount, PsType  valueType);

    // Set the values in a block (float64)
    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr block, double[] srcValueArray, uint valueCount, PsType  valueType);

    // Copy the values from a block into a destination array (uint8)
    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr block, byte[] dstValueArray, uint valueCount, PsType type);

    // Copy the values from a block into a destination array (uint16)
    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr block, UInt16[] dstValueArray, uint valueCount, PsType type);

    // Copy the values from a block into a destination array (uint32)
    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr block, UInt32[] dstValueArray, uint valueCount, PsType type);

    // Copy the values from a block into a destination array (uint64)
    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr block, UInt64[] dstValueArray, uint valueCount, PsType type);

    // Copy the values from a block into a destination array (int8)
    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr block, sbyte[] dstValueArray, uint valueCount, PsType type);

    // Copy the values from a block into a destination array (int16)
    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr block, Int16[] dstValueArray, uint valueCount, PsType type);

    // Copy the values from a block into a destination array (int32)
    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr block, Int32[] dstValueArray, uint valueCount, PsType type);

    // Copy the values from a block into a destination array (int64)
    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr block, Int64[] dstValueArray, uint valueCount, PsType type);

    // Copy the values from a block into a destination array (float)
    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr block, float[] dstValueArray, uint valueCount, PsType type);

    // Copy the values from a block into a destination array (double)
    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr block, double[] dstValueArray, uint valueCount, PsType type);
}
