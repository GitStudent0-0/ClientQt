#include "Client.h"

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
  startTimer(10000);
  LOG4CPLUS_DEBUG(m_logger, "Client created");
}

Client::~Client()
{
  if (m_socket)
    m_socket->deleteLater();
  LOG4CPLUS_DEBUG(m_logger, "Client closed");
}

void Client::connectToServer(const QString& host, int port)
{
  m_serverHost = host;
  m_serverPort = port;
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
  in.startTransaction();

  QString nextMessage;
  in >> nextMessage;

  if (!in.commitTransaction())
    return;

  if (nextMessage == m_currentMessage)
    return;

  m_currentMessage = nextMessage;
  QTextStream(stdout) << m_currentMessage << "\n";
}

void Client::connected() { LOG4CPLUS_DEBUG(m_logger, "Connect to the server"); }

void Client::disconnected()
{
  LOG4CPLUS_DEBUG(m_logger, "The server closed the connection");
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
