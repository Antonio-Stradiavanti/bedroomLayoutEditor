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
#include <QLineEdit>
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

    void on_sendScannedItem(BedroomFurnitureItem*);

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
    void on_brushGlobalColorDialogButton();
    void on_penGlobalColorDialogButton();
    // Глобальные параметры
    void on_parametersListChanged(const QString&);
    void on_resetParameter();
    void on_saveParameter();
    // Локальные параметры
    void on_brushLocalColorDialogButton();
    void on_penLocalColorDialogButton();
    void on_resetLocalProperties();
    void on_saveLocalProperties();

private:
    /* Свойства */
    Ui::MainWindow* ui;

    QFrame* frame;
    QLabel* fileInfo;
    QString curFileName;

    QWidget* sidePanel;
    QVBoxLayout* groupBoxLayout;

    // ***
    QSize colorButtonSize;
    QColor penColor;
    QPixmap penColorButtonIcon;
    QColor brushColor;
    QPixmap brushColorButtonIcon;
    // ***
    // Глобальные свойства
    QGroupBox* globalProperties;
    QVBoxLayout* globalPropertiesLayout;
    // Кисть
    QHBoxLayout* brushGlobalColorLayout;
    QLabel* brushGlobalColorLabel;
    QPushButton* brushGlobalColorDialogButton;
    // Перо
    QHBoxLayout* penGlobalWidthLayout;
    QLabel* penGlobalWidthLabel;
    QSpinBox* penGlobalWidthValue;
    QHBoxLayout* penGlobalColorLayout;
    QLabel* penGLobalColorLabel;
    //---
    QPushButton* penGlobalColorDialogButton;
    QHBoxLayout* penGlobalLayout;
    QPushButton* saveGlobalProperties;
    // Параметры
    QGroupBox* parameters;
    QHBoxLayout* parametersHorLayout;
    QHBoxLayout* buttonsParamHorLayout;
    QVBoxLayout* parametersVerLayout;
    QComboBox* parametersListMenu; QDoubleSpinBox* parameterValue;
    QList<QString> parametersList;
    QPushButton* saveParameterButton;
    QPushButton* resetParameterButton;
    // Свойства элемента схемы
    BedroomFurnitureItem* itemHandler;
    QGroupBox* localPropertiesDefault;
    QGroupBox* localPropertiesSelected;
    QVBoxLayout* localPropertiesLayoutDefault;
    QVBoxLayout* localPropertiesLayoutSelected;
    QLabel* placeholder;
    //---
    // Кисть
    QHBoxLayout* brushLocalColorLayout;
    QLabel* brushLocalColorLabel;
    QPushButton* brushLocalColorDialogButton;
    // Перо
    QHBoxLayout* penLocalWidthLayout;
    QLabel* penLocalWidthLabel;
    QSpinBox* penLocalWidthValue;
    QHBoxLayout* penLocalColorLayout;
    QLabel* penGLocalColorLabel;
    QPushButton* penLocalColorDialogButton;
    QHBoxLayout* penLocalLayout;
    //---
    QHBoxLayout* coordinatesHorLayout;
    QLabel* coordinatesLabel;
    QLineEdit* coordinatesValue;
    // Смещение
    QHBoxLayout* shiftHorLayout;
    QLabel* shiftLabel;
    QDoubleSpinBox* dxValue;
    QDoubleSpinBox* dyValue;
    // Масштаб
    QHBoxLayout* scaleHorLayout;
    QLabel* scaleLabel;
    QDoubleSpinBox* scaleValue;
    // Кнопки
    QHBoxLayout* buttonsLocalHorLayout;
    QPushButton* resetLocalProperties;
    QPushButton* saveLocalProperties;
    //---
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
