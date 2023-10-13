// pod-io
// Kyle J Burgess

#ifndef POD_FILE_H
#define POD_FILE_H

#include <fstream>

enum FileMode
{
    FM_READ,
    FM_WRITE,
};

class File
{
public:

    File(const char* filename, FileMode mode);

    File(const File&) = delete;

    File& operator=(const File&) = delete;

    ~File();

    // Write data with size bytes
    // returns the number of bytes written
    size_t write(const void* data, size_t size);

    // Read data into ptr with size bytes
    // returns the number of bytes read
    size_t read(void* ptr, size_t size);

    // Returns true if the file is open
    [[nodiscard]]
    bool is_open() const;

protected:
    FILE* m_file;
    FileMode m_mode;
};

#endif
