// class dependencies
#include "middleman.h"
#include "logic/chessboard.h"

#include <boost/thread/thread.hpp>

// system includes

Middleman::Middleman(const std::vector<AI*>& aiList,
    const std::vector<AIInfo>& aiInfos) :
    board(0), currentTurn(WHITE), rounds(0), view(0),
    aiList(aiList), aiInfos(aiInfos)
{
    assert(aiList.size() == aiInfos.size());
}

Middleman::~Middleman()
{
    deleteAndClear(aiList);
    deleteAndClear(gameStates);
}

// public methods
void Middleman::startGame()
{
    // TODO: read board type from config file or something
    board = new ChessBoard(ChessBoard::createBoard());
    rounds = 0;

    deleteAndClear(gameStates);
    gameStates.push_back(board->clone());

    // white starts
    currentTurn = WHITE;

    // initialize AI players.
    for (std::size_t i = 0; i < aiList.size(); i++)
    {
        aiList.at(i)->init(board, this);
    }

    client.init(this);

    // add players
    view->init(board, this);



}

std::vector<std::size_t>  Middleman::getValidMovesAt(std::size_t x, std::size_t y) const
{
    return board->getValidMoves(x, y);
}

unsigned int Middleman::move(   std::size_t fromX, std::size_t fromY,
                                std::size_t toX,   std::size_t toY,
                                unsigned int promoteTo,
                                bool fromRemote)
{
    // make mutable versions of parameters,
    // since board's move() will update them when necessary.
    std::size_t x0 = fromX;
    std::size_t y0 = fromY;
    std::size_t x1 = toX;
    std::size_t y1 = toY;

    std::string currentMove = newLogEntry(fromX, fromY, toX, toY);
    unsigned int retValue = board->move(x0, y0, x1, y1, currentTurn, promoteTo);
    if (retValue & Board::MOVE_OK)
    {
        gameLog.push_back(currentMove);

        playRound();

        // nothing changed, do original move
        if (x0 == fromX && y0 == fromY && x1 == toX && y1 == toY)
        {
            if (retValue & Board::PROMOTION_OK)
            {
                promoteUpdate(fromX, fromY, toX, toY, promoteTo);
            }
            else
            {
                moveUpdate(fromX, fromY, toX, toY);
            }
        }
        else
        {
            // first make the extra move generated by move
            moveUpdate(x0, y0, x1, y1, true);
            // and then make the original move
            moveUpdate(fromX, fromY, toX, toY, false);
        }
        if (!fromRemote)
        {
            remoteUpdate(fromX, fromY, toX, toY, promoteTo);
        }
    }

    return retValue;
}

void Middleman::undo(unsigned int steps)
{
    if (gameStates.size() <= steps)
    {
        steps = gameStates.size() - 1;
    }
    if (steps >= HALF_TURN)
    {
        for (unsigned int i = 0; i < steps; ++i)
        {
            delete gameStates.back();
            gameStates.pop_back();
            gameLog.pop_back();
        }
        board = gameStates.back()->clone();
        currentTurn = gameStates.size() % 2 ? WHITE : BLACK;
        rounds -= steps;
        boardUpdate();
    }
}


unsigned int Middleman::getGameConditionMask() const
{
    return board->getGameConditionMask(currentTurn);
}


// Methods (possibly) over IP
void Middleman::sendChallenge(const std::string& remotePlayer)
{
    client.sendChallenge(remotePlayer);
}


void Middleman::promptChallenge(const std::string& challenger)
{
    view->promptChallenge(challenger);
}


void Middleman::respondToChallenge(bool accept)
{
    client.respondToChallenge(accept);

    if (accept)
    {
        // init game..
        // dium daum
    }
}

void Middleman::updateUsers(const std::vector<std::string>& users)
{
    view->updateUsers(users);
}

// private methods
void Middleman::playRound()
{
    gameStates.push_back(board->clone());
    currentTurn = currentTurn == WHITE ? BLACK : WHITE;
    ++rounds;
}


void Middleman::moveUpdate( std::size_t fromX, std::size_t fromY,
                            std::size_t toX,   std::size_t toY, bool continuous)
{
    view->move(fromX, fromY, toX, toY, continuous);
    board->initRoundSpecificState();
    updateBoardForAI();
}


void Middleman::promoteUpdate(  std::size_t fromX,  std::size_t fromY,
                                std::size_t toX,    std::size_t toY,
                                unsigned int promoteTo)
{
    view->promoteMove(fromX, fromY, toX, toY, promoteTo);
    updateBoardForAI();

    board->initRoundSpecificState();
}


void Middleman::remoteUpdate(   std::size_t fromX,  std::size_t fromY,
                                std::size_t toX,    std::size_t toY,
                                unsigned int promoteTo)
{
    client.move(fromX, fromY, toX, toY, promoteTo);
}

bool Middleman::connect()
{
    return client.connect("dzarg.mine.nu", "6667");
}


void Middleman::boardUpdate()
{
    view->setBoard(board, rounds);

    updateBoardForAI();
}


void Middleman::updateBoardForAI()
{
    for (std::size_t i = 0; i < aiList.size(); ++i)
    {
        aiList.at(i)->setBoard(board, rounds);
    }
}


const std::string Middleman::newLogEntry(std::size_t fromX, std::size_t fromY,
    std::size_t toX, std::size_t toY) const
{
    std::string currentMove;
    currentMove += board->getSymbolAt(fromX, fromY);
    currentMove += static_cast<char>(fromX + 'A');
    currentMove += static_cast<char>((board->getWidth() - fromY) + '0');

    currentMove += " ";
    currentMove += board->getSymbolAt(toX, toY);
    currentMove += static_cast<char>(toX + 'A');
    currentMove += static_cast<char>((board->getWidth() - toY) + '0');

    return currentMove;
}


void Middleman::setControl(unsigned int whiteController,
    unsigned int blackController)
{
    view->setControl(whiteController == 0, blackController == 0);

    for (std::size_t i = 0; i < aiList.size(); i++)
    {
        aiList.at(i)->setControl(whiteController == i + 1,
            blackController == i + 1);
    }
}

void Middleman::setAICutoffDepth(int white, int black)
{
    for (std::size_t i = 0; i < aiList.size(); i++)
    {
        aiList.at(i)->setCutoffDepth(white, black);
    }
}

template <typename T>
void Middleman::deleteAndClear(std::vector<T>& vector)
{
    while (!vector.empty())
    {
        delete vector.back();
        vector.pop_back();
    }
}
