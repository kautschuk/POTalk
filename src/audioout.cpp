#include "audioout.h"

#include <QAudioOutput>
#include <QDebug>

AudioOut::AudioOut(const QAudioFormat &format, QObject *parent) : QObject(parent), m_format{format} {}

void AudioOut::queueSamples(const QByteArray &samples) {

    const auto bytesFree = m_audioOut->bytesFree();
    const auto data = samples.left(bytesFree);

    if (m_dev->write(data) < 0) {
        qWarning() << "Error passing samples to QAudioInput.";
        //what now?
    }
}

void AudioOut::changeDevice(const QAudioDeviceInfo &device) {

    if (device.isNull() || !device.isFormatSupported(m_format)) {
        throw "device_null_or_unsupported";
    }

    delete m_audioOut;
    m_audioOut = new QAudioOutput(device, m_format, this);

    m_dev = m_audioOut->start();
}

bool AudioOut::changeDevice(const QString &deviceName) {

    QAudioDeviceInfo device;
    const auto &deviceList = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    for (auto &item : deviceList) {
        if (deviceName == item.deviceName()) {
            device = item;
            break;
        }
    }

    try {
        changeDevice(device);
    } catch (const char *str) {
        return false;
    }

    return true;
}
