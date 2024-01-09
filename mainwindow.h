#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QGraphicsScene>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QColorDialog>

#include <QFrame>
#include <QGroupBox>

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

    void on_updateInfoSignal(QString);
    void on_updateFileNameSignal(QString);

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

    void on_saveAction_triggered();

    void on_loadItemsAction_triggered();

    void on_saveAsAction_triggered();
    // Глобальные свойства
    void on_saveGlobalProperties();
    void on_brushColorDialogButton();
    void on_penColorDialogButton();
    // Глобальные параметры
    void on_parametersListChanged(const QString&);
    void on_resetParameter();
    void on_saveParameter();

private:
    /* Свойства */
    Ui::MainWindow* ui;

    QFrame* frame;
    QLabel* fileInfo;
    QString curFileName;

    QWidget* sidePanel;
    QVBoxLayout* groupBoxLayout;

    QSize colorButtonSize;
    // Глобальные свойства
    QGroupBox* globalProperties;
    QVBoxLayout* globalPropertiesLayout;
    // Кисть
    QHBoxLayout* brushColorLayout;
    QLabel* brushColorLabel;
    QColor brushColor;
    QPixmap brushColorButtonIcon;
    QPushButton* brushColorDialogButton;
    // Перо
    QHBoxLayout* penWidthLayout;
    QLabel* penWidthLabel;
    QSpinBox* penWidthValue;
    QHBoxLayout* penColorLayout;
    QLabel* penColorLabel;
    QColor penColor;
    QPixmap penColorButtonIcon;
    QPushButton* penColorDialogButton;
    QHBoxLayout* penLayout;
    QPushButton* saveGlobalProperties;
    // Параметры
    QGroupBox* parameters;
    QHBoxLayout* parametersHorLayout;
    QHBoxLayout* buttonsHorLayout;
    QVBoxLayout* parametersVerLayout;
    QComboBox* parametersListMenu; QDoubleSpinBox* parameterValue;
    QList<QString> parametersList;
    QPushButton* saveParameterButton;
    QPushButton* resetParameterButton;
    // Свойства элемента схемы
    QGroupBox* localProperties;
    QVBoxLayout* localPropertiesLayout;
    QLabel placeholder;
    // ---
    GraphicsView* view;
    GraphicsScene* scene;

    QHBoxLayout* mainLayout;

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
    /* Иконки */
    void setDrawingIcons(BedroomFurniture);
    void unsetDrawingIcons(BedroomFurniture);


};
#endif // MAINWINDOW_H
