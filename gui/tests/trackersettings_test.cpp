#include <QtTest>
#include "../src/trackersettings.h"

// To run this test, create a build directory and use CMake:
// $ mkdir build && cd build
// $ cmake .. && make
// $ ./trackersettings_test

class TrackerSettingsTest : public QObject {
    Q_OBJECT
private slots:
    void rawAccelSignal_data();
    void rawAccelSignal();
};

void TrackerSettingsTest::rawAccelSignal_data()
{
    QTest::addColumn<float>("x");
    QTest::addColumn<float>("y");
    QTest::addColumn<float>("z");

    QTest::newRow("basic") << 1.0f << 2.0f << 3.0f;
}

void TrackerSettingsTest::rawAccelSignal()
{
    QFETCH(float, x);
    QFETCH(float, y);
    QFETCH(float, z);

    TrackerSettings settings;
    QSignalSpy spy(&settings, SIGNAL(rawAccelChanged(float,float,float)));

    QVariantMap map;
    map["accx"] = x;
    map["accy"] = y;
    map["accz"] = z;
    settings.setLiveDataMap(map);

    QCOMPARE(spy.count(), 1);
    const QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toFloat(), x);
    QCOMPARE(arguments.at(1).toFloat(), y);
    QCOMPARE(arguments.at(2).toFloat(), z);
}

QTEST_MAIN(TrackerSettingsTest)
#include "trackersettings_test.moc"
