#ifndef CLIENT_H
#define CLIENT_H
#include <QDataStream>
#include <QTcpSocket>
#include <QThread>
class Client : public QObject
{
  Q_OBJECT
  QDataStream in;
  QTcpSocket* m_socket = nullptr;

public:
  explicit Client(QObject* parent);
  ~Client();
  void connectToServer(QString& host, int port);
  void sendMessage(const QString& text)
  {
    if (m_socket->state() == QAbstractSocket::ConnectedState)
    {
      m_socket->write(text.toUtf8());
      m_socket->flush(); // Выталкиваем данные в сеть сразу
    }
  }
private slots:
  void disconnected();
  void readyRead();
};

#endif // CLIENT_H
