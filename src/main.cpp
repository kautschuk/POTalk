#include "audioin.h"
#include "audioout.h"
#include "socket.h"
#include "waveformprovider.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QAudioDeviceInfo>
#include <QStringListModel>

constexpr int c_sampleRate = 16000;
constexpr int c_sampleSize = 16;
constexpr int c_channelCount = 1;

int main(int argc, char **argv) {

    //qputenv("QSG_VISUALIZE", "overdraw");

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    QAudioFormat format;
    format.setSampleRate(c_sampleRate);
    format.setSampleSize(c_sampleSize);
    format.setChannelCount(c_channelCount);
    format.setCodec(QStringLiteral("audio/pcm"));
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QStringListModel inputDevices([&format]() {
        QStringList retval;
        const auto &deviceList = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
        for (auto &item : deviceList) {
            if (item.isFormatSupported(format)) {
                retval << item.deviceName();
            }
        }

        return retval;
    }());
    engine.rootContext()->setContextProperty(QStringLiteral("inputDevices"), &inputDevices);

    QStringListModel outputDevices([&format]() {
        QStringList retval;
        const auto &deviceList = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
        for (auto &item : deviceList) {
            if (item.isFormatSupported(format)) {
                retval << item.deviceName();
            }
        }

        return retval;
    }());
    engine.rootContext()->setContextProperty(QStringLiteral("outputDevices"), &outputDevices);

    AudioIn audioIn(format);
    engine.rootContext()->setContextProperty(QStringLiteral("audioIn"), &audioIn);

    AudioOut audioOut(format);
    engine.rootContext()->setContextProperty(QStringLiteral("audioOut"), &audioOut);

    Socket socket;
    engine.rootContext()->setContextProperty(QStringLiteral("socket"), &socket);
    QObject::connect(&audioIn, &AudioIn::newSamples, &socket, &Socket::sendData);
    QObject::connect(&socket, &Socket::newData, &audioOut, &AudioOut::queueSamples);

    WaveformProvider waveformIn;
    engine.rootContext()->setContextProperty(QStringLiteral("waveformIn"), &waveformIn);
    QObject::connect(&audioIn, &AudioIn::newSamples, &waveformIn, &WaveformProvider::updateWaveformSeries);

    WaveformProvider waveformRecv;
    engine.rootContext()->setContextProperty(QStringLiteral("waveformRecv"), &waveformRecv);
    QObject::connect(&socket, &Socket::newData, &waveformRecv, &WaveformProvider::updateWaveformSeries);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return QApplication::exec();
}
