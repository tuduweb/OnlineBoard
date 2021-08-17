#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "PaintingBoard.hpp"
#include "RoomService.hpp"

#if defined (ANDROID)
#include <QtAndroidExtras/QtAndroidExtras>
#include <QtAndroidExtras/QAndroidJniObject>
#else
#include <QBuffer>
#endif

#include <QLocale>
#include <QTranslator>

#include <QDebug>
#include <QDir>

#include <QImage>

#include <QNetworkProxy>

#if defined (ANDROID)
bool checkPermission() {
    QtAndroid::PermissionResult r = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
    if(r == QtAndroid::PermissionResult::Denied) {
        QtAndroid::requestPermissionsSync( QStringList() << "android.permission.WRITE_EXTERNAL_STORAGE" );
        r = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
        if(r == QtAndroid::PermissionResult::Denied) {
             return false;
        }
   }
   return true;
}
#endif


QString currentPath;

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    QNetworkProxyFactory::setUseSystemConfiguration(false);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "OnlineBoard_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    BackendSync::getIns();

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        qInfo() << url << objUrl;
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);




    qDebug() << "path: "  << QDir::currentPath();
    QImage image;
    image.load(":/icons/danger.png");
#if defined (ANDROID)
    qDebug() << "Check Permision : " << checkPermission();

    auto androidContext = QtAndroid::androidContext();

    QAndroidJniObject dir = QAndroidJniObject::fromString(QString(""));

    QAndroidJniObject path = androidContext.callObjectMethod("getExternalFilesDir",
                                                             "(Ljava/lang/String;)Ljava/io/File;",
                                                             dir.object());

    qInfo() << "Path: " + path.toString();

    QDir localDir(path.toString());

    qInfo() << "dir is exsit : " << localDir.exists();
    {
        bool res = localDir.mkpath(path.toString() + "/hello/");
        qInfo() << "mkpath : " << res;
    }

    qInfo() << "image infos : " << image.size();

    bool res = image.save(path.toString() + "/danger.png");
    qInfo() << "save res : " << res;

    engine.rootContext()->setContextProperty("localUrl", path.toString());
    currentPath = path.toString();
#else
    engine.rootContext()->setContextProperty("localUrl", QDir::currentPath());
    currentPath = QDir::currentPath();
#endif

#if 0
    QImage image2(50, 50, QImage::Format_RGB16);
    image2.fill(QColor("red"));

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    //bool res2 = image2.save(&buffer, "image/png");
    bool res2 = image.save(&buffer, "image/png");
    qInfo() << res2;
    buffer.close();

    qInfo() << QLatin1String("data:") + "image/png" + QLatin1String(";base64,") + QLatin1String(ba.toBase64().constData());

    engine.rootContext()->setContextProperty("testImage", image2);
#endif

    qmlRegisterType<RoomService>("obc.qml.Controls", 1, 0, "RoomService");
    
    qmlRegisterType<RoomProviderItem>("obc.qml.Controls", 1, 0, "RoomProviderItem");
    qmlRegisterType<RoomUserItem>("obc.qml.Controls", 1, 0, "RoomUserItem");

    //qmlRegisterType<PaintedItem>("obc.qml.Controls", 1, 0, "APaintedItem");

    qmlRegisterType<PaintingBoard>("obc.qml.Controls", 1, 0, "PaintingBoard");

    engine.load(url);


    return app.exec();
}
