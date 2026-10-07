#ifndef CLIENT_H
#define CLIENT_H
#include <QCoreApplication>
#include <QDataStream>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>
class Client : public QObject
{
  Q_OBJECT
  QDataStream in;
  QTcpSocket* m_socket = nullptr;
  QString currentMessage;

public:
  explicit Client(QObject* parent);
  ~Client();
  void connectToServer(const QString& host, int port);

private slots:
  void errorOccurred(QAbstractSocket::SocketError socketError);
  void disconnected();
  void readyRead();
};

#endif // CLIENT_H
