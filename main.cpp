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
      QMetaObject::invokeMethod(client, "sendStop");
}

int main(int argc, char *argv[])
{
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);
  Initializer initializer;
  PropertyConfigurator::doConfigure("log4cplus.properties");
  Logger m_logger = Logger::getInstance("main");
  QCoreApplication a(argc, argv);
  QCommandLineParser parser;
  QCommandLineOption hostOption("i", "IP", "-i");
  QCommandLineOption portOption("p", "Port", "-p");
  QCommandLineOption protocolOption("protocol", "Protocol", "--protocol");
  parser.addOption(hostOption);
  parser.addOption(portOption);
  parser.addOption(protocolOption);
  parser.addHelpOption();
  parser.addVersionOption();
  parser.process(a);

  if (!parser.isSet(hostOption))
  {
    LOG4CPLUS_ERROR(m_logger, "Error: Option "
                                  << hostOption.valueName().toStdString()
                                  << " was not specified");
    parser.showHelp(1);
  }
  QString host = parser.value(hostOption);

  if (!parser.isSet(portOption))
  {
    LOG4CPLUS_ERROR(m_logger, "Error: Option "
                                  << portOption.valueName().toStdString()
                                  << " was not specified");
    parser.showHelp(1);
  }
  int port = parser.value(portOption).toInt();

  if (!parser.isSet(protocolOption))
  {
    LOG4CPLUS_ERROR(m_logger, "Error: Option "
                                  << protocolOption.valueName().toStdString()
                                  << " was not specified");
    parser.showHelp(1);
  }
  QString protocol = parser.value(protocolOption);

  client = new Client(qApp);
  client->connectToServer(host, port, protocol);

  return a.exec();
}
