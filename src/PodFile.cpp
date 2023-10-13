// pod-io
// Kyle J Burgess

#include "PodFile.h"

#include <cassert>

File::File(const char* filename, FileMode mode)
    : m_file(nullptr)
    , m_mode(mode)
{
    switch(m_mode)
    {
    case FM_READ:
        m_file = fopen(filename, "rb");
        break;
    case FM_WRITE:
        m_file = fopen(filename, "wb");
        break;
    default:
        break;
    }
}

File::~File()
{
    if (m_file != nullptr)
    {
        fclose(m_file);
    }
}

bool File::is_open() const
{
    return (m_file != nullptr);
}

size_t File::write(const void* data, size_t size)
{
    assert(m_file != nullptr);
    assert(m_mode == FM_WRITE);
    return fwrite(data, 1u, size, m_file);
}

size_t File::read(void* ptr, size_t size)
{
    assert(m_file != nullptr);
    assert(m_mode == FM_READ);
    return fread(ptr, 1u, size, m_file);
}
