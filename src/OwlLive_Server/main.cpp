#include <QCoreApplication>
#include "Server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server *server = new Server(nullptr);

    server->Open(8888);

    return a.exec();
}
