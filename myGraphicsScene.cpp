#include "myGraphicsScene.h"

/* Рисуем */
GraphicsScene::GraphicsScene(QObject* parent)
    : QGraphicsScene{parent}
    , element{nullptr}
{}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* event){
    // Все как обычно
    if (event->button() == Qt::LeftButton) {

        switch (drawingFlag) {
        case Drawing::DrawingWall:
            element = new WallElement();
            element->setTopLetfPoint(event->pos());
            this->addItem(element);
            break;
        default:
            break;
        }

    }
    QGraphicsScene::mousePressEvent(event);
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event){

    if (element) {
        element->setBottomRightPoint(event->pos());
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event){
    if (element) {
        element = nullptr;
        isDrawn = true;
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void GraphicsScene::setDrawingFlag(Drawing drawingFlag) {
    this->drawingFlag = drawingFlag;
}

/* Отображаем */
GraphicsView::GraphicsView(QWidget* parent)
    : QGraphicsView(parent)
{
    this->setRenderHint(QPainter::Antialiasing);

    myScene = new GraphicsScene();
    myScene->setDrawingFlag(drawingFlag);
    this->setScene(myScene);

}
GraphicsView::~GraphicsView() {
    delete myScene;
}

void GraphicsView::wheelEvent(QWheelEvent* event) {
    if (event->modifiers() & Qt::ControlModifier) {
        double factor = 1.2;
        if (event->angleDelta().y() < 0) {
            factor = 1.0 / factor;
        }
        scale(factor, factor);
    } else {
        QGraphicsView::wheelEvent(event);
    }

}

void GraphicsView::setDrawingFlag(Drawing drawingFlag) {
        this->drawingFlag = drawingFlag;
}

