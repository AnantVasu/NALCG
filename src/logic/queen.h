#ifndef _NALCG_QUEEN_H_
#define _NALCG_QUEEN_H_

// class includes
#include "piece.h"
#include "colour.h"
#include "square.h"

// system includes

class Queen : public Piece
{
    // Constants
    static const int X_DIRECTIONS[];
    static const int Y_DIRECTIONS[];

    // Members

    // Methods

public:
    Queen(const Colour& colour);
    virtual ~Queen() { }
    Queen* clone() const { return new Queen(*this); }

    // Methods

    // Getters
    virtual std::vector<std::size_t> getValidMoves(std::size_t location,
        const std::vector<Square>& squares, std::size_t protect = ~0) const;

    // Setters
};

#endif // _NALCG_QUEEN_H_
