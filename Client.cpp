#include "Client.h"

Client::Client(QObject* parent)
  : QTcpSocket(parent)
{}

Client::~Client() {}
void Client::connectToServer(QString& host, int port)
{
  this->connectToHost(host, port);
}