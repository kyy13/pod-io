// pod-io
// Kyle J Burgess

using System;
using System.Runtime.InteropServices;
using System.Text;

public enum              PodCompression : UInt32
{
    POD_COMPRESSION_0          = 0u,                     // No compression (largest size)
    POD_COMPRESSION_1          = 1u,                     // Least compression
    POD_COMPRESSION_2          = 2u,
    POD_COMPRESSION_3          = 3u,
    POD_COMPRESSION_4          = 4u,
    POD_COMPRESSION_5          = 5u,
    POD_COMPRESSION_6          = 6u,
    POD_COMPRESSION_7          = 7u,
    POD_COMPRESSION_8          = 8u,
    POD_COMPRESSION_9          = 9u,                     // Best compression (smallest size)
    POD_COMPRESSION_NONE       = POD_COMPRESSION_0,
    POD_COMPRESSION_DEFAULT    = POD_COMPRESSION_6,
    POD_COMPRESSION_BEST       = POD_COMPRESSION_9,
};

// Endianness
public enum              PodEndian : UInt32
{
    POD_ENDIAN_LITTLE          = 0u,                     // Save the file in little endian format
    POD_ENDIAN_BIG             = 1u,                     // Save the file in big endian format
    POD_ENDIAN_NATIVE          = 2u,                     // Save the file in the endianness of the host
};

// Checksum Type
public enum              PodChecksum : UInt32
{
    POD_CHECKSUM_NONE          = 0u,                     // Read/write a file with no checksum
    POD_CHECKSUM_ADLER32       = 1u,                     // Read/write a file with an adler32 checksum
    POD_CHECKSUM_CRC32         = 2u,                     // Read/write a file with a crc32 checksum
};

public class PodContainer : IDisposable
{
    public PodContainer()
    {
        m_container = PodCreateContainer();
    }

    public void Dispose()
    {
        TryDispose();
        GC.SuppressFinalize(this);
    }

    public void Load(string fileName, PodChecksum checksum, UInt32 checksumValue = 0)
    {
        PodResult r = PodLoadFile(m_container, Encoding.ASCII.GetBytes(fileName + '\0'), checksum, checksumValue);

        if (r != PodResult.POD_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public void Save(string fileName, PodCompression compression, PodChecksum checksum, UInt32 checksumValue = 0, PodEndian endianness = PodEndian.POD_ENDIAN_NATIVE)
    {
        PodResult r = PodSaveFile(m_container, Encoding.ASCII.GetBytes(fileName + '\0'), compression, checksum, checksumValue, endianness);

        if (r != PodResult.POD_SUCCESS)
        {
            throw new Exception(r.ToString());
        }
    }

    public IntPtr GetItem(string key)
    {
        return PodGetItem(m_container, Encoding.ASCII.GetBytes(key + '\0'));
    }

    public bool TryGetItem(string key, out IntPtr item)
    {
        IntPtr ptr = PodTryGetItem(m_container, Encoding.ASCII.GetBytes(key + '\0'));

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

        PodResult r = PodRemoveItem(m_container, item);

        if (r != PodResult.POD_SUCCESS)
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

        PodResult r = PodTryCountValues(item, out count);

        if (r != PodResult.POD_SUCCESS)
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

        PodResult r = PodTryCountKeyChars(item, out count);

        if (count == 0)
        {
            return false;
        }

        if (r != PodResult.POD_SUCCESS)
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

        PodResult r = PodTryCopyKey(item, dst, count);

        if (r != PodResult.POD_SUCCESS)
        {
            key = null;
            return false;
        }

        key = Encoding.ASCII.GetString(dst);
        return true;
    }

    public IntPtr GetFirstItem()
    {
        return PodGetFirstItem(m_container);
    }

    public IntPtr GetNextItem(IntPtr item)
    {
        return PodGetNextItem(m_container, item);
    }

    // ASCII

    public void SetAsciiString(IntPtr item, string str)
    {
        var bytes = Encoding.ASCII.GetBytes(str);

        PodResult r = PodSetValues(item, bytes, (UInt32)bytes.Length, PodType.POD_ASCII_CHAR8);

        if (r != PodResult.POD_SUCCESS)
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

        PodResult r = PodTryCopyValues(item, dst, count, PodType.POD_ASCII_CHAR8);

        if (r != PodResult.POD_SUCCESS)
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

        PodResult r = PodSetValues(item, bytes, (UInt32)bytes.Length, PodType.POD_UTF8_CHAR8);

        if (r != PodResult.POD_SUCCESS)
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

        PodResult r = PodTryCopyValues(item, dst, count, PodType.POD_UTF8_CHAR8);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodSetValues(item, values, (UInt32)values.Length, PodType.POD_UINT8);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodTryCopyValues(item, dst, count, PodType.POD_UINT8);

        if (r != PodResult.POD_SUCCESS)
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

        PodResult r = PodTryCopyValues(item, dst, 1u, PodType.POD_UINT8);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodSetValues(item, values, (UInt32)values.Length, PodType.POD_UINT16);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodTryCopyValues(item, dst, count, PodType.POD_UINT16);

        if (r != PodResult.POD_SUCCESS)
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

        PodResult r = PodTryCopyValues(item, dst, 1u, PodType.POD_UINT16);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodSetValues(item, values, (UInt32)values.Length, PodType.POD_UINT32);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodTryCopyValues(item, dst, count, PodType.POD_UINT32);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodTryCopyValues(item, dst, 1u, PodType.POD_UINT32);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodSetValues(item, values, (UInt32)values.Length, PodType.POD_UINT64);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodTryCopyValues(item, dst, count, PodType.POD_UINT64);

        if (r != PodResult.POD_SUCCESS)
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

        PodResult r = PodTryCopyValues(item, dst, 1u, PodType.POD_UINT64);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodSetValues(item, values, (UInt32)values.Length, PodType.POD_INT8);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodTryCopyValues(item, dst, count, PodType.POD_INT8);

        if (r != PodResult.POD_SUCCESS)
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

        PodResult r = PodTryCopyValues(item, dst, 1u, PodType.POD_INT8);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodSetValues(item, values, (UInt32)values.Length, PodType.POD_INT16);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodTryCopyValues(item, dst, count, PodType.POD_INT16);

        if (r != PodResult.POD_SUCCESS)
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

        PodResult r = PodTryCopyValues(item, dst, 1u, PodType.POD_INT16);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodSetValues(item, values, (UInt32)values.Length, PodType.POD_INT32);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodTryCopyValues(item, dst, count, PodType.POD_INT32);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodTryCopyValues(item, dst, 1u, PodType.POD_INT32);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodSetValues(item, values, (UInt32)values.Length, PodType.POD_INT64);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodTryCopyValues(item, dst, count, PodType.POD_INT64);

        if (r != PodResult.POD_SUCCESS)
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

        PodResult r = PodTryCopyValues(item, dst, 1u, PodType.POD_INT64);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodSetValues(item, values, (UInt32)values.Length, PodType.POD_FLOAT32);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodTryCopyValues(item, dst, count, PodType.POD_FLOAT32);

        if (r != PodResult.POD_SUCCESS)
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

        PodResult r = PodTryCopyValues(item, dst, 1u, PodType.POD_FLOAT32);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodSetValues(item, values, (UInt32)values.Length, PodType.POD_FLOAT32);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodTryCopyValues(item, dst, count, PodType.POD_FLOAT64);

        if (r != PodResult.POD_SUCCESS)
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
        PodResult r = PodTryCopyValues(item, dst, 1u, PodType.POD_FLOAT64);

        if (r != PodResult.POD_SUCCESS)
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

    ~PodContainer()
    {
        TryDispose();
    }

    protected virtual void TryDispose()
    {
        if (!m_disposed)
        {
            PodDeleteContainer(m_container);
            m_disposed = true;
        }
    }

    protected IntPtr m_container;

    protected bool m_disposed = false;

    // -------------------------------------------
    // pod-io C++ Library Methods and Structs
    // -------------------------------------------

    // Result of pod-io functions
    protected enum           PodResult : UInt32
    {
        POD_SUCCESS                = 0u,                     // Success
        POD_NULL_REFERENCE         = 1u,                     // Tried to pass null for an item or container
        POD_TYPE_MISMATCH          = 2u,                     // Tried to get a type that does not match the stored type
        POD_OUT_OF_RANGE           = 3u,                     // Tried to copy values out of the range of the stored buffer
        POD_FILE_CORRUPT           = 4u,                     // Save file is corrupt
        POD_FILE_NOT_FOUND         = 5u,                     // Unable to open file for read or write
        POD_ARGUMENT_ERROR         = 6u,                     // Provided an incorrect argument to a method
        POD_ZLIB_ERROR             = 7u,                     // Error during zlib initialization
    };

    // Types of Data
    protected enum           PodType : UInt32
    {
        POD_ASCII_CHAR8            = 0x02000001u,            // 8-bit ASCII character
        POD_UTF8_CHAR8             = 0x03000001u,            // 8-bit UTF8 bytes
        POD_UINT8                  = 0x00000001u,            // 8-bit unsigned integer
        POD_UINT16                 = 0x00000002u,            // 16-bit unsigned integer
        POD_UINT32                 = 0x00000004u,            // 32-bit unsigned integer
        POD_UINT64                 = 0x00000008u,            // 64-bit unsigned integer
        POD_INT8                   = 0x00010001u,            // 8-bit signed twos-complement integer
        POD_INT16                  = 0x00010002u,            // 16-bit signed twos-complement integer
        POD_INT32                  = 0x00010004u,            // 32-bit signed twos-complement integer
        POD_INT64                  = 0x00010008u,            // 64-bit signed twos-complement integer
        POD_FLOAT32                = 0x01010004u,            // 32-bit IEEE floating point number
        POD_FLOAT64                = 0x01010008u,            // 64-bit IEEE floating point number
    };

    // See pod_io.h for DLL documentation

    [DllImport("libpod-io", EntryPoint = "pod_alloc", CallingConvention = CallingConvention.Cdecl)]
    protected static extern IntPtr      PodCreateContainer();

    [DllImport("libpod-io", EntryPoint = "pod_free", CallingConvention = CallingConvention.Cdecl)]
    protected static extern void        PodDeleteContainer(IntPtr container);

    [DllImport("libpod-io", EntryPoint = "pod_load_file", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodLoadFile(IntPtr container, byte[] fileName, PodChecksum checksum, UInt32 checksumValue);

    [DllImport("libpod-io", EntryPoint = "pod_save_file", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodSaveFile(IntPtr container, byte[] fileName, PodCompression compression, PodChecksum checksum, UInt32 checksumValue, PodEndian endianness);

    [DllImport("libpod-io", EntryPoint = "pod_get_item", CallingConvention = CallingConvention.Cdecl)]
    protected static extern IntPtr      PodGetItem(IntPtr container, byte[] key);

    [DllImport("libpod-io", EntryPoint = "pod_try_get_item", CallingConvention = CallingConvention.Cdecl)]
    protected static extern IntPtr      PodTryGetItem(IntPtr container, byte[] key);

    [DllImport("libpod-io", EntryPoint = "pod_remove_item", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodRemoveItem(IntPtr container, IntPtr item);

    [DllImport("libpod-io", EntryPoint = "pod_try_count_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodTryCountValues(IntPtr item, out UInt32 valueCount);

    [DllImport("libpod-io", EntryPoint = "pod_try_get_type", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodTryGetType(IntPtr item, out PodType valueType);

    [DllImport("libpod-io", EntryPoint = "pod_try_count_key_chars", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodTryCountKeyChars(IntPtr item, out UInt32 charCount);

    [DllImport("libpod-io", EntryPoint = "pod_try_copy_key", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodTryCopyKey(IntPtr item, byte[] key, UInt32 count);

    [DllImport("libpod-io", EntryPoint = "pod_get_first_item", CallingConvention = CallingConvention.Cdecl)]
    protected static extern IntPtr      PodGetFirstItem(IntPtr container);

    [DllImport("libpod-io", EntryPoint = "pod_get_next_item", CallingConvention = CallingConvention.Cdecl)]
    protected static extern IntPtr      PodGetNextItem(IntPtr container, IntPtr item);

    [DllImport("libpod-io", EntryPoint = "pod_set_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodSetValues(IntPtr item, byte[] srcValueArray, UInt32 valueCount, PodType valueType);

    [DllImport("libpod-io", EntryPoint = "pod_set_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodSetValues(IntPtr item, UInt16[] srcValueArray, UInt32 valueCount, PodType valueType);

    [DllImport("libpod-io", EntryPoint = "pod_set_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodSetValues(IntPtr item, UInt32[] srcValueArray, UInt32 valueCount, PodType valueType);

    [DllImport("libpod-io", EntryPoint = "pod_set_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodSetValues(IntPtr item, UInt64[] srcValueArray, UInt32 valueCount, PodType valueType);

    [DllImport("libpod-io", EntryPoint = "pod_set_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodSetValues(IntPtr item, sbyte[] srcValueArray, UInt32 valueCount, PodType valueType);

    [DllImport("libpod-io", EntryPoint = "pod_set_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodSetValues(IntPtr item, Int16[] srcValueArray, UInt32 valueCount, PodType valueType);

    [DllImport("libpod-io", EntryPoint = "pod_set_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodSetValues(IntPtr item, Int32[] srcValueArray, UInt32 valueCount, PodType valueType);

    [DllImport("libpod-io", EntryPoint = "pod_set_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodSetValues(IntPtr item, Int64[] srcValueArray, UInt32 valueCount, PodType valueType);

    [DllImport("libpod-io", EntryPoint = "pod_set_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodSetValues(IntPtr item, float[] srcValueArray, UInt32 valueCount, PodType valueType);

    [DllImport("libpod-io", EntryPoint = "pod_set_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodSetValues(IntPtr item, double[] srcValueArray, UInt32 valueCount, PodType valueType);

    [DllImport("libpod-io", EntryPoint = "pod_try_copy_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodTryCopyValues(IntPtr item, byte[] dstValueArray, UInt32 valueCount, PodType type);

    [DllImport("libpod-io", EntryPoint = "pod_try_copy_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodTryCopyValues(IntPtr item, UInt16[] dstValueArray, UInt32 valueCount, PodType type);

    [DllImport("libpod-io", EntryPoint = "pod_try_copy_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodTryCopyValues(IntPtr item, UInt32[] dstValueArray, UInt32 valueCount, PodType type);

    [DllImport("libpod-io", EntryPoint = "pod_try_copy_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodTryCopyValues(IntPtr item, UInt64[] dstValueArray, UInt32 valueCount, PodType type);

    [DllImport("libpod-io", EntryPoint = "pod_try_copy_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodTryCopyValues(IntPtr item, sbyte[] dstValueArray, UInt32 valueCount, PodType type);

    [DllImport("libpod-io", EntryPoint = "pod_try_copy_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodTryCopyValues(IntPtr item, Int16[] dstValueArray, UInt32 valueCount, PodType type);

    [DllImport("libpod-io", EntryPoint = "pod_try_copy_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodTryCopyValues(IntPtr item, Int32[] dstValueArray, UInt32 valueCount, PodType type);

    [DllImport("libpod-io", EntryPoint = "pod_try_copy_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodTryCopyValues(IntPtr item, Int64[] dstValueArray, UInt32 valueCount, PodType type);

    [DllImport("libpod-io", EntryPoint = "pod_try_copy_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodTryCopyValues(IntPtr item, float[] dstValueArray, UInt32 valueCount, PodType type);

    [DllImport("libpod-io", EntryPoint = "pod_try_copy_values", CallingConvention = CallingConvention.Cdecl)]
    protected static extern PodResult    PodTryCopyValues(IntPtr item, double[] dstValueArray, UInt32 valueCount, PodType type);
}
