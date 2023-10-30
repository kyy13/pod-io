// pod-io
// Kyle J Burgess

#include "pod_io.h"
#include "PodFile.h"
#include "PodBytes.h"
#include "zlib.h"

#include <vector>
#include <cstring>
#include <iostream>

const char* fileName = "checksum_file.test.bin";

template<pod_endian_t endian, pod_checksum_t checksum>
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
        (endian == POD_ENDIAN_LITTLE && is_big_endian()) ||
        (endian == POD_ENDIAN_BIG && is_little_endian());

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
        case POD_CHECKSUM_ADLER32:
            c1 = adler32(c1, buffer.data(), buffer.size() - 4);
            break;
        case POD_CHECKSUM_CRC32:
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

template<pod_endian_t endian, pod_checksum_t checksum>
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

    auto container = pod_alloc();

    pod_set_values(pod_get_item(container, "test string"), c8, 20, POD_ASCII_CHAR8);

    pod_set_values(pod_get_item(container, "UKeyA"), u8, 6, POD_UINT8);
    pod_set_values(pod_get_item(container, "UKeyB"), u16, 7, POD_UINT16);
    pod_set_values(pod_get_item(container, "UKeyC"), u32, 8, POD_UINT32);
    pod_set_values(pod_get_item(container, "UKeyD"), u64, 9, POD_UINT64);

    pod_set_values(pod_get_item(container, "KeyE"), i8, 6, POD_INT8);
    pod_set_values(pod_get_item(container, "KeyF"), i16, 7, POD_INT16);
    pod_set_values(pod_get_item(container, "KeyG"), i32, 8, POD_INT32);
    pod_set_values(pod_get_item(container, "KeyH"), i64, 9, POD_INT64);

    pod_set_values(pod_get_item(container, "FloatKeyI"), f32, 11, POD_FLOAT32);
    pod_set_values(pod_get_item(container, "DoubleKeyJ"), f64, 12, POD_FLOAT64);

    if (pod_save_file(container, fileName, POD_COMPRESSION_6, checksum, 0x236534AAu, endian) != POD_SUCCESS)
    {
        return false;
    }

    pod_free(container);

    if (!testFile<endian, checksum>())
    {
        return false;
    }

    return true;
}

int main()
{
    if (!test<POD_ENDIAN_NATIVE, POD_CHECKSUM_CRC32>())
    {
        std::cout << "failed, endian = " << POD_ENDIAN_NATIVE << ", checksum = " << POD_CHECKSUM_CRC32 << "\n";
        return -1;
    }

    if (!test<POD_ENDIAN_LITTLE, POD_CHECKSUM_CRC32>())
    {
        std::cout << "failed, endian = " << POD_ENDIAN_LITTLE << ", checksum = " << POD_CHECKSUM_CRC32 << "\n";
        return -1;
    }

    if (!test<POD_ENDIAN_BIG, POD_CHECKSUM_CRC32>())
    {
        std::cout << "failed, endian = " << POD_ENDIAN_BIG << ", checksum = " << POD_CHECKSUM_CRC32 << "\n";
        return -1;
    }

    if (!test<POD_ENDIAN_NATIVE, POD_CHECKSUM_ADLER32>())
    {
        std::cout << "failed, endian = " << POD_ENDIAN_NATIVE << ", checksum = " << POD_CHECKSUM_ADLER32 << "\n";
        return -1;
    }

    if (!test<POD_ENDIAN_LITTLE, POD_CHECKSUM_ADLER32>())
    {
        std::cout << "failed, endian = " << POD_ENDIAN_LITTLE << ", checksum = " << POD_CHECKSUM_ADLER32 << "\n";
        return -1;
    }

    if (!test<POD_ENDIAN_BIG, POD_CHECKSUM_ADLER32>())
    {
        std::cout << "failed, endian = " << POD_ENDIAN_BIG << ", checksum = " << POD_CHECKSUM_ADLER32 << "\n";
        return -1;
    }

    return 0;
}
