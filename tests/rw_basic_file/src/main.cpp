// PODserializer
// Kyle J Burgess

#include "PODserializer.h"

#include <iostream>

int main()
{
    PsSerializer* serializer = psCreateSerializer();

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

    if (r != PS_SUCCESS)
    {
        std::cout << r << "\n";
        return -1;
    }

    psDeleteSerializer(serializer);

    return 0;
}
