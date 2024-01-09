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

    , curFileName{"*буфер*"}

{
    ui->setupUi(this);

    this->statusBar();
    fileInfo = new QLabel();
    fileInfo->setText("Файл : " + curFileName);
    this->statusBar()->addPermanentWidget(fileInfo);

    scene = new GraphicsScene();
    view = new GraphicsView();
    view->setScene(scene);

    frame = new QFrame();
    this->setCentralWidget(frame);

    sidePanel = new QWidget();
    sidePanel->setVisible(false);

    mainLayout = new QHBoxLayout(frame);
    mainLayout->setSizeConstraint(QLayout::SetMinimumSize);
    groupBoxLayout = new QVBoxLayout(sidePanel);

    mainLayout->addWidget(view, 2);
    mainLayout->addWidget(sidePanel, 1);
    /* Настраиваю глобальные свойства */
    globalProperties = new QGroupBox("Глобальные свойства");
    globalPropertiesLayout = new QVBoxLayout(globalProperties);
    // Кисть
    brushColorLayout = new QHBoxLayout();
    brushColorLabel = new QLabel("Цвет заливки");
    colorButtonSize = QSize(24, 24);
    brushColorButtonIcon = QPixmap(colorButtonSize);
    brushColor = BedroomFurnitureItem::getGlobalBrush().color();
    brushColorButtonIcon.fill(brushColor);
    brushColorDialogButton = new QPushButton(
        QIcon(brushColorButtonIcon),
        ""
        );
    brushColorDialogButton->setFixedSize(colorButtonSize);
    brushColorLayout->addWidget(brushColorLabel);
    brushColorLayout->addWidget(brushColorDialogButton);
    // Перо
    penLayout = new QHBoxLayout();
    penColorLayout = new QHBoxLayout();
    penWidthLayout = new QHBoxLayout();
    penColorLabel = new QLabel("Цвет обводки");
    penColorButtonIcon = QPixmap(colorButtonSize);
    penColor = BedroomFurnitureItem::getGlobalPen().color();
    penColorButtonIcon.fill(penColor);
    penColorDialogButton = new QPushButton(
        QIcon(penColorButtonIcon),
        ""
        );
    penColorDialogButton->setFixedSize(colorButtonSize);
    penColorLayout->addWidget(penColorLabel);
    penColorLayout->addWidget(penColorDialogButton);
    //---
    penWidthLabel = new QLabel("Толщина обводки");
    penWidthValue = new QSpinBox();
    penWidthValue->setMinimum(1);
    penWidthValue->setMaximum(10);
    penWidthLayout->addWidget(penWidthLabel);
    penWidthLayout->addWidget(penWidthValue);
    //---
    penLayout->addLayout(penColorLayout);
    penLayout->addLayout(penWidthLayout);
    //---
    saveGlobalProperties = new QPushButton("Сохранить изменения");
    //---
    globalPropertiesLayout->addLayout(brushColorLayout);
    globalPropertiesLayout->addLayout(penLayout);
    globalPropertiesLayout->addWidget(saveGlobalProperties);
    //---
    /* Настраиваю панель настройки глобальных параметров */
    parameters = new QGroupBox("Глобальные параметры");

    // Будет содержать выпадающее меню и наборный счетчик
    parametersHorLayout = new QHBoxLayout();
    parametersListMenu = new QComboBox();
    parametersList = {
        "Толщина стены",
        "Длина сидения кресла",
        "Длина кровати",
        "Ширина пианино",
        "Ширина столешницы",
        "Ширина шкафа"
    };
    parametersListMenu->addItem("Толщина стены");
    parametersListMenu->addItem("Длина сидения кресла");
    parametersListMenu->addItem("Длина кровати");
    parametersListMenu->addItem("Ширина пианино");
    parametersListMenu->addItem("Ширина столешницы");
    parametersListMenu->addItem("Ширина шкафа");
    parameterValue = new QDoubleSpinBox();
    parameterValue->setSingleStep(0.1);
    parameterValue->setMinimum(20.0);
    parameterValue->setMaximum(500.0);
    parameterValue->setValue(WallTypeItem::getParameter());
    parametersHorLayout->addWidget(parametersListMenu, 2);
    parametersHorLayout->addWidget(parameterValue, 1);
    // Будет содержать кнопку и меню позв изменить значения параметров.
    buttonsHorLayout = new QHBoxLayout();
    saveParameterButton = new QPushButton("Сохранить изменения");
    resetParameterButton = new QPushButton("Сбросить значение параметра");
    buttonsHorLayout->addWidget(resetParameterButton);
    buttonsHorLayout->addWidget(saveParameterButton);
    // Устанавливаю все layout
    parametersVerLayout = new QVBoxLayout(parameters);
    parametersVerLayout->addLayout(parametersHorLayout);
    parametersVerLayout->addLayout(buttonsHorLayout);
    //---
    localProperties = new QGroupBox("Свойства определенного элемента схемы");

    groupBoxLayout->addWidget(globalProperties);
    groupBoxLayout->addWidget(parameters);
    groupBoxLayout->addWidget(localProperties);


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

    connect(scene, &GraphicsScene::updateInfoSignal, this, &MainWindow::on_updateInfoSignal);
    connect(scene, &GraphicsScene::updateFileNameSignal, this, &MainWindow::on_updateFileNameSignal);

    /* Боковая панель */
    // Глобальные свойства
    connect(brushColorDialogButton, &QPushButton::clicked, this, &MainWindow::on_brushColorDialogButton);
    connect(penColorDialogButton, &QPushButton::clicked, this, &MainWindow::on_penColorDialogButton);
    connect(saveGlobalProperties, &QPushButton::clicked, this, &MainWindow::on_saveGlobalProperties);
    // Глобальные параметры
    connect(parametersListMenu, &QComboBox::currentTextChanged, this, &MainWindow::on_parametersListChanged);
    connect(saveParameterButton, &QPushButton::clicked, this, &MainWindow::on_saveParameter);
    connect(resetParameterButton, &QPushButton::clicked, this, &MainWindow::on_resetParameter);
    /* Панель инструментов */
    QWidget* empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    ui->toolBar->addWidget(empty);
    ui->toolBar->addAction(ui->editPropertiesAction);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete frame;
    delete fileInfo;
    delete view;
    delete scene;

    delete globalProperties;
    delete localProperties;
    delete parameters;

}
/* TODO */
void MainWindow::on_editPropertiesAction_triggered()
{
    if (!sidePanel->isVisible()) {
        sidePanel->setVisible(true);
        ui->editPropertiesAction->setIcon(
            otherIconsList[OtherIcons::PropertiesEnabled]
            );

    } else {
        sidePanel->setVisible(false);
        ui->editPropertiesAction->setIcon(
            otherIconsList[OtherIcons::Properties]
            );
    }

}

void MainWindow::setDrawingIcons(BedroomFurniture type)
{
    this->view->setDragMode(QGraphicsView::NoDrag);
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
    view->setDragMode(QGraphicsView::ScrollHandDrag);
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
        view->setDragMode(QGraphicsView::NoDrag);
    }
    else {
        m_erasing = false;
        scene->setErasing(false);
        view->setDragMode(QGraphicsView::ScrollHandDrag);
    }
}
void MainWindow::on_erasedSignal() {
    ui->eraseAction->setIcon(
        QIcon(":/icons/erase_action.png")
        );
    view->setDragMode(QGraphicsView::ScrollHandDrag);
}
/* Сериализация */

void MainWindow::on_saveAction_triggered()
{
    if (curFileName == "*буфер*") {
        curFileName = QFileDialog::getSaveFileName(
            this, "Выполнить сериализацию в файл", "",
            "XML files (*.xml);;Json files (*.json);;All files (*.*)"
            );
        if (!curFileName.isEmpty()) {
            scene->toXml(curFileName);

        }
        else {
            on_updateInfoSignal("Файл не выбран, отмена сериализации");
            curFileName = "*буфер*";
        }
    }
    else {
        scene->toXml(curFileName);
    }
}

void MainWindow::on_loadItemsAction_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, "Загрузить из XML файла", "",
        "XML files (*.xml);;Json files (*.json);;All files (*.*)"
        );

    if (!fileName.isEmpty()) {
        curFileName = fileName;
        scene->fromXml(curFileName);
    }
    else {
        on_updateInfoSignal("Файл не выбран, отмена сериализации");
    }
}

void MainWindow::on_saveAsAction_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, "Загрузить из XML файла", "",
        "XML files (*.xml);;Json files (*.json);;All files (*.*)"
        );
    if (!fileName.isEmpty()) {
        curFileName = fileName;
        scene->toXml(curFileName);
    }
    else {
        on_updateInfoSignal("Файл не выбран, отмена сериализации");
    }
}

void MainWindow::on_updateInfoSignal(QString message)
{
    this->statusBar()->showMessage(message, 10000);
}
void MainWindow::on_updateFileNameSignal(QString fileName)
{
    curFileName = fileName;
    this->fileInfo->setText("Файл : " + curFileName);
}
/* Боковая панель */
// Глобальные свойства
void MainWindow::on_brushColorDialogButton() {
    QPalette palette  = brushColorDialogButton->palette();
    QColor color = palette.color(QPalette::Button);
    QColor tmpColor = QColorDialog::getColor(color, this);
    if (tmpColor.isValid()) {
        brushColor = tmpColor;
        brushColorButtonIcon.fill(brushColor);
        brushColorDialogButton->setIcon(
            QIcon(brushColorButtonIcon)
            );
    }
}

void MainWindow::on_penColorDialogButton() {
    QPalette palette  = penColorDialogButton->palette();
    QColor color = palette.color(QPalette::Button);
    QColor tmpColor = QColorDialog::getColor(color, this);
    if (tmpColor.isValid()) {
        penColor = tmpColor;
        penColorButtonIcon.fill(penColor);
        penColorDialogButton->setIcon(
            QIcon(penColorButtonIcon)
            );
    }
}

void MainWindow::on_saveGlobalProperties() {
    BedroomFurnitureItem::setGlobalBrush(
        QBrush(brushColor)
    );
    BedroomFurnitureItem::setGlobalPen(
        QPen(penColor, penWidthValue->value())
    );
    scene->update();
}
// Глобальные параметры
void MainWindow::on_parametersListChanged(const QString& text) {
    if (text == parametersList[0]) {
        parameterValue->setValue(WallTypeItem::getParameter());
    }
    else if (text == parametersList[1]) {
        parameterValue->setValue(ChairItem::getParameter());
    }
    else if (text == parametersList[2]) {
        parameterValue->setValue(BedItem::getParameter());
    }
    else if (text == parametersList[3]) {
        parameterValue->setValue(PianoItem::getParameter());
    }
    else if (text == parametersList[4]) {
        parameterValue->setValue(DeskItem::getParameter());
    }
    else if (text == parametersList[5]) {
        parameterValue->setValue(WardrobeItem::getParameter());
    }
}
void MainWindow::on_resetParameter() {
    on_parametersListChanged(parametersListMenu->currentText());
}
void MainWindow::on_saveParameter() {
    QString text = parametersListMenu->currentText();
    if (text == parametersList[0]) {
        scene->setWallTypeParameter(parameterValue->value());
    }
    else if (text == parametersList[1]) {
        scene->setChairParameter(parameterValue->value());
    }
    else if (text == parametersList[2]) {
        scene->setBedParameter(parameterValue->value());
    }
    else if (text == parametersList[3]) {
        scene->setPianoParameter(parameterValue->value());
    }
    else if (text == parametersList[4]) {
       scene->setDeskParameter(parameterValue->value());
    }
    else if (text == parametersList[5]) {
        scene->setWardrobeParameter(parameterValue->value());
    }
}
