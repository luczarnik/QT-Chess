#ifndef CHOOSETILE_H
#define CHOOSETILE_H

#include <QLabel>
#include "utils.h"
#include "QMouseEvent"
#include "piecewidget.h"
#include "chessboard.h"

class ChooseTile : public QLabel
{
    Q_OBJECT
public:
    ChooseTile(QWidget *Parent,Chess::PIECE piece,
                           Chess::COLOR color,const Chess::Pos&,const Chess::Pos&, Chess::ChessBoard* chessboard);
    void resizeEvent(QResizeEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
signals:
    void chosen(Chess::PIECE piece);
    void clicked();

private:
    Chess::COLOR pieceColor;
    Chess::PIECE pieceType;
    PieceWidget *pieceWidget;
    Chess::ChessBoard* chessboard;
    Chess::Pos position;
    Chess::Pos attacked;

signals:
    void refresh(const Chess::Pos&);
    void madeMove();
};

#endif // CHOOSETILE_H
