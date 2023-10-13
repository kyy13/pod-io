// pod-io
// Kyle J Burgess

#include "pod_io.h"
#include "PodBytes.h"
#include "PodTypes.h"
#include "PodLookup.h"

#include <stdexcept>

PodContainer* podCreateContainer()
{
    return new PodContainer;
}

void podDeleteContainer(PodContainer* container)
{
    delete container;
}

PodItem* podGetItem(PodContainer* container, const char* key)
{
    if (container == nullptr)
    {
        return nullptr;
    }

    std::string str;

    try
    {
        str.assign(key);
    }
    catch (const std::length_error&)
    {
        return nullptr;
    }

    // limit key size
    if (str.size() > std::numeric_limits<uint32_t>::max())
    {
        return nullptr;
    }

    auto& map = container->map;

    auto it = map.find(key);

    if (it == map.end())
    {
        const PodData data =
            {
                .count = 0,
                .type = POD_UINT8,
            };

        it = map.insert(std::make_pair(std::move(str), data)).first;
    }

    return reinterpret_cast<PodItem*>(&(*it));
}

PodResult podRemoveItem(PodContainer* container, PodItem* item)
{
    if ((container == nullptr) || (item == nullptr))
    {
        return POD_NULL_REFERENCE;
    }

    auto& key = reinterpret_cast<const std::pair<std::string,PodData>*>(item)->first;

    container->map.erase(key);

    return POD_SUCCESS;
}

PodItem* podTryGetItem(PodContainer* container, const char* key)
{
    if (container == nullptr)
    {
        return nullptr;
    }

    std::string str;

    try
    {
        str.assign(key);
    }
    catch (const std::length_error&)
    {
        return nullptr;
    }

    // limit key size
    if (str.size() > std::numeric_limits<uint32_t>::max())
    {
        return nullptr;
    }

    auto& map = container->map;

    auto it = map.find(key);

    if (it == map.end())
    {
        return nullptr;
    }

    return reinterpret_cast<PodItem*>(&(*it));
}

PodResult podSetValues(PodItem* item, const void* srcValueArray, uint32_t valueCount, PodType valueType)
{
    if (item == nullptr)
    {
        return POD_NULL_REFERENCE;
    }

    uint32_t maxCount = MaxCountLookup[size_of_type(valueType)];

    if (valueCount > maxCount)
    {
        return POD_OUT_OF_RANGE;
    }

    size_t size = static_cast<size_t>(valueCount) * size_of_type(valueType);

    auto& data = reinterpret_cast<std::pair<std::string,PodData>*>(item)->second;

    data.values.resize(size);
    data.count = valueCount;
    data.type = valueType;

    memcpy(data.values.data(), srcValueArray, size);

    return POD_SUCCESS;
}

PodResult podTryCountValues(const PodItem* item, uint32_t& valueCount)
{
    if (item == nullptr)
    {
        return POD_NULL_REFERENCE;
    }

    auto& data = reinterpret_cast<const std::pair<std::string,PodData>*>(item)->second;

    valueCount = data.count;

    return POD_SUCCESS;
}

PodResult podTryGetType(const PodItem* item, PodType& valueType)
{
    if (item == nullptr)
    {
        return POD_NULL_REFERENCE;
    }

    auto& data = reinterpret_cast<const std::pair<std::string,PodData>*>(item)->second;

    valueType = data.type;

    return POD_SUCCESS;
}

PodResult podTryCopyValues(const PodItem* item, void* dstValueArray, uint32_t valueCount, PodType type)
{
    if (item == nullptr)
    {
        return POD_NULL_REFERENCE;
    }

    auto& data = reinterpret_cast<const std::pair<std::string,PodData>*>(item)->second;

    if (data.count == 0)
    {
        return POD_SUCCESS;
    }

    if (type != data.type)
    {
        return POD_TYPE_MISMATCH;
    }

    size_t size = valueCount * size_of_type(type);
    if (size > data.values.size())
    {
        return POD_OUT_OF_RANGE;
    }

    memcpy(dstValueArray, data.values.data(), size);

    return POD_SUCCESS;
}

PodResult podTryCountKeyChars(const PodItem* item, uint32_t& count)
{
    if (item == nullptr)
    {
        return POD_NULL_REFERENCE;
    }

    auto& key = reinterpret_cast<const std::pair<std::string,PodData>*>(item)->first;

    count = static_cast<uint32_t>(key.size());

    return POD_SUCCESS;
}

PodResult podTryCopyKey(const PodItem* item, char* buffer, uint32_t charCount)
{
    if (item == nullptr)
    {
        return POD_NULL_REFERENCE;
    }

    auto& key = reinterpret_cast<const std::pair<std::string,PodData>*>(item)->first;

    if (charCount != key.size())
    {
        return POD_OUT_OF_RANGE;
    }

    memcpy(buffer, key.data(), key.size());

    return POD_SUCCESS;
}

PodItem* podGetFirstItem(PodContainer* container)
{
    if (container == nullptr)
    {
        return nullptr;
    }

    auto& map = container->map;

    auto it = map.begin();

    if (it == map.end())
    {
        return nullptr;
    }

    return reinterpret_cast<PodItem*>(&(*it));
}

PodItem* podGetNextItem(PodContainer* container, PodItem* item)
{
    if ((container == nullptr) || (item == nullptr))
    {
        return nullptr;
    }

    auto& key = reinterpret_cast<const std::pair<std::string,PodData>*>(item)->first;

    auto& map = container->map;

    auto it = map.find(key);

    if (it == map.end())
    {
        return nullptr;
    }

    return reinterpret_cast<PodItem*>(&(*it));
}
