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

        /* Флаги */
        , m_element{BedroomFurniture::None}
        //, element{nullptr}
    {
        /* Параметры */
        createSelection();
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
            QSizeF selectionSize;
            // TODO Обобщить код, так как он отличается только в случае со стенами, остальное можно вынести в ф-цию, но ф-ции прорисовки настенных объектов можно обобщить и упростить.
            tmpX =
                event->scenePos().x() > topLeftPoint.x() ?
                topLeftPoint.x() : event->scenePos().x()
            ;
            tmpY =
                event->scenePos().y() > topLeftPoint.y() ?
                topLeftPoint.y() : event->scenePos().y()
            ;

            switch(m_element) {
            case BedroomFurniture::Wall:
            case BedroomFurniture::Window:
            case BedroomFurniture::Door:
                if (
                    qAbs(event->scenePos().x() - topLeftPoint.x()) >
                    qAbs(event->scenePos().y() - topLeftPoint.y())
                    )
                {
                    wallTypeItemWidth = qAbs(event->scenePos().x() - topLeftPoint.x());
                    selectionSize = QSizeF(
                        wallTypeItemWidth
                        ,
                        WallTypeItem::getHeight()
                        );

                    m_rotate = false;
                }
                else {
                    wallTypeItemWidth = qAbs(event->scenePos().y() - topLeftPoint.y());
                    selectionSize = QSizeF(
                        WallTypeItem::getHeight()
                        ,
                        wallTypeItemWidth
                        );
                    m_rotate = true;
                }
                break;
            case BedroomFurniture::Chair:
                if (qAbs(event->scenePos().x() - topLeftPoint.x()) > ChairItem::getHeight())
                {
                    selectionSize = QSizeF(
                        ChairItem::getHeight()
                        ,
                        ChairItem::getWidth()
                        );
                    m_rotate = true;
                }
                else {
                    selectionSize = QSizeF(
                        ChairItem::getWidth()
                        ,
                        ChairItem::getHeight()
                        );
                    m_rotate = false;
                }
                break;
            case BedroomFurniture::Bed:
                if (qAbs(event->scenePos().x() - topLeftPoint.x()) > BedItem::getHeight())
                {
                    selectionSize = QSizeF(
                        BedItem::getHeight()
                        ,
                        BedItem::getWidth()
                        );
                    m_rotate = true;

                }
                else {
                    selectionSize = QSizeF(
                        BedItem::getWidth()
                        ,
                        BedItem::getHeight()
                        );
                    m_rotate = false;
                }
                break;
            case BedroomFurniture::Piano:
                if (
                    qAbs(event->scenePos().x() - topLeftPoint.x()) >
                    PianoItem::getHeight()
                   )
                {
                    selectionSize = QSizeF(
                        PianoItem::getWidth()
                        ,
                        PianoItem::getHeight()
                        );
                    m_rotate = false;
                }
                else {
                    selectionSize = QSizeF(
                        PianoItem::getHeight()
                        ,
                        PianoItem::getWidth()
                    );
                    m_rotate = true;
                }
                break;
            case BedroomFurniture::Desk:
                if (
                    qAbs(event->scenePos().x() - topLeftPoint.x()) >
                    DeskItem::getHeight()
                    )
                {
                    selectionSize = QSizeF(
                        DeskItem::getWidth()
                        ,
                        DeskItem::getHeight()
                        );
                    m_rotate = false;
                }
                else {
                    selectionSize = QSizeF(
                        DeskItem::getHeight()
                        ,
                        DeskItem::getWidth()
                        );
                    m_rotate = true;
                }
                break;
            case BedroomFurniture::Wardrobe:
                if (
                    qAbs(event->scenePos().x() - topLeftPoint.x()) >
                    WardrobeItem::getHeight()
                    )
                {
                    selectionSize = QSizeF(
                        WardrobeItem::getWidth()
                        ,
                        WardrobeItem::getHeight()
                        );
                    m_rotate = false;
                }
                else {
                    selectionSize = QSizeF(
                        WardrobeItem::getHeight()
                        ,
                        WardrobeItem::getWidth()
                        );
                    m_rotate = true;
                }

                break;
            }

            selection->setRect(
                QRectF(
                    QPointF(tmpX, tmpY)
                    ,
                    selectionSize
                )
            );

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

                createItem();
                if (m_rotate)
                    g->rotateMe(-90);

                this->update();
                // Локальное
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

            if (BedroomFurnitureItem* item = dynamic_cast<BedroomFurnitureItem*>(it))
            {
                bool rot = false;
                out.writeStartElement("BedroomFurnitureItem");

                out.writeAttribute(
                    "t",
                    QString::number(static_cast<int>(item->getType()))
                    );
                out.writeAttribute("f", QString::number(item->getIsRotated()));
                if (item->getIsRotated()) {
                    item->rotateMe(0);
                    rot = true;
                }
                out.writeAttribute(
                    "x",
                    QString::number(item->getRectTopLeft().x())
                    );
                out.writeAttribute(
                    "y",
                    QString::number(item->getRectTopLeft().y())
                    );
                if (!item->getIsRotated() && rot) {
                    item->rotateMe(-90);
                }

                double p = 0.0;
                switch (item->getType()) {
                case BedroomFurniture::Wall:
                case BedroomFurniture::Window:
                case BedroomFurniture::Door:
                    if (WallTypeItem* wallType = dynamic_cast<WallTypeItem*>(item))
                    {
                        out.writeAttribute(
                            "w",
                            QString::number(wallType->getWidth())
                            );
                    }
                    p = WallTypeItem::getParameter();
                    break;
                case BedroomFurniture::Chair:
                    p = ChairItem::getParameter();
                    break;
                case BedroomFurniture::Bed:
                    p = BedItem::getParameter();
                    break;
                case BedroomFurniture::Piano:
                    p = PianoItem::getParameter();
                    break;
                case BedroomFurniture::Desk:
                    p = DeskItem::getParameter();
                    break;
                case BedroomFurniture::Wardrobe:
                    p = WardrobeItem::getParameter();
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
                else if (in.name().toString() == "BedroomFurnitureItem" && isBedroomFurnitureItems)
                {
                    m_element = static_cast<BedroomFurniture>
                        (
                            in.attributes().value("t").toInt()
                        );

                    tmpX = in.attributes().value("x").toDouble();
                    tmpY = in.attributes().value("y").toDouble();
                    double p = in.attributes().value("p").toDouble();

                    m_rotate = in.attributes().value("f").toInt();

                    switch (m_element) {
                    case BedroomFurniture::Wall:
                    case BedroomFurniture::Window:
                    case BedroomFurniture::Door:
                        wallTypeItemWidth = in.attributes().value("w").toDouble();
                        setWallTypeParameter(p);
                        break;
                    case BedroomFurniture::Chair:
                        setChairParameter(p);
                        break;
                    case BedroomFurniture::Bed:
                        setBedParameter(p);
                        break;
                    case BedroomFurniture::Desk:
                        setDeskParameter(p);
                        break;
                    case BedroomFurniture::Wardrobe:
                        setWardrobeParameter(p);
                        break;
                    }
                }
                break;
            case QXmlStreamReader::EndElement:
                createItem();
                if (m_rotate)
                    g->rotateMe(-90);
                this->update();
                break;
            default:
                break;
            }
        };
        resetDrawing();
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
        emit updateFileNameSignal("*буфер*");
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

    // Переменные, связанные с созданием элементов схемы
    double tmpX, tmpY;
    double wallTypeItemWidth;

    bool m_erasing;
    bool m_rotate;

    /* Методы */

    void setWallTypeParameter(double wallThickness) {
        if (WallTypeItem::getParameter() == wallThickness)
            return;

        WallTypeItem::setParameter(wallThickness);

        if (this->items().empty()) return;

        for (const auto& it : this->items()) {
            if (BedroomFurnitureItem* item = dynamic_cast<BedroomFurnitureItem*>(it))
            {
                if (item->getType() == BedroomFurniture::Wall ||
                    item->getType() == BedroomFurniture::Window ||
                    item->getType() == BedroomFurniture::Door
                    )
                {
                    item->redrawMe();
                    item->update();
                }
            }
        }
    }
    void setChairParameter(double chairSeatSize) {
        if (ChairItem::getParameter() == chairSeatSize)
            return;

        ChairItem::setParameter(chairSeatSize);

        if (this->items().empty()) return;

        for (const auto& it : this->items()) {
            if (BedroomFurnitureItem* item = dynamic_cast<BedroomFurnitureItem*>(it))
            {
                if (
                    item->getType() == BedroomFurniture::Chair
                   )
                {
                    item->redrawMe();
                    item->update();
                }
            }
        }
    }
    void setBedParameter(double bedHeight) {
        if (BedItem::getParameter() == bedHeight)
            return;

        BedItem::setParam(bedHeight);

        if (this->items().empty())
            return;

        for (const auto& it : this->items()) {
            if (BedroomFurnitureItem* item = dynamic_cast<BedroomFurnitureItem*>(it))
            {
                if (
                    item->getType() == BedroomFurniture::Bed
                    )
                {
                    item->redrawMe();
                    item->update();
                }
            }
        }
    }
    void setPianoParameter(double pianoHeight) {
        if (PianoItem::getParameter() == pianoHeight)
            return;

        PianoItem::setParameter(pianoHeight);

        if (this->items().empty())
            return;

        for (const auto& it : this->items()) {
            if (BedroomFurnitureItem* item = dynamic_cast<BedroomFurnitureItem*>(it))
            {
                if (
                    item->getType() == BedroomFurniture::Piano
                    )
                {
                    item->redrawMe();
                    item->update();
                }
            }
        }
    }
    void setDeskParameter(double deskBoardHeight) {
        if (DeskItem::getParameter() == deskBoardHeight)
            return;

        DeskItem::setParameter(deskBoardHeight);

        if (this->items().empty())
            return;

        for (const auto& it : this->items()) {
            if (BedroomFurnitureItem* item = dynamic_cast<BedroomFurnitureItem*>(it))
            {
                if (
                    item->getType() == BedroomFurniture::Desk
                    )
                {
                    item->redrawMe();
                    item->update();
                }
            }
        }

    }
    void setWardrobeParameter(double wardrobeHeight) {
        if (WardrobeItem::getParameter() == wardrobeHeight)
            return;

        WardrobeItem::setParameter(wardrobeHeight);

        if (this->items().empty())
            return;

        for (const auto& it : this->items()) {
            if (BedroomFurnitureItem* item = dynamic_cast<BedroomFurnitureItem*>(it))
            {
                if (
                    item->getType() == BedroomFurniture::Wardrobe
                    )
                {
                    item->redrawMe();
                    item->update();
                }
            }
        }
    }

    void createItem() {
        switch(m_element) {
        case BedroomFurniture::Wall:
            g = new WallItem(
                tmpX,
                tmpY,
                wallTypeItemWidth
                );
            break;
        case BedroomFurniture::Window:
            g = new WindowItem(
                tmpX,
                tmpY,
                wallTypeItemWidth
                );
            break;
        case BedroomFurniture::Door:
            g = new DoorItem(
                tmpX,
                tmpY,
                wallTypeItemWidth
                );
            break;
        case BedroomFurniture::Chair:
            g = new ChairItem(
                tmpX,
                tmpY
                );
            break;
        case BedroomFurniture::Bed:
            g = new BedItem(
                tmpX,
                tmpY
                );
            break;
        case BedroomFurniture::Piano:
            g = new PianoItem(
                tmpX,
                tmpY
                );
            break;
        case BedroomFurniture::Desk:
            g = new DeskItem(
                tmpX,
                tmpY
                );
            break;
        case BedroomFurniture::Wardrobe:
            g = new WardrobeItem(
                tmpX,
                tmpY
                );
            break;
        }
        if (g) {
            connect(g, &BedroomFurnitureItem::enterHover, this, &GraphicsScene::on_enterHover);
            connect(g, &BedroomFurnitureItem::leaveHover, this, &GraphicsScene::on_leaveHover);
            connect(this, &GraphicsScene::erasingSignal, g, &BedroomFurnitureItem::on_erasingSignal);
            connect(this, &GraphicsScene::erasedSignal, g, &BedroomFurnitureItem::on_erasedSignal);
            g->redrawMe();
            this->addItem(g);
            emit updateInfoSignal(
                "На сцену успешно добавлен элемент; "
                "x=" + QString::number(g->getRect().x()) +
                ", y=" + QString::number(g->getRect().y()) +
                ", w=" + QString::number(g->getRect().width()) +
                ", h=" + QString::number(g->getRect().height()) +
                " ;"
                );
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
