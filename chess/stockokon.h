#ifndef STOCKOKON_H
#define STOCKOKON_H

#include "utils.h"
#include "chessboard.h"
#include "QObject"

using namespace Chess;

class StockOkon : public QObject
{
    Q_OBJECT
private:
    Chess::COLOR side;
    int evaluateBest(int,COLOR);
    ChessBoard *chessboard;
public:
    StockOkon(Chess::COLOR,Chess::ChessBoard* chessboard,QObject* );
public slots:
    void makeMove();

signals:
    void refresh(const Chess::Pos&);

};

#endif // STOCKOKON_H
