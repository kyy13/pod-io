// pod-index
// Kyle J Burgess

using System;
using System.Runtime.InteropServices;
using System.Text;

public enum              PxCompression : UInt32
{
    PX_COMPRESSION_0          = 0u,                     // No compression (largest size)
    PX_COMPRESSION_1          = 1u,                     // Least compression
    PX_COMPRESSION_2          = 2u,
    PX_COMPRESSION_3          = 3u,
    PX_COMPRESSION_4          = 4u,
    PX_COMPRESSION_5          = 5u,
    PX_COMPRESSION_6          = 6u,
    PX_COMPRESSION_7          = 7u,
    PX_COMPRESSION_8          = 8u,
    PX_COMPRESSION_9          = 9u,                     // Best compression (smallest size)
    PX_COMPRESSION_NONE       = PX_COMPRESSION_0,
    PX_COMPRESSION_DEFAULT    = PX_COMPRESSION_6,
    PX_COMPRESSION_BEST       = PX_COMPRESSION_9,
};

// Endianness
public enum              PxEndian : UInt32
{
    PX_ENDIAN_LITTLE          = 0u,                     // Save the file in little endian format
    PX_ENDIAN_BIG             = 1u,                     // Save the file in big endian format
    PX_ENDIAN_NATIVE          = 2u,                     // Save the file in the endianness of the host
};

// Checksum Type
public enum              PxChecksum : UInt32
{
    PX_CHECKSUM_NONE          = 0u,                     // Read/write a file with no checksum
    PX_CHECKSUM_ADLER32       = 1u,                     // Read/write a file with an adler32 checksum
    PX_CHECKSUM_CRC32         = 2u,                     // Read/write a file with a crc32 checksum
};

public class PxContainer : IDisposable
{
    public PxContainer()
    {
        m_container = PxCreateContainer();
    }

    public void Dispose()
    {
        TryDispose();
        GC.SuppressFinalize(this);
    }

    public void Load(string fileName, PxChecksum checksum, UInt32 checksumValue = 0)
    {
        PxResult r = PxLoadFile(m_container, Encoding.ASCII.GetBytes(fileName + '\0'), checksum, checksumValue);

        if (r != PxResult.PX_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void Save(string fileName, PxCompression compression, PxChecksum checksum, UInt32 checksumValue = 0, PxEndian endianness = PxEndian.PX_ENDIAN_NATIVE)
    {
        PxResult r = PxSaveFile(m_container, Encoding.ASCII.GetBytes(fileName + '\0'), compression, checksum, checksumValue, endianness);

        if (r != PxResult.PX_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public IntPtr GetItem(string key)
    {
        return PxGetItem(m_container, Encoding.ASCII.GetBytes(key + '\0'));
    }

    public IntPtr TryGetItem(string key)
    {
        return PxTryGetItem(m_container, Encoding.ASCII.GetBytes(key + '\0'));
    }

    public void RemoveItem(IntPtr item)
    {
        if (item == null)
        {
            return;
        }

        PxResult r = PxRemoveItem(m_container, item);

        if (r != PxResult.PX_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void RemoveItem(string key)
    {
        RemoveItem(TryGetItem(key));
    }

    public UInt32? TryCountValues(IntPtr item)
    {
        if (item == null)
        {
            return null;
        }

        PxResult r = PxTryCountValues(item, out UInt32 count);

        if (count == 0)
        {
            return null;
        }

        if (r != PxResult.PX_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return count;
    }

    public UInt32? TryCountValues(string key)
    {
        return TryCountValues(GetItem(key));
    }

    public UInt32? TryCountKeyChars(IntPtr item)
    {
        if (item == null)
        {
            return null;
        }

        PxResult r = PxTryCountKeyChars(item, out UInt32 count);

        if (count == 0)
        {
            return null;
        }

        if (r != PxResult.PX_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return count;
    }

    public string TryGetKey(IntPtr item)
    {
        UInt32? count = TryCountKeyChars(item);

        if (count == null)
        {
            return null;
        }

        byte[] dst = new byte[(int)count];

        PxResult r = PxTryCopyKey(item, dst, count.Value);

        if (r != PxResult.PX_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return Encoding.ASCII.GetString(dst);
    }

    public IntPtr GetFirstItem()
    {
        return PxGetFirstItem(m_container);
    }

    public IntPtr GetNextItem(IntPtr item)
    {
        return PxGetNextItem(m_container, item);
    }

    // ASCII

    public void SetAsciiString(IntPtr item, string str)
    {
        var bytes = Encoding.ASCII.GetBytes(str);

        PxResult r = PxSetValues(item, bytes, (UInt32)bytes.Length, PxType.PX_ASCII_CHAR8);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        byte[] dst = new byte[(int)count];

        PxResult r = PxTryCopyValues(item, dst, count.Value, PxType.PX_ASCII_CHAR8);

        if (r != PxResult.PX_SUCCESS)
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

        PxResult r = PxSetValues(item, bytes, (UInt32)bytes.Length, PxType.PX_UTF8_CHAR8);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        byte[] dst = new byte[(int)count];

        PxResult r = PxTryCopyValues(item, dst, count.Value, PxType.PX_UTF8_CHAR8);

        if (r != PxResult.PX_SUCCESS)
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
        PxResult r = PxSetValues(item, values, (UInt32)values.Length, PxType.PX_UINT8);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        byte[] dst = new byte[(int)count];

        PxResult r = PxTryCopyValues(item, dst, count.Value, PxType.PX_UINT8);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PxResult.PX_TYPE_MISMATCH.ToString());
        }

        byte[] dst = new byte[1u];

        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_UINT8);

        if (r != PxResult.PX_SUCCESS)
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
        PxResult r = PxSetValues(item, values, (UInt32)values.Length, PxType.PX_UINT16);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        UInt16[] dst = new UInt16[(int)count];

        PxResult r = PxTryCopyValues(item, dst, count.Value, PxType.PX_UINT16);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PxResult.PX_TYPE_MISMATCH.ToString());
        }

        UInt16[] dst = new UInt16[1u];

        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_UINT16);

        if (r != PxResult.PX_SUCCESS)
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
        PxResult r = PxSetValues(item, values, (UInt32)values.Length, PxType.PX_UINT32);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        UInt32[] dst = new UInt32[(int)count];

        PxResult r = PxTryCopyValues(item, dst, count.Value, PxType.PX_UINT32);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PxResult.PX_TYPE_MISMATCH.ToString());
        }

        UInt32[] dst = new UInt32[1u];

        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_UINT32);

        if (r != PxResult.PX_SUCCESS)
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
        PxResult r = PxSetValues(item, values, (UInt32)values.Length, PxType.PX_UINT64);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        UInt64[] dst = new UInt64[(int)count];

        PxResult r = PxTryCopyValues(item, dst, count.Value, PxType.PX_UINT64);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PxResult.PX_TYPE_MISMATCH.ToString());
        }

        UInt64[] dst = new UInt64[1u];

        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_UINT64);

        if (r != PxResult.PX_SUCCESS)
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
        PxResult r = PxSetValues(item, values, (UInt32)values.Length, PxType.PX_INT8);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        sbyte[] dst = new sbyte[(int)count];

        PxResult r = PxTryCopyValues(item, dst, count.Value, PxType.PX_INT8);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PxResult.PX_TYPE_MISMATCH.ToString());
        }

        sbyte[] dst = new sbyte[1u];

        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_INT8);

        if (r != PxResult.PX_SUCCESS)
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
        PxResult r = PxSetValues(item, values, (UInt32)values.Length, PxType.PX_INT16);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        Int16[] dst = new Int16[(int)count];

        PxResult r = PxTryCopyValues(item, dst, count.Value, PxType.PX_INT16);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PxResult.PX_TYPE_MISMATCH.ToString());
        }

        Int16[] dst = new Int16[1u];

        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_INT16);

        if (r != PxResult.PX_SUCCESS)
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
        PxResult r = PxSetValues(item, values, (UInt32)values.Length, PxType.PX_INT32);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        Int32[] dst = new Int32[(int)count];

        PxResult r = PxTryCopyValues(item, dst, count.Value, PxType.PX_INT32);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PxResult.PX_TYPE_MISMATCH.ToString());
        }

        Int32[] dst = new Int32[1u];

        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_INT32);

        if (r != PxResult.PX_SUCCESS)
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
        PxResult r = PxSetValues(item, values, (UInt32)values.Length, PxType.PX_INT64);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        Int64[] dst = new Int64[(int)count];

        PxResult r = PxTryCopyValues(item, dst, count.Value, PxType.PX_INT64);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PxResult.PX_TYPE_MISMATCH.ToString());
        }

        Int64[] dst = new Int64[1u];

        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_INT64);

        if (r != PxResult.PX_SUCCESS)
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
        PxResult r = PxSetValues(item, values, (UInt32)values.Length, PxType.PX_FLOAT32);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        float[] dst = new float[(int)count];

        PxResult r = PxTryCopyValues(item, dst, count.Value, PxType.PX_FLOAT32);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PxResult.PX_TYPE_MISMATCH.ToString());
        }

        float[] dst = new float[1u];

        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_FLOAT32);

        if (r != PxResult.PX_SUCCESS)
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
        PxResult r = PxSetValues(item, values, (UInt32)values.Length, PxType.PX_FLOAT32);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        double[] dst = new double[(int)count];

        PxResult r = PxTryCopyValues(item, dst, count.Value, PxType.PX_FLOAT64);

        if (r != PxResult.PX_SUCCESS)
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
        UInt32? count = TryCountValues(item);

        if (count == null)
        {
            return null;
        }

        if (count != 1u)
        {
            throw new Exception(PxResult.PX_TYPE_MISMATCH.ToString());
        }

        double[] dst = new double[1u];

        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_FLOAT64);

        if (r != PxResult.PX_SUCCESS)
        {
            throw new Exception(r.ToString());
        }

        return dst[0];
    }

    public double? GetFloat64(string key)
    {
        return GetFloat64(TryGetItem(key));
    }

    ~PxContainer()
    {
        TryDispose();
    }

    protected virtual void TryDispose()
    {
        if (!m_disposed)
        {
            PxDeleteContainer(m_container);
            m_disposed = true;
        }
    }

    protected IntPtr m_container;

    protected bool m_disposed = false;

    // -------------------------------------------
    // PODstore C++ Library Methods and Structs
    // -------------------------------------------

    // Result of PODstore functions
    protected enum           PxResult : UInt32
    {
        PX_SUCCESS                = 0u,                     // Success
        PX_NULL_REFERENCE         = 1u,                     // Tried to pass null for an item or container
        PX_TYPE_MISMATCH          = 2u,                     // Tried to get a type that does not match the stored type
        PX_OUT_OF_RANGE           = 3u,                     // Tried to copy values out of the range of the stored buffer
        PX_FILE_CORRUPT           = 4u,                     // Save file is corrupt
        PX_FILE_NOT_FOUND         = 5u,                     // Unable to open file for read or write
        PX_ARGUMENT_ERROR         = 6u,                     // Provided an incorrect argument to a method
        PX_ZLIB_ERROR             = 7u,                     // Error during zlib initialization
    };

    // Types of Data
    protected enum           PxType : UInt32
    {
        PX_ASCII_CHAR8            = 0x02000001u,            // 8-bit ASCII character
        PX_UTF8_CHAR8             = 0x03000001u,            // 8-bit UTF8 bytes
        PX_UINT8                  = 0x00000001u,            // 8-bit unsigned integer
        PX_UINT16                 = 0x00000002u,            // 16-bit unsigned integer
        PX_UINT32                 = 0x00000004u,            // 32-bit unsigned integer
        PX_UINT64                 = 0x00000008u,            // 64-bit unsigned integer
        PX_INT8                   = 0x00010001u,            // 8-bit signed twos-complement integer
        PX_INT16                  = 0x00010002u,            // 16-bit signed twos-complement integer
        PX_INT32                  = 0x00010004u,            // 32-bit signed twos-complement integer
        PX_INT64                  = 0x00010008u,            // 64-bit signed twos-complement integer
        PX_FLOAT32                = 0x01010004u,            // 32-bit IEEE floating point number
        PX_FLOAT64                = 0x01010008u,            // 64-bit IEEE floating point number
    };

    // See PODstore.h for DLL documentation

    [DllImport("libpodindex", EntryPoint = "pxCreateContainer", CallingConvention = CallingConvention.Cdecl)]
    protected static extern IntPtr      PxCreateContainer();

    [DllImport("libpodindex", EntryPoint = "pxDeleteContainer", CallingConvention = CallingConvention.Cdecl)]
    protected static extern void        PxDeleteContainer(IntPtr container);

    [DllImport("libpodindex", EntryPoint = "pxLoadFile", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxLoadFile(IntPtr container, byte[] fileName, PxChecksum checksum, UInt32 checksumValue);

    [DllImport("libpodindex", EntryPoint = "pxSaveFile", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxSaveFile(IntPtr container, byte[] fileName, PxCompression compression, PxChecksum checksum, UInt32 checksumValue, PxEndian endianness);

    [DllImport("libpodindex", EntryPoint = "pxGetItem", CallingConvention = CallingConvention.Cdecl)]
    protected static extern IntPtr      PxGetItem(IntPtr container, byte[] key);

    [DllImport("libpodindex", EntryPoint = "pxTryGetItem", CallingConvention = CallingConvention.Cdecl)]
    protected static extern IntPtr      PxTryGetItem(IntPtr container, byte[] key);

    [DllImport("libpodindex", EntryPoint = "pxRemoveItem", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxRemoveItem(IntPtr container, IntPtr item);

    [DllImport("libpodindex", EntryPoint = "pxTryCountValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxTryCountValues(IntPtr item, out UInt32 valueCount);

    [DllImport("libpodindex", EntryPoint = "pxTryGetType", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxTryGetType(IntPtr item, out PxType valueType);

    [DllImport("libpodindex", EntryPoint = "pxTryCountKeyChars", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxTryCountKeyChars(IntPtr item, out UInt32 charCount);

    [DllImport("libpodindex", EntryPoint = "pxTryCopyKey", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxTryCopyKey(IntPtr item, byte[] key, UInt32 count);

    [DllImport("libpodindex", EntryPoint = "pxGetFirstItem", CallingConvention = CallingConvention.Cdecl)]
    protected static extern IntPtr      PxGetFirstItem(IntPtr container);

    [DllImport("libpodindex", EntryPoint = "pxGetNextItem", CallingConvention = CallingConvention.Cdecl)]
    protected static extern IntPtr      PxGetNextItem(IntPtr container, IntPtr item);

    [DllImport("libpodindex", EntryPoint = "pxSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxSetValues(IntPtr item, byte[] srcValueArray, UInt32 valueCount, PxType valueType);

    [DllImport("libpodindex", EntryPoint = "pxSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxSetValues(IntPtr item, UInt16[] srcValueArray, UInt32 valueCount, PxType valueType);

    [DllImport("libpodindex", EntryPoint = "pxSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxSetValues(IntPtr item, UInt32[] srcValueArray, UInt32 valueCount, PxType valueType);

    [DllImport("libpodindex", EntryPoint = "pxSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxSetValues(IntPtr item, UInt64[] srcValueArray, UInt32 valueCount, PxType valueType);

    [DllImport("libpodindex", EntryPoint = "pxSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxSetValues(IntPtr item, sbyte[] srcValueArray, UInt32 valueCount, PxType valueType);

    [DllImport("libpodindex", EntryPoint = "pxSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxSetValues(IntPtr item, Int16[] srcValueArray, UInt32 valueCount, PxType valueType);

    [DllImport("libpodindex", EntryPoint = "pxSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxSetValues(IntPtr item, Int32[] srcValueArray, UInt32 valueCount, PxType valueType);

    [DllImport("libpodindex", EntryPoint = "pxSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxSetValues(IntPtr item, Int64[] srcValueArray, UInt32 valueCount, PxType valueType);

    [DllImport("libpodindex", EntryPoint = "pxSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxSetValues(IntPtr item, float[] srcValueArray, UInt32 valueCount, PxType valueType);

    [DllImport("libpodindex", EntryPoint = "pxSetValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxSetValues(IntPtr item, double[] srcValueArray, UInt32 valueCount, PxType valueType);

    [DllImport("libpodindex", EntryPoint = "pxTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxTryCopyValues(IntPtr item, byte[] dstValueArray, UInt32 valueCount, PxType type);

    [DllImport("libpodindex", EntryPoint = "pxTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxTryCopyValues(IntPtr item, UInt16[] dstValueArray, UInt32 valueCount, PxType type);

    [DllImport("libpodindex", EntryPoint = "pxTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxTryCopyValues(IntPtr item, UInt32[] dstValueArray, UInt32 valueCount, PxType type);

    [DllImport("libpodindex", EntryPoint = "pxTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxTryCopyValues(IntPtr item, UInt64[] dstValueArray, UInt32 valueCount, PxType type);

    [DllImport("libpodindex", EntryPoint = "pxTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxTryCopyValues(IntPtr item, sbyte[] dstValueArray, UInt32 valueCount, PxType type);

    [DllImport("libpodindex", EntryPoint = "pxTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxTryCopyValues(IntPtr item, Int16[] dstValueArray, UInt32 valueCount, PxType type);

    [DllImport("libpodindex", EntryPoint = "pxTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxTryCopyValues(IntPtr item, Int32[] dstValueArray, UInt32 valueCount, PxType type);

    [DllImport("libpodindex", EntryPoint = "pxTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxTryCopyValues(IntPtr item, Int64[] dstValueArray, UInt32 valueCount, PxType type);

    [DllImport("libpodindex", EntryPoint = "pxTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxTryCopyValues(IntPtr item, float[] dstValueArray, UInt32 valueCount, PxType type);

    [DllImport("libpodindex", EntryPoint = "pxTryCopyValues", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PxResult    PxTryCopyValues(IntPtr item, double[] dstValueArray, UInt32 valueCount, PxType type);
}
