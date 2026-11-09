#include "Client.h"
#include "Command.h"
#include "CommandStart.h"
#include "Protocol.h"
#include <QRandomGenerator>

Client::Client(QObject* parent, const QString& host, int port,
               const DataFormat& protocol, qint64 pid)
  : QObject(parent)
  , m_logger(Logger::getInstance("Client.Network"))
  , m_id(pid)
{
  m_socket = new QTcpSocket(this);
  connect(m_socket, &QTcpSocket::connected, this, &Client::connected);
  connect(m_socket, &QTcpSocket::readyRead, this, &Client::readyRead);
  connect(m_socket, &QTcpSocket::errorOccurred, this, &Client::errorOccurred);
  connect(m_socket, &QTcpSocket::disconnected, this, &Client::disconnected);
  connect(this, &Client::finished, this, &Client::stop);
  connect(this, &Client::started, this, &Client::sendStart);
  startTimer(7000);
  LOG4CPLUS_INFO(m_logger, "Client created id: " << m_id);
  m_serverHost = host;
  m_serverPort = port;
  m_protocol = Protocol::create(protocol);
  m_socket->connectToHost(m_serverHost, m_serverPort);
}

Client::~Client()
{
  delete m_protocol;
  m_protocol = nullptr;
  delete m_socket;
  m_socket = nullptr;
  LOG4CPLUS_INFO(m_logger, "Client closed id: " << m_id);
}

void Client::timerEvent(QTimerEvent* event)
{
  if (m_socket->state() == QAbstractSocket::UnconnectedState)
  {
    LOG4CPLUS_DEBUG(m_logger, "Attempt to connect to the server...");
    m_socket->connectToHost(m_serverHost, m_serverPort);
  }
}

void Client::readyRead()
{
  m_buffer.append(m_socket->readAll());
  char delimiter = '\n';
  while (m_buffer.indexOf(delimiter) != -1)
  {
    int index = m_buffer.indexOf(delimiter);
    QByteArray data = m_buffer.left(index);
    m_buffer.remove(0, index + 1);
    Command* cmd = m_protocol->read(data);
    if (!cmd)
      LOG4CPLUS_ERROR(m_logger, "Error: Сommand not in the specified format");
    if (cmd->getCMDType() == CMDType::MESSAGE)
      cmd->toDebug(qDebug());
    delete cmd;
    cmd = nullptr;
  }
}

void Client::connected()
{
  LOG4CPLUS_INFO(m_logger, "Connect to the server");
  emit started();
}

void Client::disconnected()
{
  LOG4CPLUS_INFO(m_logger, "The server closed the connection");
}

void Client::sendStart()
{
  int random_id = QRandomGenerator::global()->bounded(10, 30);
  CommandStart cmdStart(CMDType::START, random_id);
  QByteArray data = m_protocol->write(&cmdStart);
  data.append('\n');
  if (m_socket->state() == QAbstractSocket::UnconnectedState)
    return;
  m_socket->write(data);
}

void Client::sendStop()
{
  Command cmdStop(CMDType::STOP);
  QByteArray data = m_protocol->write(&cmdStop);
  data.append('\n');
  m_socket->write(data);
  m_socket->flush();
  emit finished();
}

void Client::stop()
{
  killTimer(m_timerId);
  LOG4CPLUS_INFO(m_logger, "The client closed the connection");
  QCoreApplication::quit();
}

void Client::errorOccurred(QAbstractSocket::SocketError socketError)
{
  switch (socketError)
  {
  case QAbstractSocket::RemoteHostClosedError:
    break;
  case QAbstractSocket::HostNotFoundError:
    LOG4CPLUS_ERROR(m_logger, "Error: The host was not found. Please check the "
                              "host name and port settings.");
    break;
  case QAbstractSocket::ConnectionRefusedError:
    LOG4CPLUS_ERROR(
        m_logger,
        "Error: The connection was refused by the peer. Make sure the server is "
        "running, and check that the host name and port settings are correct.");
    break;
  default:
    LOG4CPLUS_ERROR(m_logger, "Error: The following error occurred: "
                                  << m_socket->errorString().toStdString());
  }
}