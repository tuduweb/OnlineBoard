#ifndef PAINTEDITEM_H
#define PAINTEDITEM_H
#include <QQuickPaintedItem>
#include <QVector>
#include <QPointF>
#include <QLineF>
#include <QPen>

#include <QJsonDocument>


class ElementGroup
{
public:
    ElementGroup()
    {
    }

    ElementGroup(const QPen &pen)
        : m_pen(pen)
    {
    }

    ElementGroup(const ElementGroup &e)
    {
        m_lines = e.m_lines;
        m_pen = e.m_pen;
    }

    ElementGroup & operator=(const ElementGroup &e)
    {
        if(this != &e)
        {
            m_lines = e.m_lines;
            m_pen = e.m_pen;
        }
        return *this;
    }

    ~ElementGroup()
    {
    }

    QVector<QLineF> m_lines;
    QPen m_pen;
};


class MarkElement
{
public:
    MarkElement()
    {
    }

    MarkElement(const QString& _url, const QRectF& _rect)
    : m_markUrl(_url), m_rect(_rect)
    {
    }

    QString m_markUrl;
    QRectF m_rect;

    const QString& markUrl() const { return m_markUrl; }
    const QRectF& rect() const { return m_rect; }
};


#include "core/BackendSync.hpp"


class PaintedItem : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(int penWidth READ penWidth WRITE setPenWidth)
    Q_PROPERTY(QColor penColor READ penColor WRITE setPenColor)


    Q_PROPERTY(int paintMode READ paintMode WRITE setPaintMode NOTIFY paintModeChanged)
    Q_PROPERTY(int markId READ markId WRITE setMarkId NOTIFY markIdChanged)


public:
    PaintedItem(QQuickItem *parent = 0);
    ~PaintedItem();

    bool isEnabled() const{ return m_bEnabled; }
    void setEnabled(bool enabled){ m_bEnabled = enabled; }

    int penWidth() const { return m_pen.width(); }
    void setPenWidth(int width) { m_pen.setWidth(width); }

    QColor penColor() const { return m_pen.color(); }
    void setPenColor(QColor color) { m_pen.setColor(color); }

    int paintMode() const { return m_paintMode; }
    void setPaintMode(int mode) { m_paintMode = mode; m_bEnabled = !mode; emit paintModeChanged(); update(); }

    int markId() const { return m_markId; }
    void setMarkId(int id) { m_markId = id; emit markIdChanged(); qInfo() << "mark Id:" << m_markId; }


    Q_INVOKABLE void clear();
    Q_INVOKABLE void undo();

    void paint(QPainter *painter);

    Q_INVOKABLE bool saveImage();
    Q_INVOKABLE bool saveImage(const QVariant& var);


    //////////////////////////暂时在这个里面搞所有的测试 最后再抽离
    void initMark();
    Q_INVOKABLE QStringList getMarks();//QUrl在qml中搞不成model..应该是说QVector搞不成.


    /////////////////////////加入图形的方法
    void paintMark(int _markId, const QPointF& pos);


public slots:
    void parseAyncMessage(const QString& message);

signals:
    void paintModeChanged();
    void markIdChanged();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void purgePaintElements();



    //根据QQuickPinchArea重写pinchevent
    void touchEvent(QTouchEvent *event);



protected:
    QPointF m_lastPoint;
    QVector<ElementGroup*> m_elements;
    ElementGroup * m_element; // the Current ElementGroup

    QVector<MarkElement*> m_markElms;//之后需要放在一起

    bool m_bEnabled;
    bool m_bPressed;
    bool m_bMoved;
    QPen m_pen; // the Current Pen

    int m_paintMode;
    int m_markId;

    QImage m_icon;


    BackendSync* backendSync;
};

#endif // PAINTEDITEM_H
