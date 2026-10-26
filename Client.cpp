#include "Client.h"
#include "Command.h"
#include "CommandMessage.h"
#include "CommandStart.h"
#include "JsonDecoder.h"
#include "JsonEncoder.h"
#include <QRandomGenerator>

Client::Client(QObject* parent)
  : QObject(parent)
  , m_logger(Logger::getInstance("Client"))
{
  m_socket = new QTcpSocket(this);
  in.setDevice(m_socket);
  in.setVersion(QDataStream::Qt_5_15);
  connect(m_socket, &QTcpSocket::connected, this, &Client::connected);
  connect(m_socket, &QTcpSocket::readyRead, this, &Client::readyRead);
  connect(m_socket, &QTcpSocket::errorOccurred, this, &Client::errorOccurred);
  connect(m_socket, &QTcpSocket::disconnected, this, &Client::disconnected);
  connect(this, &Client::finished, this, &Client::stop);
  connect(this, &Client::started, this, &Client::sendToStart);
  startTimer(7000);
  LOG4CPLUS_DEBUG(m_logger, "Client created");
}

Client::~Client()
{
  if (m_socket)
    m_socket->deleteLater();
  LOG4CPLUS_DEBUG(m_logger, "Client closed");
}

void Client::connectToServer(const QString& host, int port, QString protocol)
{
  m_serverHost = host;
  m_serverPort = port;
  m_protocol = protocol;
  m_socket->connectToHost(m_serverHost, m_serverPort);
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
    CommandMessage cmd;
    JsonDecoder decoder;
    QJsonObject jsonDoc;
    bool succes = decoder.parseToRawObject(data, jsonDoc);
    if (succes && decoder.readJson(jsonDoc, cmd) &&
        cmd.getCMDType() == CMDType::MESSAGE)
      cmd.toDebug(qDebug());
  }
}

void Client::connected()
{
  LOG4CPLUS_DEBUG(m_logger, "Connect to the server");
  emit started();
}

void Client::disconnected()
{
  LOG4CPLUS_DEBUG(m_logger, "The server closed the connection");
}

void Client::sendToStart()
{
  int random_id = QRandomGenerator::global()->bounded(10, 30);
  CommandStart cmdStart(CMDType::START, random_id);
  JsonEncoder encoder;
  QByteArray data = encoder.writeJson(cmdStart);
  data.append('\n');
  if (m_socket->state() == QAbstractSocket::UnconnectedState)
    return;
  m_socket->write(data);
}

void Client::sendToStop()
{
  Command cmdStop(CMDType::STOP);
  JsonEncoder encoder;
  QByteArray data = encoder.writeJson(cmdStop);
  data.append('\n');
  m_socket->write(data);
  m_socket->flush();
  emit finished();
}

void Client::stop()
{
  killTimer(m_timerId);
  LOG4CPLUS_DEBUG(m_logger, "The client closed the connection");
  QCoreApplication::quit();
}

void Client::errorOccurred(QAbstractSocket::SocketError socketError)
{
  switch (socketError)
  {
  case QAbstractSocket::RemoteHostClosedError:
    break;
  case QAbstractSocket::HostNotFoundError:
    LOG4CPLUS_ERROR(m_logger, "Client: The host was not found. Please check the "
                              "host name and port settings.");
    break;
  case QAbstractSocket::ConnectionRefusedError:
    LOG4CPLUS_ERROR(
        m_logger,
        "Client: The connection was refused by the peer. Make sure the server is "
        "running, and check that the host name and port settings are correct.");
    break;
  default:
    LOG4CPLUS_ERROR(m_logger, "Client: The following error occurred: "
                                  << m_socket->errorString().toStdString());
  }
}
