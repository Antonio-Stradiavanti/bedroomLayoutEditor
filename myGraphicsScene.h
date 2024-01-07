#ifndef MYGRAPHICSSCENE_H
#define MYGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QList>

#include <QGraphicsSceneMouseEvent>
#include <QWheelEvent>
#include <QTransform>

#include "graphicElement.h"


class GraphicsScene : public QGraphicsScene {
    Q_OBJECT
public:
    GraphicsScene(QObject* parent = nullptr)
        : QGraphicsScene{parent}

        , m_erasing{false}
        , m_rotate{false}

        , g{nullptr}
        /* Параметры */
        , wallThickness{27.0}
        , chairParamSeat{90.0}
        , bedParamHeight{200.0}
        , pianoParamHeight{160.0}
        , deskParamHeight{90.0}
        , wardrobeParamHeight{100}
        /* Флаги */
        , m_element{BedroomFurniture::None}
        //, element{nullptr}
    {

        computeChairSize();
        computeBedSize();
        computePianoSize();
        computeDeskSize();
        computeWardrobeSize();

        selection = new QGraphicsRectItem();
        addItem(selection);
        selection->hide();
    }
    ~GraphicsScene() {
        delete brush;
        delete selection;
    }
    /* Сеттеры */
    void setDrawingElement(BedroomFurniture drawingElement) {
        m_element = drawingElement;
        m_erasing = false;
        emit erasedSignal();
    }
    void setErasing(bool erasing) {
        m_erasing = erasing;
        if (m_erasing) {
            resetDrawing();
            emit erasingSignal();
        }
        else {
            emit erasedSignal();
        }
    }
    void setWallThickness(double newWallThikness) {
        wallThickness = newWallThikness;
    }
    void setSeat(double newSeat) {
        chairParamSeat = newSeat;

    }
    /* Геттеры */
    BedroomFurniture getDrawingElement() {
        return m_element;
    }
    double getWallThickness() {
        return wallThickness;
    }
    /* Обработчики событий */
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        if (event->button() == Qt::LeftButton &&
            m_element != BedroomFurniture::None)
        {
            topLeftPoint = event->scenePos();
            selection->setPen(
                QPen(Qt::black, 2, Qt::DashLine)
                );
        }
        QGraphicsScene::mousePressEvent(event);
    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override
    {
        if (
            (event->buttons() & Qt::LeftButton) &&
            m_element != BedroomFurniture::None
           )
        {
            QPointF selectionTopLeft; QSizeF selectionSize;
            // TODO Обобщить код, так как он отличается только в случае со стенами, остальное можно вынести в ф-цию, но ф-ции прорисовки настенных объектов можно обобщить и упростить.
            selectionTopLeft.setX(
                event->scenePos().x() > topLeftPoint.x() ?
                    topLeftPoint.x() : event->scenePos().x()
                );
            selectionTopLeft.setY(
                event->scenePos().y() > topLeftPoint.y() ?
                    topLeftPoint.y() : event->scenePos().y()
                );
            switch(m_element) {
            case BedroomFurniture::Wall:
            case BedroomFurniture::Window:
            case BedroomFurniture::Door:
                if (qAbs(event->scenePos().x() - topLeftPoint.x()) > wallThickness)
                {
                    selectionSize.setWidth(
                        qAbs(event->scenePos().x() - topLeftPoint.x())
                    );
                    selectionSize.setHeight(
                        wallThickness
                    );
                    m_rotate = true;
                }
                else {
                    selectionSize.setWidth(
                        wallThickness
                        );
                    selectionSize.setHeight(
                        qAbs(event->scenePos().y() - topLeftPoint.y())
                        );
                    m_rotate = false;
                }
                break;
            case BedroomFurniture::Chair:
                if (qAbs(event->scenePos().x() - topLeftPoint.x()) > chairItemHeight)
                {
                    selectionSize.setWidth(
                        chairItemHeight
                        );
                    selectionSize.setHeight(
                        chairItemWidth
                        );
                    m_rotate = true;
                }
                else {
                    selectionSize.setWidth(
                        chairItemWidth
                    );
                    selectionSize.setHeight(
                        chairItemHeight
                    );
                    m_rotate = false;
                }
                break;
            case BedroomFurniture::Bed:
                if (qAbs(event->scenePos().x() - topLeftPoint.x()) > bedParamHeight)
                {
                    selectionSize.setWidth(
                        bedItemHeight
                        );
                    selectionSize.setHeight(
                        bedItemWidth
                        );
                    m_rotate = true;

                }
                else {
                    selectionSize.setWidth(
                        bedItemWidth
                        );
                    selectionSize.setHeight(
                        bedItemHeight
                        );
                    m_rotate = false;
                }
                break;
            case BedroomFurniture::Piano:
                selectionSize = determinePosition(
                    qAbs(event->scenePos().x() - topLeftPoint.x()),
                    pianoParamHeight,
                    pianoItemWidth,
                    pianoItemHeight
                    );
                break;
            case BedroomFurniture::Desk:
                selectionSize = determinePosition(
                    qAbs(event->scenePos().x() - topLeftPoint.x()),
                    deskParamHeight,
                    deskItemWidth,
                    deskItemHeight
                    );
                break;
            case BedroomFurniture::Wardrobe:
                selectionSize = determinePosition(
                    qAbs(event->scenePos().x() - topLeftPoint.x()),
                    wardrobeParamHeight,
                    wardrobeItemWidth,
                    wardrobeItemHeight
                    );
                break;
            }

            selection->setRect(selectionTopLeft.x(), selectionTopLeft.y(), selectionSize.width(), selectionSize.height());

            emit showToolTip(
                event->screenPos(),
                QString("ширина w=%1, высота h=%2")
                     .arg(selection->rect().width())
                     .arg(selection->rect().height())
                );

            selection->update();
            if (!selection->isVisible()) {
                selection->show();
            }
        }
        QGraphicsScene::mouseMoveEvent(event);
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override
    {
        if ( (event->button() == Qt::LeftButton))
        {
            if (m_element != BedroomFurniture::None) {
                // Вынести проверку на горизонтальность
                switch(m_element) {
                case BedroomFurniture::Wall:
                    g = new WallItem(
                        selection->rect().x(),
                        selection->rect().y(),
                        selection->rect().width(),
                        selection->rect().height(),
                        wallThickness,
                        m_rotate
                        );
                    break;
                case BedroomFurniture::Window:
                    g = new WindowItem(
                        selection->rect().x(),
                        selection->rect().y(),
                        selection->rect().width(),
                        selection->rect().height(),
                        wallThickness,
                        m_rotate
                        );
                    break;
                case BedroomFurniture::Door:
                    g = new DoorItem(
                        selection->rect().x(),
                        selection->rect().y(),
                        selection->rect().width(),
                        selection->rect().height(),
                        wallThickness,
                        m_rotate
                        );
                    break;
                case BedroomFurniture::Chair:
                    g = new ChairItem(
                        selection->rect().x(),
                        selection->rect().y(),
                        chairItemWidth,
                        chairItemHeight,
                        chairParamSeat
                        );
                    if (m_rotate) g->repositionFromHorizont(selection->rect());
                    break;
                case BedroomFurniture::Bed:
                    g = new BedItem(
                        selection->rect().x(),
                        selection->rect().y(),
                        bedItemWidth,
                        bedItemHeight,
                        bedParamHeight
                        );
                    if (m_rotate) g->repositionFromHorizont(selection->rect());
                    break;
                case BedroomFurniture::Piano:
                    g = new PianoItem(
                        selection->rect().x(),
                        selection->rect().y(),
                        pianoItemWidth,
                        pianoItemHeight,
                        pianoParamHeight
                        );
                    if (m_rotate) g->repositionFromHorizont(selection->rect());
                    break;
                case BedroomFurniture::Desk:
                    g = new DeskItem(
                        selection->rect().x(),
                        selection->rect().y(),
                        deskItemWidth,
                        deskItemHeight,
                        deskParamHeight
                        );
                    if (m_rotate) g->repositionFromHorizont(selection->rect());
                    break;
                case BedroomFurniture::Wardrobe:
                    g = new WardrobeItem(
                        selection->rect().x(),
                        selection->rect().y(),
                        wardrobeItemWidth,
                        wardrobeItemHeight
                        );
                    if (m_rotate) g->repositionFromHorizont(selection->rect());
                    break;
                }
                createItem();
                resetDrawing();
                selection->hide();
            }
            else if (m_erasing && g) {
                disconnect(g, &BedroomFurnitureItem::enterHover, this, &GraphicsScene::on_enterHover);
                disconnect(g, &BedroomFurnitureItem::leaveHover, this, &GraphicsScene::on_leaveHover);
                disconnect(this, &GraphicsScene::erasingSignal, g, &BedroomFurnitureItem::on_erasingSignal);
                disconnect(this, &GraphicsScene::erasedSignal, g, &BedroomFurnitureItem::on_erasedSignal);

                removeItem(g);
                emit erasedSignal();
                g = nullptr;
                m_erasing = false;
                update();
            }
        }
        QGraphicsScene::mouseReleaseEvent(event);
    }
public slots:
    void on_enterHover() {
        if (m_erasing) {
            g = qobject_cast<BedroomFurnitureItem*>(
                sender()
            );
        }
    }
    void on_leaveHover() {
        if (m_erasing) {
            g = nullptr;
        }
    }
    void on_clearSignal() {
        this->clear();
    }

signals:
    void drawnWall();
    void drawnWindow();
    void drawnDoor();
    void drawnChair();
    void drawnBed();
    void drawnPiano();
    void drawnDesk();
    void drawnWardrobe();

    void erasingSignal();
    void erasedSignal();

    void showToolTip(QPoint pos, QString text);

private:
    /* Свойства */
    //GpahicELement* element;
    QPointF topLeftPoint;

    BedroomFurnitureItem* g;
    QGraphicsRectItem* selection;

    QBrush* brush;
    
    BedroomFurniture m_element;

    double wallThickness;

    double chairParamSeat;
    double chairItemWidth;
    double chairItemHeight;

    double bedParamHeight;
    double bedItemWidth;
    double bedItemHeight;

    double pianoParamHeight;
    double pianoItemWidth;
    double pianoItemHeight;

    double deskParamHeight;
    double deskItemWidth;
    double deskItemHeight;

    double wardrobeParamHeight;
    double wardrobeItemWidth;
    double wardrobeItemHeight;

    bool m_erasing;
    bool m_rotate;

    /* Методы */
    void computeChairSize() {
        chairItemWidth = (23.0/15.0)*chairParamSeat;
        chairItemHeight = 1.1*chairParamSeat;
    }
    void computeBedSize() {
        bedItemWidth = 0.7*bedParamHeight;
        bedItemHeight = bedParamHeight;
    }
    void computePianoSize() {
        pianoItemWidth = 2.3*pianoParamHeight;
        pianoItemHeight = 1.7*pianoParamHeight;
    }
    void computeDeskSize() {
        deskItemWidth = 3.0*deskParamHeight;
        deskItemHeight = (5.0/3.0)*deskParamHeight;
    }
    void computeWardrobeSize() {
        wardrobeItemWidth = 2.5*wardrobeParamHeight;
        wardrobeItemHeight = wardrobeParamHeight;
    }

    void createItem() {
        if (g) {
            connect(g, &BedroomFurnitureItem::enterHover, this, &GraphicsScene::on_enterHover);
            connect(g, &BedroomFurnitureItem::leaveHover, this, &GraphicsScene::on_leaveHover);
            connect(this, &GraphicsScene::erasingSignal, g, &BedroomFurnitureItem::on_erasingSignal);
            connect(this, &GraphicsScene::erasedSignal, g, &BedroomFurnitureItem::on_erasedSignal);

            addItem(g);
            g = nullptr;
        }
    }
    QSizeF determinePosition(double width, double itemParam, double itemWidth, double itemHeight)
    {
        QSizeF selectionSize;
        if (width > itemParam)
        {
            selectionSize = QSizeF(
                itemWidth, itemHeight
                );
            m_rotate = false;
        }
        else {
            selectionSize = QSizeF(
                itemHeight, itemWidth
                );
            m_rotate = true;
        }
        return selectionSize;
    }
    void resetDrawing() {
        switch (m_element) {
        case BedroomFurniture::Wall:
            emit drawnWall();
            break;
        case BedroomFurniture::Window:
            emit drawnWindow();
            break;
        case BedroomFurniture::Door:
            emit drawnDoor();
            break;
        case BedroomFurniture::Chair:
            emit drawnChair();
            break;
        case BedroomFurniture::Bed:
            emit drawnBed();
            break;
        case BedroomFurniture::Piano:
            emit drawnPiano();
            break;
        case BedroomFurniture::Desk:
            emit drawnDesk();
            break;
        case BedroomFurniture::Wardrobe:
            emit drawnWardrobe();
            break;
        default:
            break;
        }
        m_element = BedroomFurniture::None;
    }

};

#endif // MYGRAPHICSSCENE_H
