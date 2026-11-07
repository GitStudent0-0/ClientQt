#include "Client.h"
#include "Protocol.h"
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

LogLevel getLogLevel(QString log)
{
  log = log.toUpper();
  if (log == "TRACE")
    return TRACE_LOG_LEVEL;
  else if (log == "DEBUG")
    return DEBUG_LOG_LEVEL;
  else if (log == "INFO")
    return INFO_LOG_LEVEL;
  else if (log == "WARN")
    return WARN_LOG_LEVEL;
  else if (log == "ERROR")
    return ERROR_LOG_LEVEL;
  else if (log == "FATAL")
    return FATAL_LOG_LEVEL;
  return DEBUG_LOG_LEVEL;
}

int main(int argc, char *argv[])
{
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);

  Initializer initializer;
  PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(LOG_CONFIG_PATH));
  Logger m_logger = Logger::getInstance("Client.Main");

  QCoreApplication a(argc, argv);
  QCommandLineParser parser;
  QCommandLineOption hostOption("i", "IP", "-i");
  QCommandLineOption portOption("p", "Port", "-p");
  QCommandLineOption protocolOption("protocol", "Protocol", "--protocol");
  QCommandLineOption logOption(
      "log", "Log level: TRACE < DEBUG < INFO < WARN < ERROR < FATAL", "--log");

  parser.addOption(hostOption);
  parser.addOption(portOption);
  parser.addOption(protocolOption);
  parser.addOption(logOption);
  parser.addHelpOption();
  parser.addVersionOption();
  parser.process(a);

  if (!parser.isSet(hostOption))
  {
    LOG4CPLUS_FATAL(m_logger, "Error: Option "
                                  << hostOption.valueName().toStdString()
                                  << " was not specified");
    parser.showHelp(1);
  }
  QString host = parser.value(hostOption);

  if (!parser.isSet(portOption))
  {
    LOG4CPLUS_FATAL(m_logger, "Error: Option "
                                  << portOption.valueName().toStdString()
                                  << " was not specified");
    parser.showHelp(1);
  }
  int port = parser.value(portOption).toInt();

  if (!parser.isSet(protocolOption))
  {
    LOG4CPLUS_FATAL(m_logger, "Error: Option "
                                  << protocolOption.valueName().toStdString()
                                  << " was not specified");
    parser.showHelp(1);
  }
  QString protocol = parser.value(protocolOption);

  if (!parser.isSet(logOption))
  {
    LOG4CPLUS_FATAL(m_logger, "Error: Option " << logOption.valueName().toStdString()
                                               << " was not specified");
    parser.showHelp(1);
  }

  QString log = parser.value(logOption);
  Logger::getRoot().setLogLevel(getLogLevel(log));

  if (protocol.toUpper() == "JSON")
    client = new Client(qApp, host, port, DataFormat::JSON);
  else if (protocol.toUpper() == "XML")
    client = new Client(qApp, host, port, DataFormat::XML);
  else
    LOG4CPLUS_FATAL(m_logger, "Error: Protocol specified incorrectly");

  return a.exec();
}
