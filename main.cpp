#include "Client.h"
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <log4cplus/configurator.h>
#include <log4cplus/initializer.h>
#include <signal.h>
using namespace log4cplus;
static Client* client = nullptr;
void signalHandler(int sigInt)
{
  if (sigInt == SIGINT || sigInt == SIGTERM)
    if (client)
      QMetaObject::invokeMethod(client, "sendToStop");
}

int main(int argc, char *argv[])
{
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);
  Initializer initializer;
  PropertyConfigurator::doConfigure("log4cplus.properties");
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

  client = new Client(qApp);
  client->connectToServer(host, port, protocol);

  return a.exec();
}
