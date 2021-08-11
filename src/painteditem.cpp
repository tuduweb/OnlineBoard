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

PaintedItem::PaintedItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_element(0)
    , m_bEnabled(true)
    , m_bPressed(false)
    , m_bMoved(false)
    , m_pen(Qt::black)
    , m_paintMode(0)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptTouchEvents(true);

    m_icon.load(":/icons/danger.png");

    //QString path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);


//    QTimer* saveTimer = new QTimer();
//    saveTimer->setInterval(500);	//0.5
//    connect(saveTimer, &QTimer::timeout, [&](){
//        QImage image;
//        image.load(":/largeImage.jpg");
//        QString dir = currentPath + "/" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".jpg";
//        image.save(dir);
//        qInfo() << "save dir " << dir;
//    });
//    saveTimer->start();

}

PaintedItem::~PaintedItem()
{
    purgePaintElements();
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

    int size = m_elements.size();
    ElementGroup *element;
    for(int i = 0; i < size; ++i)
    {
        element = m_elements.at(i);
        painter->setPen(element->m_pen);
        painter->drawLines(element->m_lines);
    }

    qInfo() << "paint mode : " << paintMode();
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


    paint->end();
    delete paint;
    delete pix;

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
        //qDebug() << "mouse pressed";
        m_bPressed = true;
        m_element = new ElementGroup(m_pen);
        m_elements.append(m_element);
        m_lastPoint = event->localPos();
        event->setAccepted(true);
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


