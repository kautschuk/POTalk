#ifndef AUDIOIN_H
#define AUDIOIN_H

#include <QAudioFormat>

class QAudioInput;
class QAudioDeviceInfo;
class QIODevice;

class AudioIn final : public QObject {

    Q_OBJECT
    Q_DISABLE_COPY_MOVE(AudioIn)

public:
    explicit AudioIn(const QAudioFormat &format, QObject *parent = nullptr);

    void changeDevice(const QAudioDeviceInfo &device);
    Q_INVOKABLE [[nodiscard]] bool changeDevice(const QString &deviceName);

private:
    const QAudioFormat m_format;

    QAudioInput *m_audioIn = nullptr;
    QIODevice *m_dev = nullptr; //non-owning

    void onReadyRead();

Q_SIGNALS:
    void newSamples(QByteArray samples);
};

#endif // AUDIOIN_H
