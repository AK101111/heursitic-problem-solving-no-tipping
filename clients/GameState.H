#pragma once
#include <smmintrin.h>
#include <utility>
#include <vector>
#include <iostream>
#include <limits>

namespace rtt{
enum State{ADDITION, REMOVAL};

struct GameState{
private:
    static const int gameK = 25;
    static const int boardLen = 60;
    static const int depth = 30;

    // index, wt pair
    std::vector<bool> weightsLeftSelf;
    std::vector<bool> weightsLeftOpponent;

    int selfLeft, oppLeft;
    // positions available are from 0-60(inclusive) which correspond to -30,30
    std::vector<int> weights;
    State state;
    // torque is negative if inside with right hand rule
    // torque from -1 one
    int onetorque;
    // torque from -3
    int threetorque;
    // torque from -3
    bool balanceWith(const std::pair<int, int>& indexWeight) const;
    bool legalToAdd(const std::pair<int, int>& indexWeight) const;
    bool balanceWithout(const std::pair<int, int>& indexWeight) const;
    bool legalToRemove(const std::pair<int, int>& indexWeight) const;
    void add(const std::pair<int, int>& indexWeight);
    void remove(const std::pair<int, int>& indexWeight);
    int heuristic() const;
    std::vector<std::pair<std::pair<int, int>, GameState> > nextMoves() const;
    std::pair<int, std::pair<int, int> > alphaBetaSearch(GameState gameState, int depth, int alpha, int beta, bool maxPlayer);
public:
    GameState();
    // index(-30 to 30), weight
    void absorb(const std::vector<int>& boardState);
    std::pair<int, int> play();
    void printBoard();
};
}
