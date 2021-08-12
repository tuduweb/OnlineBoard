#include "painteditem.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QDebug>

#include <QTimer>

#include <QDir>
extern QString currentPath;
#include <QDateTime>

#include "utils/OBCHelpers.hpp"

PaintedItem::PaintedItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_element(0)
    , m_bEnabled(true)
    , m_bPressed(false)
    , m_bMoved(false)
    , m_pen(Qt::black)
    , m_paintMode(0)
    , m_markId(-1)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptTouchEvents(true);

    m_icon.load(":/icons/danger.png");

    //QString path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);


    QString sentMsg = "message test time: %1 cnt: %2 ";
    for(char c = 'a'; c < 'z'; ++c) {
        sentMsg += c;
    }

   QTimer* saveTimer = new QTimer();
   saveTimer->setInterval(100);	//0.5
   
   connect(saveTimer, &QTimer::timeout, [this, sentMsg](){
       static int cnt = 0;
    //    QImage image;
    //    image.load(":/largeImage.jpg");
    //    QString dir = currentPath + "/" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".jpg";
    //    image.save(dir);
    //    qInfo() << "save dir " << dir;
        if(cnt++ < 10)
            return;

        backendSync->sendMessasge(sentMsg.arg(QString::number(QDateTime::currentMSecsSinceEpoch())).arg(cnt - 10));
        qInfo() << sentMsg.arg(QString::number(QDateTime::currentMSecsSinceEpoch())).arg(cnt - 10);
   });
   //saveTimer->start();



    backendSync = new BackendSync();

    /* Json Test */
    QJsonObject obj;
    obj.insert("type", "mark");
    obj.insert("data", "testData");

    //parseAyncMessage(JsonToString(obj));
    qInfo() << JsonToString(obj);

    //parseAyncMessage("{\"type\":\"mark\",\"data\":{\"x\":10,\"y\":20,\"w\":40,\"h\":50}}");
    qInfo() << "{\"type\":\"mark\",\"data\":{\"x\":10,\"y\":20,\"w\":40,\"h\":50}}";

    parseAyncMessage("{\"type\":\"mark\",\"id\":1,\"x\":25,\"y\":25}");

    connect(backendSync, &BackendSync::receivedMessage, this, [=](QHostAddress addr, quint16 port, QString msg){
        parseAyncMessage(msg);
    });

}

PaintedItem::~PaintedItem()
{
    purgePaintElements();
    
    delete backendSync;

}

void PaintedItem::clear()
{
    purgePaintElements();
    update();
}

void PaintedItem::undo()
{
    if(m_elements.size())
    {
        delete m_elements.takeLast();
        update();
    }
}

void PaintedItem::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);

    {
        int size = m_elements.size();
        ElementGroup *element;
        for(int i = 0; i < size; ++i)
        {
            element = m_elements.at(i);
            painter->setPen(element->m_pen);
            painter->drawLines(element->m_lines);
        }
    }

    {
        int size = m_markElms.size();
        MarkElement *element;
        for(int i = 0; i < size; ++i) {
            element = m_markElms.at(i);
            //element->
            QImage image;
            //TODO: 需要找到方法：如何在qml和c++两种形式中，使用qrc资源。
            image.load(":" + element->markUrl());
            painter->drawImage(element->rect(), image);
            //qInfo() << image.size();
        }
    }

    


    if(paintMode() == 1) {
        qInfo() << "drawImage : " << m_icon.size();
        painter->drawImage(QRect(30, 30, 40, 40), m_icon);
    }

}

bool PaintedItem::saveImage()
{
    QPixmap *pix = new QPixmap(500,500);
    //QImage background(50, 50, QImage::Format_RGB16);
    pix->fill(QColor("yellow"));
    QPainter *paint = new QPainter;
    paint->begin(pix);

    paint->setPen(QColor(255,34,255,255));
    paint->drawRect(15,15, 100, 100);

    int size = m_elements.size();
    ElementGroup *element;
    for(int i = 0; i < size; ++i)
    {
        element = m_elements.at(i);
        paint->setPen(element->m_pen);
        paint->drawLines(element->m_lines);
    }

    qInfo() << "paint mode : " << paintMode();
    if(paintMode() == 1) {
        qInfo() << "drawImage : " << m_icon.size();
        paint->drawImage(QRect(30, 30, 40, 40), m_icon);
    }

    pix->save(currentPath + "/jpg" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".jpg", "JPG");

    pix->save(currentPath + "/png" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".png", "PNG");

    qInfo() << "saved Path:" << currentPath;

    paint->end();
    delete paint;
    delete pix;

    return false;
}

bool PaintedItem::saveImage(const QVariant& var) {
    qInfo() << var;
    qInfo() << var.type();

    if(var.type() == QMetaType::QUrl) {

        QPixmap *pix = new QPixmap(500,500);
        //pix->load(var.toUrl().toString());
        //QImage background(50, 50, QImage::Format_RGB16);
        //pix->fill(QColor("yellow"));
        QPainter *paint = new QPainter;
        paint->begin(pix);

        QUrl imageUrl = var.toUrl();
        qInfo() << imageUrl.isValid();
        QImage bg;
        //Android: //content://com.android.providers.media.documents/document/image%3A41829
        //Win10: ////file:///C:/Users/bin/OneDrive/图片/img-6daa1b4cffcd3b247860ff9ef0f663fa.jpg //有毛病 读不出来
        bg.load(var.toUrl().toString());
        //bg.load("C:/Users/bin/OneDrive/图片/img-6daa1b4cffcd3b247860ff9ef0f663fa.jpg");
        bg.scaled(500, 500);
        paint->drawImage(QPoint(0, 0), bg);
        qInfo() << "draw bg size" << bg.size();

        //paint->setPen(QColor(255,34,255,255));
        //paint->drawRect(15,15, 100, 100);

        int size = m_elements.size();
        ElementGroup *element;
        for(int i = 0; i < size; ++i)
        {
            element = m_elements.at(i);
            paint->setPen(element->m_pen);
            paint->drawLines(element->m_lines);
        }

        qInfo() << "paint mode : " << paintMode();
        if(paintMode() == 1) {
            qInfo() << "drawImage : " << m_icon.size();
            paint->drawImage(QRect(30, 30, 40, 40), m_icon);
        }

        pix->save(currentPath + "/jpg" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".jpg", "JPG");

        pix->save(currentPath + "/png" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".png", "PNG");

        qInfo() << "saved Path:" << currentPath;

        paint->end();
        delete paint;
        delete pix;


    }

    return false;
}


void PaintedItem::mousePressEvent(QMouseEvent *event)
{
    m_bMoved = false;
    if(!m_bEnabled || !(event->button() & acceptedMouseButtons()))
    {
        QQuickPaintedItem::mousePressEvent(event);
    }
    else
    {
        if(m_markId >= 0) {
            static auto lastPos = QPointF(0, 0);

            const auto& pos = event->localPos();
            const auto& marks = getMarks();
            QPointF dis = pos - lastPos;

            if(sqrt(pow(dis.x(), 2) + pow(dis.y(), 2)) > 20) {

                m_markElms.append(new MarkElement(
                        marks[m_markId < marks.size() ? m_markId : 0],
                        QRectF(pos - QPointF(25, 25), QSizeF(50, 50))
                        )
                    );
                
                lastPos = pos;

                backendSync->sendMessasge("placed : " + m_markElms.last()->markUrl() + QString(" point(%1, %2)").arg(pos.x()).arg(pos.y()) );

            }else{
                qInfo() << pos << lastPos << "too closed";
            }

            update();

        }else{
            //qDebug() << "mouse pressed";
            m_bPressed = true;
            m_element = new ElementGroup(m_pen);
            m_elements.append(m_element);
            m_lastPoint = event->localPos();
            event->setAccepted(true);
        }

    }

}

void PaintedItem::mouseMoveEvent(QMouseEvent *event)
{
    if(!m_bEnabled || !m_bPressed || !m_element)
    {
        QQuickPaintedItem::mousePressEvent(event);
    }
    else
    {
        //qDebug() << "mouse move";
        m_element->m_lines.append(QLineF(m_lastPoint, event->localPos()));
        m_lastPoint = event->localPos();
        update();
    }
}

void PaintedItem::mouseReleaseEvent(QMouseEvent *event)
{
    if(!m_element || !m_bEnabled || !(event->button() & acceptedMouseButtons()))
    {
        QQuickPaintedItem::mousePressEvent(event);
    }
    else
    {
        //qDebug() << "mouse released";
        m_bPressed = false;
        m_bMoved = false;
        m_element->m_lines.append(QLineF(m_lastPoint, event->localPos()));
        update();

        qInfo() << "el size: " << m_elements.size() << " size: " << m_element->m_lines.size();
    }
}

void PaintedItem::touchEvent(QTouchEvent *event)
{
    QQuickPaintedItem::touchEvent(event);
    qInfo() << event->touchPoints();
}


void PaintedItem::purgePaintElements()
{
    int size = m_elements.size();
    if(size > 0)
    {
        for(int i = 0; i < size; ++i)
        {
            delete m_elements.at(i);
        }
        m_elements.clear();
    }
    m_element = 0;
}

void PaintedItem::initMark()
{
    //可能要造一种数据结构?
}

QStringList PaintedItem::getMarks()
{
    static QStringList urls = {
        "/assets/marks/mark1.png",
        "/assets/marks/mark2.png",
        "/assets/marks/mark3.png"
    };

    // for(const auto& url : urls) {
    //     qInfo() << "url :" << url;
    // }


    return urls;
}


void PaintedItem::paintMark(int _markId, const QPointF& pos) {

    const auto& marks = getMarks();

    if(_markId < 0 || _markId >= marks.size())
    {
        qWarning() << "markId invalid" << _markId << __FILE__ << __LINE__;
        return;
    }

    //暂时不管大小等东西..
    m_markElms.append(new MarkElement(
        marks[_markId],
        QRectF(pos - QPointF(25, 25), QSizeF(50, 50))
        )
    );

    //refresh cache
    update();
    qInfo() << "mark size: " << m_markElms.size();

}


void PaintedItem::parseAyncMessage(const QString& message) {
    QJsonObject obj = JsonFromString(message);//format
    qInfo() << "parseAyncMessage : " << obj.contains("type");
    
    if(obj.contains("type")) {

        if(obj["type"] == "mark") {
            //qInfo() << "parsed : " << obj["data"];
            paintMark(obj["id"].toInt(), {obj["x"].toDouble(), obj["y"].toDouble()});
        }else if(obj["type"] == "bg") {
            //
        }
    }
}