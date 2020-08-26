#ifndef AUDIOOUT_H
#define AUDIOOUT_H

#include <QAudioFormat>

class QAudioOutput;
class QAudioDeviceInfo;
class QIODevice;

class AudioOut final : public QObject {

    Q_OBJECT
    Q_DISABLE_COPY_MOVE(AudioOut)

public:
    explicit AudioOut(const QAudioFormat &format, QObject *parent = nullptr);

    void changeDevice(const QAudioDeviceInfo &device);
    Q_INVOKABLE [[nodiscard]] bool changeDevice(const QString &deviceName);

private:
    const QAudioFormat m_format;

    QAudioOutput *m_audioOut = nullptr;
    QIODevice *m_dev = nullptr; //non-owning

public Q_SLOTS:
    void queueSamples(const QByteArray &samples);
};

#endif // AUDIOOUT_H
