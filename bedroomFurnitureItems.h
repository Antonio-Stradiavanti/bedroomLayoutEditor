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
    enum class UsingBrush {
        Global, Local
    };
    enum class UsingPen {
        Global, Local
    };

    BedroomFurnitureItem(double x, double y, double w, double h, QGraphicsItem* parent = nullptr)
        : QGraphicsItem{parent}
        , is_rotated{false}
        , m_erasing{false}

        , usedBrush{UsingBrush::Global}
        , usedPen{UsingPen::Global}

        , topLetfPoint{}
        , rect{x, y, w, h}

        , type{BedroomFurniture::None}
    {        

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
    /* Виртуальные методы */
    virtual void redrawMe() = 0;
    /* Сеттеры */
    static void setGlobalBrush(const QBrush brush) {
        globalBrush = brush;
    }
    static void setGlobalPen(const QPen pen) {
        globalPen = pen;
    }
    void setUsedBrush(UsingBrush usedBrush) {
        this->usedBrush = usedBrush;
    }
    void setUsedPen(UsingPen usedPen) {
        this->usedPen = usedPen;
    }
    void setBottomRightPoint(QPointF bottomRightPoint) {
        rect.setTopLeft(topLetfPoint);
        rect.setBottomRight(bottomRightPoint);
    }
    void setBrush(const QBrush brush) {
        this->brush = brush;
    }
    void setPen(const QPen pen) {
        this->pen = pen;
    }
    void setErasing(bool erasing) {
        m_erasing = erasing;
    }
    /* Геттеры */
    static QBrush getGlobalBrush() {
        return globalBrush;
    }
    static QPen getGlobalPen() {
        return globalPen;
    }
    QBrush getBrush() {
        return brush;
    }
    QPen getPen() {
        return pen;
    }
    BedroomFurniture getType() {
        return type;
    }
    bool getIsRotated() {
        return is_rotated;
    }
    QPointF getRectTopLeft() {
        return this->mapToScene(rect.topLeft());
    }
    QRectF getRect() {
        return this->mapRectToScene(rect);
    }
    /* Методы */
    void rotateMe(double dg) {
        setTransformOriginPoint(rect.center());
        setRotation(dg);
        double dx, dy;
        if (qAbs(dg) == 90) {
            is_rotated = true;
            dx = 0.5*(rect.height() - rect.width());
            dy = 0.5*(rect.width() - rect.height());
            moveBy(dx, dy);
        }
        else if (dg == 0) {
            is_rotated = false;
            dx = 0.5*(rect.width() - rect.height());
            dy = 0.5*(rect.height() - rect.width());
            moveBy(dx, dy);
        }

    }
public slots:
    void on_erasingSignal() {
        m_erasing = true;
    }
    void on_erasedSignal() {
        m_erasing = false;
    }

signals:
    void enterHover();
    void leaveHover();

protected:
    static QBrush globalBrush;
    static QPen globalPen;
    /* Свойства */
    BedroomFurniture type;
    UsingBrush usedBrush;
    UsingPen usedPen;

    QPointF topLetfPoint;
    QRectF rect;

    bool is_rotated;

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

        usedBrush == UsingBrush::Global ?
            painter->setBrush(globalBrush) :
            painter->setBrush(brush)
            ;
        usedPen == UsingPen::Global ?
            painter->setPen(globalPen) :
            painter->setPen(pen)
            ;
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

        setToolTip(
            QString("ширина w=%1, высота h=%2")
                .arg(mapRectToScene(rect).width())
                .arg(mapRectToScene(rect).height())
        );

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
    WallTypeItem(double x, double y, double w, QGraphicsItem* parent = nullptr)
        : BedroomFurnitureItem(x, y, w, height, parent)
    {
        brick = height / 3.0;
    }
    /* Сеттеры */
    static void setParameter(double wallThickness) {
        WallTypeItem::param = wallThickness;
        height = param;
    }
    /* Геттеры */
    static double getParameter() {
        return WallTypeItem::param;
    }
    static double getHeight() {
        return WallTypeItem::height;
    }
    double getWidth() {
        return rect.width();
    }
    /* Методы */
    void redrawMe() override {
        brick = height / 3.0;
        rect.setHeight(height);
    }
protected:
    /* Свойства */
    static double param;
    static double height;

    double brick;
};


class WallItem : public WallTypeItem {
public:
    WallItem(double x, double y, double w, QGraphicsItem* parent = nullptr)
        : WallTypeItem(x, y, w, parent)
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
    /* Конструкторы */
    WindowItem(double x, double y, double w, QGraphicsItem* parent = nullptr)
        : WallTypeItem(x, y, w, parent)
        , line{}
        , rect1{}
        , rect2{}
        , rect3{}
        , rect4{}
    {
        this->type = BedroomFurniture::Window;
    }
    /* Методы */
    void redrawMe() override {
        WallTypeItem::redrawMe();

        const double
            &rx = rect.topLeft().x(),
            &ry = rect.topLeft().y(),
                &rw = rect.width();

        rect1 = QRectF(
            rect.topLeft(),
            QSizeF(brick, height)
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
            QSizeF(brick, height)
            );
        line = QLineF(
            QPointF(rx + 2.0*brick, ry + 0.5*height)
            ,
            QPointF(rx + rw - 2.0*brick, ry + 0.5*height)
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
    DoorItem(double x, double y, double w, QGraphicsItem* parent = nullptr)
        : WallTypeItem(x, y, w, parent)
        , rect1{}
        , rect2{}
        , rect3{}
        , rect4{}
        , rect5{}
        , v1{}
        , v2{}
    {
        this->type = BedroomFurniture::Door;        
    }
private:
    /* Свойства */
    QRectF rect1, rect2, rect3, rect4, rect5;
    QPainterPath v1, v2;

    /* Методы */
    void redrawMe() override {
        WallTypeItem::redrawMe();

        const double
            &rx = rect.topLeft().x(),
            &ry = rect.topLeft().y(),
            &rw = rect.width();

        rect1 = QRectF(
            rect.topLeft(),
            QSizeF(brick, height)
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
    /* Конструкторы */
    ChairItem(double x, double y, QGraphicsItem* parent = nullptr)
        : BedroomFurnitureItem(x, y, width, height, parent)
        , d{}
        , chairSeat{}
        , chairArm1{}, chairArm2{}
        , chairArmRest1{}, chairArmRest2{}
        , chairSeatBack{}
        , line{}
    {
        this->type = BedroomFurniture::Chair;
    }
    /* Сеттеры */
    static void setParameter(double chairSeatSize) {
        param = chairSeatSize;
        width = (23.0/15.0)*param;
        height = 1.1*param;
    }
    /* Геттеры */
    static double getParameter() {
        return param;
    }
    static double getWidth() {
        return width;
    }
    static double getHeight() {
        return height;
    }
    /* Вирт. методы */
    void redrawMe() override {
        const double
            &rx = rect.topLeft().x(),
            &ry = rect.topLeft().y()
            ;

        rect.setWidth(width);
        rect.setHeight(height);

        // d -> длина стороны квадрата, оп. скр угол прям.
        d = param / 12.0;
        double armSideM = param / 15.0; // 4 если seat = 60

        QSizeF armRestSize = QSizeF(param / 3.0, 0.7*param); // Толстые подлокотники
        QSizeF armSize = QSizeF(0.2*param, 1.1*param); // Тонкие подлокотники
        QSizeF seatBackSize = QSizeF(0.4*param + param, 0.3*param);

        chairSeat = QRectF( // Седушка
            QPointF(rx + (4.0/15.0)*param, ry),
            QSizeF(param, param)
            );
        chairSeatBack = QRectF( // Спинка
            QPointF(rx + armSideM, ry)
            ,
            seatBackSize
            );
        line = QLineF( // Полоска на спинке
            QPointF(rx + armSideM, ry + 0.2*param),
            QPointF(rx + width - armSideM, ry + 0.2*param)
            );
        chairArmRest1 = QRectF(
            QPointF(rx , ry + 0.2*param)
            ,
            armRestSize
            );
        chairArmRest2 = QRectF(
            QPointF(rx + (18.0/15.0)*param, ry + 0.2*param)
            ,
            armRestSize
            );
        chairArm1 = QRectF(
            QPointF(rx + armSideM, ry)
            ,
            armSize
            );
        chairArm2 = QRectF(
            QPointF(rx + width - armSize.width() -  armSideM, ry)
            ,
            armSize
            );
    }

private:
    static double param;
    static double width, height;
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
    /* Конструкторы */
    BedItem(double x, double y, QGraphicsItem* parent = nullptr)
        : BedroomFurnitureItem(x, y, width, height, parent)
        , d{}
        , cushion{}
        , bedSpread{}
        , line1{}, line2{}
    {
        this->type = BedroomFurniture::Bed;
    }
    /* Сеттеры */
    static void setParam(double bedHeight) {
        param = bedHeight;
        width = 0.7*param;
        height = param;
    }
    /* Геттеры */
    static double getParameter() {
        return param;
    }
    static double getWidth() {
        return width;
    }
    static double getHeight() {
        return height;
    }
    /* Вирт методы */
    void redrawMe() override {
        const double
            &rx = rect.topLeft().x(),
            &ry = rect.topLeft().y()
            ;

        rect.setWidth(width);
        rect.setHeight(height);

        // Подушка
        double cw = 0.45*param,
               ch = 0.3*param
            ;

        d = 0.1*param;

        double topCushionMargin = 0.03*param;
        double sideCushionMargins = 0.5*(width - cw);

        double dy = 3.0*topCushionMargin + ch;

        cushion = QRectF(
            QPointF(rx + sideCushionMargins, ry + topCushionMargin)
            ,
            QSizeF(cw, ch)
            );
        // Одеяло
        double sy = ry + dy;

        bedSpread.moveTo(rx, ry + height);
        bedSpread.arcTo(rx, sy, d, d, 180, -90);
        bedSpread.lineTo(rx + width - d, sy);
        bedSpread.arcTo(rx + width - d, sy, d, d, 90, -90);
        bedSpread.lineTo(rx + width, ry + param);
        bedSpread.lineTo(rx, ry + param);

        double spacingBetweenLines = 0.05*param;

        line1 = QLineF(
            QPointF(rx, sy + spacingBetweenLines)
            ,
            QPointF(rx + width, sy + spacingBetweenLines)
            );
        line2 = QLineF(
            QPointF(rx, sy + 2.0*spacingBetweenLines)
            ,
            QPointF(rx + width, sy + 2.0*spacingBetweenLines)
            );
    }
private:
    static double param;
    static double width, height;
    /* Свойства */
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
    /* Конструкторы */
    PianoItem(double x, double y, QGraphicsItem* parent = nullptr)
        : BedroomFurnitureItem(x, y, width, height, parent)
        , pianoRect{}, keysRect{}, pianoChairRect{}
        , line{}
    {
        this->type = BedroomFurniture::Piano;   
    }
    /* Сеттеры */
    static void setParameter(double pianoHeight) {
        param = pianoHeight;
        width = 2.3*param;
        height = 1.7*param;
    }
    /* Геттеры */
    static double getParameter() {
        return param;
    }
    static double getWidth() {
        return width;
    }
    static double getHeight() {
        return height;
    }
    /* Вирт методы */
    void redrawMe() override {
        const double
            &rx = rect.topLeft().x(),
            &ry = rect.topLeft().y()
            ;

        rect.setWidth(width);
        rect.setHeight(height);

        double
            lh = 0.6*param
            , topKeysM = 0.25*(param - lh)
            , sideKeysM = 0.1875*(param - lh)
            , topChairM = 0.2*param
            , sideChairM = 0.5*param
            ;

        pianoRect = QRectF(
            rect.topLeft()
            ,
            QSizeF(
                width,
                param
                )
            );
        keysRect = QRectF(
            QPointF(rx + sideKeysM, ry + lh + topKeysM)
            ,
            QSizeF(width - 2.0*sideKeysM, param - lh - topKeysM)
            );

        line = QLineF(
            QPointF(rx, ry + lh),
            QPointF(rx + width, ry + lh)
            );

        pianoChairRect = QRectF(
            QPointF(rx + sideChairM, ry + param + topChairM)
            ,
            QSizeF(width - 2.0*sideChairM, 0.5*sideChairM)
            );
    }
private:
    static double param;
    static double width, height;

    /* Свойства */
    QRectF pianoRect, keysRect, pianoChairRect;
    QLineF line;
    /* Вирт методы */
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
    /* Конструкторы */
    DeskItem(double x, double y, QGraphicsItem* parent = nullptr)
        : BedroomFurnitureItem(x, y, width, height, parent)
        , d{}
        , deskBoard{}, deskChairSeat{}, deskChairArmRest1{}, deskChairArmRest2{}, deskChairSeatBack{}
        , line{}
        , cross{}

    {
        this->type = BedroomFurniture::Desk;
    }
    /* Сеттеры */
    static void setParameter(double deskBoardHeight) {
        param = deskBoardHeight;
        width = 3.0*param;
        height = (5.0/3.0)*param;
    }
    /* Геттеры */
    static double getParameter() {
        return param;
    }
    static double getWidth() {
        return width;
    }
    static double getHeight() {
        return height;
    }
    /* Вирт методы */
    void redrawMe() override {
        const double
            &rx = rect.topLeft().x(),
            &ry = rect.topLeft().y()
            ;

        rect.setWidth(width);
        rect.setHeight(height);

        d = param / 12.0;

        QSizeF armSize = QSizeF(
            param / 3.0, 0.5*param + d
            );

        deskBoard = QRectF(
            rect.topLeft(),
            QSizeF(width, param)
            );
        deskChairSeat = QRectF(
            QPointF(rx + param, ry + param)
            ,
            QSizeF(param, 0.5*param)
            );

        deskChairArmRest1 = QRectF(
            QPointF(
                rx + (11.0/15.0)*param,
                ry + param - d
                )
            ,
            armSize
            );
        deskChairArmRest2 = QRectF(
            QPointF(
                rx + width - (11.0/15.0)*param - param / 3.0,
                ry + param - d
                )
            ,
            armSize
            );

        double
            seatBackTopM = 0.5*param - 0.1*param
            , seatBackSideM = 0.8*param;

        deskChairSeatBack = QRectF(
            QPointF(rx + seatBackSideM, ry + param + seatBackTopM)
            ,
            QSizeF(1.4*param, (4.0/15.0)*param)
            );

        line = QLineF(
            QPointF(rx + seatBackSideM, ry + 1.5*param)
            ,
            QPointF(rx + width - seatBackSideM, ry + 1.5*param)
            );
        double cdx = rx + seatBackSideM + armSize.width() + d,
            cdy = ry + param;

        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 4; ++j) {
                cross.moveTo(cdx + j*2.0*d, cdy + i*2.0*d);
                cross.lineTo(cdx + d + j*2.0*d, cdy + d + i*2.0*d);

                cross.moveTo(cdx + d + j*2.0*d, cdy + i*2.0*d);
                cross.lineTo(cdx + j*2.0*d, cdy + d + i*2.0*d);
            }
        }
    }
private:
    static double param;
    static double width, height;
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
    /* Конструкторы */
    WardrobeItem(double x, double y, QGraphicsItem* parent = nullptr)
        : BedroomFurnitureItem(x, y, width, height, parent)
        , line{}
    {
        this->type = BedroomFurniture::Wardrobe;
    }
    /* Сеттеры */
    static void setParameter(double wardrobeHeight) {
        param = wardrobeHeight;
        width = 2.5*param;
        height = param;
    }
    /* Геттеры */
    static double getParameter() {
        return param;
    }
    static double getWidth() {
        return width;
    }
    static double getHeight() {
        return height;
    }
    /* Вирт методы */
    void redrawMe() override {
        const double
            &rx = rect.topLeft().x(),
            &ry = rect.topLeft().y()
            ;

        rect.setWidth(width);
        rect.setHeight(height);

        line = QLineF(
            QPointF(rx, ry + param - 0.1*param),
            QPointF(rx + width, ry + param - 0.1*param)
        );
    }
private:
    static double param;
    static double width, height;
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
