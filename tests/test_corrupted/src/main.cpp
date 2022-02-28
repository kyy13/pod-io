// PODserializer
// Kyle J Burgess

#include "PODserializer.h"

#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdlib>

const char* filename = "corrupt_file.test.bin";

template<PsEndian endian, PsChecksum checksum>
std::vector<char> create()
{
    const char* val0 = "test";
    uint32_t val1[4] = {0, 40, 400, 4000};
    int64_t val2[8] = {-1, -2, -3, -4, -5, -6, -7, -8};

    auto serializer = psCreateSerializer();

    psSetValues(psGetBlock(serializer, "val0"), val0, 4, PS_CHAR8);
    psSetValues(psGetBlock(serializer, "val1"), val1, 4, PS_UINT32);
    psSetValues(psGetBlock(serializer, "val2"), val2, 8, PS_INT64);

    if (psSaveFile(serializer, filename, checksum, endian) != PS_SUCCESS)
    {
        std::cout << "failed to psSaveFile()" << std::endl;
        return {};
    }

    psDeleteSerializer(serializer);

    // Read the file

    std::ifstream ifs(filename, std::ios::binary);

    if (!ifs.is_open())
    {
        std::cout << "failed to open file for reading" << std::endl;
        return {};
    }

    return std::vector<char>(std::istreambuf_iterator<char>(ifs), {});
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

template<PsEndian endian, PsChecksum checksum>
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

        auto serializer = psCreateSerializer();

        PsResult result;

        try
        {
            result = psLoadFile(serializer, filename);
        }
        catch(...)
        {
            return false;
        }

        if (result != PS_FILE_CORRUPT && result != PS_SUCCESS)
        {
            std::cout << "returned " << result << std::endl;
            return false;
        }

        psDeleteSerializer(serializer);

        ++i;
    }

    return true;
}

int main()
{
    if (!test<PS_ENDIAN_LITTLE, PS_CHECKSUM_NONE>())
    {
        return -1;
    }

    return 0;
}
