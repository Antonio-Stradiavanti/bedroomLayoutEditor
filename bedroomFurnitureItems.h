#ifndef BEDROOMFURNITUREITEMS_H
#define BEDROOMFURNITUREITEMS_H

#include <QGraphicsItem>
#include <QPainter>
#include <QDebug>
#include <QKeyEvent>

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>

#include <QCursor>

enum class BedroomFurniture {
    None, Wall, Window,
    Bed, Chair,
    Piano, Door,
    Desk, Wardrobe
};
// TODO Убрать флаг is_horizontal
class BedroomFurnitureItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    BedroomFurnitureItem(double x, double y, double w, double h, QGraphicsItem* parent = nullptr)
        : QGraphicsItem{parent}
        , is_horizontal{false}

        , topLetfPoint{}
        , rect{x, y, w, h}

        , type{BedroomFurniture::None}
    {        
        m_erasing = false;

        brush = QBrush(Qt::blue, Qt::SolidPattern);
        pen = QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

        setFlags(
            QGraphicsItem::ItemIsMovable |
            QGraphicsItem::ItemIsSelectable |
            QGraphicsItem::ItemIsFocusable
            );
        this->setAcceptHoverEvents(true);
        this->setToolTip("");
    }
    virtual ~BedroomFurnitureItem() {}
    /* Сеттеры */
    void setTopLetfPoint(QPointF topLetfPoint) {
        this->topLetfPoint = topLetfPoint;
    }
    void setBottomRightPoint(QPointF bottomRightPoint) {
        rect.setTopLeft(topLetfPoint);
        rect.setBottomRight(bottomRightPoint);
    }

    void setBrush(const QBrush& brush) {
        this->brush = brush;
    }
    void setPen(const QPen& pen) {
        this->pen = pen;
    }
    void setErasing(bool erasing) {
        m_erasing = erasing;
    }
    /* Геттеры */
    BedroomFurniture getType() {
        return type;
    }
    bool getIsHorizontal() {
        return is_horizontal;
    }
    QRectF getRect() {
        return mapRectToScene(rect);
    }
    /* Переопределил */

    // void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    // void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    // void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    /* Методы */
    void rotateFromHorizontalToVertical() {
        is_horizontal = true;
        setTransformOriginPoint(rect.center());
        setRotation(-90);
        double dx = 0.5*(rect.height() - rect.width()),
            dy = 0.5*(rect.width() - rect.height());

        moveBy(dx, dy);
    }
public slots:
    void on_erasingSignal() {
        m_erasing = true;
    }
    void on_erasedSignal() {
        m_erasing = false;
    }
    // virtual void on_parametersChanged() = 0;

signals:
    void enterHover();
    void leaveHover();

protected:
    /* Свойства */
    BedroomFurniture type;

    QPointF topLetfPoint;
    QRectF rect;

    bool is_horizontal;

    QBrush brush;
    QPen pen;

    bool m_erasing;

    /* Методы */
    QRectF boundingRect() const override {
        return rect;
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);

        painter->setBrush(brush);
        painter->setPen(pen);
    }
    /* Обработчики событий */
    void keyPressEvent(QKeyEvent* event) override
    {
        switch(event->key()) {
        case Qt::Key_Right:
            this->moveBy(10, 0);
            break;
        case Qt::Key_Left:
            this->moveBy(-10, 0);
            break;
        case Qt::Key_Up:
            this->moveBy(0, -10);
            break;
        case Qt::Key_Down:
            this->moveBy(0, 10);
            break;
        default:
            break;

        }

        update();
    }
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override
    {
        if (m_erasing) {
            setCursor(
                QCursor(
                    QPixmap(":/cursors/pointer_cursor_erasing.png")
                    )
                );
        }
        else {
            setCursor(
                QCursor(
                    QPixmap(":/cursors/pointer_cursor_hovered.png")
                    )
                );
        }
        QPointF scenePoint = this->mapToScene(rect.topLeft());

        if (type == BedroomFurniture::Bed && is_horizontal) {
                setToolTip(
                    QString("ширина w=%1, высота h=%2")
                    .arg(rect.height())
                    .arg(rect.width())
                );
            }
            else {
                setToolTip(
                    QString("ширина w=%1, высота h=%2")
                    .arg(rect.width())
                    .arg(rect.height())
                );
            }
        emit enterHover();
        QGraphicsItem::hoverEnterEvent(event);
    }
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override
    {
        setCursor(
            QCursor(
                QPixmap(":/cursors/pointer_cursor.png")
                )
            );

        emit leaveHover();
        QGraphicsItem::hoverLeaveEvent(event);
    }
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        setCursor(
            QCursor(
                QPixmap(":/cursors/move_cursor_selected.png")
                )
            );
        QGraphicsItem::mousePressEvent(event);
    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override
    {
        QGraphicsItem::mouseMoveEvent(event);
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override
    {
        setCursor(
            QCursor(
                QPixmap(":/cursors/pointer_cursor_hovered.png")
                )
            );
        QGraphicsItem::mouseReleaseEvent(event);
    }
};

class WallTypeItem : public BedroomFurnitureItem {
public:
    WallTypeItem(double x, double y, double w, double h, QGraphicsItem* parent = nullptr)
        : BedroomFurnitureItem(x, y, w, h, parent)
    {
        brick = wallThickness / 3.0;
    }
    /* Сеттеры */
    static void setWallThickness(double newWallThickness) {
        WallTypeItem::wallThickness = newWallThickness;
    }
    /* Геттеры */
    static double getWallThickness() {
        return WallTypeItem::wallThickness;
    }
protected:
    static double wallThickness;
    double brick;
};


class WallItem : public WallTypeItem {
public:
    WallItem(double x, double y, double w, double h, QGraphicsItem* parent = nullptr)
        : WallTypeItem(x, y, w, h, parent)
    {
        this->type = BedroomFurniture::Wall;
    }
private:
    /* Методы */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override
    {
        BedroomFurnitureItem::paint(painter, option, widget);
        painter->setBrush(brush);
        painter->drawRect(rect);
    }
    QPainterPath shape() const override {
        QPainterPath path;
        path.addRect(rect);
        return path;
    }
};

class WindowItem : public WallTypeItem {
public:
    WindowItem(double x, double y, double w, double h, QGraphicsItem* parent = nullptr)
        : WallTypeItem(x, y, w, h, parent)
    {
        this->type = BedroomFurniture::Window;

        const double
            &rx = rect.topLeft().x(),
            &ry = rect.topLeft().y(),
            &rw = rect.width(),
            &rh = rect.height();
        rect1 = QRectF(
            rect.topLeft(),
            QSizeF(brick, rh)
            );
        rect2 = QRectF(
            QPointF(rx + brick, ry + brick),
            QSizeF(brick, brick)
            );
        rect3 = QRectF(
            QPointF(rx + rw - 2.0*brick, ry + brick),
            QSizeF(brick, brick)
            );
        rect4 = QRectF(
            QPointF(rx + rw - brick, ry)
            ,
            QSizeF(brick, rh)
            );
        line = QLineF(
            QPointF(rx + 2.0*brick, ry + 0.5*rh)
            ,
            QPointF(rx + rw - 2.0*brick, ry + 0.5*rh)
        );
    }
private:
    /* Свойства */
    QLineF line;
    QRectF rect1, rect2, rect3, rect4;

    /* Методы */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override
    {
        BedroomFurnitureItem::paint(painter, option, widget);

        painter->drawRect(rect1);
        painter->drawRect(rect2);
        painter->drawLine(line);
        painter->drawRect(rect3);
        painter->drawRect(rect4);

        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(rect);

    }
    QPainterPath shape() const override {
        QPainterPath path;
        path.addRect(rect);

        return path;
    }

};

class DoorItem : public WallTypeItem {
public:
    DoorItem(double x, double y, double w, double h, QGraphicsItem* parent = nullptr)
        : WallTypeItem(x, y, w, h, parent)
    {
        this->type = BedroomFurniture::Door;

        const double
            &rx = rect.topLeft().x(),
            &ry = rect.topLeft().y(),
            &rw = rect.width(),
            &rh = rect.height();

        rect1 = QRectF(
            rect.topLeft(),
            QSizeF(brick, rh)
            );
        rect2 = QRectF(
            QPointF(rx + brick, ry)
            ,
            QSizeF(rw - 2.0*brick, brick)
            );
        rect3 = QRectF(
            QPointF(rx + rw - brick, ry)
            ,
            rect1.size()
            );
        rect4 = QRectF(
            QPointF(rx + brick, ry + brick)
            ,
            QSizeF(brick, brick)
        );
        rect5 = QRectF(
            QPointF(rx + rw - 2.0*brick, ry + brick)
            ,
            rect4.size()
        );
    }
private:
    /* Свойства */
    QRectF rect1, rect2, rect3, rect4, rect5;

    /* Методы */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override
    {
        BedroomFurnitureItem::paint(painter, option, widget);

        painter->drawRect(rect1);
        painter->drawRect(rect2);
        painter->drawRect(rect3);
        painter->drawRect(rect4);
        painter->drawRect(rect5);

        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(rect);

    }
    QPainterPath shape() const override
    {
        QPainterPath path;
        path.addRect(rect);
        path.addRect(rect1);
        path.addRect(rect2);
        path.addRect(rect3);
        path.addRect(rect4);
        path.addRect(rect5);
        return path;
    }
};

class ChairItem : public BedroomFurnitureItem {
public:
    ChairItem(double x, double y, double w, double h, QGraphicsItem* parent = nullptr)
        : BedroomFurnitureItem(x, y, w, h, parent)
    {
        this->type = BedroomFurniture::Chair;
        const double
            &rx = rect.topLeft().x(),
            &ry = rect.topLeft().y(),
            &rw = rect.width(),   // 23/15*seat
            &rh = rect.height(); // rh = 1.1*seat
        // d -> длина стороны квадрата, оп. скр угол прям.
        d = chairParamSeatSize / 12.0;
        double armSideM = chairParamSeatSize / 15.0; // 4 если seat = 60

        QSizeF armRestSize = QSizeF(chairParamSeatSize / 3.0, 0.7*chairParamSeatSize); // Толстые подлокотники
        QSizeF armSize = QSizeF(0.2*chairParamSeatSize, 1.1*chairParamSeatSize); // Тонкие подлокотники
        QSizeF seatBackSize = QSizeF(0.4*chairParamSeatSize + chairParamSeatSize, 0.3*chairParamSeatSize);

        chairSeat = QRectF( // Седушка
            QPointF(rx + (4.0/15.0)*chairParamSeatSize, ry),
            QSizeF(chairParamSeatSize, chairParamSeatSize)
            );
        chairSeatBack = QRectF( // Спинка
            QPointF(rx + armSideM, ry)
            ,
            seatBackSize
        );
        line = QLineF( // Полоска на спинке
            QPointF(rx + armSideM, ry + 0.2*chairParamSeatSize),
            QPointF(rx + rw - armSideM, ry + 0.2*chairParamSeatSize)
            );
        chairArmRest1 = QRectF(
            QPointF(rx , ry + 0.2*chairParamSeatSize)
            ,
            armRestSize
            );
        chairArmRest2 = QRectF(
            QPointF(rx + (18.0/15.0)*chairParamSeatSize, ry + 0.2*chairParamSeatSize)
            ,
            armRestSize
            );
        chairArm1 = QRectF(
            QPointF(rx + armSideM, ry)
            ,
            armSize
            );
        chairArm2 = QRectF(
            QPointF(rx + rw - armSize.width() -  armSideM, ry)
            ,
            armSize
            );
    }
    /* Сеттеры */
    static void setChairParamSeatSize(double p) {
        chairParamSeatSize = p;
    }
    /* Геттеры */
    static double getChairParamSeatSize() {
        return chairParamSeatSize;
    }
private:
    static double chairParamSeatSize;
    /* Свойства */
    double d;
    QRectF chairSeat, chairArm1, chairArm2, chairArmRest1, chairArmRest2, chairSeatBack;
    QLineF line;
    /* Методы */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override
    {
        BedroomFurnitureItem::paint(painter, option, widget);


        painter->drawRoundedRect(chairSeat, d, d);
        painter->drawRoundedRect(chairArm1, d, d);
        painter->drawRoundedRect(chairArm2, d, d);
        painter->drawRoundedRect(chairArmRest1, d, d);
        painter->drawRoundedRect(chairArmRest2, d, d);
        painter->drawRoundedRect(chairSeatBack, d, d);
        painter->drawLine(line);

        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(rect);

    }
    QPainterPath shape() const override
    {
        QPainterPath path;
        path.addRect(rect);
        return path;
    }
};

class BedItem : public BedroomFurnitureItem {
public:
    BedItem(double x, double y, double w, double h, QGraphicsItem* parent = nullptr)
        : BedroomFurnitureItem(x, y, w, h, parent)

    {
        this->type = BedroomFurniture::Bed;

        const double
            &rx = rect.topLeft().x(),
            &ry = rect.topLeft().y(),
            &rw = rect.width(),
            &rh = rect.height();
        // bh -> Большая из сторон, bw -> Меньшая из сторон

        cw = 0.45*rh;
        ch = 0.3*rh;

        d = 0.1*rh;

        double topCushionMargin = 0.03*rh;
        double sideCushionMargins = 0.5*(rw - cw);

        double dy = 3.0*topCushionMargin + ch;

        cushion = QRectF(
            QPointF(rx + sideCushionMargins, ry + topCushionMargin)
            ,
            QSizeF(cw, ch)
            );

        bedSpread = QPainterPath();
        double sy = ry + dy;

        bedSpread.moveTo(rx, ry + rh);
        bedSpread.arcTo(rx, sy, d, d, 180, -90);
        bedSpread.lineTo(rx + rw - d, sy);
        bedSpread.arcTo(rx + rw - d, sy, d, d, 90, -90);
        bedSpread.lineTo(rx + rw, ry + rh);
        bedSpread.lineTo(rx, ry + rh);

        double spacingBetweenLines = 0.05*rh;

        line1 = QLineF(
            QPointF(rx, sy + spacingBetweenLines)
            ,
            QPointF(rx + rw, sy + spacingBetweenLines)
            );
        line2 = QLineF(
            QPointF(rx, sy + 2.0*spacingBetweenLines)
            ,
            QPointF(rx + rw, sy + 2.0*spacingBetweenLines)
            );
    }
    /* Сеттеры */
    static void setBedParamHeight(double p) {
        bedParamHeight = p;
    }
    /* Геттеры */
    static double getBedParamHeight() {
        return bedParamHeight;
    }
private:
    static double bedParamHeight;
    /* Свойства */
    double cw, ch;
    //double bw, bh;
    double d;

    QPainterPath bedSpread;
    QRectF cushion;
    QLineF line1, line2;

    /* Методы */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override
    {
        BedroomFurnitureItem::paint(painter, option, widget);

        painter->drawRect(rect);
        painter->drawRoundedRect(cushion, 0.5*d, 0.5*d);
        painter->drawPath(bedSpread);
        painter->drawLine(line1);
        painter->drawLine(line2);

    }
    QPainterPath shape() const override
    {
        QPainterPath path;
        path.addRect(rect);
        return path;
    }

};

class PianoItem : public BedroomFurnitureItem {
public:
    PianoItem(double x, double y, double w, double h, QGraphicsItem* parent = nullptr)
        : BedroomFurnitureItem(x, y, w, h, parent)
    {
        this->type = BedroomFurniture::Piano;

        const double
            &rx = rect.topLeft().x(),
            &ry = rect.topLeft().y(),
            &rw = rect.width(),
            &rh = rect.height();

        double
              lh = 0.6*pianoParamHeight
            , topKeysM = 0.25*(pianoParamHeight - lh)
            , sideKeysM = 0.1875*(pianoParamHeight - lh)
            , topChairM = 0.2*pianoParamHeight
            , sideChairM = 0.5*pianoParamHeight
        ;

        pianoRect = QRectF(
            rect.topLeft()
            ,
            QSizeF(
                rw,
                pianoParamHeight
            )
        );
        keysRect = QRectF(
            QPointF(rx + sideKeysM, ry + lh + topKeysM)
            ,
            QSizeF(rw - 2.0*sideKeysM, pianoParamHeight - lh - topKeysM)
            );

        line = QLineF(
            QPointF(rx, ry + lh),
            QPointF(rx + rw, ry + lh)
        );

        pianoChairRect = QRectF(
            QPointF(rx + sideChairM, ry + pianoParamHeight + topChairM)
            ,
            QSizeF(rw - 2.0*sideChairM, 0.5*sideChairM)
            );
    }
    static void setPianoParamHeight(double p) {
        pianoParamHeight = p;
    }
    static double getPianoParamHeight() {
        return pianoParamHeight;
    }
private:
    static double pianoParamHeight;
    /* Свойства */
    QRectF pianoRect, keysRect, pianoChairRect;
    QLineF line;
    /* Методы */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override
    {
        BedroomFurnitureItem::paint(painter, option, widget);

        painter->drawRect(pianoRect);
        painter->drawLine(line);

        painter->setBrush(Qt::white);
        painter->drawRect(keysRect);

        painter->setBrush(brush);
        painter->drawRect(pianoChairRect);

        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(rect);

    }
    QPainterPath shape() const override
    {
        QPainterPath path;
        path.addRect(rect);
        return path;
    }
};

class DeskItem : public BedroomFurnitureItem {
public:
    DeskItem(double x, double y, double w, double h, QGraphicsItem* parent = nullptr)
        : BedroomFurnitureItem(x, y, w, h, parent)

    {
        this->type = BedroomFurniture::Desk;

        d = deskParamHeight / 12.0;

        const double
            &rx = rect.topLeft().x(),
            &ry = rect.topLeft().y(),
            &rw = rect.width(),
            &rh = rect.height();

        QSizeF armSize = QSizeF(
            deskParamHeight / 3.0, 0.5*deskParamHeight + d
            );

        deskBoard = QRectF(
            rect.topLeft(),
            QSizeF(rw, deskParamHeight)
            );
        deskChairSeat = QRectF(
            QPointF(rx + deskParamHeight, ry + deskParamHeight)
            ,
            QSizeF(deskParamHeight, 0.5*deskParamHeight)
            );

        deskChairArmRest1 = QRectF(
            QPointF(
                rx + (11.0/15.0)*deskParamHeight,
                ry + deskParamHeight - d
            )
            ,
            armSize
            );
        deskChairArmRest2 = QRectF(
            QPointF(
                rx + rw - (11.0/15.0)*deskParamHeight - deskParamHeight / 3.0,
                ry + deskParamHeight - d
                )
            ,
            armSize
            );

        double
            seatBackTopM = 0.5*deskParamHeight - 0.1*deskParamHeight
            , seatBackSideM = 0.8*deskParamHeight;

        deskChairSeatBack = QRectF(
            QPointF(rx + seatBackSideM, ry + deskParamHeight + seatBackTopM)
            ,
            QSizeF(1.4*deskParamHeight, (4.0/15.0)*deskParamHeight)
            );

        line = QLineF(
            QPointF(rx + seatBackSideM, ry + 1.5*deskParamHeight)
            ,
            QPointF(rx + rw - seatBackSideM, ry + 1.5*deskParamHeight)
            );
        double cdx = rx + seatBackSideM + armSize.width() + d,
               cdy = ry + deskParamHeight;

        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 4; ++j) {
                cross.moveTo(cdx + j*2.0*d, cdy + i*2.0*d);
                cross.lineTo(cdx + d + j*2.0*d, cdy + d + i*2.0*d);

                cross.moveTo(cdx + d + j*2.0*d, cdy + i*2.0*d);
                cross.lineTo(cdx + j*2.0*d, cdy + d + i*2.0*d);
            }
        }

    }
    static void setDeskParamHeight(double p) {
        deskParamHeight = p;
    }
    static double getDeskParamHeight() {
        return deskParamHeight;
    }
private:
    static double deskParamHeight;
    /* Свойства */
    double d;
    QRectF deskBoard, deskChairSeat, deskChairArmRest1, deskChairArmRest2, deskChairSeatBack;
    QLineF line;
    QPainterPath cross;
    /* Методы */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override
    {
        BedroomFurnitureItem::paint(painter, option, widget);

        painter->drawRect(deskChairSeat);
        painter->drawRoundedRect(deskChairArmRest1, d, d);
        painter->drawRoundedRect(deskChairArmRest2, d, d);
        painter->drawRoundedRect(deskChairSeatBack, d, d);
        painter->drawRect(deskBoard);

        painter->drawLine(line);

        painter->drawPath(cross);

        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(rect);

    }
    QPainterPath shape() const override
    {
        QPainterPath path;
        path.addRect(rect);
        return path;
    }
};

class WardrobeItem : public BedroomFurnitureItem {
public:
    WardrobeItem(double x, double y, double w, double h, QGraphicsItem* parent = nullptr)
        : BedroomFurnitureItem(x, y, w, h, parent)
    {
        this->type = BedroomFurniture::Wardrobe;
        const double
            &rx = rect.topLeft().x(),
            &ry = rect.topLeft().y(),
            &rw = rect.width(),
            &rh = rect.height();
        line = QLineF(
            QPointF(rx, ry + rh - 0.1*rh),
            QPointF(rx + rw, ry + rh - 0.1*rh)
            );
    }
    /* Сеттеры */
    static void setWardrobeParamHeight(double p) {
        wardrobeParamHeight = p;
    }
    /* Геттеры */
    static double getWardrobeParamHeight() {
        return wardrobeParamHeight;
    }
private:
    static double wardrobeParamHeight;
    /* Свойства */
    QLineF line;
    /* Методы */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override
    {
        BedroomFurnitureItem::paint(painter, option, widget);

        painter->drawRect(rect);
        painter->drawLine(line);

    }
    QPainterPath shape() const override
    {
        QPainterPath path;
        path.addRect(rect);
        return path;
    }
};
#endif // BEDROOMFURNITUREITEMS_H
