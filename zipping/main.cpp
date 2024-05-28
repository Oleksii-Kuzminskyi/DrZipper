#include <QtGui/QIcon>

#include <QtWidgets/QApplication>

#include "Zipper.h"

extern QString read_content(const QString & filename);

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/zipper.png"));

    app.setStyleSheet(read_content(":/appstyle.qss")); //to do: add file

    Zipper zipper;
    zipper.setWindowFlag(Qt::FramelessWindowHint);
    zipper.show();


    return app.exec();
}
