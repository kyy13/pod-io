// pod-index
// Kyle J Burgess

#include "pod_index.h"
#include "PxFile.h"
#include "PxBytes.h"
#include "zlib.h"

#include <vector>
#include <cstring>
#include <iostream>

const char* fileName = "checksum_file.test.bin";

template<PxEndian endian, PxChecksum checksum>
bool testFile()
{
    File file(fileName, FM_READ);

    if (!file.is_open())
    {
        return false;
    }

    std::vector<uint8_t> buffer(1024);

    size_t bytesRead = 0;

    while (true)
    {
        size_t tmp = file.read(buffer.data(), 1024);
        bytesRead += tmp;

        if (tmp < 1024)
        {
            buffer.resize(bytesRead);
            break;
        }

        buffer.resize(bytesRead + 1024);
    }

    uint32_t c0 = 0x236534AAu, c1 = 0x236534AAu;

    std::vector<uint8_t> crcbuf(4);
    memcpy(crcbuf.data(), buffer.data() + buffer.size() - 4, 4);

    bool requiresByteSwap =
        (endian == PS_ENDIAN_LITTLE && is_big_endian()) ||
        (endian == PS_ENDIAN_BIG && is_little_endian());

    if (requiresByteSwap)
    {
        get_bytes<uint32_t, true>(c0, crcbuf, 0, 4);
    }
    else
    {
        get_bytes<uint32_t, false>(c0, crcbuf, 0, 4);
    }

    switch(checksum)
    {
        case PS_CHECKSUM_ADLER32:
            c1 = adler32(c1, buffer.data(), buffer.size() - 4);
            break;
        case PS_CHECKSUM_CRC32:
            c1 = crc32(c1, buffer.data(), buffer.size() - 4);
            break;
        default:
            break;
    }

    if (c0 != c1)
    {
        std::cout << "c0 = " << c0 << "\n";
        std::cout << "c1 = " << c1 << "\n";
        return false;
    }

    return true;
}

template<PxEndian endian, PxChecksum checksum>
bool test()
{
    uint8_t c8[20];
    uint8_t u8[6];
    uint16_t u16[7];
    uint32_t u32[8];
    uint64_t u64[9];
    int8_t i8[6];
    int16_t i16[7];
    int32_t i32[8];
    int64_t i64[9];
    float f32[11];
    double f64[12];

    auto serializer = pxCreateContainer();

    pxSetValues(pxGetItem(serializer, "test string"), c8, 20, PS_ASCII_CHAR8);

    pxSetValues(pxGetItem(serializer, "UKeyA"), u8, 6, PS_UINT8);
    pxSetValues(pxGetItem(serializer, "UKeyB"), u16, 7, PS_UINT16);
    pxSetValues(pxGetItem(serializer, "UKeyC"), u32, 8, PS_UINT32);
    pxSetValues(pxGetItem(serializer, "UKeyD"), u64, 9, PS_UINT64);

    pxSetValues(pxGetItem(serializer, "KeyE"), i8, 6, PS_INT8);
    pxSetValues(pxGetItem(serializer, "KeyF"), i16, 7, PS_INT16);
    pxSetValues(pxGetItem(serializer, "KeyG"), i32, 8, PS_INT32);
    pxSetValues(pxGetItem(serializer, "KeyH"), i64, 9, PS_INT64);

    pxSetValues(pxGetItem(serializer, "FloatKeyI"), f32, 11, PS_FLOAT32);
    pxSetValues(pxGetItem(serializer, "DoubleKeyJ"), f64, 12, PS_FLOAT64);

    if (pxSaveFile(serializer, fileName, PS_COMPRESSION_6, checksum, 0x236534AAu, endian) != PS_SUCCESS)
    {
        return false;
    }

    pxDeleteContainer(serializer);

    if (!testFile<endian, checksum>())
    {
        return false;
    }

    return true;
}

int main()
{
    if (!test<PS_ENDIAN_NATIVE, PS_CHECKSUM_CRC32>())
    {
        std::cout << "failed, endian = " << PS_ENDIAN_NATIVE << ", checksum = " << PS_CHECKSUM_CRC32 << "\n";
        return -1;
    }

    if (!test<PS_ENDIAN_LITTLE, PS_CHECKSUM_CRC32>())
    {
        std::cout << "failed, endian = " << PS_ENDIAN_LITTLE << ", checksum = " << PS_CHECKSUM_CRC32 << "\n";
        return -1;
    }

    if (!test<PS_ENDIAN_BIG, PS_CHECKSUM_CRC32>())
    {
        std::cout << "failed, endian = " << PS_ENDIAN_BIG << ", checksum = " << PS_CHECKSUM_CRC32 << "\n";
        return -1;
    }

    if (!test<PS_ENDIAN_NATIVE, PS_CHECKSUM_ADLER32>())
    {
        std::cout << "failed, endian = " << PS_ENDIAN_NATIVE << ", checksum = " << PS_CHECKSUM_ADLER32 << "\n";
        return -1;
    }

    if (!test<PS_ENDIAN_LITTLE, PS_CHECKSUM_ADLER32>())
    {
        std::cout << "failed, endian = " << PS_ENDIAN_LITTLE << ", checksum = " << PS_CHECKSUM_ADLER32 << "\n";
        return -1;
    }

    if (!test<PS_ENDIAN_BIG, PS_CHECKSUM_ADLER32>())
    {
        std::cout << "failed, endian = " << PS_ENDIAN_BIG << ", checksum = " << PS_CHECKSUM_ADLER32 << "\n";
        return -1;
    }

    return 0;
}
