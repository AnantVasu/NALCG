#ifndef _NALCG_SQUARE_H_
#define _NALCG_SQUARE_H_

// system includes
#include <string>

// class includes
#include "piece.h"
#include "colours.h"

class Square
{
    // Constants

    // Members
    Piece* piece;

    // Methods

public:
    Square(Piece* piece = 0);
    virtual ~Square() { }

    // Methods
    bool hasUnit() { return piece != 0; }

    // Getters
    const std::string getName() const;
    enum Colours getColour() const;
    
    // Setters
};

#endif // _NALCG_SQUARE_H_