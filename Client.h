#ifndef CLIENT_H
#define CLIENT_H
#include <QTcpSocket>

class Client : public QTcpSocket
{
  Q_OBJECT

public:
  Client(QObject* parent);
  ~Client();
  void connectToServer(QString& host, int port);
private slots:
  void disconnected();
  void readyRead();
};

#endif // CLIENT_H
