#include "audioin.h"

#include <QAudioInput>

AudioIn::AudioIn(const QAudioFormat &format, QObject *parent) : QObject(parent), m_format{format} {}

void AudioIn::changeDevice(const QAudioDeviceInfo &device) {

    if (device.isNull() || !device.isFormatSupported(m_format)) {
        throw "device_null_or_unsupported";
    }

    delete m_audioIn;
    m_audioIn = new QAudioInput(device, m_format, this);

    m_dev = m_audioIn->start();
    QObject::connect(m_dev, &QIODevice::readyRead, this, &AudioIn::onReadyRead);
}

bool AudioIn::changeDevice(const QString &deviceName) {

    QAudioDeviceInfo device;
    const auto &deviceList = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
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

void AudioIn::onReadyRead() {

    const auto bytesReady = m_audioIn->bytesReady();
    const auto data = m_dev->read(bytesReady);

    Q_EMIT newSamples(data);
}
