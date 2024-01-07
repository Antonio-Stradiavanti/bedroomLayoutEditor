#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)

    , ui(new Ui::MainWindow)
    , isPropertiesPanelVisible{false}

    , otherIconsList {
        QIcon(":/icons/properties.png"),
        QIcon(":/icons/properties_enabled.png")
      }
    , drawingIconsList {
        QIcon(":/icons/wall.png"), QIcon(":/icons/wall_enabled.png"),
        QIcon(":/icons/window.png"), QIcon(":/icons/window_enabled.png"),
        QIcon(":/icons/bed.png"), QIcon(":/icons/bed_enabled.png"),
        QIcon(":/icons/chair.png"), QIcon(":/icons/chair_enabled.png"),
        QIcon(":/icons/piano.png"), QIcon(":/icons/piano_enabled.png"),
        QIcon(":/icons/door.png"), QIcon(":/icons/door_enabled.png"),
        QIcon(":/icons/desk.png"), QIcon(":/icons/desk_enabled.png"),
        QIcon(":/icons/wardrobe.png"), QIcon(":/icons/wardrobe_enabled.png")

      }
    , m_erasing{false}

{
    this->setWindowTitle("Редактор планировки спальни");

    ui->setupUi(this);

    scene = new GraphicsScene();
    view = new GraphicsView();
    view->setScene(scene);

    layout = new QHBoxLayout();
    layout->addWidget(view);

    this->centralWidget()->setLayout(layout);

    /* Связь сигнала и слота */
    connect(scene, &GraphicsScene::drawnWall, this, &MainWindow::on_drawnWall);
    connect(scene, &GraphicsScene::drawnWindow, this, &MainWindow::on_drawnWindow);
    connect(scene, &GraphicsScene::drawnDoor, this, &MainWindow::on_drawnDoor);

    connect(scene, &GraphicsScene::drawnChair, this, &MainWindow::on_drawnChair);
    connect(scene, &GraphicsScene::drawnBed, this, &MainWindow::on_drawnBed);
    connect(scene, &GraphicsScene::drawnPiano, this, &MainWindow::on_drawnPiano);
    connect(scene, &GraphicsScene::drawnDesk, this, &MainWindow::on_drawnDesk);
    connect(scene, &GraphicsScene::drawnWardrobe, this, &MainWindow::on_drawnWardrobe);

    connect(scene, &GraphicsScene::erasedSignal, this, &MainWindow::on_erasedSignal);

    connect(ui->clearAction, &QAction::triggered, scene, &GraphicsScene::on_clearSignal);
    //ui->groupBox->setVisible(isPropertiesPanelVisible);

    connect(scene, &GraphicsScene::showToolTip, view, &GraphicsView::on_showToolTip);

    QWidget* empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    ui->toolBar->addWidget(empty);
    ui->toolBar->addAction(ui->editPropertiesAction);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete view;
    delete scene;
}
/* TODO */
void MainWindow::on_editPropertiesAction_triggered()
{
    if (!isPropertiesPanelVisible) {
        isPropertiesPanelVisible = true;
        ui->editPropertiesAction->setIcon(
            otherIconsList[OtherIcons::PropertiesEnabled]
            );
    } else {
        isPropertiesPanelVisible = false;
        ui->editPropertiesAction->setIcon(
            otherIconsList[OtherIcons::Properties]
            );
    }
    //ui->groupBox->setVisible(isPropertiesPanelVisible);
}

void MainWindow::setDrawingIcons(BedroomFurniture type)
{
    if (m_erasing) {
        m_erasing = false;
        on_erasedSignal();
    }
    switch(type) {
    case BedroomFurniture::Wall:
        ui->drawingWallAction->setIcon(drawingIconsList[DrawingIcons::DrawingWallEnabled]);
        break;
    case BedroomFurniture::Window:
        ui->drawingWindowAction->setIcon(drawingIconsList[DrawingIcons::DrawingWindowEnabled]);
        break;
    case BedroomFurniture::Door:
        ui->drawingDoorAction->setIcon(drawingIconsList[DrawingIcons::DrawingDoorEnabled]);
        break;
    case BedroomFurniture::Chair:
        ui->drawingChairAction->setIcon(drawingIconsList[DrawingIcons::DrawingChairEnabled]);
        break;
    case BedroomFurniture::Bed:
        ui->drawingBedAction->setIcon(drawingIconsList[DrawingIcons::DrawingBedEnabled]);
        break;
    case BedroomFurniture::Piano:
        ui->drawingPianoAction->setIcon(drawingIconsList[DrawingIcons::DrawingPianoEnabled]);
        break;
    case BedroomFurniture::Desk:
        ui->drawingDeskAction->setIcon(drawingIconsList[DrawingIcons::DrawingDeskEnabled]);
        break;
    case BedroomFurniture::Wardrobe:
        ui->drawingWardrobeAction->setIcon(drawingIconsList[DrawingIcons::DrawingWardrobeEnabled]);
    default:
        break;
    }
}

void MainWindow::unsetDrawingIcons(BedroomFurniture type)
{
    switch(type) {
    case BedroomFurniture::Wall:
        ui->drawingWallAction->setIcon(drawingIconsList[DrawingIcons::DrawingWall]);
        break;
    case BedroomFurniture::Window:
        ui->drawingWindowAction->setIcon(drawingIconsList[DrawingIcons::DrawingWindow]);
        break;
    case BedroomFurniture::Door:
        ui->drawingDoorAction->setIcon(drawingIconsList[DrawingIcons::DrawingDoor]);
        break;
    case BedroomFurniture::Chair:
        ui->drawingChairAction->setIcon(drawingIconsList[DrawingIcons::DrawingChair]);
        break;
    case BedroomFurniture::Bed:
        ui->drawingBedAction->setIcon(drawingIconsList[DrawingIcons::DrawingBed]);
        break;
    case BedroomFurniture::Piano:
        ui->drawingPianoAction->setIcon(drawingIconsList[DrawingIcons::DrawingPiano]);
        break;
    case BedroomFurniture::Desk:
        ui->drawingDeskAction->setIcon(drawingIconsList[DrawingIcons::DrawingDesk]);
        break;
    case BedroomFurniture::Wardrobe:
        ui->drawingWardrobeAction->setIcon(drawingIconsList[DrawingIcons::DrawingWardrobe]);
        break;
    default:
        break;
    }
}

void MainWindow::on_drawingWallAction_triggered()
{
    unsetDrawingIcons(scene->getDrawingElement());
    scene->setDrawingElement(BedroomFurniture::Wall);
    setDrawingIcons(BedroomFurniture::Wall);
}
void MainWindow::on_drawnWall() {
    unsetDrawingIcons(BedroomFurniture::Wall);
}

void MainWindow::on_drawingWindowAction_triggered()
{
    unsetDrawingIcons(scene->getDrawingElement());
    scene->setDrawingElement(BedroomFurniture::Window);
    setDrawingIcons(BedroomFurniture::Window);
}
void MainWindow::on_drawnWindow()
{
    unsetDrawingIcons(BedroomFurniture::Window);
}

void MainWindow::on_drawingDoorAction_triggered()
{
    unsetDrawingIcons(scene->getDrawingElement());
    scene->setDrawingElement(BedroomFurniture::Door);
    setDrawingIcons(BedroomFurniture::Door);
}
void MainWindow::on_drawnDoor()
{
    unsetDrawingIcons(BedroomFurniture::Door);
}

void MainWindow::on_drawingChairAction_triggered()
{
    unsetDrawingIcons(scene->getDrawingElement());
    scene->setDrawingElement(BedroomFurniture::Chair);
    setDrawingIcons(BedroomFurniture::Chair);
}
void MainWindow::on_drawnChair()
{
    unsetDrawingIcons(BedroomFurniture::Chair);
}

void MainWindow::on_drawingBedAction_triggered()
{
    unsetDrawingIcons(scene->getDrawingElement());
    scene->setDrawingElement(BedroomFurniture::Bed);
    setDrawingIcons(BedroomFurniture::Bed);
}
void MainWindow::on_drawnBed()
{
    unsetDrawingIcons(BedroomFurniture::Bed);
}

void MainWindow::on_drawingPianoAction_triggered()
{
    unsetDrawingIcons(scene->getDrawingElement());
    scene->setDrawingElement(BedroomFurniture::Piano);
    setDrawingIcons(BedroomFurniture::Piano);
}
void MainWindow::on_drawnPiano()
{
    unsetDrawingIcons(BedroomFurniture::Piano);
}

void MainWindow::on_drawingDeskAction_triggered()
{
    unsetDrawingIcons(scene->getDrawingElement());
    scene->setDrawingElement(BedroomFurniture::Desk);
    setDrawingIcons(BedroomFurniture::Desk);
}
void MainWindow::on_drawnDesk()
{
    unsetDrawingIcons(BedroomFurniture::Desk);
}

void MainWindow::on_drawingWardrobeAction_triggered()
{
    unsetDrawingIcons(scene->getDrawingElement());
    scene->setDrawingElement(BedroomFurniture::Wardrobe);
    setDrawingIcons(BedroomFurniture::Wardrobe);
}
void MainWindow::on_drawnWardrobe()
{
    unsetDrawingIcons(BedroomFurniture::Wardrobe);
}

void MainWindow::on_eraseAction_triggered()
{
    if (!m_erasing) {
        m_erasing = true;
        scene->setErasing(true);
        ui->eraseAction->setIcon(
            QIcon(":/icons/erase_action_enabled.png")
            );
    }
    else {
        m_erasing = false;
        scene->setErasing(false);
    }
}
void MainWindow::on_erasedSignal() {
    ui->eraseAction->setIcon(
        QIcon(":/icons/erase_action.png")
        );
}


