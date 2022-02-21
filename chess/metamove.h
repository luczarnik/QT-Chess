#ifndef METAMOVE_H
#define METAMOVE_H
#include "utils.h"
#include "piece.h"

struct MetaMove
{
    Chess::Piece* b_king_attacked_by_non_penetrable;
    Chess::Piece* w_king_attacked_by_non_penetrable;
    Chess::Move move;
    Chess::Piece* takenPiece;
    Chess::Piece* movedPiece;
    bool castle;
    bool b_en_passant;
    Chess::Pos en_passant_source;
};

#endif // METAMOVE_H
