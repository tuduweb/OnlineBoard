#ifndef PAINTEDITEM_H
#define PAINTEDITEM_H
#include <QQuickPaintedItem>
#include <QVector>
#include <QPointF>
#include <QLineF>
#include <QPen>

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

class PaintedItem : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(int penWidth READ penWidth WRITE setPenWidth)
    Q_PROPERTY(QColor penColor READ penColor WRITE setPenColor)


    Q_PROPERTY(int paintMode READ paintMode WRITE setPaintMode NOTIFY paintModeChanged)


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


    Q_INVOKABLE void clear();
    Q_INVOKABLE void undo();

    void paint(QPainter *painter);

    Q_INVOKABLE bool saveImage();


    //////////////////////////暂时在这个里面搞所有的测试 最后再抽离
    void initMark();
    Q_INVOKABLE QStringList getMarks();//QUrl在qml中搞不成model..应该是说QVector搞不成.



signals:
    void paintModeChanged();

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
    bool m_bEnabled;
    bool m_bPressed;
    bool m_bMoved;
    QPen m_pen; // the Current Pen

    int m_paintMode;

    QImage m_icon;
};

#endif // PAINTEDITEM_H
