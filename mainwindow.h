#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QGraphicsScene>

#include <QHBoxLayout>

#include "myGraphicsView.h"
#include "myGraphicsScene.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void on_drawnWall();
    void on_drawnWindow();
    void on_drawnDoor();
    void on_drawnChair();
    void on_drawnBed();
    void on_drawnPiano();
    void on_drawnDesk();
    void on_drawnWardrobe();

    void on_erasedSignal();

private slots:
    void on_editPropertiesAction_triggered();

    void on_drawingWallAction_triggered();

    void on_eraseAction_triggered();

    void on_drawingWindowAction_triggered();

    void on_drawingDoorAction_triggered();

    void on_drawingChairAction_triggered();

    void on_drawingBedAction_triggered();

    void on_drawingPianoAction_triggered();

    void on_drawingDeskAction_triggered();

    void on_drawingWardrobeAction_triggered();

private:
    /* Свойства */
    Ui::MainWindow* ui;

    GraphicsView* view;
    GraphicsScene* scene;

    QHBoxLayout* layout;

    bool isPropertiesPanelVisible;

    enum OtherIcons {
        Properties, PropertiesEnabled
    };
    QList<QIcon> otherIconsList;
    /* Иконки */
    enum DrawingIcons {
        DrawingWall, DrawingWallEnabled,
        DrawingWindow, DrawingWindowEnabled,
        DrawingBed, DrawingBedEnabled,
        DrawingChair, DrawingChairEnabled,
        DrawingPiano, DrawingPianoEnabled,
        DrawingDoor, DrawingDoorEnabled,
        DrawingDesk, DrawingDeskEnabled,
        DrawingWardrobe, DrawingWardrobeEnabled
    };
    QList<QIcon>
        drawingIconsList;

    bool m_erasing;

private:
    /* Закрытые методы */
    void setDrawingIcons(BedroomFurniture);
    void unsetDrawingIcons(BedroomFurniture);


};
#endif // MAINWINDOW_H
