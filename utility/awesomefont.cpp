#include "awesomefont.h"

AwesomeFont* AwesomeFont::m_instance = nullptr;
AwesomeFont::AwesomeFont(QObject *parent)
    : QObject{parent} {
    int font_id=QFontDatabase::addApplicationFont(":/myfonts/resources/fontawesome-webfont.ttf");
    QString font_name=QFontDatabase::applicationFontFamilies(font_id).at(0);
    iconFont=QFont(font_name);
}
