#include "piecewidget.h"
#include "QPixmap"
#include "QWidget"
#include <Qt>
#include "QMouseEvent"
#include "QImage"

PieceWidget::PieceWidget(Chess::PIECE type,Chess::COLOR color,QWidget* parent)
    :type(type),color(color),QLabel(parent)
{
    if (type == Chess::PIECE::BLANK)
        return;
    resize(parent->size());

    QPixmap pixmap("D:/Qt Projects/chessView/resources/king.png","png");


    graphicRepresentation= new QPixmap(pixmap.copy(type*45,color*45,45,45));//potential leak
    QPixmap scaled = graphicRepresentation->scaled(parentWidget()->width(),
                                                  parentWidget()->height()
                                                  ,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    setPixmap(scaled);
    setMask(scaled.mask());



}

void PieceWidget::resizeEvent(QResizeEvent *event)
{
    int width=parentWidget()->width();
    int height= parentWidget()->height();

    resize(event->size());
    if (graphicRepresentation->isNull()) qInfo() << "Faild ";
    QPixmap scaled = graphicRepresentation->scaled(width,height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    setPixmap(scaled);
    setMask(scaled.mask());
}
