#ifndef WAVEFORMPROVIDER_H
#define WAVEFORMPROVIDER_H

#include <QObject>
#include <QFuture>

namespace QtCharts {
class QAbstractSeries;
class QXYSeries;
}

class WaveformProvider final : public QObject {

    Q_OBJECT
    Q_DISABLE_COPY_MOVE(WaveformProvider)

    Q_PROPERTY(int sampleCount READ sampleCount NOTIFY sampleCountChanged)
    Q_PROPERTY(int autoScaleY READ autoScaleY NOTIFY autoScaleYChanged)

public:
    explicit WaveformProvider(QObject *parent = nullptr);

    Q_INVOKABLE void setWaveformSeries(QtCharts::QAbstractSeries *series);

    void setSampleSize(int bits);

    int sampleCount() const;
    int autoScaleY() const;

private:
    QtCharts::QXYSeries *m_waveformSeries = nullptr;

    int m_sampleSize = 16;

    int m_sampleCount = 0;
    int m_autoScaleY = 0;

    QFuture<void> m_update;

    void updateWaveformSeries_impl(const QByteArray &samples);

Q_SIGNALS:
    void sampleCountChanged();
    void autoScaleYChanged();

public Q_SLOTS:
    void updateWaveformSeries(const QByteArray &samples);
};

#endif // WAVEFORMPROVIDER_H
