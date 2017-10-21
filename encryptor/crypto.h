#pragma once

#include <string>
#include <cstdio>
#include <gpgme.h>
#include <vector>

namespace ModularBackup {
namespace Crypto {

class Context
{
public:
    // TODO: auto-init
    static void init();

    Context();
    virtual ~Context();

    void addRecipients(const char* pattern);

    void encrypt(const char* clearText, const char* cipherText);

    void decrypt(const char* cipherText, const char* clearText);

private:
    gpgme_ctx_t m_context = nullptr;

    // recipient array should be null-terminated
    // owns the keys
    std::vector<gpgme_key_t> m_recipients = {nullptr};
};

}
}
