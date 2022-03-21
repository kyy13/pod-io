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

    public bool TryGetItem(string key, out IntPtr item)
    {
        IntPtr ptr = PxTryGetItem(m_container, Encoding.ASCII.GetBytes(key + '\0'));

        if (ptr == null)
        {
            item = IntPtr.Zero;
            return false;
        }

        item = ptr;
        return true;
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
        if (TryGetItem(key, out IntPtr item))
        {
            RemoveItem(item);
        }
    }

    public bool TryCountValues(IntPtr item, out UInt32 count)
    {
        if (item == null)
        {
            count = 0;
            return false;
        }

        PxResult r = PxTryCountValues(item, out count);

        if (r != PxResult.PX_SUCCESS)
        {
            return false;
        }

        if (count == 0)
        {
            return false;
        }

        return true;
    }

    public bool TryCountValues(string key, out UInt32 count)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            count = 0;
            return false;
        }

        return TryCountValues(item, out count);
    }

    public bool TryCountKeyChars(IntPtr item, out UInt32 count)
    {
        if (item == null)
        {
            count = 0;
            return false;
        }

        PxResult r = PxTryCountKeyChars(item, out count);

        if (count == 0)
        {
            return false;
        }

        if (r != PxResult.PX_SUCCESS)
        {
            return false;
        }

        return true;
    }

    public bool TryGetKey(IntPtr item, out string key)
    {
        UInt32 count;

        if (!TryCountKeyChars(item, out count))
        {
            key = null;
            return false;
        }

        byte[] dst = new byte[(int)count];

        PxResult r = PxTryCopyKey(item, dst, count);

        if (r != PxResult.PX_SUCCESS)
        {
            key = null;
            return false;
        }

        key = Encoding.ASCII.GetString(dst);
        return true;
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

    public bool TryGetAsciiString(IntPtr item, out string str)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            str = null;
            return false;
        }

        byte[] dst = new byte[(int)count];

        PxResult r = PxTryCopyValues(item, dst, count, PxType.PX_ASCII_CHAR8);

        if (r != PxResult.PX_SUCCESS)
        {
            str = null;
            return false;
        }

        str = Encoding.ASCII.GetString(dst);
        return true;
    }

    public bool TryGetAsciiString(string key, out string str)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            str = null;
            return false;
        }

        return TryGetAsciiString(item, out str);
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

    public bool TryGetUtf8String(IntPtr item, out string value)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            value = null;
            return false;
        }

        byte[] dst = new byte[(int)count];

        PxResult r = PxTryCopyValues(item, dst, count, PxType.PX_UTF8_CHAR8);

        if (r != PxResult.PX_SUCCESS)
        {
            value = null;
            return false;
        }

        value = Encoding.UTF8.GetString(dst);
        return true;
    }

    public bool TryGetUtf8String(string key, out string value)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            value = null;
            return false;
        }

        return TryGetUtf8String(item, out value);
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

    public bool TryGetUInt8Array(IntPtr item, out byte[] values)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            values = null;
            return false;
        }

        byte[] dst = new byte[count];
        PxResult r = PxTryCopyValues(item, dst, count, PxType.PX_UINT8);

        if (r != PxResult.PX_SUCCESS)
        {
            values = null;
            return false;
        }

        values = dst;
        return true;
    }

    public bool TryGetUInt8Array(string key, out byte[] values)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            values = null;
            return false;
        }

        return TryGetUInt8Array(item, out values);
    }

    public bool TryGetUInt8(IntPtr item, out byte value)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            value = 0;
            return false;
        }

        if (count != 1u)
        {
            value = 0;
            return false;
        }

        byte[] dst = new byte[1u];

        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_UINT8);

        if (r != PxResult.PX_SUCCESS)
        {
            value = 0;
            return false;
        }

        value = dst[0];
        return true;
    }

    public bool TryGetUInt8(string key, out byte value)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            value = 0;
            return false;
        }

        return TryGetUInt8(item, out value);
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

    public bool TryGetUInt16Array(IntPtr item, out UInt16[] values)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            values = null;
            return false;
        }

        UInt16[] dst = new UInt16[count];
        PxResult r = PxTryCopyValues(item, dst, count, PxType.PX_UINT16);

        if (r != PxResult.PX_SUCCESS)
        {
            values = null;
            return false;
        }

        values = dst;
        return true;
    }

    public bool TryGetUInt16Array(string key, out UInt16[] values)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            values = null;
            return false;
        }

        return TryGetUInt16Array(item, out values);
    }

    public bool TryGetUInt16(IntPtr item, out UInt16 value)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            value = 0;
            return false;
        }

        if (count != 1u)
        {
            value = 0;
            return false;
        }

        UInt16[] dst = new UInt16[1u];

        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_UINT16);

        if (r != PxResult.PX_SUCCESS)
        {
            value = 0;
            return false;
        }

        value = dst[0];
        return true;
    }

    public bool TryGetUInt16(string key, out UInt16 value)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            value = 0;
            return false;
        }

        return TryGetUInt16(item, out value);
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

    public bool TryGetUInt32Array(IntPtr item, out UInt32[] values)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            values = null;
            return false;
        }

        UInt32[] dst = new UInt32[count];
        PxResult r = PxTryCopyValues(item, dst, count, PxType.PX_UINT32);

        if (r != PxResult.PX_SUCCESS)
        {
            values = null;
            return false;
        }

        values = dst;
        return true;
    }

    public bool TryGetUInt32Array(string key, out UInt32[] values)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            values = null;
            return false;
        }

        return TryGetUInt32Array(item, out values);
    }

    public bool TryGetUInt32(IntPtr item, out UInt32 value)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            value = 0;
            return false;
        }

        if (count != 1u)
        {
            value = 0;
            return false;
        }

        UInt32[] dst = new UInt32[1u];
        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_UINT32);

        if (r != PxResult.PX_SUCCESS)
        {
            value = 0;
            return false;
        }

        value = dst[0];
        return true;
    }

    public bool TryGetUInt32(string key, out UInt32 value)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            value = 0;
            return false;
        }

        return TryGetUInt32(item, out value);
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

    public bool TryGetUInt64Array(IntPtr item, out UInt64[] values)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            values = null;
            return false;
        }

        UInt64[] dst = new UInt64[count];
        PxResult r = PxTryCopyValues(item, dst, count, PxType.PX_UINT64);

        if (r != PxResult.PX_SUCCESS)
        {
            values = null;
            return false;
        }

        values = dst;
        return true;
    }

    public bool TryGetUInt64Array(string key, out UInt64[] values)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            values = null;
            return false;
        }

        return TryGetUInt64Array(item, out values);
    }

    public bool TryGetUInt64(IntPtr item, out UInt64 value)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            value = 0;
            return false;
        }

        if (count != 1u)
        {
            value = 0;
            return false;
        }

        UInt64[] dst = new UInt64[1u];

        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_UINT64);

        if (r != PxResult.PX_SUCCESS)
        {
            value = 0;
            return false;
        }

        value = dst[0];
        return true;
    }

    public bool TryGetUInt64(string key, out UInt64 value)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            value = 0;
            return false;
        }

        return TryGetUInt64(item, out value);
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

    public bool TryGetInt8Array(IntPtr item, out sbyte[] values)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            values = null;
            return false;
        }

        sbyte[] dst = new sbyte[count];
        PxResult r = PxTryCopyValues(item, dst, count, PxType.PX_INT8);

        if (r != PxResult.PX_SUCCESS)
        {
            values = null;
            return false;
        }

        values = dst;
        return true;
    }

    public bool TryGetInt8Array(string key, out sbyte[] values)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            values = null;
            return false;
        }

        return TryGetInt8Array(item, out values);
    }

    public bool TryGetInt8(IntPtr item, out sbyte value)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            value = 0;
            return false;
        }

        if (count != 1u)
        {
            value = 0;
            return false;
        }

        sbyte[] dst = new sbyte[1u];

        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_INT8);

        if (r != PxResult.PX_SUCCESS)
        {
            value = 0;
            return false;
        }

        value = dst[0];
        return true;
    }

    public bool TryGetInt8(string key, out sbyte value)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            value = 0;
            return false;
        }

        return TryGetInt8(item, out value);
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

    public bool TryGetInt16Array(IntPtr item, out Int16[] values)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            values = null;
            return false;
        }

        Int16[] dst = new Int16[count];
        PxResult r = PxTryCopyValues(item, dst, count, PxType.PX_INT16);

        if (r != PxResult.PX_SUCCESS)
        {
            values = null;
            return false;
        }

        values = dst;
        return true;
    }

    public bool TryGetInt16Array(string key, out Int16[] values)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            values = null;
            return false;
        }

        return TryGetInt16Array(item, out values);
    }

    public bool TryGetInt16(IntPtr item, out Int16 value)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            value = 0;
            return false;
        }

        if (count != 1u)
        {
            value = 0;
            return false;
        }

        Int16[] dst = new Int16[1u];

        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_INT16);

        if (r != PxResult.PX_SUCCESS)
        {
            value = 0;
            return false;
        }

        value = dst[0];
        return true;
    }

    public bool TryGetInt16(string key, out Int16 value)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            value = 0;
            return false;
        }

        return TryGetInt16(item, out value);
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

    public bool TryGetInt32Array(IntPtr item, out Int32[] values)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            values = null;
            return false;
        }

        Int32[] dst = new Int32[count];
        PxResult r = PxTryCopyValues(item, dst, count, PxType.PX_INT32);

        if (r != PxResult.PX_SUCCESS)
        {
            values = null;
            return false;
        }

        values = dst;
        return true;
    }

    public bool TryGetInt32Array(string key, out Int32[] values)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            values = null;
            return false;
        }

        return TryGetInt32Array(item, out values);
    }

    public bool TryGetInt32(IntPtr item, out Int32 value)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            value = 0;
            return false;
        }

        if (count != 1u)
        {
            value = 0;
            return false;
        }

        Int32[] dst = new Int32[1u];
        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_INT32);

        if (r != PxResult.PX_SUCCESS)
        {
            value = 0;
            return false;
        }

        value = dst[0];
        return true;
    }

    public bool TryGetInt32(string key, out Int32 value)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            value = 0;
            return false;
        }

        return TryGetInt32(item, out value);
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

    public bool TryGetInt64Array(IntPtr item, out Int64[] values)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            values = null;
            return false;
        }

        Int64[] dst = new Int64[count];
        PxResult r = PxTryCopyValues(item, dst, count, PxType.PX_INT64);

        if (r != PxResult.PX_SUCCESS)
        {
            values = null;
            return false;
        }

        values = dst;
        return true;
    }

    public bool TryGetInt64Array(string key, out Int64[] values)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            values = null;
            return false;
        }

        return TryGetInt64Array(item, out values);
    }

    public bool TryGetInt64(IntPtr item, out Int64 value)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            value = 0;
            return false;
        }

        if (count != 1u)
        {
            value = 0;
            return false;
        }

        Int64[] dst = new Int64[1u];

        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_INT64);

        if (r != PxResult.PX_SUCCESS)
        {
            value = 0;
            return false;
        }

        value = dst[0];
        return true;
    }

    public bool TryGetInt64(string key, out Int64 value)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            value = 0;
            return false;
        }

        return TryGetInt64(item, out value);
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

    public bool TryGetFloat32Array(IntPtr item, out float[] values)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            values = null;
            return false;
        }

        float[] dst = new float[(int)count];
        PxResult r = PxTryCopyValues(item, dst, count, PxType.PX_FLOAT32);

        if (r != PxResult.PX_SUCCESS)
        {
            values = null;
            return false;
        }

        values = dst;
        return true;
    }

    public bool TryGetFloat32Array(string key, out float[] values)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            values = null;
            return false;
        }

        return TryGetFloat32Array(item, out values);
    }

    public bool TryGetFloat32(IntPtr item, out float value)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            value = 0;
            return false;
        }

        if (count != 1u)
        {
            value = 0;
            return false;
        }

        float[] dst = new float[1u];

        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_FLOAT32);

        if (r != PxResult.PX_SUCCESS)
        {
            value = 0;
            return false;
        }

        value = dst[0];
        return true;
    }

    public bool TryGetFloat32(string key, out float value)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            value = 0;
            return false;
        }

        return TryGetFloat32(item, out value);
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

    public bool TryGetFloat64Array(IntPtr item, out double[] values)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            values = null;
            return false;
        }

        double[] dst = new double[(int)count];
        PxResult r = PxTryCopyValues(item, dst, count, PxType.PX_FLOAT64);

        if (r != PxResult.PX_SUCCESS)
        {
            values = null;
            return false;
        }

        values = dst;
        return true;
    }

    public bool TryGetFloat64Array(string key, out double[] values)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            values = null;
            return false;
        }

        return TryGetFloat64Array(item, out values);
    }

    public bool TryGetFloat64(IntPtr item, out double value)
    {
        UInt32 count;

        if (!TryCountValues(item, out count))
        {
            value = 0;
            return false;
        }

        if (count != 1u)
        {
            value = 0;
            return false;
        }

        double[] dst = new double[1u];
        PxResult r = PxTryCopyValues(item, dst, 1u, PxType.PX_FLOAT64);

        if (r != PxResult.PX_SUCCESS)
        {
            value = 0;
            return false;
        }

        value = dst[0];
        return true;
    }

    public bool TryGetFloat64(string key, out double value)
    {
        IntPtr item;

        if (!TryGetItem(key, out item))
        {
            value = 0;
            return false;
        }

        return TryGetFloat64(item, out value);
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
    // pod-index C++ Library Methods and Structs
    // -------------------------------------------

    // Result of pod-index functions
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

    // See pod_index.h for DLL documentation

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
