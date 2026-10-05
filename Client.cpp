#include "Client.h"

Client::Client(QObject* parent)
  : QObject(parent)
{
  m_socket = new QTcpSocket(this);
  connect(m_socket, &QTcpSocket::readyRead, this, &Client::readyRead);
  connect(m_socket, &QTcpSocket::disconnected, this, &Client::disconnected);
}

Client::~Client() {}
void Client::connectToServer(QString& host, int port)
{
  m_socket->connectToHost(host, port);
}

void Client::readyRead()
{
  QByteArray data = m_socket->readAll();
  QString text = QString::fromUtf8(data);
  qDebug() << "Server replied: " << text;
}

void Client::disconnected()
{
  m_socket->close();
  qDebug() << "The server closed the connection";
}

// void Client::sendMessage(const QString& message)
// {
//   QByteArray block;
//   QDataStream out(&block, QIODevice::WriteOnly);
//   out.setVersion(QDataStream::Qt_5_15);
//   out << message;
//   this->write(block);
// }