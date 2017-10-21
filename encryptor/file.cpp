#include "file.h"

#include <assert.h>
#include <exception>
#include <memory>
#include <stdexcept>

using namespace std;
using namespace ModularBackup;

const char* File::modeString(Mode mode)
{
    switch (mode) {
    case Mode::Read:
        return "r";
    case Mode::Write:
        return "w";
    }

    assert(false);
    return nullptr;
}

File::File(const string &filename, Mode mode)
{
    m_file = fopen(filename.c_str(), modeString(mode));
    if (!m_file) {
        throw runtime_error("Failed to open file " + filename);
    }
    auto ret = gpgme_data_new_from_fd(&m_data, fileno(m_file));
    if (ret) {
        throw runtime_error("Failed to create gpgme_data");
    }
    assert(m_data);
}

File::~File()
{
    if (m_data) {
        gpgme_data_release(m_data);
    }

    if (m_file) {
        fclose(m_file);
    }
}

void File::seek(size_t offset)
{
    auto result = gpgme_data_seek(m_data, offset, SEEK_SET);
    if (result < 0) {
        throw runtime_error("Failed to seek data file");
    }
}

string File::toString()
{
    enum {
        MaxLen = 4096
    };

    unique_ptr<char[]> buffer(new char[MaxLen]);
    auto size = gpgme_data_read(m_data, buffer.get(), MaxLen - 1);
    if (size < 0) {
        throw runtime_error("Failed to read data");
    }
    buffer[size] = '\0';

    string str(buffer.get());
    return str;
}
