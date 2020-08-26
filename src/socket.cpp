#include "socket.h"

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QHostInfo>

Socket::Socket(QObject *parent) : QObject(parent) {

    QObject::connect(&m_socket, &QAbstractSocket::stateChanged, [this]() {
        Q_EMIT boundChanged();
    });

    QObject::connect(&m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
                     [this](const QAbstractSocket::SocketError err) {
        qWarning() << err;
        m_socket.abort();
    });

    QObject::connect(&m_socket, &QUdpSocket::readyRead, this, &Socket::readPendingDatagrams);
}

void Socket::sendData(const QByteArray &data) {

    if (!isBound()) {
        return;
    }

    for (int i=0; i<data.size(); i+=m_payloadSize) {
        const auto segment = data.mid(i, m_payloadSize);
        if (m_socket.writeDatagram(segment, segment.size(), m_remoteHost, m_socket.localPort()) < 0) {
            qWarning() << "Error sending datagram.";
            break;
        }
    }
}

void Socket::bind(const QString &remoteHost, quint16 port) {

    //mustn't be called while another lookup is in progress
    Q_ASSERT(!m_lookupInProgress);

    //try constructing an IPv4 or IPv6 address based on the string
    if (auto addr = QHostAddress(remoteHost); !addr.isNull()) {
        m_remoteHost.swap(addr);
        m_socket.bind(port, QAbstractSocket::DontShareAddress);
    } else { //try DNS lookup
        m_lookupInProgress = true;
        Q_EMIT lookupInProgressChanged();

        QHostInfo::lookupHost(remoteHost, [this, port](const QHostInfo &info) {
            m_lookupInProgress = false;
            Q_EMIT lookupInProgressChanged();

            if (!info.addresses().isEmpty()) {
                //use the first address
                m_remoteHost = info.addresses().at(0);
                m_socket.bind(port, QAbstractSocket::DontShareAddress);
            } else {
                //no luck
                qWarning() << info.errorString();
            }
        });
    }
}

void Socket::unbind() {

    m_socket.close();
}

quint16 Socket::payloadSize() const {

    return m_payloadSize;
}

void Socket::setPayloadSize(quint16 size) {

    if (m_payloadSize == size) {
        return;
    }

    Q_EMIT payloadSizeChanged();
}

bool Socket::isBound() const {

    return m_socket.state() == QAbstractSocket::BoundState;
}

bool Socket::isLookupInProgress() const {

    return m_lookupInProgress;
}

void Socket::readPendingDatagrams() {

    //aggregate payloads of pending datagrams
    QByteArray buffer;

    while (m_socket.hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_socket.receiveDatagram();

        if (datagram.isValid()) {
            buffer.append(datagram.data());
        } else {
            qWarning() << "Invalid datagram received.";
            //what now?
        }
    }

    Q_EMIT newData(buffer);
}
