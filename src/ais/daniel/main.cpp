#include "AI.h"

bool isAnswerLegal(std::string *answer, std::vector<std::string> *moves) {
  for (int i = 0; i < moves->size(); ++i) {
    if (*answer == moves->at(i)) {
      return true;
    }
    if (answer->size() > 5) {
      switch(answer->at(5)) {
      case 'Q':
      case 'R':
      case 'B':
      case 'N':
	if (answer->substr(0,5) == moves->at(i)) {
	  return true;
	}
      default:
	break;
      }
    }
  }
  return false;
}

int main() {
  int value;
  MovementGenerator mg;
  Position p(&mg);
  AIDaniel ai(&p);
  std::string input;
  std::vector<std::string> *moves;
  p.print();
  moves = p.getLegalMoves();
  while (true) {
    std::cout << "Legal moves:" << std::endl;
    for (int i = 0; i < moves->size(); ++i) {
      std::cout << moves->at(i) << ", ";
    }
    input = "";
    while (!(input == "Q" || isAnswerLegal(&input, moves))) {
      std::cout << std::endl << "Enter move: ";
      std::cin >> input;
      if (!(input == "Q" || isAnswerLegal(&input, moves))) {
	std::cout << "That move is not a valid move. Try again." << std::endl;
      }
    }
    if (input == "Q") {
      break;
    }
    ai.makeMove(input);
    ai.getNextMove();
    p.print();
    moves = p.getLegalMoves();
    if (value == std::numeric_limits<int>::min()) {
      std::cout << "The white player has been defeated." << std::endl;
      break;
    }
    else if (value == std::numeric_limits<int>::max()) {
      std::cout << "The black player has been defeated." << std::endl;
      break;
    }
    else if (moves->empty()) {
      std::cout << "Stalemate." << std::endl;
      break;
    }
  }
  return 0;
}
