#include "crypto.h"

#include "file.h"

#include <assert.h>
#include <locale.h>
#include <QDebug>
#include <stdexcept>

using namespace std;
using namespace ModularBackup::Crypto;

namespace {
void throwIfError(gpgme_error_t error, string message) {
    if (error) {
        throw runtime_error(message + ": " + gpgme_strerror(error));
    }
}
}

void Context::init()
{
    setlocale(LC_ALL, "");
    gpgme_check_version(nullptr);
    gpgme_set_locale(nullptr, LC_CTYPE, setlocale(LC_CTYPE, nullptr));
#ifdef LC_MESSAGES
    gpgme_set_locale(nullptr, LC_MESSAGES, setlocale(LC_MESSAGES, nullptr));
#endif

    throwIfError(gpgme_engine_check_version(GPGME_PROTOCOL_OPENPGP), "Versio check failed");
}

Context::Context()
{
    throwIfError(gpgme_new(&m_context), "Failed create gpgme context");
    gpgme_set_ctx_flag(m_context, "export-session-key", "1");

    assert(m_recipients.size() == 1);

    //  gpgme_set_armor(context, 1);
   //   error = gpgme_context_set_engine_info(context, GPGME_PROTOCOL_OPENPGP, nullptr, keyRingDir);
   //   assert(!error);

}

Context::~Context()
{
    for (auto key : m_recipients) {
        if (key) {
            gpgme_key_release(key);
        }
    }

    if (m_context) {
        gpgme_release(m_context);
    }
}

void Context::addRecipients(const char* pattern)
{
    throwIfError(gpgme_op_keylist_start(m_context, pattern, 1), "Keylist start failed");

    while (true) {
        gpgme_key_t recipient = nullptr;
        auto res = gpgme_op_keylist_next(m_context, &recipient);
        if (gpg_err_code(res) == GPG_ERR_EOF) {
            return;
        }

        throwIfError(res, "keylist_next");

        // ownership transferred to m_recipients
        m_recipients.insert(m_recipients.begin(), recipient);
        assert(m_recipients.back() == nullptr);

        auto user = recipient->uids;
        printf("Encrypting for %s (%s) <%s>\n", user->name, user->comment, user->email);
    }
}

void Context::encrypt(const char* clearText, const char* cipherText)
{
    FileIn in(clearText);
    FileOut out(cipherText);

    assert(m_recipients.size() >= 2);
    throwIfError(gpgme_op_encrypt(m_context, m_recipients.data(),
                   GPGME_ENCRYPT_ALWAYS_TRUST, in, out), "encrypt");

    auto result = gpgme_op_encrypt_result(m_context);
    if (result->invalid_recipients) {
        throw runtime_error(string("Invalid recipient found: ") + result->invalid_recipients->fpr);
    }
}

void Context::decrypt(const char* cipherText, const char* clearText)
{
    FileIn in(cipherText);
    FileOut out(clearText);

    throwIfError(gpgme_op_decrypt(m_context, in, out), "decrypt");

    auto result = gpgme_op_decrypt_result(m_context);
    qDebug() << "file:" << result->file_name;
    qDebug() << "sess key:" << result->session_key;
}
