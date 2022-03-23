// pod-index
// Kyle J Burgess

#include "pod_index.h"

#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdlib>

const char* filename = "corrupt_file.test.bin";

template<PxEndian endian, PxChecksum checksum>
std::vector<char> create()
{
    const char* val0 = "test";
    uint32_t val1[4] = {0, 40, 400, 4000};
    int64_t val2[8] = {-1, -2, -3, -4, -5, -6, -7, -8};

    auto container = pxCreateContainer();

    pxSetValues(pxGetItem(container, "val0"), val0, 4, PX_ASCII_CHAR8);
    pxSetValues(pxGetItem(container, "val1"), val1, 4, PX_UINT32);
    pxSetValues(pxGetItem(container, "val2"), val2, 8, PX_INT64);

    if (pxSaveFile(container, filename, PX_COMPRESSION_6, checksum, 0x01020304u, endian) != PX_SUCCESS)
    {
        std::cout << "failed to psSaveBlocksToFile()" << std::endl;
        return {};
    }

    pxDeleteContainer(container);

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

template<PxEndian endian, PxChecksum checksum>
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

        auto container = pxCreateContainer();

        PxResult result;

        try
        {
            result = pxLoadFile(container, filename, checksum, 0x01020304u);
        }
        catch(...)
        {
            return false;
        }

        if (result != PX_FILE_CORRUPT && result != PX_SUCCESS)
        {
            std::cout << "returned " << result << std::endl;
            return false;
        }

        pxDeleteContainer(container);

        ++i;
    }

    return true;
}

int main()
{
    if (!test<PX_ENDIAN_LITTLE, PX_CHECKSUM_NONE>())
    {
        return -1;
    }

    return 0;
}
