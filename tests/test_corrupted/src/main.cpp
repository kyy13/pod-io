// pod-io
// Kyle J Burgess

#include "pod_io.h"

#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdlib>

const char* filename = "corrupt_file.test.bin";

template<pod_endian_t endian, pod_checksum_t checksum>
std::vector<char> create()
{
    const char* val0 = "test";
    uint32_t val1[4] = {0, 40, 400, 4000};
    int64_t val2[8] = {-1, -2, -3, -4, -5, -6, -7, -8};

    auto container = pod_alloc();

    pod_set_values(pod_get_item(container, "val0"), val0, 4, POD_ASCII_CHAR8);
    pod_set_values(pod_get_item(container, "val1"), val1, 4, POD_UINT32);
    pod_set_values(pod_get_item(container, "val2"), val2, 8, POD_INT64);

    if (pod_save_file(container, filename, POD_COMPRESSION_6, checksum, 0x01020304u, endian) != POD_SUCCESS)
    {
        std::cout << "failed to psSaveBlocksToFile()" << std::endl;
        return {};
    }

    pod_free(container);

    // Read the file

    std::ifstream ifs(filename, std::ios::binary);

    if (!ifs.is_open())
    {
        std::cout << "failed to open file for reading" << std::endl;
        return {};
    }

    return {std::istreambuf_iterator<char>(ifs), {}};
}

bool write(std::vector<char>& buffer)
{
    // Write the file

    std::ofstream ofs(filename, std::ios::binary);

    if (!ofs.is_open())
    {
        std::cout << "failed to open file for writing" << std::endl;
        return false;
    }

    ofs.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));

    return true;
}

template<pod_endian_t endian, pod_checksum_t checksum>
bool test()
{
    srand(0x30405060u);

    size_t i = 0;

    while(true)
    {
        std::cout << i << std::endl;

        // create the file
        auto buf = create<endian, checksum>();

        if (buf.size() == 0)
        {
            std::cout << "buffer size is 0" << std::endl;
            return false;
        }

        if (i >= buf.size())
        {
            break;
        }

        // corrupt the file
        uint8_t oldval = *reinterpret_cast<uint8_t*>(&buf[i]);
        uint8_t newval;

        do
        {
            newval = static_cast<uint8_t>(rand() % 256);
        } while (newval == oldval);

        buf[i] = newval;

        // write the file
        if (!write(buf))
        {
            std::cout << "failed to write" << std::endl;
            return false;
        }

        // deserialize

        auto container = pod_alloc();

        pod_result_t result;

        try
        {
            result = pod_load_file(container, filename, checksum, 0x01020304u);
        }
        catch(...)
        {
            return false;
        }

        if (result != POD_FILE_CORRUPT && result != POD_SUCCESS)
        {
            std::cout << "returned " << result << std::endl;
            return false;
        }

        pod_free(container);

        ++i;
    }

    return true;
}

int main()
{
    if (!test<POD_ENDIAN_LITTLE, POD_CHECKSUM_NONE>())
    {
        return -1;
    }

    return 0;
}
