#ifndef MYGRAPHICSSCENE_H
#define MYGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QList>
#include <QFile>

#include <QGraphicsSceneMouseEvent>
#include <QWheelEvent>
#include <QTransform>

#include <QFileDialog>

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include "bedroomFurnitureItems.h"


class GraphicsScene : public QGraphicsScene {
    Q_OBJECT
public:
    GraphicsScene(QObject* parent = nullptr)
        : QGraphicsScene{parent}

        , m_erasing{false}
        , m_rotate{false}

        , g{nullptr}

        , wallTypeItemWidth{}
        , wallTypeItemHeight{}

        /* Флаги */
        , m_element{BedroomFurniture::None}
        //, element{nullptr}
    {
        /* Параметры */
        createSelection();

        setWallTypeItemSize(27.0);
        setChairItemSize(90.0);
        setBedItemSize(200.0);
        setPianoItemSize(160.0);
        setDeskItemSize(90.0);
        setWardrobeSize(100);


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
    /* Геттеры */
    BedroomFurniture getDrawingElement() {
        return m_element;
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
                wallTypeItemHeight = WallTypeItem::getWallThickness();
                if (
                    qAbs(event->scenePos().x() - topLeftPoint.x()) >
                    qAbs(event->scenePos().y() - topLeftPoint.y())
                    )
                {
                    wallTypeItemWidth = qAbs(event->scenePos().x() - topLeftPoint.x());
                    selectionSize = QSizeF(
                        wallTypeItemWidth,
                        wallTypeItemHeight
                        );

                    m_rotate = false;
                }
                else {
                    wallTypeItemWidth = qAbs(event->scenePos().y() - topLeftPoint.y());
                    selectionSize = QSizeF(
                        wallTypeItemHeight,
                        wallTypeItemWidth
                        );
                    m_rotate = true;
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
                if (qAbs(event->scenePos().x() - topLeftPoint.x()) > bedItemHeight)
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
                if (
                    qAbs(event->scenePos().x() - topLeftPoint.x()) >
                    pianoItemHeight
                   )
                {
                    selectionSize = QSizeF(
                        pianoItemWidth, pianoItemHeight
                        );
                    m_rotate = false;
                }
                else {
                    selectionSize = QSizeF(
                        pianoItemHeight, pianoItemWidth
                        );
                    m_rotate = true;
                }
                break;
            case BedroomFurniture::Desk:
                if (
                    qAbs(event->scenePos().x() - topLeftPoint.x()) >
                    deskItemHeight
                    )
                {
                    selectionSize = QSizeF(
                        deskItemWidth, deskItemHeight
                        );
                    m_rotate = false;
                }
                else {
                    selectionSize = QSizeF(
                        deskItemHeight, deskItemWidth
                        );
                    m_rotate = true;
                }
                break;
            case BedroomFurniture::Wardrobe:
                if (
                    qAbs(event->scenePos().x() - topLeftPoint.x()) >
                    wardrobeItemHeight
                    )
                {
                    selectionSize = QSizeF(
                        wardrobeItemWidth,
                        wardrobeItemHeight
                        );
                    m_rotate = false;
                }
                else {
                    selectionSize = QSizeF(
                        wardrobeItemHeight,
                        wardrobeItemWidth
                    );
                    m_rotate = true;
                }

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
                        wallTypeItemWidth,
                        wallTypeItemHeight
                        );
                    break;
                case BedroomFurniture::Window:
                    g = new WindowItem(
                        selection->rect().x(),
                        selection->rect().y(),
                        wallTypeItemWidth,
                        wallTypeItemHeight
                        );
                    break;
                case BedroomFurniture::Door:
                    g = new DoorItem(
                        selection->rect().x(),
                        selection->rect().y(),
                        wallTypeItemWidth,
                        wallTypeItemHeight
                        );
                    break;
                case BedroomFurniture::Chair:
                    g = new ChairItem(
                        selection->rect().x(),
                        selection->rect().y(),
                        chairItemWidth,
                        chairItemHeight
                        );
                    break;
                case BedroomFurniture::Bed:
                    g = new BedItem(
                        selection->rect().x(),
                        selection->rect().y(),
                        bedItemWidth,
                        bedItemHeight
                        );
                    break;
                case BedroomFurniture::Piano:
                    g = new PianoItem(
                        selection->rect().x(),
                        selection->rect().y(),
                        pianoItemWidth,
                        pianoItemHeight
                        );
                    break;
                case BedroomFurniture::Desk:
                    g = new DeskItem(
                        selection->rect().x(),
                        selection->rect().y(),
                        deskItemWidth,
                        deskItemHeight
                        );
                    break;
                case BedroomFurniture::Wardrobe:
                    g = new WardrobeItem(
                        selection->rect().x(),
                        selection->rect().y(),
                        wardrobeItemWidth,
                        wardrobeItemHeight
                        );
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
    /* Сериализация */
    void toXml(QString fileName) {

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            emit updateInfoSignal("Ошибка при открытии файла " + fileName + " на запись.");
            return;
        }

        QXmlStreamWriter out(&file);
        out.setAutoFormatting(true);

        out.writeStartDocument(); // Пишет xmlPrologue

        out.writeStartElement("BedroomFurnitureItems");

        for (const auto& it : this->items()) {

            if (BedroomFurnitureItem* g = dynamic_cast<BedroomFurnitureItem*>(it))
            {
                out.writeStartElement("BedroomFurnitureItem");

                out.writeAttribute(
                    "t",
                    QString::number(static_cast<int>(g->getType()))
                    );
                out.writeAttribute(
                    "x",
                    QString::number(g->getRect().x())
                    );
                out.writeAttribute(
                    "y",
                    QString::number(g->getRect().y())
                    );
                out.writeAttribute(
                    "w",
                    QString::number(g->getRect().width())
                    );
                out.writeAttribute(
                    "h",
                    QString::number(g->getRect().height())
                    );
                out.writeAttribute("f", QString::number(g->getIsHorizontal()));

                double p = 0.0;
                switch (g->getType()) {
                case BedroomFurniture::Wall:
                case BedroomFurniture::Window:
                case BedroomFurniture::Door:
                    p = WallTypeItem::getWallThickness();
                    break;
                case BedroomFurniture::Chair:
                    p = ChairItem::getChairParamSeatSize();
                    break;
                case BedroomFurniture::Bed:
                    p = BedItem::getBedParamHeight();
                    break;
                case BedroomFurniture::Piano:
                    p = PianoItem::getPianoParamHeight();
                    break;
                case BedroomFurniture::Desk:
                    p = DeskItem::getDeskParamHeight();
                    break;
                case BedroomFurniture::Wardrobe:
                    p = WardrobeItem::getWardrobeParamHeight();
                    break;
                default:
                    break;
                }
                out.writeAttribute("p", QString::number(p));

                out.writeEndElement();
            }
        }

        out.writeEndElement();

        out.writeEndDocument();

        emit updateInfoSignal("Схема успешно сохранена в файл");
        emit updateFileNameSignal(fileName);
        file.close();
    }
    void fromXml(QString fileName) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            emit updateInfoSignal(
                "Ошибка при открытии файла " + fileName + "на чтение."
                );
            return;
        }

        QByteArray data = file.readAll();
        file.close();

        QXmlStreamReader in(data);
        g = nullptr;

        bool isBedroomFurnitureItems = false;

        on_clearSignal();

        while(!in.atEnd()) {
            QXmlStreamReader::TokenType token = in.readNext();

            switch(token) {
            case QXmlStreamReader::StartElement:
                if (in.name().toString() == "BedroomFurnitureItems") {
                    isBedroomFurnitureItems = true;
                }
                else if (in.name().toString() == "BedroomFurnitureItem" && isBedroomFurnitureItems && in.attributes().size() == 7) {
                    BedroomFurniture t = static_cast<BedroomFurniture>
                        (
                            in.attributes().value("t").toInt()
                            );

                    double x = in.attributes().value("x").toDouble();
                    double y = in.attributes().value("y").toDouble();
                    double w = in.attributes().value("w").toDouble();
                    double h = in.attributes().value("h").toDouble();
                    double p = in.attributes().value("p").toDouble();
                    m_rotate = in.attributes().value("f").toInt();

                    switch (t) {
                    case BedroomFurniture::Wall:
                        setWallTypeItemSize(p);
                        g = new WallItem(x, y, w, WindowItem::getWallThickness());
                        break;
                    case BedroomFurniture::Window:
                        setWallTypeItemSize(p);
                        g = new WindowItem(x, y, w, WindowItem::getWallThickness());
                        break;
                    case BedroomFurniture::Door:
                        setWallTypeItemSize(p);
                        WallTypeItem::setWallThickness(p);
                        g = new DoorItem(x, y, w, WindowItem::getWallThickness());
                        break;
                    case BedroomFurniture::Chair:
                        setChairItemSize(p);
                        g = new ChairItem(x, y, chairItemWidth, chairItemHeight);
                        break;
                    case BedroomFurniture::Bed:
                        setBedItemSize(p);
                        g = new BedItem(x, y, bedItemWidth, bedItemHeight);
                        break;
                    case BedroomFurniture::Desk:
                        setDeskItemSize(p);
                        g = new DeskItem(x, y, deskItemWidth, deskItemHeight);
                        break;
                    case BedroomFurniture::Wardrobe:
                        setWardrobeSize(p);
                        g = new WardrobeItem(x, y, wardrobeItemWidth, wardrobeItemHeight);
                        break;
                    }
                }
                break;
            case QXmlStreamReader::EndElement:
                createItem();
                break;
            default:
                break;
            }
        };
        emit updateInfoSignal("Схема успешно загружена из файла");
        emit updateFileNameSignal(fileName);
    }

    void toJson(QString);
    void fromJson(QString);
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
        if (!this->items().empty()) {
            this->clear();
            createSelection();
        }
        emit updateInfoSignal("Создана новая схема");
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

    void updateInfoSignal(QString);
    void updateFileNameSignal(QString);

    void showToolTip(QPoint pos, QString text);

private:
    /* Свойства */
    //GpahicELement* element;
    QPointF topLeftPoint;

    BedroomFurnitureItem* g;
    QGraphicsRectItem* selection;

    QBrush* brush;
    
    BedroomFurniture m_element;

    double wallTypeItemWidth;
    double wallTypeItemHeight;

    double chairItemWidth;
    double chairItemHeight;

    double bedItemWidth;
    double bedItemHeight;

    double pianoItemWidth;
    double pianoItemHeight;

    double deskItemWidth;
    double deskItemHeight;

    double wardrobeItemWidth;
    double wardrobeItemHeight;

    bool m_erasing;
    bool m_rotate;

    /* Методы */

    void setWallTypeItemSize(double p) {
        WallTypeItem::setWallThickness(p);
    }
    void setChairItemSize(double p) {
        ChairItem::setChairParamSeatSize(p);
        chairItemWidth = (23.0/15.0)*p;
        chairItemHeight = 1.1*p;
    }
    void setBedItemSize(double p) {
        BedItem::setBedParamHeight(p);
        bedItemWidth = 0.7*p;
        bedItemHeight = p;
    }
    void setPianoItemSize(double p) {
        PianoItem::setPianoParamHeight(p);
        pianoItemWidth = 2.3*p;
        pianoItemHeight = 1.7*p;
    }
    void setDeskItemSize(double p) {
        DeskItem::setDeskParamHeight(p);
        deskItemWidth = 3.0*p;
        deskItemHeight = (5.0/3.0)*p;
    }
    void setWardrobeSize(double p) {
        WardrobeItem::setWardrobeParamHeight(p);
        wardrobeItemWidth = 2.5*p;
        wardrobeItemHeight = p;
    }

    void createItem() {
        if (g) {
            connect(g, &BedroomFurnitureItem::enterHover, this, &GraphicsScene::on_enterHover);
            connect(g, &BedroomFurnitureItem::leaveHover, this, &GraphicsScene::on_leaveHover);
            connect(this, &GraphicsScene::erasingSignal, g, &BedroomFurnitureItem::on_erasingSignal);
            connect(this, &GraphicsScene::erasedSignal, g, &BedroomFurnitureItem::on_erasedSignal);

            this->addItem(g);
            if (m_rotate)
                g->rotateFromHorizontalToVertical();
            this->update();
            emit updateInfoSignal(
                "На сцену успешно добавлен элемент; "
                "x=" + QString::number(g->getRect().x()) +
                ", y=" + QString::number(g->getRect().y()) +
                ", w=" + QString::number(g->getRect().width()) +
                ", h=" + QString::number(g->getRect().height()) +
                " ;"
                );
            g = nullptr;
        }
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
    void createSelection() {
        selection = new QGraphicsRectItem();
        addItem(selection);
        selection->hide();
    }

};

#endif // MYGRAPHICSSCENE_H
