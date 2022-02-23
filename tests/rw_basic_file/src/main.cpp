// PODserializer
// Kyle J Burgess

#include "PODserializer.h"

#include <iostream>

int main()
{
    // Write File

    auto serializer = psCreateSerializer();

    auto testA = psGetBlock(serializer, "TestB");

    uint32_t values[5] =
        {
            1,
            2,
            3,
            4,
            5,
        };

    psSetValues(testA, values, PS_UINT32, 5);

    auto r = psSaveFile(serializer, "testFileA.bin", PS_CHECKSUM_NONE, PS_ENDIAN_NATIVE);

    psDeleteSerializer(serializer);

    if (r != PS_SUCCESS)
    {
        return -1;
    }

    // Read File

    serializer = psCreateSerializer();

    r = psLoadFile(serializer, "testFileA.bin");

    psDeleteSerializer(serializer);

    if (r != PS_SUCCESS)
    {
        return -1;
    }

    return 0;
}
