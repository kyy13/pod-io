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
        (endian == PX_ENDIAN_LITTLE && is_big_endian()) ||
        (endian == PX_ENDIAN_BIG && is_little_endian());

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
        case PX_CHECKSUM_ADLER32:
            c1 = adler32(c1, buffer.data(), buffer.size() - 4);
            break;
        case PX_CHECKSUM_CRC32:
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

    auto container = pxCreateContainer();

    pxSetValues(pxGetItem(container, "test string"), c8, 20, PX_ASCII_CHAR8);

    pxSetValues(pxGetItem(container, "UKeyA"), u8, 6, PX_UINT8);
    pxSetValues(pxGetItem(container, "UKeyB"), u16, 7, PX_UINT16);
    pxSetValues(pxGetItem(container, "UKeyC"), u32, 8, PX_UINT32);
    pxSetValues(pxGetItem(container, "UKeyD"), u64, 9, PX_UINT64);

    pxSetValues(pxGetItem(container, "KeyE"), i8, 6, PX_INT8);
    pxSetValues(pxGetItem(container, "KeyF"), i16, 7, PX_INT16);
    pxSetValues(pxGetItem(container, "KeyG"), i32, 8, PX_INT32);
    pxSetValues(pxGetItem(container, "KeyH"), i64, 9, PX_INT64);

    pxSetValues(pxGetItem(container, "FloatKeyI"), f32, 11, PX_FLOAT32);
    pxSetValues(pxGetItem(container, "DoubleKeyJ"), f64, 12, PX_FLOAT64);

    if (pxSaveFile(container, fileName, PX_COMPRESSION_6, checksum, 0x236534AAu, endian) != PX_SUCCESS)
    {
        return false;
    }

    pxDeleteContainer(container);

    if (!testFile<endian, checksum>())
    {
        return false;
    }

    return true;
}

int main()
{
    if (!test<PX_ENDIAN_NATIVE, PX_CHECKSUM_CRC32>())
    {
        std::cout << "failed, endian = " << PX_ENDIAN_NATIVE << ", checksum = " << PX_CHECKSUM_CRC32 << "\n";
        return -1;
    }

    if (!test<PX_ENDIAN_LITTLE, PX_CHECKSUM_CRC32>())
    {
        std::cout << "failed, endian = " << PX_ENDIAN_LITTLE << ", checksum = " << PX_CHECKSUM_CRC32 << "\n";
        return -1;
    }

    if (!test<PX_ENDIAN_BIG, PX_CHECKSUM_CRC32>())
    {
        std::cout << "failed, endian = " << PX_ENDIAN_BIG << ", checksum = " << PX_CHECKSUM_CRC32 << "\n";
        return -1;
    }

    if (!test<PX_ENDIAN_NATIVE, PX_CHECKSUM_ADLER32>())
    {
        std::cout << "failed, endian = " << PX_ENDIAN_NATIVE << ", checksum = " << PX_CHECKSUM_ADLER32 << "\n";
        return -1;
    }

    if (!test<PX_ENDIAN_LITTLE, PX_CHECKSUM_ADLER32>())
    {
        std::cout << "failed, endian = " << PX_ENDIAN_LITTLE << ", checksum = " << PX_CHECKSUM_ADLER32 << "\n";
        return -1;
    }

    if (!test<PX_ENDIAN_BIG, PX_CHECKSUM_ADLER32>())
    {
        std::cout << "failed, endian = " << PX_ENDIAN_BIG << ", checksum = " << PX_CHECKSUM_ADLER32 << "\n";
        return -1;
    }

    return 0;
}
