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
    void mouseMoveEvent(QMouseEvent*) override;
    void dropEvent(QDropEvent*) override;
    void dragEnterEvent(QDragEnterEvent*) override;
    void placePiece(Chess::PIECE,Chess::COLOR);
    void create_context_menu();
    void hide_context_menu();
    void setChooseTile(ChooseTile* ch) {chooseTile=ch;}
    void removeChooseTile() {chooseTile=nullptr;}
    void restore_color();
    void enhanceWith(const Chess::Pos& pos);
    void mouseReleaseEvent(QMouseEvent *event);
signals:
    void clicked();
    void attacked(const Chess::Pos&,std::list<Chess::Move>);
    void refresh(const Chess::Pos&);
    void promotion(const Chess::Pos&, const Chess::Pos&);
    void madeMove();

private:
    Chess::COLOR pieceColor;
    Chess::PIECE pieceType;
    PieceWidget *pieceWidget;
    Chess::Pos position;
    Chess::Pos attackedFrom;
    bool underAttack;
    Chess::ChessBoard* chessboard;
    ChooseTile* chooseTile;
    Chess::COLOR tileColor;
    QPoint dragStartPosition;

    void setEmoty();
    void setFigure(QImage image);
    void resizeEvent(QResizeEvent*) override;

};



#endif // TILE_H
