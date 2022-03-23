// pod-index
// Kyle J Burgess

#include "pod_index.h"

#include <vector>
#include <cstring>
#include <iostream>

bool test(PxCompression compression, PxEndian endian, PxChecksum checksum)
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

    if (pxSaveFile(container, "rw_basic_file.test.bin", compression, checksum, 0x01020304u, endian) != PX_SUCCESS)
    {
        std::cout << "0\n";
        return false;
    }

    pxDeleteContainer(container);
    uint32_t count;

    std::vector<uint8_t> n_c8;
    std::vector<uint8_t> n_u8;
    std::vector<uint16_t> n_u16;
    std::vector<uint32_t> n_u32;
    std::vector<uint64_t> n_u64;
    std::vector<int8_t> n_i8;
    std::vector<int16_t> n_i16;
    std::vector<int32_t> n_i32;
    std::vector<int64_t> n_i64;
    std::vector<float> n_f32;
    std::vector<double> n_f64;

    container = pxCreateContainer();

    if (pxLoadFile(container, "rw_basic_file.test.bin", checksum, 0x01020304u) != PX_SUCCESS)
    {
        std::cout << "1\n";
        return false;
    }

    auto block = pxGetItem(container, "test string");
    if (pxTryCountValues(block, count) != PX_SUCCESS)
    {
        std::cout << "2\n";
        return false;
    }

    n_c8.resize(count);
    if (pxTryCopyValues(block, n_c8.data(), count, PX_ASCII_CHAR8) != PX_SUCCESS)
    {
        std::cout << "3\n";
        return false;
    }

    block = pxGetItem(container, "UKeyA");
    if (pxTryCountValues(block, count) != PX_SUCCESS)
    {
        std::cout << "4\n";
        return false;
    }

    n_u8.resize(count);
    if (pxTryCopyValues(block, n_u8.data(), count, PX_UINT8) != PX_SUCCESS)
    {
        std::cout << "5\n";
        return false;
    }

    block = pxGetItem(container, "UKeyB");
    if (pxTryCountValues(block, count) != PX_SUCCESS)
    {
        std::cout << "6\n";
        return false;
    }

    n_u16.resize(count);
    if (pxTryCopyValues(block, n_u16.data(), count, PX_UINT16) != PX_SUCCESS)
    {
        std::cout << "7\n";
        return false;
    }

    block = pxGetItem(container, "UKeyC");
    if (pxTryCountValues(block, count) != PX_SUCCESS)
    {
        std::cout << "8\n";
        return false;
    }

    n_u32.resize(count);
    if (pxTryCopyValues(block, n_u32.data(), count, PX_UINT32) != PX_SUCCESS)
    {
        std::cout << "8\n";
        return false;
    }

    block = pxGetItem(container, "UKeyD");
    if (pxTryCountValues(block, count) != PX_SUCCESS)
    {
        std::cout << "9\n";
        return false;
    }

    n_u64.resize(count);
    if (pxTryCopyValues(block, n_u64.data(), count, PX_UINT64) != PX_SUCCESS)
    {
        std::cout << "10\n";
        return false;
    }

    block = pxGetItem(container, "KeyE");
    if (pxTryCountValues(block, count) != PX_SUCCESS)
    {
        std::cout << "11\n";
        return false;
    }

    n_i8.resize(count);
    if (pxTryCopyValues(block, n_i8.data(), count, PX_INT8) != PX_SUCCESS)
    {
        std::cout << "12\n";
        return false;
    }

    block = pxGetItem(container, "KeyF");
    if (pxTryCountValues(block, count) != PX_SUCCESS)
    {
        std::cout << "13\n";
        return false;
    }

    n_i16.resize(count);
    if (pxTryCopyValues(block, n_i16.data(), count, PX_INT16) != PX_SUCCESS)
    {
        std::cout << "14\n";
        return false;
    }

    block = pxGetItem(container, "KeyG");
    if (pxTryCountValues(block, count) != PX_SUCCESS)
    {
        std::cout << "15\n";
        return false;
    }

    n_i32.resize(count);
    if (pxTryCopyValues(block, n_i32.data(), count, PX_INT32) != PX_SUCCESS)
    {
        std::cout << "16\n";
        return false;
    }

    block = pxGetItem(container, "KeyH");
    if (pxTryCountValues(block, count) != PX_SUCCESS)
    {
        std::cout << "17\n";
        return false;
    }

    n_i64.resize(count);
    if (pxTryCopyValues(block, n_i64.data(), count, PX_INT64) != PX_SUCCESS)
    {
        std::cout << "18\n";
        return false;
    }

    block = pxGetItem(container, "FloatKeyI");
    if (pxTryCountValues(block, count) != PX_SUCCESS)
    {
        std::cout << "19\n";
        return false;
    }

    n_f32.resize(count);
    if (pxTryCopyValues(block, n_f32.data(), count, PX_FLOAT32) != PX_SUCCESS)
    {
        std::cout << "20\n";
        return false;
    }

    block = pxGetItem(container, "DoubleKeyJ");
    if (pxTryCountValues(block, count) != PX_SUCCESS)
    {
        std::cout << "21\n";
        return false;
    }

    n_f64.resize(count);
    if (pxTryCopyValues(block, n_f64.data(), count, PX_FLOAT64) != PX_SUCCESS)
    {
        std::cout << "22\n";
        return false;
    }

    pxDeleteContainer(container);

    if (memcmp(c8, n_c8.data(), 20 * sizeof(uint8_t)) != 0)
    {
        std::cout << "23\n";
        return false;
    }

    if (memcmp(u8, n_u8.data(), 6 * sizeof(uint8_t)) != 0)
    {
        std::cout << "24\n";
        return false;
    }

    if (memcmp(u16, n_u16.data(), 7 * sizeof(uint16_t)) != 0)
    {
        std::cout << "25\n";
        return false;
    }

    if (memcmp(u32, n_u32.data(), 8 * sizeof(uint32_t)) != 0)
    {
        std::cout << "26\n";
        return false;
    }

    if (memcmp(u64, n_u64.data(), 9 * sizeof(uint64_t)) != 0)
    {
        std::cout << "27\n";
        return false;
    }

    if (memcmp(i8, n_i8.data(), 6 * sizeof(int8_t)) != 0)
    {
        std::cout << "28\n";
        return false;
    }

    if (memcmp(i16, n_i16.data(), 7 * sizeof(int16_t)) != 0)
    {
        std::cout << "29\n";
        return false;
    }

    if (memcmp(i32, n_i32.data(), 8 * sizeof(int32_t)) != 0)
    {
        std::cout << "30\n";
        return false;
    }

    if (memcmp(i64, n_i64.data(), 9 * sizeof(int64_t)) != 0)
    {
        std::cout << "31\n";
        return false;
    }

    if (memcmp(f32, n_f32.data(), 11 * sizeof(float)) != 0)
    {
        std::cout << "32\n";
        return false;
    }

    if (memcmp(f64, n_f64.data(), 12 * sizeof(double)) != 0)
    {
        std::cout << "33\n";
        return false;
    }

    return true;
}

bool test(PxEndian endian, PxChecksum checksum)
{
    if (!test(PX_COMPRESSION_0, endian, checksum))
    {
        std::cout << "failed compress 0\n";
        return false;
    }

    if (!test(PX_COMPRESSION_1, endian, checksum))
    {
        std::cout << "failed compress 1\n";
        return false;
    }

    if (!test(PX_COMPRESSION_2, endian, checksum))
    {
        std::cout << "failed compress 2\n";
        return false;
    }

    if (!test(PX_COMPRESSION_3, endian, checksum))
    {
        std::cout << "failed compress 3\n";
        return false;
    }

    if (!test(PX_COMPRESSION_4, endian, checksum))
    {
        std::cout << "failed compress 4\n";
        return false;
    }

    if (!test(PX_COMPRESSION_5, endian, checksum))
    {
        std::cout << "failed compress 5\n";
        return false;
    }

    if (!test(PX_COMPRESSION_6, endian, checksum))
    {
        std::cout << "failed compress 6\n";
        return false;
    }

    if (!test(PX_COMPRESSION_7, endian, checksum))
    {
        std::cout << "failed compress 7\n";
        return false;
    }

    if (!test(PX_COMPRESSION_8, endian, checksum))
    {
        std::cout << "failed compress 8\n";
        return false;
    }

    if (!test(PX_COMPRESSION_9, endian, checksum))
    {
        std::cout << "failed compress 9\n";
        return false;
    }

    return true;
}

bool test(PxChecksum checksum)
{
    if (!test(PX_ENDIAN_NATIVE, checksum))
    {
        std::cout << "failed native endianness\n";
        return false;
    }

    if (!test(PX_ENDIAN_LITTLE, checksum))
    {
        std::cout << "failed little endianness\n";
        return false;
    }

    if (!test(PX_ENDIAN_BIG, checksum))
    {
        std::cout << "failed big endianness\n";
        return false;
    }

    return true;
}

int main()
{
    if (!test(PX_CHECKSUM_NONE))
    {
        std::cout << "failed no checksum\n";
        return -1;
    }

    if (!test(PX_CHECKSUM_ADLER32))
    {
        std::cout << "failed adler32\n";
        return -1;
    }

    if (!test(PX_CHECKSUM_CRC32))
    {
        std::cout << "failed crc32\n";
        return -1;
    }

    return 0;
}
