// PODstore
// Kyle J Burgess

using System;
using System.Runtime.InteropServices;
using System.Text;

public class PsContainer : IDisposable
{
    public PsContainer()
    {
        m_container = PsCreateContainer();
    }

    public void Dispose()
    {
        TryDispose();
        GC.SuppressFinalize(this);
    }

    public void Load(string fileName, PsChecksum checksum, uint checksumValue = 0)
    {
        PsResult r = PsLoadFile(m_container, Encoding.ASCII.GetBytes(fileName + '\0'), checksum, checksumValue);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void Save(string fileName, PsCompression compression, PsChecksum checksum, uint checksumValue = 0, PsEndian endianness = PsEndian.PS_ENDIAN_NATIVE)
    {
        PsResult r = PsSaveFile(m_container, Encoding.ASCII.GetBytes(fileName + '\0'), compression, checksum, checksumValue, endianness);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public IntPtr GetItem(string key)
    {
        return PsGetItem(m_container, Encoding.ASCII.GetBytes(key + '\0'));
    }

    public IntPtr TryGetItem(string key)
    {
        return PsTryGetItem(m_container, Encoding.ASCII.GetBytes(key + '\0'));
    }

    public void RemoveItem(IntPtr item)
    {
        if (item == null)
        {
            return;
        }

        PsResult r = PsRemoveItem(m_container, item);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void RemoveItem(string key)
    {
        RemoveItem(TryGetItem(key));
    }

    public uint? TryCountValues(IntPtr item)
    {
        if (item == null)
        {
            return null;
        }

        PsResult r = PsTryCountValues(item, out uint count);

        if (count == 0)
        {
            return null;
        }

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return count;
    }

    public uint? TryCountValues(string key)
    {
        return TryCountValues(GetItem(key));
    }

    public uint? TryCountKeyChars(IntPtr item)
    {
        if (item == null)
        {
            return null;
        }

        PsResult r = PsTryCountKeyChars(item, out uint count);

        if (count == 0)
        {
            return null;
        }

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return count;
    }

    public string TryGetKey(IntPtr item)
    {
        uint? count = PsTryCountKeyChars(item);

        if (count == null)
        {
            return null;
        }

        byte[] dst = new byte[count];

        r = PsTryCopyKey(item, dst, count);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return Encoding.ASCII.GetString(dst);
    }

    public IntPtr GetFirstItem()
    {
        return PsGetFirstItem(m_container);
    }

    public IntPtr GetNextItem(IntPtr item)
    {
        return PsGetNextItem(m_container, item);
    }

    // ASCII

    public void SetAsciiString(IntPtr item, string str)
    {
        var bytes = Encoding.ASCII.GetBytes(str);

        PsResult r = PsSetValues(item, bytes, (uint)bytes.Length, PsType.PS_ASCII_CHAR8);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetAsciiString(string key, string str)
    {
        SetAsciiString(GetItem(key), str);
    }

    public string GetAsciiString(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        byte[] dst = new byte[count];

        r = PsTryCopyValues(item, dst, count, PsType.PS_ASCII_CHAR8);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return Encoding.ASCII.GetString(dst);
    }

    public string GetAsciiString(string key)
    {
        return GetAsciiString(TryGetItem(key));
    }

    // UTF8

    public void SetUtf8String(IntPtr item, string str)
    {
        var bytes = Encoding.UTF8.GetBytes(str);

        PsResult r = PsSetValues(item, bytes, (uint)bytes.Length, PsType.PS_UTF8_CHAR8);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetUtf8String(string key, string str)
    {
        SetUtf8String(GetItem(key), str);
    }

    public string GetUtf8String(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        byte[] dst = new byte[count];

        r = PsTryCopyValues(item, dst, count, PsType.PS_UTF8_CHAR8);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return Encoding.UTF8.GetString(dst);
    }

    public string GetUtf8String(string key)
    {
        return GetUtf8String(TryGetItem(key));
    }

    // UInt8

    public void SetUInt8Array(IntPtr item, byte[] values)
    {
        PsResult r = PsSetValues(item, values, (uint)values.Length, PsType.PS_UINT8);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetUInt8Array(string key, byte[] values)
    {
        SetUInt8Array(GetItem(key), values);
    }

    public void SetUInt8(IntPtr item, byte value)
    {
        SetUInt8Array(item, new byte[]{ value });
    }

    public void SetUInt8(string key, byte value)
    {
        SetUInt8Array(key, new byte[]{ value });
    }

    public byte[] GetUInt8Array(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        byte[] dst = new byte[count];

        r = PsTryCopyValues(item, dst, count, PsType.PS_UINT8);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public byte[] GetUInt8Array(string key)
    {
        return GetUInt8Array(TryGetItem(key));
    }

    public byte? GetUInt8(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PsResult.PS_TYPE_MISMATCH.ToString());
        }

        byte[] dst = new byte[1u];

        r = PsTryCopyValues(item, dst, 1u, PsType.PS_UINT8);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public byte? GetUInt8(string key)
    {
        return GetUInt8(TryGetItem(key));
    }

    // UInt16

    public void SetUInt16Array(IntPtr item, UInt16[] values)
    {
        PsResult r = PsSetValues(item, values, (uint)values.Length, PsType.PS_UINT16);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetUInt16Array(string key, UInt16[] values)
    {
        SetUInt16Array(GetItem(key), values);
    }

    public void SetUInt16(IntPtr item, UInt16 value)
    {
        SetUInt16Array(item, new UInt16[]{ value });
    }

    public void SetUInt16(string key, UInt16 value)
    {
        SetUInt16Array(key, new UInt16[]{ value });
    }

    public UInt16[] GetUInt16Array(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        UInt16[] dst = new UInt16[count];

        r = PsTryCopyValues(item, dst, count, PsType.PS_UINT16);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public UInt16[] GetUInt16Array(string key)
    {
        return GetUInt16Array(TryGetItem(key));
    }

    public UInt16? GetUInt16(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PsResult.PS_TYPE_MISMATCH.ToString());
        }

        UInt16[] dst = new UInt16[1u];

        r = PsTryCopyValues(item, dst, 1u, PsType.PS_UINT16);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public UInt16? GetUInt16(string key)
    {
        return GetUInt16(TryGetItem(key));
    }

    // UInt32

    public void SetUInt32Array(IntPtr item, UInt32[] values)
    {
        PsResult r = PsSetValues(item, values, (uint)values.Length, PsType.PS_UINT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetUInt32Array(string key, UInt32[] values)
    {
        SetUInt32Array(GetItem(key), values);
    }

    public void SetUInt32(IntPtr item, UInt32 value)
    {
        SetUInt32Array(item, new UInt32[]{ value });
    }

    public void SetUInt32(string key, UInt32 value)
    {
        SetUInt32Array(key, new UInt32[]{ value });
    }

    public UInt32[] GetUInt32Array(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        UInt32[] dst = new UInt32[count];

        r = PsTryCopyValues(item, dst, count, PsType.PS_UINT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public UInt32[] GetUInt32Array(string key)
    {
        return GetUInt32Array(TryGetItem(key));
    }

    public UInt32? GetUInt32(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PsResult.PS_TYPE_MISMATCH.ToString());
        }

        UInt32[] dst = new UInt32[1u];

        r = PsTryCopyValues(item, dst, 1u, PsType.PS_UINT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public UInt32? GetUInt32(string key)
    {
        return GetUInt32(TryGetItem(key));
    }

    // UInt64

    public void SetUInt64Array(IntPtr item, UInt64[] values)
    {
        PsResult r = PsSetValues(item, values, (uint)values.Length, PsType.PS_UINT64);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetUInt64Array(string key, UInt64[] values)
    {
        SetUInt64Array(GetItem(key), values);
    }

    public void SetUInt64(IntPtr item, UInt64 value)
    {
        SetUInt64Array(item, new UInt64[]{ value });
    }

    public void SetUInt64(string key, UInt64 value)
    {
        SetUInt64Array(key, new UInt64[]{ value });
    }
    
    public UInt64[] GetUInt64Array(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        UInt64[] dst = new UInt64[count];

        r = PsTryCopyValues(item, dst, count, PsType.PS_UINT64);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public UInt64[] GetUInt64Array(string key)
    {
        return GetUInt64Array(TryGetItem(key));
    }

    public UInt64? GetUInt64(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PsResult.PS_TYPE_MISMATCH.ToString());
        }

        UInt64[] dst = new UInt64[1u];

        r = PsTryCopyValues(item, dst, 1u, PsType.PS_UINT64);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public UInt64? GetUInt64(string key)
    {
        return GetUInt64(TryGetItem(key));
    }

    // Int8

    public void SetInt8Array(IntPtr item, sbyte[] values)
    {
        PsResult r = PsSetValues(item, values, (uint)values.Length, PsType.PS_INT8);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetInt8Array(string key, sbyte[] values)
    {
        SetInt8Array(GetItem(key), values);
    }

    public void SetInt8(IntPtr item, sbyte value)
    {
        SetInt8Array(item, new sbyte[]{ value });
    }

    public void SetInt8(string key, sbyte value)
    {
        SetInt8Array(key, new sbyte[]{ value });
    }
    
    public sbyte[] GetInt8Array(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        sbyte[] dst = new sbyte[count];

        r = PsTryCopyValues(item, dst, count, PsType.PS_INT8);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public sbyte[] GetInt8Array(string key)
    {
        return GetInt8Array(TryGetItem(key));
    }

    public sbyte? GetInt8(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PsResult.PS_TYPE_MISMATCH.ToString());
        }

        sbyte[] dst = new sbyte[1u];

        r = PsTryCopyValues(item, dst, 1u, PsType.PS_INT8);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public sbyte? GetInt8(string key)
    {
        return GetInt8(TryGetItem(key));
    }

    // Int16

    public void SetInt16Array(IntPtr item, Int16[] values)
    {
        PsResult r = PsSetValues(item, values, (uint)values.Length, PsType.PS_INT16);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetInt16Array(string key, Int16[] values)
    {
        SetInt16Array(GetItem(key), values);
    }

    public void SetInt16(IntPtr item, Int16 value)
    {
        SetInt16Array(item, new Int16[]{ value });
    }

    public void SetInt16(string key, Int16 value)
    {
        SetInt16Array(key, new Int16[]{ value });
    }
    
    public Int16[] GetInt16Array(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        Int16[] dst = new Int16[count];

        r = PsTryCopyValues(item, dst, count, PsType.PS_INT16);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public Int16[] GetInt16Array(string key)
    {
        return GetInt16Array(TryGetItem(key));
    }

    public Int16? GetInt16(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PsResult.PS_TYPE_MISMATCH.ToString());
        }

        Int16[] dst = new Int16[1u];

        r = PsTryCopyValues(item, dst, 1u, PsType.PS_INT16);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public Int16? GetInt16(string key)
    {
        return GetInt16(TryGetItem(key));
    }

    // Int32

    public void SetInt32Array(IntPtr item, Int32[] values)
    {
        PsResult r = PsSetValues(item, values, (uint)values.Length, PsType.PS_INT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetInt32Array(string key, Int32[] values)
    {
        SetInt32Array(GetItem(key), values);
    }

    public void SetInt32(IntPtr item, Int32 value)
    {
        SetInt32Array(item, new Int32[]{ value });
    }

    public void SetInt32(string key, Int32 value)
    {
        SetInt32Array(key, new Int32[]{ value });
    }

    public Int32[] GetInt32Array(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        Int32[] dst = new Int32[count];

        r = PsTryCopyValues(item, dst, count, PsType.PS_INT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public Int32[] GetInt32Array(string key)
    {
        return GetInt32Array(TryGetItem(key));
    }

    public Int32? GetInt32(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PsResult.PS_TYPE_MISMATCH.ToString());
        }

        Int32[] dst = new Int32[1u];

        r = PsTryCopyValues(item, dst, 1u, PsType.PS_INT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public Int32? GetInt32(string key)
    {
        return GetInt32(TryGetItem(key));
    }

    // Int64

    public void SetInt64Array(IntPtr item, Int64[] values)
    {
        PsResult r = PsSetValues(item, values, (uint)values.Length, PsType.PS_INT64);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetInt64Array(string key, Int64[] values)
    {
        SetInt64Array(GetItem(key), values);
    }

    public void SetInt64(IntPtr item, Int64 value)
    {
        SetInt64Array(item, new Int64[]{ value });
    }

    public void SetInt64(string key, Int64 value)
    {
        SetInt64Array(key, new Int64[]{ value });
    }
    
    public Int64[] GetInt64Array(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        Int64[] dst = new Int64[count];

        r = PsTryCopyValues(item, dst, count, PsType.PS_INT64);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public Int64[] GetInt64Array(string key)
    {
        return GetInt64Array(TryGetItem(key));
    }

    public Int64? GetInt64(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PsResult.PS_TYPE_MISMATCH.ToString());
        }

        Int64[] dst = new Int64[1u];

        r = PsTryCopyValues(item, dst, 1u, PsType.PS_INT64);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public Int64? GetInt64(string key)
    {
        return GetInt64(TryGetItem(key));
    }

    // Float32

    public void SetFloat32Array(IntPtr item, float[] values)
    {
        PsResult r = PsSetValues(item, values, (uint)values.Length, PsType.PS_FLOAT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetFloat32Array(string key, float[] values)
    {
        SetFloat32Array(GetItem(key), values);
    }

    public void SetFloat32(IntPtr item, float value)
    {
        SetFloat32Array(item, new float[]{ value });
    }

    public void SetFloat32(string key, float value)
    {
        SetFloat32Array(key, new float[]{ value });
    }
    
    public float[] GetFloat32Array(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        float[] dst = new float[count];

        r = PsTryCopyValues(item, dst, count, PsType.PS_FLOAT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public float[] GetFloat32Array(string key)
    {
        return GetFloat32Array(TryGetItem(key));
    }

    public float? GetFloat32(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PsResult.PS_TYPE_MISMATCH.ToString());
        }

        float[] dst = new float[1u];

        r = PsTryCopyValues(item, dst, 1u, PsType.PS_FLOAT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public float? GetFloat32(string key)
    {
        return GetFloat32(TryGetItem(key));
    }

    // Float64

    public void SetFloat64Array(IntPtr item, double[] values)
    {
        PsResult r = PsSetValues(item, values, (uint)values.Length, PsType.PS_FLOAT32);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void SetFloat64Array(string key, double[] values)
    {
        SetFloat64Array(GetItem(key), values);
    }

    public void SetFloat64(IntPtr item, double value)
    {
        SetFloat64Array(item, new double[]{ value });
    }

    public void SetFloat64(string key, double value)
    {
        SetFloat64Array(key, new double[]{ value });
    }
    
    public double[] GetFloat64Array(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        double[] dst = new double[count];

        r = PsTryCopyValues(item, dst, count, PsType.PS_FLOAT64);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst;
    }

    public double[] GetFloat64Array(string key)
    {
        return GetFloat64Array(TryGetItem(key));
    }

    public double? GetFloat64(IntPtr item)
    {
        uint? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PsResult.PS_TYPE_MISMATCH.ToString());
        }

        double[] dst = new double[1u];

        r = PsTryCopyValues(item, dst, 1u, PsType.PS_FLOAT64);

        if (r != PsResult.PS_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public double? GetFloat64(string key)
    {
        return GetFloat64(TryGetItem(key));
    }

    ~PODstore()
    {
        TryDispose();
    }

    protected virtual void TryDispose()
    {
        if (!m_disposed)
        {
            PsDeleteContainer(m_container);
            m_disposed = true;
        }
    }

    protected IntPtr m_container;

    protected bool m_disposed = false;

    // -------------------------------------------
    // PODstore C++ Library Methods and Structs
    // -------------------------------------------

    // Result of PODstore functions
    protected enum           PsResult : uint
    {
        PS_SUCCESS                = 0u,                     // Success
        PS_NULL_REFERENCE         = 1u,                     // Tried to pass null for an item or container
        PS_TYPE_MISMATCH          = 2u,                     // Tried to get a type that does not match the stored type
        PS_OUT_OF_RANGE           = 3u,                     // Tried to copy values out of the range of the stored buffer
        PS_FILE_CORRUPT           = 4u,                     // Save file is corrupt
        PS_FILE_NOT_FOUND         = 5u,                     // Unable to open file for read or write
        PS_ARGUMENT_ERROR         = 6u,                     // Provided an incorrect argument to a method
        PS_ZLIB_ERROR             = 7u,                     // Error during zlib initialization
    };

    // Types of Data
    protected enum           PsType : uint
    {
        PS_ASCII_CHAR8            = 0x02000001u,            // 8-bit ASCII character
        PS_UTF8_CHAR8             = 0x03000001u,            // 8-bit UTF8 bytes
        PS_UINT8                  = 0x00000001u,            // 8-bit unsigned integer
        PS_UINT16                 = 0x00000002u,            // 16-bit unsigned integer
        PS_UINT32                 = 0x00000004u,            // 32-bit unsigned integer
        PS_UINT64                 = 0x00000008u,            // 64-bit unsigned integer
        PS_INT8                   = 0x00010001u,            // 8-bit signed twos-complement integer
        PS_INT16                  = 0x00010002u,            // 16-bit signed twos-complement integer
        PS_INT32                  = 0x00010004u,            // 32-bit signed twos-complement integer
        PS_INT64                  = 0x00010008u,            // 64-bit signed twos-complement integer
        PS_FLOAT32                = 0x01010004u,            // 32-bit IEEE floating point number
        PS_FLOAT64                = 0x01010008u,            // 64-bit IEEE floating point number
    };

    public enum              PsCompression : uint
    {
        PS_COMPRESSION_0          = 0u,                     // No compression (largest size)
        PS_COMPRESSION_1          = 1u,                     // Least compression
        PS_COMPRESSION_2          = 2u,
        PS_COMPRESSION_3          = 3u,
        PS_COMPRESSION_4          = 4u,
        PS_COMPRESSION_5          = 5u,
        PS_COMPRESSION_6          = 6u,
        PS_COMPRESSION_7          = 7u,
        PS_COMPRESSION_8          = 8u,
        PS_COMPRESSION_9          = 9u,                     // Best compression (smallest size)
        PS_COMPRESSION_NONE       = PS_COMPRESSION_0,
        PS_COMPRESSION_DEFAULT    = PS_COMPRESSION_6,
        PS_COMPRESSION_BEST       = PS_COMPRESSION_9,
    };

    // Endianness
    public enum              PsEndian : uint
    {
        PS_ENDIAN_LITTLE          = 0u,                     // Save the file in little endian format
        PS_ENDIAN_BIG             = 1u,                     // Save the file in big endian format
        PS_ENDIAN_NATIVE          = 2u,                     // Save the file in the endianness of the host
    };

    // Checksum Type
    public enum              PsChecksum : uint
    {
        PS_CHECKSUM_NONE          = 0u,                     // Read/write a file with no checksum
        PS_CHECKSUM_ADLER32       = 1u,                     // Read/write a file with an adler32 checksum
        PS_CHECKSUM_CRC32         = 2u,                     // Read/write a file with a crc32 checksum
    };

    // See PODstore.h for DLL documentation

    [DllImport("libPODstore", EntryPoint = "psCreateContainer", CallingConvention = CallingConvention.Cdecl)]
    protected static extern IntPtr      PsCreateContainer();

    [DllImport("libPODstore", EntryPoint = "psDeleteContainer", CallingConvention = CallingConvention.Cdecl)]
    protected static extern void        PsDeleteContainer(IntPtr container);

    [DllImport("libPODstore", EntryPoint = "psLoadFile", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsLoadFile(IntPtr container, byte[] fileName, PsChecksum checksum, uint checksumValue);

    [DllImport("libPODstore", EntryPoint = "psSaveFile", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSaveFile(IntPtr container, byte[] fileName, PsCompression compression, PsChecksum checksum, uint checksumValue, PsEndian endianness);

    [DllImport("libPODstore", EntryPoint = "psGetItem", CallingConvention = CallingConvention.Cdecl)]
    protected static extern IntPtr      PsGetItem(IntPtr container, byte[] key);

    [DllImport("libPODstore", EntryPoint = "psTryGetItem", CallingConvention = CallingConvention.Cdecl)]
    protected static extern IntPtr      PsTryGetItem(IntPtr container, byte[] key);

    [DllImport("libPODstore", EntryPoint = "psRemoveItem", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsRemoveItem(IntPtr container, IntPtr item);

    [DllImport("libPODstore", EntryPoint = "psTryCountValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCountValues(IntPtr item, out uint valueCount);

    [DllImport("libPODstore", EntryPoint = "psTryGetType", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryGetType(IntPtr item, out PsType valueType);

    [DllImport("libPODstore", EntryPoint = "psTryCountKeyChars", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCountKeyChars(IntPtr item, out uint charCount);

    [DllImport("libPODstore", EntryPoint = "psTryCopyKey", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyKey(IntPtr item, byte[] key, uint count);

    [DllImport("libPODstore", EntryPoint = "psGetFirstItem", CallingConvention = CallingConvention.Cdecl)]
    protected static extern IntPtr      PsGetFirstItem(IntPtr container);

    [DllImport("libPODstore", EntryPoint = "psGetNextItem", CallingConvention = CallingConvention.Cdecl)]
    protected static extern IntPtr      PsGetNextItem(IntPtr container, IntPtr item);

    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr item, byte[] srcValueArray, uint valueCount, PsType valueType);

    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr item, UInt16[] srcValueArray, uint valueCount, PsType valueType);

    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr item, UInt32[] srcValueArray, uint valueCount, PsType valueType);

    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr item, UInt64[] srcValueArray, uint valueCount, PsType valueType);

    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr item, sbyte[] srcValueArray, uint valueCount, PsType valueType);

    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr item, Int16[] srcValueArray, uint valueCount, PsType valueType);

    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr item, Int32[] srcValueArray, uint valueCount, PsType valueType);

    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr item, Int64[] srcValueArray, uint valueCount, PsType valueType);

    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr item, float[] srcValueArray, uint valueCount, PsType valueType);

    [DllImport("libPODstore", EntryPoint = "psSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsSetValues(IntPtr item, double[] srcValueArray, uint valueCount, PsType valueType);

    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr item, byte[] dstValueArray, uint valueCount, PsType type);

    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr item, UInt16[] dstValueArray, uint valueCount, PsType type);

    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr item, UInt32[] dstValueArray, uint valueCount, PsType type);

    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr item, UInt64[] dstValueArray, uint valueCount, PsType type);

    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr item, sbyte[] dstValueArray, uint valueCount, PsType type);

    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr item, Int16[] dstValueArray, uint valueCount, PsType type);

    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr item, Int32[] dstValueArray, uint valueCount, PsType type);

    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr item, Int64[] dstValueArray, uint valueCount, PsType type);

    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr item, float[] dstValueArray, uint valueCount, PsType type);

    [DllImport("libPODstore", EntryPoint = "psTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PsResult    PsTryCopyValues(IntPtr item, double[] dstValueArray, uint valueCount, PsType type);
}
