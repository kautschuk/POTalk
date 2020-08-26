#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QUdpSocket>

class QNetworkDatagram;

class Socket : public QObject {

    Q_OBJECT
    Q_DISABLE_COPY_MOVE(Socket)

    Q_PROPERTY(quint16 payloadSize READ payloadSize WRITE setPayloadSize NOTIFY payloadSizeChanged)

    Q_PROPERTY(bool bound READ isBound NOTIFY boundChanged)
    Q_PROPERTY(bool lookupInProgress READ isLookupInProgress NOTIFY lookupInProgressChanged)

public:
    explicit Socket(QObject *parent = nullptr);

    void sendData(const QByteArray &data);

    Q_INVOKABLE void bind(const QString &remoteHost, quint16 port);
    Q_INVOKABLE void unbind();

    quint16 payloadSize() const;
    void setPayloadSize(quint16 size);

    bool isBound() const;
    bool isLookupInProgress() const;

private:
    QUdpSocket m_socket;

    quint16 m_payloadSize = 512;

    QHostAddress m_remoteHost;
    bool m_lookupInProgress = false;

Q_SIGNALS:
    void newData(QByteArray data);

    void payloadSizeChanged();

    void boundChanged();
    void lookupInProgressChanged();

private Q_SLOTS:
    void readPendingDatagrams();
};

#endif // SOCKET_H
