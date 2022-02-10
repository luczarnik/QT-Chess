#include "tile.h"
#include "QMimeData"
#include "QDrag"
#include "piecewidget.h"
#include "utils.h"
#include "QBoxLayout"
#include "QTextCursor"
#include "choosetile.h"


Tile::Tile(QWidget* parent,Chess::COLOR color,Chess::Pos pos,Chess::ChessBoard* chessboard)
    :pieceType(Chess::PIECE::BLANK), position(pos),chessboard(chessboard),
      QLabel(parent)
{

    chooseTile=nullptr;
    pieceWidget=nullptr;
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setAcceptDrops(true);
    //new QBoxLayout(QBoxLayout::LeftToRight,this);
    if (color == Chess::COLOR::BLACK)   setStyleSheet("QLabel { background-color : green; }");
    else                                setStyleSheet("QLabel { background-color : white; }");

}


void Tile::mousePressEvent(QMouseEvent * event)
{
    emit clicked();
    if (pieceType == Chess::PIECE::BLANK) return ;

    QMimeData *mimeData = new QMimeData;
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);

    dataStream << pieceWidget->getType();
    mimeData->setData("type", itemData);

    itemData.clear();
    dataStream << pieceWidget->getColor();
    mimeData->setData("color",itemData);

/*
    Chess::PIECE type; Chess::COLOR color;
    QByteArray arr;
    QDataStream outstream(&arr,QIODevice::ReadOnly);
    arr.clear();
    arr=mimeData->data("color");
    outstream >>  color;
    arr.clear();
    arr=mimeData->data("type");
    outstream >> type;


    qInfo()<<type<<color<<pieceWidget->getType()<< pieceWidget->getColor()<<" original";
    qInfo()<<mimeData->hasFormat("type");*/

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

    event->ignore();
}

void Tile::dropEvent(QDropEvent* event)
{
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
    //if (pieceWidget!=nullptr)   pieceWidget->deleteLater();
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
    if (pieceWidget!=nullptr) pieceWidget->deleteLater();
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
