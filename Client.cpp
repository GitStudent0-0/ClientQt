#include "Client.h"

Client::Client(QObject* parent)
  : QObject(parent)
{
  m_socket = new QTcpSocket(this);
  in.setDevice(m_socket);
  in.setVersion(QDataStream::Qt_5_15);
  connect(m_socket, &QTcpSocket::connected, this, &Client::connected);
  connect(m_socket, &QTcpSocket::readyRead, this, &Client::readyRead);
  connect(m_socket, &QTcpSocket::errorOccurred, this, &Client::errorOccurred);
  connect(m_socket, &QTcpSocket::disconnected, this, &Client::disconnected);
  m_timer = new QTimer(this);
  m_timer->setInterval(3000);
  connect(m_timer, &QTimer::timeout, this, &Client::tryReconnect);
}

Client::~Client()
{
  m_timer->stop();
  if (m_socket)
    m_socket->abort();
}
void Client::connectToServer(const QString& host, int port)
{
  serverHost = host;
  serverPort = port;
  tryReconnect();
}

void Client::tryReconnect()
{
  if (m_socket->state() == QAbstractSocket::UnconnectedState)
  {
    qDebug() << "Attempt to connect to the server...";
    m_socket->connectToHost(serverHost, serverPort);
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

void Client::connected()
{
  qDebug() << "Connect to the server";
  m_timer->stop();
}

void Client::disconnected()
{
  qDebug() << "The server closed the connection";
  m_timer->start();
}

void Client::errorOccurred(QAbstractSocket::SocketError socketError)
{
  switch (socketError)
  {
  case QAbstractSocket::RemoteHostClosedError:
    break;
  case QAbstractSocket::HostNotFoundError:
    qCritical() << "Client: The host was not found. Please check the host "
                   "name and port settings.";
    break;
  case QAbstractSocket::ConnectionRefusedError:
    qCritical() << "Client: The connection was refused by the peer. Make "
                   "sure the server is running, and check that the host "
                   "name and port settings are correct.";
    break;
  default:
    qCritical() << QString("Client: The following error occurred: %1.")
                       .arg(m_socket->errorString());
  }

  if (!m_timer->isActive())
    m_timer->start();
}
