#include "chesswidget.h"
#include <QGuiApplication>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChessWidget chess;
    chess.show();

    return a.exec();
}
