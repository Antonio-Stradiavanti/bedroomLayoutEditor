#include "bedroomFurnitureItems.h"

QBrush BedroomFurnitureItem::globalBrush = QBrush(Qt::blue, Qt::SolidPattern);
QPen BedroomFurnitureItem::globalPen = QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

double WallTypeItem::param = 27.0;
double WallTypeItem::height = WallTypeItem::param;

double ChairItem::param = 90.0;
double ChairItem::width = (23.0/15.0)*param;
double ChairItem::height = 1.1*param;

double BedItem::param = 200.0;
double BedItem::width = 0.7*param;
double BedItem::height = param;

double PianoItem::param = 160.0;
double PianoItem::width = 2.3*param;
double PianoItem::height = 1.7*param;

double DeskItem::param = 90.0;
double DeskItem::width = 3.0*param;
double DeskItem::height = (5.0/3.0)*param;

double WardrobeItem::param = 100.0;
double WardrobeItem::width = 2.5*param;
double WardrobeItem::height = param;
