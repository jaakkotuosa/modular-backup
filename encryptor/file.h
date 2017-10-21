#pragma once

#include <string>
#include <cstdio>
#include <gpgme.h>

namespace ModularBackup {

class File
{
public:
    enum class Mode {
        Read,
        Write
    };

    File(const std::string& filename, Mode mode);
    virtual ~File();


    operator gpgme_data_t() {
        return m_data;
    }

    void seek(size_t offset);

    std::string toString();

private:
    static const char* modeString(Mode mode);

private:
    FILE* m_file = nullptr;
    gpgme_data_t m_data = nullptr;
};

class FileIn : public File
{
public:
    FileIn(const std::string& filename)
        : File(filename, File::Mode::Read)
    {
    }
};

class FileOut : public File
{
public:
    FileOut(const std::string& filename)
        : File(filename, File::Mode::Write)
    {
    }
};

}

