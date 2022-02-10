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

    QPixmap pixmap("D:/Qt Projects/chessView/resources/pieces.png","png");


    graphicRepresentation= new QPixmap(pixmap.copy(type*200,color*200,200,200));//potential leak
    QPixmap scaled = graphicRepresentation->scaled(parentWidget()->width(),
                                                  parentWidget()->height()
                                                  ,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    setPixmap(scaled);
}

void PieceWidget::resizeEvent(QResizeEvent *event)
{
    int width=parentWidget()->width();
    int height= parentWidget()->height();

    resize(QSize(width,height));
    if (graphicRepresentation->isNull()) qInfo() << "Faild ";
    QPixmap scaled = graphicRepresentation->scaled(width,height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    setPixmap(scaled);
}
