#include <QApplication>
#include "formatChanger.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FormatChanger formatChanger;
    formatChanger.show();

    return app.exec();
}
