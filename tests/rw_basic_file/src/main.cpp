// PODserializer
// Kyle J Burgess

#include "PODserializer.h"

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

    psSaveFile(serializer, "testFileA.bin", PS_NO_CHECKSUM, PS_NATIVE_ENDIAN);

    psDeleteSerializer(serializer);

    return 0;
}
