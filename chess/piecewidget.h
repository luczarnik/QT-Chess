#ifndef PIECEWIDGET_H
#define PIECEWIDGET_H

#include <QLabel>
#include "utils.h"
#include "unordered_map"
#include "QImage"

class PieceWidget : public QLabel
{
    Q_OBJECT
public:

    PieceWidget(Chess::PIECE,Chess::COLOR,QWidget*);
    void resizeEvent(QResizeEvent*) override;

    const Chess::PIECE& getType(){ return type;}
    const Chess::COLOR& getColor() {return color;}

private:
    QPixmap* graphicRepresentation;
    Chess::PIECE type;
    Chess::COLOR color;
    QImage* image;
};

#endif // PIECEWIDGET_H
