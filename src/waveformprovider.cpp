#include "waveformprovider.h"

#include <QXYSeries>
#include <QtConcurrentRun>

namespace {

//assumes signed
qreal byteArrayToReal(const QByteArray &ba) {

    Q_ASSERT(ba.size() <= static_cast<int>(sizeof(qreal)));

    qreal retval = 0;

#if 1 // little endian
    for (int i=ba.size()-1; i>=0; --i) {
        retval += ba.at(i) << (i*sizeof(char));
#else // big endian
    for (int i=0; i<ba.size(); ++i) {
        retval += ba.at(i) << ((ba.size()-i)*sizeof(char));
#endif
    }

    return retval;
}

} // namespace

WaveformProvider::WaveformProvider(QObject *parent) : QObject(parent) {}

void WaveformProvider::setWaveformSeries(QtCharts::QAbstractSeries *series) {

    m_waveformSeries = qobject_cast<QtCharts::QXYSeries*>(series);

    //QML is expected to pass valid 'XYSeries'
    Q_CHECK_PTR(m_waveformSeries);
}

void WaveformProvider::setSampleSize(int bits) {

    m_sampleSize = bits;
}

int WaveformProvider::sampleCount() const {

    return m_sampleCount;
}

int WaveformProvider::autoScaleY() const {

    return m_autoScaleY;
}

void WaveformProvider::updateWaveformSeries_impl(const QByteArray &samples) {

    if (m_waveformSeries == nullptr) {
        return;
    }

    //bytes per sample
    const int bps = m_sampleSize / 8;

    QVector<QPointF> points;
    points.reserve(samples.size()/bps);

    qreal maxVal = 0;

    for (int i=0; i<=samples.size()-bps; i+=bps) {
        const qreal val = byteArrayToReal(samples.mid(i,bps));

        if (maxVal < qAbs(val)) {
            maxVal = qAbs(val);
        }

        const QPointF p(points.size(), val);
        points.append(p);
    }

    if (m_sampleCount != points.size()) {
        m_sampleCount = points.size();
        Q_EMIT sampleCountChanged();
    }

    if (!qFuzzyCompare(m_autoScaleY, maxVal)) {
        m_autoScaleY = maxVal;
        Q_EMIT autoScaleYChanged();
    }

    m_waveformSeries->replace(points);
}

void WaveformProvider::updateWaveformSeries(const QByteArray &samples) {

    //prevent parallel jobs
    if (m_update.isFinished()) {
        QFuture<void> future = QtConcurrent::run(this, &WaveformProvider::updateWaveformSeries_impl, samples);
    }
}
