#ifndef _NALCG_AIDANIEL_H_
#define _NALCG_AIDANIEL_H_

#include "position.h"

#include <string>
#include <algorithm>
#include <map>

#define CUTOFFDEPTH 3 // Actually one higher.

class AIDaniel {
 private:
    Position *position;
    int bestMove;
    bool isInterrupted;
    int alphaBeta(Position *pos, int depth, int alpha, int beta, bool max);

 public:
    AIDaniel(Position *p);
    int getNextMove();
    void setPosition(Position *p);
    void interrupt();
    ~AIDaniel();
};
#endif // _NALCG_AIDaniel_H_