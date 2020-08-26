import QtQuick 2.14
import QtCharts 2.14

ChartView {
    property var waveformProvider
    property color color

    legend.visible: false

    backgroundColor: "transparent"
    backgroundRoundness: 0.0

    ValueAxis {
        id: axisX

        labelFormat: "%d"
        gridVisible: false

        min: 0
        max: waveformProvider.sampleCount //series(0).count doesn't work
    }

    ValueAxis {
        id: axisY

        labelFormat: "%+d"

        tickCount: 3

        //prevent scaling down into the noisefloor and leave 20% headroom
        readonly property int range: Math.max(2048, waveformProvider.autoScaleY * 1.2)

        min: -1 * range
        max: range
    }

    Component.onCompleted: {
        createSeries(ChartView.SeriesTypeLine, "", axisX, axisY);
        series(0).color = color
        series(0).width = 1.0
        series(0).useOpenGL = visible

        waveformProvider.setWaveformSeries(series(0))
    }
}
