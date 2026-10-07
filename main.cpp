#include "Client.h"
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCommandLineParser parser;
    QCommandLineOption hostOption("i", "IP", "default-IP");
    QCommandLineOption portOption("p", "Port", "default-port");
    QCommandLineOption protocolOption("protocol", "Protocol", "default-protocol");
    parser.addOption(hostOption);
    parser.addOption(portOption);
    parser.addOption(protocolOption);
    parser.process(a);

    QString host = "";
    if (parser.isSet(hostOption))
      host = parser.value(hostOption);

    int port = 0;
    if (parser.isSet(portOption))
      port = parser.value(portOption).toInt();

    QString protocol;
    if (parser.isSet(protocolOption))
      protocol = parser.value(protocolOption);
    Client* socket = new Client(qApp);
    QTimer::singleShot(1000, socket, [socket, host, port]()
                       { socket->connectToServer(host, port); });
    return QCoreApplication::exec();
}
