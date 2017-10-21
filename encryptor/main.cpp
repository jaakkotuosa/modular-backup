
#include "crypto.h"
#include "file.h"

#include <assert.h>
#include <iostream>
#include <QCoreApplication>

using namespace std;
using namespace ModularBackup;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    try {
        Crypto::Context::init();

        Crypto::Context context;
        context.addRecipients("Backup encryption key");

        const char* clearText = "C:/Users/Jaakko/Documents/code/ModularBackup/clear.txt";
        const char* cipherText = "C:/Users/Jaakko/Documents/code/ModularBackup/cipher.txt";
        const char* resultText = "C:/Users/Jaakko/Documents/code/ModularBackup/result.txt";

        context.encrypt(clearText, cipherText);
        context.decrypt(cipherText, resultText);

        FileIn result(resultText);
        cout << "Result: " << result.toString() << endl;
    } catch (exception e) {
        cerr << "Fail: " << e.what() << endl;
    }
    return 0; //a.exec();
}
