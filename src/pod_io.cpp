// pod-io
// Kyle J Burgess

#include "pod_io.h"
#include "PodBytes.h"
#include "PodTypes.h"
#include "PodLookup.h"

#include <stdexcept>

pod_container_t* pod_alloc()
{
    return new pod_container_t;
}

void pod_free(pod_container_t* container)
{
    delete container;
}

pod_item_t* pod_get_item(pod_container_t* container, const char* key)
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

    return reinterpret_cast<pod_item_t*>(&(*it));
}

pod_result_t pod_remove_item(pod_container_t* container, pod_item_t* item)
{
    if ((container == nullptr) || (item == nullptr))
    {
        return POD_NULL_REFERENCE;
    }

    auto& key = reinterpret_cast<const std::pair<std::string,PodData>*>(item)->first;

    container->map.erase(key);

    return POD_SUCCESS;
}

pod_item_t* pod_try_get_item(pod_container_t* container, const char* key)
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

    return reinterpret_cast<pod_item_t*>(&(*it));
}

pod_result_t pod_set_values(pod_item_t* item, const void* srcValueArray, uint32_t valueCount, pod_type_t valueType)
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

pod_result_t pod_try_count_values(const pod_item_t* item, uint32_t* valueCount)
{
    if (item == nullptr)
    {
        return POD_NULL_REFERENCE;
    }

    auto& data = reinterpret_cast<const std::pair<std::string,PodData>*>(item)->second;

    if (valueCount != nullptr)
    {
        *valueCount = data.count;
    }

    return POD_SUCCESS;
}

pod_result_t pod_try_get_type(const pod_item_t* item, pod_type_t* valueType)
{
    if (item == nullptr)
    {
        return POD_NULL_REFERENCE;
    }

    auto& data = reinterpret_cast<const std::pair<std::string,PodData>*>(item)->second;

    if (valueType != nullptr)
    {
        *valueType = data.type;
    }

    return POD_SUCCESS;
}

pod_result_t pod_try_copy_values(const pod_item_t* item, void* dstValueArray, uint32_t valueCount, pod_type_t type)
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

pod_result_t pod_try_count_key_chars(const pod_item_t* item, uint32_t* count)
{
    if (item == nullptr)
    {
        return POD_NULL_REFERENCE;
    }

    auto& key = reinterpret_cast<const std::pair<std::string,PodData>*>(item)->first;

    if (count != nullptr)
    {
        *count = static_cast<uint32_t>(key.size());
    }

    return POD_SUCCESS;
}

pod_result_t pod_try_copy_key(const pod_item_t* item, char* buffer, uint32_t charCount)
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

pod_item_t* pod_get_first_item(pod_container_t* container)
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

    return reinterpret_cast<pod_item_t*>(&(*it));
}

pod_item_t* pod_get_next_item(pod_container_t* container, pod_item_t* item)
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

    return reinterpret_cast<pod_item_t*>(&(*it));
}
