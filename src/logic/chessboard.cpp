// system includes

// class dependencies
#include "chessboard.h"
#include "piece.h"
#include "pawn.h"
#include "colours.h"

ChessBoard::ChessBoard() :
    Board(WIDTH, HEIGHT)
{

}

// Chess board looks like this
//
//    A  B  C  D  E  F  G  H
// 8 |br|bk|bb|bq|bK|bb|bk|br| 8
// 7 |bp|bp|bp|bp|bp|bp|bp|bp| 7
// 6 |  |  |  |  |  |  |  |  | 6
// 5 |  |  |  |  |  |  |  |  | 5
// 4 |  |  |  |  |  |  |  |  | 4
// 3 |  |  |  |  |  |  |  |  | 3
// 2 |wp|wp|wp|wp|wp|wp|wp|wp| 2
// 1 |wr|wk|wb|wq|wK|wb|wk|wr| 1
//    A  B  C  D  E  F  G  H
//
// br = black rook, bk = black knight, bb = black bishop
// bq = black queen, bK = black king, bp = black pawn
// wr = white rook, wk = white knight, wb = white bishop
// wq = white queen, wK = white king, wp = white pawn
//
// Therefore the numbering in one dimensional array goes like this:
//    A  B  C  D  E  F  G  H
// 8 |00|01|02|03|04|05|06|07| 8
// 7 |08|09|10|11|12|13|14|15| 7
// 6 |16|17|18|19|20|21|22|23| 6
// 5 |24|25|26|27|28|29|30|31| 5
// 4 |32|33|34|35|36|37|38|39| 4
// 3 |40|41|42|43|44|45|46|47| 3
// 2 |48|49|50|51|52|53|54|55| 2
// 1 |56|57|58|59|60|61|62|63| 1
//    A  B  C  D  E  F  G  H
std::vector<Square> ChessBoard::createBoard()
{
    enum Colours black = BLACK;
    enum Colours white = WHITE;
    
    std::vector<Square> squares;

    for (std::size_t i = 0; i < 64; ++i)
    {
        Square* square = 0;
        Piece* piece = 0;
        if (i >= 8 && i <= 15)
        {
            piece = new Pawn(black);
        }
        else if (i >= 48 && i <= 55)
        {
            piece = new Pawn(white);
        }

        if (piece) { square = new Square(piece); }
        else { square = new Square(); }

        squares.push_back(*square);
        delete square;
    }

    return squares;
}