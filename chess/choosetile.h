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
                           Chess::COLOR color, Chess::ChessBoard* chessboard);
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
};

#endif // CHOOSETILE_H
