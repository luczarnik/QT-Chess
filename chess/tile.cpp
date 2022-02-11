#include "tile.h"
#include "QMimeData"
#include "QDrag"
#include "piecewidget.h"
#include "utils.h"
#include "QBoxLayout"
#include "QTextCursor"
#include "choosetile.h"
#include "QApplication"


Tile::Tile(QWidget* parent,Chess::COLOR color,Chess::Pos pos,Chess::ChessBoard* chessboard)
    :pieceType(Chess::PIECE::BLANK), position(pos),chessboard(chessboard),tileColor(color),
      QLabel(parent)
{
    chooseTile=nullptr;
    pieceWidget=nullptr;
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setAcceptDrops(true);
    //new QBoxLayout(QBoxLayout::LeftToRight,this);
    if (color == Chess::COLOR::BLACK)   setStyleSheet("QLabel { background-color : green; }");
    else                                setStyleSheet("QLabel { background-color : white; }");
    underAttack=false;

}
void Tile::restore_color()
{
    if (tileColor==Chess::WHITE) setStyleSheet("QLabel { background-color : white; }");
    else    setStyleSheet("QLabel { background-color : green; }");
    underAttack=false;
}


void Tile::mousePressEvent(QMouseEvent * event)
{
    if (underAttack)
    {
        chessboard->move(attackedFrom,position);
        emit clicked();
        placePiece(chessboard->piece_at(position),chessboard->color_at(position));
        repaint();
        return;
    }

    if (pieceType == Chess::PIECE::BLANK) return ;


    dragStartPosition=event->pos();
}

void Tile::mouseReleaseEvent(QMouseEvent *event)
{
    emit clicked();
    std::list<Chess::Move> hint= chessboard->moveList(position);
    emit attacked(position,hint);
}

void Tile::mouseMoveEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton))
            return;

    if ((event->pos() - dragStartPosition).manhattanLength()
             < QApplication::startDragDistance())
            return;
    if (pieceType==Chess::BLANK) return;

    emit clicked();
    std::list<Chess::Move> hint= chessboard->moveList(position);
    emit attacked(position,hint);

    QMimeData *mimeData = new QMimeData;
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);

    dataStream << pieceWidget->getType();
    mimeData->setData("type", itemData);

    itemData.clear();
    dataStream << pieceWidget->getColor();
    mimeData->setData("color",itemData);
    itemData.clear();
    dataStream << position.x<<position.y;
    mimeData->setData("pos",itemData);

    QCursor cursor(Qt::ArrowCursor);
    QPixmap cursorPixmap=cursor.pixmap();


    QPixmap pixmap = pieceWidget->pixmap(Qt::ReturnByValue);
    QDrag *drag = new QDrag(this);

    drag->setDragCursor(cursorPixmap,Qt::MoveAction);
    drag->setDragCursor(cursorPixmap,Qt::IgnoreAction);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(pixmap.width()/2,pixmap.height()/2));

    pieceWidget->hide();
    this->repaint();
    Qt::DropAction ans = drag->exec(Qt::MoveAction);
    if (ans==Qt::IgnoreAction)
    {
        pieceWidget->show();
        this->repaint();
    }
    else
    {
        pieceType=Chess::PIECE::BLANK;
        pieceWidget->deleteLater();
        pieceWidget=nullptr;
    }
}

void Tile::dropEvent(QDropEvent* event)
{
    emit clicked();
    Chess::PIECE type;
    Chess::COLOR color;
    Chess::Pos from;
    const QMimeData *mime=event->mimeData();
    QByteArray buffer;
    QDataStream stream(&buffer,QIODevice::ReadOnly);

    buffer.clear();
    buffer=mime->data("type");  stream >> type;
    buffer.clear();
    buffer=mime->data("color"); stream >> color;
    buffer.clear();
    buffer=mime->data("pos"); stream>>from.x>>from.y;



    if (!chessboard->move(from,position)) return;

    event->acceptProposedAction();
    pieceColor=chessboard->color_at(position);
    pieceType=chessboard->piece_at(position);
    if (pieceWidget!=nullptr)   pieceWidget->deleteLater();
    pieceWidget=new PieceWidget(pieceType,pieceColor,this);
    pieceWidget->show();

}

void Tile::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->source()==this) return;
    event->acceptProposedAction();
}

void Tile::placePiece(Chess::PIECE type,Chess::COLOR color)
{
    if (pieceWidget!=nullptr)
    {
        pieceWidget->hide();
        pieceWidget->deleteLater();
    }
    pieceWidget=nullptr;
    pieceType=type;
    pieceColor=color;
    if (type == Chess::BLANK ) return;

    pieceWidget = new PieceWidget(type,color,this);
    pieceWidget->show();
}
void Tile::resizeEvent(QResizeEvent *event)
{
    if (pieceType==Chess::PIECE::BLANK) return;
    pieceWidget->resizeEvent(event);

    if (chooseTile == nullptr) return;
    chooseTile->resizeEvent(event);
}

void Tile::enhanceWith(const Chess::Pos& pos)
{
    attackedFrom=pos;
    if (pieceType== Chess::BLANK) setStyleSheet("QLabel { background-color : blue; }");
    else setStyleSheet("QLabel { background-color : red; }");
    underAttack=true;
}
