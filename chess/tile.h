 #ifndef TILE_H
#define TILE_H

#include <QLabel>
#include "QMouseEvent"
#include "utils.h"
#include "piecewidget.h"
#include "chessboard.h"
#include "choosetile.h"



class Tile : public QLabel
{
    Q_OBJECT
public:

    Tile(QWidget* parent, Chess::COLOR, Chess::Pos,Chess::ChessBoard*);
    void mousePressEvent(QMouseEvent*) override;
    void dropEvent(QDropEvent*) override;
    void dragEnterEvent(QDragEnterEvent*) override;
    void placePiece(Chess::PIECE,Chess::COLOR);
    void create_context_menu();
    void hide_context_menu();
    void setChooseTile(ChooseTile* ch) {chooseTile=ch;}
    void removeChooseTile() {chooseTile=nullptr;}
    void restore_color();
signals:
    void clicked();
    void attacked(std::list<Chess::Move>);

private:
    Chess::COLOR pieceColor;
    Chess::PIECE pieceType;
    PieceWidget *pieceWidget;
    Chess::Pos position;
    Chess::ChessBoard* chessboard;
    ChooseTile* chooseTile;
    Chess::COLOR tileColor;

    void setEmoty();
    void setFigure(QImage image);
    void resizeEvent(QResizeEvent*) override;

};



#endif // TILE_H
