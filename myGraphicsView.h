#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QToolTip>

#include "myGraphicsScene.h"

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QWidget* parent = nullptr)
        : QGraphicsView(parent)

    {
        this->setCursor(
            QCursor(
                QPixmap(":/cursors/pointer_cursor.png")
                )
            );
        this->setDragMode(QGraphicsView::ScrollHandDrag);
        this->setRenderHint(QPainter::Antialiasing);

        myScene = new GraphicsScene();
        this->setScene(myScene);
    }

    ~GraphicsView() {
        delete myScene;
    }

    void wheelEvent(QWheelEvent* event) override
    {
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

public slots:
    void on_showToolTip(QPoint pos, QString text) {
        QToolTip::showText(
        pos,
        text
        );
    }
private:
    /* Свойства */
    GraphicsScene* myScene;

};

#endif // MYGRAPHICSVIEW_H
