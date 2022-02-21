#include "chesswidget.h"
#include <QGuiApplication>
#include <QApplication>
#include "utils.h"
#include <list>
#include "frame.h"

using namespace Chess;



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    /*chessboard->move(Pos(1,3),Pos(3,3));
    chessboard->move(Pos(6,2),Pos(4,2));
    chessboard->move(Pos(3,3),Pos(4,2));
    chessboard->move(Pos(6,1),Pos(5,1));
    chessboard->move(Pos(4,2),Pos(5,1));
    chessboard->move(Pos(6,7),Pos(5,7));
    chessboard->move(Pos(5,1),Pos(6,1));
    chessboard->move(Pos(7,1),Pos(5,2));
    //chessboard->move(Pos(0,2),Pos(2,4));*/

   // ChessWidget chessboard(WHITE,true);
    //chessboard.show();
    Frame* frame = new Frame();
    frame->show();

    return a.exec();
}
