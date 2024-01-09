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

    , itemHandler{nullptr}

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
    brushGlobalColorLayout = new QHBoxLayout();
    brushGlobalColorLabel = new QLabel("Глобальный цвет заливки");
    colorButtonSize = QSize(24, 24);
    brushColorButtonIcon = QPixmap(colorButtonSize);
    brushColor = BedroomFurnitureItem::getGlobalBrush().color();
    brushColorButtonIcon.fill(brushColor);
    brushGlobalColorDialogButton = new QPushButton(
        QIcon(brushColorButtonIcon),
        ""
        );
    brushGlobalColorDialogButton->setFixedSize(colorButtonSize);
    brushGlobalColorLayout->addWidget(brushGlobalColorLabel);
    brushGlobalColorLayout->addWidget(brushGlobalColorDialogButton);
    // Перо
    penGlobalLayout = new QHBoxLayout();
    penGlobalColorLayout = new QHBoxLayout();
    penGlobalWidthLayout = new QHBoxLayout();
    penGLobalColorLabel = new QLabel("Глобальный Цвет обводки");
    penColorButtonIcon = QPixmap(colorButtonSize);
    penColor = BedroomFurnitureItem::getGlobalPen().color();
    penColorButtonIcon.fill(penColor);
    penGlobalColorDialogButton = new QPushButton(
        QIcon(penColorButtonIcon),
        ""
        );
    penGlobalColorDialogButton->setFixedSize(colorButtonSize);
    penGlobalColorLayout->addWidget(penGLobalColorLabel);
    penGlobalColorLayout->addWidget(penGlobalColorDialogButton);
    //---
    penGlobalWidthLabel = new QLabel("Глобальня толщина обводки");
    penGlobalWidthValue = new QSpinBox();
    penGlobalWidthValue->setMinimum(1);
    penGlobalWidthValue->setMaximum(10);
    penGlobalWidthValue->setValue(BedroomFurnitureItem::getGlobalPen().width());
    penGlobalWidthLayout->addWidget(penGlobalWidthLabel);
    penGlobalWidthLayout->addWidget(penGlobalWidthValue);
    //---
    penGlobalLayout->addLayout(penGlobalColorLayout);
    penGlobalLayout->addLayout(penGlobalWidthLayout);
    //---
    saveGlobalProperties = new QPushButton("Сохранить изменения");
    //---
    globalPropertiesLayout->addLayout(brushGlobalColorLayout);
    globalPropertiesLayout->addLayout(penGlobalLayout);
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
    buttonsParamHorLayout = new QHBoxLayout();
    saveParameterButton = new QPushButton("Сохранить изменения");
    resetParameterButton = new QPushButton("Сбросить значение параметра");
    buttonsParamHorLayout->addWidget(resetParameterButton);
    buttonsParamHorLayout->addWidget(saveParameterButton);
    // Устанавливаю все layout
    parametersVerLayout = new QVBoxLayout(parameters);
    parametersVerLayout->addLayout(parametersHorLayout);
    parametersVerLayout->addLayout(buttonsParamHorLayout);
    /* Настраиваю локальные свойства элемента */
    // Если элемент не выбран
    localPropertiesDefault = new QGroupBox("Свойства определенного элемента схемы");
    localPropertiesLayoutDefault = new QVBoxLayout(localPropertiesDefault);
    placeholder = new QLabel("Чтобы отобразить свойства графического элемента щелкните по нему левой кнопкой мыши");
    localPropertiesLayoutDefault->addWidget(placeholder);
    // Если элемент выбран
    localPropertiesSelected = new QGroupBox("Свойства определенного элемента схемы");
    //---
    localPropertiesLayoutSelected = new QVBoxLayout(localPropertiesSelected);
    // Кисти и перья
    brushLocalColorLayout = new QHBoxLayout();
    brushLocalColorLabel = new QLabel("Цвет заливки");
    brushLocalColorDialogButton = new QPushButton("");
    brushLocalColorDialogButton->setFixedSize(colorButtonSize);
    //---
    penLocalWidthLayout = new QHBoxLayout();
    penLocalWidthLabel = new QLabel("Толщина обводки");
    penLocalWidthValue = new QSpinBox();
    penLocalWidthValue->setMinimum(1);
    penLocalWidthValue->setMaximum(10);
    //---
    penLocalColorLayout = new QHBoxLayout();
    brushLocalColorLabel = new QLabel("Цвет обводки");
    penLocalColorDialogButton = new QPushButton("");
    penLocalColorDialogButton->setFixedSize(colorButtonSize);
    // Работа с системой координат
    coordinatesHorLayout = new QHBoxLayout();
    coordinatesLabel = new QLabel("Координаты");
    coordinatesValue = new QLineEdit();
    coordinatesValue->setReadOnly(true);
    // Смещение
    shiftHorLayout = new QHBoxLayout();
    shiftLabel = new QLabel("Смещение");
    dxValue = new QDoubleSpinBox();
    dyValue = new QDoubleSpinBox();
    dxValue->setMinimum(0.0);
    dxValue->setMaximum(1000.0);
    dxValue->setSingleStep(0.1);

    dyValue->setMinimum(0.0);
    dyValue->setMaximum(1000.0);
    dyValue->setSingleStep(0.1);
    // Масштаб (дополнительно настраивать не нужно)
    scaleHorLayout = new QHBoxLayout();
    scaleLabel = new QLabel("Масштаб");
    scaleValue = new QDoubleSpinBox();
    scaleValue->setMinimum(0.0);
    scaleValue->setMaximum(2.0);
    scaleValue->setValue(1.0);
    scaleValue->setSingleStep(0.1);
    // Кнопки
    buttonsLocalHorLayout = new QHBoxLayout();
    resetLocalProperties = new QPushButton("Оставить все как есть");
    saveLocalProperties = new QPushButton("Сохранить изменения");
    /* --- */
    // Кисти и перья
    brushLocalColorLayout->addWidget(brushLocalColorLabel);
    brushLocalColorLayout->addWidget(brushLocalColorDialogButton);
    //---
    penLocalWidthLayout->addWidget(penLocalWidthLabel);
    penLocalWidthLayout->addWidget(penLocalWidthValue);
    //---
    penLocalColorLayout->addWidget(brushLocalColorLabel);
    penLocalColorLayout->addWidget(penLocalColorDialogButton);
    // Работа с системой координат
    coordinatesHorLayout->addWidget(coordinatesLabel);
    coordinatesHorLayout->addWidget(coordinatesValue);
    // Смещение
    shiftHorLayout->addWidget(shiftLabel, 2);
    shiftHorLayout->addWidget(dxValue, 1);
    shiftHorLayout->addWidget(dyValue, 1);
    // Масштаб
    scaleHorLayout->addWidget(scaleLabel);
    scaleHorLayout->addWidget(scaleValue);
    // Кнопки
    buttonsLocalHorLayout->addWidget(resetLocalProperties);
    buttonsLocalHorLayout->addWidget(saveLocalProperties);
    //---
    localPropertiesLayoutSelected->addLayout(brushLocalColorLayout);
    localPropertiesLayoutSelected->addLayout(penLocalWidthLayout);
    localPropertiesLayoutSelected->addLayout(penLocalColorLayout);
    localPropertiesLayoutSelected->addLayout(coordinatesHorLayout);
    localPropertiesLayoutSelected->addLayout(shiftHorLayout);
    localPropertiesLayoutSelected->addLayout(scaleHorLayout);
    localPropertiesLayoutSelected->addLayout(buttonsLocalHorLayout);
    //---
    groupBoxLayout->addWidget(globalProperties);
    groupBoxLayout->addWidget(parameters);
    groupBoxLayout->addWidget(localPropertiesDefault);
    groupBoxLayout->addWidget(localPropertiesSelected);
    //---
    localPropertiesDefault->setVisible(true);
    localPropertiesSelected->setVisible(false);
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
    connect(brushGlobalColorDialogButton, &QPushButton::clicked, this, &MainWindow::on_brushGlobalColorDialogButton);
    connect(penGlobalColorDialogButton, &QPushButton::clicked, this, &MainWindow::on_penGlobalColorDialogButton);
    connect(saveGlobalProperties, &QPushButton::clicked, this, &MainWindow::on_saveGlobalProperties);
    // Глобальные параметры
    connect(parametersListMenu, &QComboBox::currentTextChanged, this, &MainWindow::on_parametersListChanged);
    connect(saveParameterButton, &QPushButton::clicked, this, &MainWindow::on_saveParameter);
    connect(resetParameterButton, &QPushButton::clicked, this, &MainWindow::on_resetParameter);
    // Локальные свойства
    connect(scene, &GraphicsScene::sendScannedItem, this, &MainWindow::on_sendScannedItem);
    connect(brushLocalColorDialogButton, &QPushButton::clicked, this, &MainWindow::on_brushLocalColorDialogButton);
    connect(penLocalColorDialogButton, &QPushButton::clicked, this, &MainWindow::on_penLocalColorDialogButton);
    connect(resetLocalProperties, &QPushButton::clicked, this, &MainWindow::on_resetLocalProperties);
    connect(saveLocalProperties, &QPushButton::clicked, this, &MainWindow::on_saveLocalProperties);
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
    delete localPropertiesDefault;
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
void MainWindow::on_brushGlobalColorDialogButton() {
    QPalette palette  = brushGlobalColorDialogButton->palette();
    QColor color = palette.color(QPalette::Button);
    QColor tmpColor = QColorDialog::getColor(color, this);
    if (tmpColor.isValid()) {
        brushColor = tmpColor;
        brushColorButtonIcon.fill(brushColor);
        brushGlobalColorDialogButton->setIcon(
            QIcon(brushColorButtonIcon)
            );
    }
}

void MainWindow::on_penGlobalColorDialogButton() {
    QPalette palette  = penGlobalColorDialogButton->palette();
    QColor color = palette.color(QPalette::Button);
    QColor tmpColor = QColorDialog::getColor(color, this);
    if (tmpColor.isValid()) {
        penColor = tmpColor;
        penColorButtonIcon.fill(penColor);
        penGlobalColorDialogButton->setIcon(
            QIcon(penColorButtonIcon)
            );
    }
}

void MainWindow::on_saveGlobalProperties() {
    BedroomFurnitureItem::setGlobalBrush(
        QBrush(brushColor)
    );
    BedroomFurnitureItem::setGlobalPen(
        QPen(penColor, penGlobalWidthValue->value())
    );
    scene->update();
}
// Глобальные параметры
void MainWindow::on_parametersListChanged(const QString& text)
{
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
void MainWindow::on_resetParameter()
{
    on_parametersListChanged(parametersListMenu->currentText());
}
void MainWindow::on_saveParameter()
{
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
// Локальные параметры
void MainWindow::on_sendScannedItem(BedroomFurnitureItem* item)
{
    if (item) {
        itemHandler = item;
        // Кисть
        brushColor = item->getBrush().color();
        brushColorButtonIcon.fill(brushColor);
        brushLocalColorDialogButton->setIcon(
            QIcon(brushColorButtonIcon)
            );
        // Перо
        penColor = item->getPen().color();
        penColorButtonIcon.fill(penColor);
        penLocalColorDialogButton->setIcon(
            QIcon(penColorButtonIcon)
            );
        penLocalWidthValue->setValue(item->getPen().width());
        // Координаты
        coordinatesValue->setText(
            "x=" + QString::number(item->getRectTopLeft().x()) + ", "
                                                                 "y=" + QString::number(item->getRectTopLeft().y())
            );
        localPropertiesDefault->setVisible(false);
        localPropertiesSelected->setVisible(true);
        on_updateInfoSignal("Свойства элемента успешно загружены");
    }
}

void MainWindow::on_brushLocalColorDialogButton()
{
    QPalette palette = brushLocalColorDialogButton->palette();
    QColor color = palette.color(QPalette::Button);
    QColor tmpColor = QColorDialog::getColor(color, this);
    if (tmpColor.isValid()) {
        brushColor = tmpColor;
        brushColorButtonIcon.fill(brushColor);
        brushLocalColorDialogButton->setIcon(
            QIcon(brushColorButtonIcon)
            );
    }
}

void MainWindow::on_penLocalColorDialogButton()
{
    QPalette palette  = penLocalColorDialogButton->palette();
    QColor color = palette.color(QPalette::Button);
    QColor tmpColor = QColorDialog::getColor(color, this);
    if (tmpColor.isValid()) {
        penColor = tmpColor;
        penColorButtonIcon.fill(penColor);
        penLocalColorDialogButton->setIcon(
            QIcon(penColorButtonIcon)
            );
    }
}

void MainWindow::on_resetLocalProperties()
{
    localPropertiesDefault->setVisible(true);
    localPropertiesSelected->setVisible(false);
    itemHandler = nullptr;
    on_updateInfoSignal("Свойства элемента не изменились");
}

void MainWindow::on_saveLocalProperties()
{
    if (itemHandler) {
        itemHandler->setBrush(
            QBrush(brushColor)
            );
        itemHandler->setUsedBrush(BedroomFurnitureItem::UsingBrush::Local);
        itemHandler->setPen(
            QPen(penColor, penLocalWidthValue->value())
            );
        itemHandler->setUsedPen(BedroomFurnitureItem::UsingPen::Local);
        itemHandler->moveBy(dxValue->value(), dyValue->value());
        itemHandler->setScale(scaleValue->value());
        itemHandler = nullptr;
        on_updateInfoSignal("Свойства элемента успешно изменены");
    }
    else {
       on_updateInfoSignal("itemHandler равен nullptr");
    }
    localPropertiesDefault->setVisible(true);
    localPropertiesSelected->setVisible(false);
}
