#include "chessboard.h"
#include <stdexcept>
#include "piece.h"
#include "utils.h"
#include "QtDebug"
#include "QDebug"
#include <algorithm>
#include <list>
#include <fstream>
#include "metamove.h"




using namespace Chess;

ChessBoard::ChessBoard(QObject* parent)
    : MOVE_TURN(WHITE), QObject(parent)
{
    MOVE_TURN=WHITE;

    initialize_pieces();
    debug_output.open("output.txt",std::ostream::out);
}


void ChessBoard::initialize_pieces()
{
    for (int i=0;i<8;i++)
    {
        pieces[Pos(1,i)]= new Pon(WHITE,Pos(1,i));

        pieces[Pos(6,i)]= new Pon(BLACK,Pos(6,i));
    }
    pieces[Pos(0,0)]=new Rook    (WHITE,Pos(0,0));
    pieces[Pos(0,1)]=new Knight  (WHITE,Pos(0,1));
    pieces[Pos(0,2)]=new Bishop  (WHITE,Pos(0,2));
    pieces[Pos(0,3)]=new Queen   (WHITE,Pos(0,3));
    pieces[Pos(0,4)]=new King    (WHITE,Pos(0,4));
    pieces[Pos(0,5)]=new Bishop  (WHITE,Pos(0,5));
    pieces[Pos(0,6)]=new Knight  (WHITE,Pos(0,6));
    pieces[Pos(0,7)]=new Rook    (WHITE,Pos(0,7));

    pieces[Pos(7,0)]=new Rook    (BLACK,Pos(7,0));
    pieces[Pos(7,1)]=new Knight  (BLACK,Pos(7,1));
    pieces[Pos(7,2)]=new Bishop  (BLACK,Pos(7,2));
    pieces[Pos(7,3)]=new Queen   (BLACK,Pos(7,3));
    pieces[Pos(7,4)]=new King    (BLACK,Pos(7,4));
    pieces[Pos(7,5)]=new Bishop  (BLACK,Pos(7,5));
    pieces[Pos(7,6)]=new Knight  (BLACK,Pos(7,6));
    pieces[Pos(7,7)]=new Rook    (BLACK,Pos(7,7));

    w_king=pieces[Pos(0,4)]; b_king=pieces[Pos(7,4)];
    w_penetrable.insert(Pos(0,2));
    w_penetrable.insert(Pos(0,3));
    w_penetrable.insert(Pos(0,5));
    w_penetrable.insert(Pos(0,0));
    w_penetrable.insert(Pos(0,7));

    b_penetrable.insert(Pos(7,2));
    b_penetrable.insert(Pos(7,3));
    b_penetrable.insert(Pos(7,5));
    b_penetrable.insert(Pos(7,0));
    b_penetrable.insert(Pos(7,7));

    refresh_chessboard(occupied);
    refresh_chessboard(attacked_by_black);
    refresh_chessboard(attacked_by_white);

    for (int i=0;i<8;i++)
    {
        occupied[0][i] =true; occupied[1][i]= true;
        occupied[6][i] =true; occupied[7][i] = true;

        w_pieces.insert(Pos(0,i));
        w_pieces.insert(Pos(1,i));
        w_pieces_non_volatile.insert(Pos(0,i));
        w_pieces_non_volatile.insert(Pos(1,i));

        b_pieces.insert(Pos(7,i));
        b_pieces.insert(Pos(6,i));
        b_pieces_non_volatile.insert(Pos(7,i));
        b_pieces_non_volatile.insert(Pos(6,i));
    }

    b_king_attacked_by_non_penetrable=nullptr;
    w_king_attacked_by_non_penetrable=nullptr;

    b_en_passant=false;
}

void ChessBoard::move(const Pos& from,  Pos to )
{
    Piece* piece=pieces[from];



    if (piece->get_type()==PON )//promotion
    {
        if ( (MOVE_TURN == WHITE && from.x==6) || (MOVE_TURN==BLACK && from.x==1))
        {
            switch (std::abs(to.x-from.x))
            {
            case 1: piece= new Queen(MOVE_TURN,to); break;
            case 2: piece= new Rook(MOVE_TURN,to); break;
            case 3: piece= new Knight(MOVE_TURN,to); break;
            case 4: piece= new Bishop(MOVE_TURN,to); break;
            }
            if (MOVE_TURN==WHITE) to.x=7;
            else to.x=0;
        }
    }
    registerMove(from,to);



    b_king_attacked_by_non_penetrable=nullptr;
    w_king_attacked_by_non_penetrable=nullptr;
    b_en_passant=false;


    bool taken=false;
    if (pieces.find(to)!=pieces.end())
    {
        hard_remove_piece(to);
        taken = true;
    }

    if (piece->get_type()==KING && abs(from.y-to.y) == 2)// castle
    {
        perform_castle(to);
        switch_turn();
        return;
    }
    else if (piece->get_type()==PON && from.y!=to.y && !taken)//en passant
        hard_remove_piece(en_passant_source);




    hard_remove_piece(from);
    hard_insert_piece(piece,to);

    if (piece->get_type()== ROOK) dynamic_cast<Rook*>(piece)->forbidd_castle();
    else if (piece->get_type()==KING) dynamic_cast<King*>(piece)->forbidd_castle();


    // check whther king is on attacked square, if so turn the check state on
    else if (piece->get_type() == PON || piece->get_type()==KNIGHT)
    {
        Pos pos;
        if (MOVE_TURN==WHITE)
        {
            refresh_chessboard(attacked_by_white);
            piece->mark_attacked(attacked_by_white,occupied);
            pos=b_king->get_pos();
            if (attacked_by_white[pos.x][pos.y]) b_king_attacked_by_non_penetrable=piece;
        }
        else
        {
            refresh_chessboard(attacked_by_black);
            piece->mark_attacked(attacked_by_black,occupied);
            pos=w_king->get_pos();
            if (attacked_by_black[pos.x][pos.y])
                w_king_attacked_by_non_penetrable=piece;
        }

        if (piece->get_type()==PON && abs(from.x-to.x)==2)
        {
            b_en_passant=true;
            en_passant_source=to;
        }

    }

    switch_turn();
    //if (!haveMoves()) qInfo()<<"The end";

}

bool ChessBoard::is_legal(const Pos& from,  Pos to)
{


    bool answer=true;
    //if (pieces.find(from) == pieces.end()) return false;
    if (MOVE_TURN==WHITE)   refresh_chessboard(attacked_by_black);
    else                    refresh_chessboard(attacked_by_white);

    Piece* piece = pieces[from];
    if (piece->get_type()==PON)
    {
        if (to.x>7) to.x=7;
        else if (to.x < 0 ) to.x=0;
    }


    if (!piece->is_legal(to,occupied)) return false;

    COLOR color = piece->get_color();
    if (color!=MOVE_TURN ||
            (pieces.find(to)!=pieces.end()&&pieces[to]->get_color()==MOVE_TURN)  ) return false;


    bool kingMove=false;
    if (piece->get_type()==KING)
    {
        if (dynamic_cast<King*>(piece)->can_castle())
        {
            if (castle(to)) return true;
            else if (abs(from.y-to.y) >1 ) return false;
        }
        kingMove =true;
    }

    Piece* enemy = nullptr;
    if (pieces.find(to)!=pieces.end())
    {
        enemy=pieces[to];
        //if in previous move king was put in check then if piece who was checking was non-penetrable type
        //it must be taken down, or king must be moved
        if ((w_king_attacked_by_non_penetrable!=nullptr||
             b_king_attacked_by_non_penetrable!=nullptr) &&
                !(w_king_attacked_by_non_penetrable==enemy || b_king_attacked_by_non_penetrable==enemy)
                && !kingMove)
            return false;

        remove_piece(to);
    }
    else if ((w_king_attacked_by_non_penetrable!=nullptr||
             b_king_attacked_by_non_penetrable!=nullptr) && !kingMove) return false;



    if (piece->get_type()==PON )
    {
        if (from.y!=to.y)
        {
            if (enemy==nullptr )
            {
                // if can enpassant then on pos "to" there certainly is no enemy,
                //so we can return withowt carring about potentialy deleted enemy pon like in other cases

                if (b_en_passant&&to.y==en_passant_source.y&&from.x==en_passant_source.x)
                {
                    enemy=pieces[en_passant_source];
                    remove_piece(en_passant_source);
                }
                else return false;
            }
        }

    }

    remove_piece(from);
    insert_piece(piece,to);


    Pos pos;
    if (color==WHITE)
    {
        //if king moved we have to check every opponent piece if is attacking king
        if (kingMove)   for (const Pos &pos :b_pieces)
            pieces[pos]->mark_attacked(attacked_by_black,occupied);

        //otherwise king is eather already attacked by penetrable, or we made path for penetrable.
        // we rejected attacking by non-penetrable in earlier stage
        else   for (const Pos &pos: b_penetrable)
            pieces[pos]->mark_attacked(attacked_by_black,occupied);

        pos = w_king->get_pos();
        //display_table(attacked_by_black);

        if (attacked_by_black[pos.x][pos.y]) answer = false;//is king checked?
        else answer = true;
    }
    else//mirror to white
    {
        if (kingMove)   for (const Pos& pos: w_pieces)
            pieces[pos]->mark_attacked(attacked_by_white,occupied);

        else            for (const Pos& pos: w_penetrable)
            pieces[pos]->mark_attacked(attacked_by_white,occupied);

        pos = b_king->get_pos();
        //display_table(attacked_by_white);

        if (attacked_by_white[pos.x][pos.y]) answer = false;
        else answer = true;
    }


    remove_piece(to);
    if (enemy!= nullptr)    insert_piece(enemy,enemy->get_pos());
    insert_piece(piece,from);



    return answer;
}


const PIECE ChessBoard::piece_at(const Pos& pos)
{
    if (pieces.find(pos) == pieces.end()) return BLANK;

    return pieces[pos]->get_type();
}
const COLOR ChessBoard::color_at(const Pos& pos)
{
    if (pieces.find(pos) == pieces.end()) return WHITE;

    return pieces[pos]->get_color();
}


void ChessBoard::remove_piece(const Pos& pos)
{

    occupied[pos.x][pos.y]=false;
    Piece *piece = pieces[pos];
    pieces.erase(pos);

    if (piece->get_color()==WHITE) w_pieces.erase(pos);
    else b_pieces.erase(pos);

    if (!is_penetrable(piece->get_type())) return;

    if (piece->get_color() == WHITE) w_penetrable.erase(pos);
    else                             b_penetrable.erase(pos);
    //delete piece;
}
void ChessBoard::hard_remove_piece(const Pos& pos)
{
    Piece *piece = pieces[pos];
    remove_piece(pos);

    if (piece->get_color()==WHITE) w_pieces_non_volatile.erase(pos);
    else b_pieces_non_volatile.erase(pos);

}

void ChessBoard::insert_piece(Piece* piece, const Pos& pos)
{
    occupied[pos.x][pos.y]=true;
    piece->move(pos);
    pieces[pos]=piece;

    if (piece->get_color()==WHITE) w_pieces.insert(pos);
    else b_pieces.insert(pos);

    if (!is_penetrable(piece->get_type())) return;

    if (piece->get_color()==WHITE) w_penetrable.insert(pos);
    else b_penetrable.insert(pos);
}
void ChessBoard::hard_insert_piece(Piece* piece, const Pos& pos)
{
    insert_piece(piece,pos);
    if (piece->get_color()==WHITE) w_pieces_non_volatile.insert(pos);
    else b_pieces_non_volatile.insert(pos);
}

bool ChessBoard::castle(const Pos& pos)
{
    if (!(pos == Pos(0,2) || pos==Pos(0,6) || pos == Pos(7,2) || pos ==Pos(7,6))) return false;



    if (pos.x ==0) for (const Pos &pos :b_pieces)
        pieces[pos]->mark_attacked(attacked_by_black,occupied);
    else for (const Pos &pos :w_pieces)
        pieces[pos]->mark_attacked(attacked_by_white,occupied);


    if (pos == Pos(0,2) && !attacked_by_black[0][2] && !attacked_by_black[0][3] && !attacked_by_black[0][4]) return castle_rook(Pos(0,0));
    if (pos == Pos(0,6) && !attacked_by_black[0][4] && !attacked_by_black[0][5] && !attacked_by_black[0][6]) return castle_rook(Pos(0,7));
    if (pos == Pos(7,2) && !attacked_by_white[7][2] && !attacked_by_white[7][3] && !attacked_by_white[7][4]) return castle_rook(Pos(7,0));
    if (pos == Pos(7,6) && !attacked_by_white[7][4] && !attacked_by_white[7][5] && !attacked_by_white[7][6]) return castle_rook(Pos(7,7));

    return false;
}

bool ChessBoard::castle_rook(const Pos& pos)
{
    if (pieces.find(pos)==pieces.end()) return false;
    return dynamic_cast<Rook*>(pieces[pos])->can_castle();
}

void ChessBoard::switch_turn()
{
    if (MOVE_TURN == WHITE ) MOVE_TURN=BLACK;
    else MOVE_TURN = WHITE;
}

void ChessBoard::perform_castle(const Pos & pos)
{
    Piece* rook , *king;
    if (pos==Pos(0,2))
    {
        rook=pieces[Pos(0,0)];
        king=pieces[Pos(0,4)];
        dynamic_cast<Rook*>(rook)->forbidd_castle();
        dynamic_cast<King*>(king)->forbidd_castle();
        hard_remove_piece(Pos(0,0));
        hard_remove_piece(Pos(0,4));
        hard_insert_piece(king,Pos(0,2));
        hard_insert_piece(rook,Pos(0,3));

    }
    else if (pos==Pos(0,6))
    {
        rook=pieces[Pos(0,7)];
        king=pieces[Pos(0,4)];
        dynamic_cast<Rook*>(rook)->forbidd_castle();
        dynamic_cast<King*>(king)->forbidd_castle();
        hard_remove_piece(Pos(0,7));
        hard_remove_piece(Pos(0,4));
        hard_insert_piece(king,Pos(0,6));
        hard_insert_piece(rook,Pos(0,5));

    }
    else if (pos==Pos(7,2))
    {
        rook=pieces[Pos(7,0)];
        king=pieces[Pos(7,4)];
        dynamic_cast<Rook*>(rook)->forbidd_castle();
        dynamic_cast<King*>(king)->forbidd_castle();
        hard_remove_piece(Pos(7,0));
        hard_remove_piece(Pos(7,4));
        hard_insert_piece(king,Pos(7,2));
        hard_insert_piece(rook,Pos(7,3));
    }
    else if (pos==Pos(7,6))
    {
        rook=pieces[Pos(7,7)];
        king=pieces[Pos(7,4)];
        dynamic_cast<Rook*>(rook)->forbidd_castle();
        dynamic_cast<King*>(king)->forbidd_castle();
        hard_remove_piece(Pos(7,7));
        hard_remove_piece(Pos(7,4));
        hard_insert_piece(king,Pos(7,6));
        hard_insert_piece(rook,Pos(7,5));

    }
    //if (!haveMoves()) qInfo()<<"The end";
}



bool ChessBoard::haveMoves()
{
    std::list<Move> moves;
    if ( MOVE_TURN==WHITE)
    {
        /*for (auto it = begin(w_pieces);it!=end(w_pieces);it++)
        {
            moves=pieces[*it]->moveList(occupied);
            for (const Move& move:moves) if(is_legal_neutral(move)) return true;
        }*/
        for (const Pos &pos : w_pieces_non_volatile)
        {
            moves=pieces[pos]->moveList(occupied);
            for (const Move& move:moves)
                if(is_legal(move.from,move.to)) return true;
        }
    }
    else
    {
        /*for (auto it = begin(b_pieces);it!=end(b_pieces);it++)
        {
            moves=pieces[*it]->moveList(occupied);
            for (const Move& move:moves) if(is_legal_neutral(move)) return true;
        }*/
        for (const Pos &pos : b_pieces_non_volatile)
        {
            moves=pieces[pos]->moveList(occupied);
            for (const Move& move:moves) if(is_legal(move.from,move.to)) return true;
        }
    }
    return false;
}

std::list<Move> ChessBoard::movesList()
{
    std::list<Move> ans;
    std::list<Move> moves;
    if ( MOVE_TURN==WHITE)
    {
        \
        for (const Pos &pos : w_pieces_non_volatile)
        {
            moves=moveList(pos);
            ans.merge(moves);
        }
    }
    else
    {
        for (const Pos &pos : b_pieces_non_volatile)
        {
            moves=moveList(pos);
            ans.merge(moves);
        }
    }
    return ans;
}

std::list<Move> ChessBoard::moveList(const Chess::Pos& pos)
{
    std::list<Move> moves;
    if (pieces.find(pos)==pieces.end()) return moves;
    moves=pieces[pos]->moveList(occupied);
    Move move;
    for (auto it=begin(moves);it!=end(moves);)
    {
       move=*it;
       if (!is_legal(move.from,move.to))
       {
           it =moves.erase(it);
           continue;
       }
       it++;
    }
    return moves;
}
void ChessBoard::registerMove(const Pos &from, const Pos &to)
{
    MetaMove metamove;
    metamove.move=Move(from,to);
    metamove.b_king_attacked_by_non_penetrable=b_king_attacked_by_non_penetrable;
    metamove.w_king_attacked_by_non_penetrable=w_king_attacked_by_non_penetrable;
    metamove.b_en_passant=b_en_passant;
    metamove.en_passant_source=en_passant_source;

    Piece* piece = pieces[from];
    metamove.movedPiece=piece;
    metamove.castle = false;
    if (piece->get_type()==KING)
        metamove.castle = dynamic_cast<King*>(piece)->can_castle();
    else if (piece->get_type()==ROOK)
        metamove.castle = dynamic_cast<Rook*>(piece)->can_castle();

    if (pieces.find(to)==pieces.end())  metamove.takenPiece=nullptr;
    else metamove.takenPiece=pieces[to];

    if (metamove.movedPiece->get_type()==PON && from.y != to.y && metamove.takenPiece==nullptr)
        metamove.takenPiece=pieces[Pos(from.x,to.y)];

    moves.push(metamove);
}

void ChessBoard::undoMove()
{
    //debug_output<<Pos(-1,-1)<<std::endl;
    if (moves.empty()) return;


    MetaMove metamove = moves.top();
    moves.pop();

    b_king_attacked_by_non_penetrable=metamove.b_king_attacked_by_non_penetrable;
    w_king_attacked_by_non_penetrable=metamove.w_king_attacked_by_non_penetrable;
    b_en_passant=metamove.b_en_passant;
    en_passant_source=metamove.en_passant_source;


    Piece* movedPiece=metamove.movedPiece;
    Piece* standingPiece=pieces[metamove.move.to];


    if (movedPiece->get_type()==KING && std::abs(metamove.move.from.y-metamove.move.to.y)==2)
    {
        if (metamove.move.to == Pos(0,2))
        {
            Piece* rook = pieces[Pos(0,3)];
            Piece* king= pieces[Pos(0,2)];
            hard_remove_piece(Pos(0,3));
            hard_insert_piece(rook,Pos(0,0));
            dynamic_cast<Rook*>(rook)->allow_castle();
            dynamic_cast<King*>(king)->allow_castle();
        }
        else if (metamove.move.to == Pos(0,6))
        {
            Piece* rook = pieces[Pos(0,5)];
            Piece* king= pieces[Pos(0,6)];
            hard_remove_piece(Pos(0,5));
            hard_insert_piece(rook,Pos(0,7));
            dynamic_cast<Rook*>(rook)->allow_castle();
            dynamic_cast<King*>(king)->allow_castle();
        }
        else if (metamove.move.to == Pos(7,2))
        {
            Piece* rook = pieces[Pos(7,3)];
            Piece* king= pieces[Pos(7,2)];
            hard_remove_piece(Pos(7,3));
            hard_insert_piece(rook,Pos(7,0));
            dynamic_cast<Rook*>(rook)->allow_castle();
            dynamic_cast<King*>(king)->allow_castle();
        }
        else if (metamove.move.to == Pos(7,6))
        {
            Piece* rook = pieces[Pos(7,5)];
            Piece* king= pieces[Pos(7,6)];
            hard_remove_piece(Pos(7,5));
            hard_insert_piece(rook,Pos(7,7));
            dynamic_cast<Rook*>(rook)->allow_castle();
            dynamic_cast<King*>(king)->allow_castle();
        }
    }


    hard_remove_piece(metamove.move.to);
    hard_insert_piece(movedPiece,metamove.move.from);

    if (movedPiece->get_type()== ROOK && metamove.castle)  dynamic_cast<Rook*>(movedPiece)->allow_castle();
    else if (movedPiece->get_type()==KING && metamove.castle)  dynamic_cast<King*>(movedPiece)->allow_castle();

    if (standingPiece!=movedPiece)
        delete standingPiece;

    if (metamove.takenPiece!=nullptr)
        hard_insert_piece(metamove.takenPiece,metamove.takenPiece->get_pos());




    switch_turn();
}

int ChessBoard::evaluate()
{
    int ans=0;
    int multiplier;
    for (auto pair : pieces)
    {
        multiplier=pair.second->get_color()==WHITE?1:-1;
        switch(pair.second->get_type())
        {
        case PON: ans+=1*multiplier; break;
        case KNIGHT: ans+=3; break;
        case BISHOP: ans+=3; break;
        case ROOK: ans+=5;break;
        case QUEEN: ans+=9; break;
        default: break;
        }
    }
}

void ChessBoard::check_status()
{
    if (haveMoves()) return;

    if (MOVE_TURN==WHITE)
    {
        if (w_king_attacked_by_non_penetrable!=nullptr)
        {
            emit checkmate(BLACK);
            return;
        }
        for (const Pos& pos : b_penetrable)
            pieces[pos]->mark_attacked(attacked_by_black,occupied);

        Pos pos=w_king->get_pos();
        if (attacked_by_black[pos.x][pos.y])
        {
            emit checkmate(BLACK);
            return;
        }
    }
    else
    {
        if (b_king_attacked_by_non_penetrable!=nullptr)
        {
            emit checkmate(WHITE);
            return;
        }
        for (const Pos& pos : w_penetrable)
            pieces[pos]->mark_attacked(attacked_by_white,occupied);

        Pos pos=b_king->get_pos();
        if (attacked_by_white[pos.x][pos.y])
        {
            emit checkmate(WHITE);
            return;
        }
    }
    emit stalemate();
}
