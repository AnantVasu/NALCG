#ifndef _NALCG_PIECE_H_
#define _NALCG_PIECE_H_

// system includes
#include <string>
#include <vector>

// class includes

class Square;
class Piece
{
public:
    enum Colour
    {
        UNDEFINED,
        WHITE,
        BLACK
    };

protected:
    // Constants

    // Members
    const Colour colour;
    const std::string name;
    const char symbol;

    // Methods
    Piece::Colour getOppositeColour() const;
    bool isOppositeColour(const Piece::Colour& colour) const;
    bool isEmptyOrEdible(std::size_t index, const std::vector<Square>& squares) const;

public:
    Piece(const Colour& colour, const std::string& name, const char symbol = 0);
    virtual ~Piece() { }
    virtual Piece* clone() const = 0;

    // Methods
    virtual std::vector<std::size_t> getValidMoves(std::size_t location,
        const std::vector<Square>& square) const = 0;
    virtual void specialMoveBehaviour(std::size_t from, std::size_t to);
    virtual void resetRoundSpecificState() { }
    virtual bool trySpecialMove(std::size_t& fromX, std::size_t& fromY,
                                std::size_t& toX,   std::size_t& toY,
                                std::vector<Square>& squares) const;

    // Getters
    const Colour& getColour() const { return colour; }
    const std::string& getName() const { return name; };
    char getSymbol() const { return symbol; }
    virtual bool isSpecialMoveAllowed() const { return false; }

    // Setters
};

#endif // _NALCG_PIECE_H_
