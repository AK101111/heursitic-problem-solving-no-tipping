#include "GameState.H"

namespace rtt{
GameState::GameState() : weightsLeftSelf(gameK+1, true), weightsLeftOpponent(gameK+1, true), weights(boardLen+2, 0){

    selfLeft = gameK;
    oppLeft = gameK;

    state = ADDITION;

    onetorque = -3;
    threetorque = -9;

    weights[boardLen/2 - 4] = 3;
    onetorque += (3 * 3);
    threetorque += (1* 3);

    hvalue = -(gameK*(gameK+1))/2;
}

bool GameState::legalToAdd(const std::pair<int, int>& indexWeight) const{
    return (weights[indexWeight.first + (boardLen/2)] == 0) && weightsLeftSelf[indexWeight.second] && balanceWith(indexWeight);
}

bool GameState::legalToRemove(const std::pair<int, int>& indexWeight) const{
    return (weights[indexWeight.first + (boardLen/2)] != 0) && balanceWithout(indexWeight);
}

bool GameState::balanceWith(const std::pair<int, int>& indexWeight) const{
    int left = onetorque;
    left += (-1 - indexWeight.first) * indexWeight.second;
    int right = threetorque;
    right += (-3 - indexWeight.first) * indexWeight.second;

    return (left >=0 and right <= 0);
}

bool GameState::balanceWithout(const std::pair<int, int>& indexWeight) const{
    int left = onetorque;
    left -= (-1 - indexWeight.first) * indexWeight.second;
    int right = threetorque;
    right -= (-3 - indexWeight.first) * indexWeight.second;

    return (left >=0 and right <= 0);
}

// always check legality before add
void GameState::add(const std::pair<int, int>& indexWeight){
    weights[indexWeight.first + (boardLen/2)] = indexWeight.second;
    weightsLeftSelf[indexWeight.second] = false;

    hvalue += indexWeight.second;

    onetorque += (-1 -indexWeight.first) * indexWeight.second;
    threetorque += (-3 -indexWeight.first) * indexWeight.second;

    selfLeft--;
    if(selfLeft == 0)
        state = REMOVAL;
}
// always check lagality of removing before calling this
void GameState::remove(const std::pair<int, int>& indexWeight){
    int wtToRemove = weights[indexWeight.first + (boardLen/2)];
    weights[indexWeight.first + (boardLen/2)] = 0;

    hvalue -= wtToRemove;

    onetorque -= (-1 -indexWeight.first) * wtToRemove;
    threetorque -= (-3 -indexWeight.first) * wtToRemove;

    //selfLeft++;
}

// absorbs game state from server
void GameState::absorb(const std::vector<int>& boardState){
    for(size_t i=0; i<boardState.size(); ++i){
        if(boardState[i] == 0 && weights[i] != 0){
            // removal
            std::cout << "detected removal " << i << " " << weights[i] << std::endl;
            weights[i] = 0;
            //weightsLeftOpponent[weights[i]] = true;
            onetorque -= (-1 - (i - boardLen/2)) * weights[i];
            threetorque -= (-3 - (i - boardLen/2)) * weights[i]; 
            //oppLeft++;
            break;
        }else if(boardState[i] != 0 && weights[i] == 0){
            // addition
            std::cout << "detected addition " << i << " " << boardState[i] << std::endl;
            weights[i] = boardState[i];
            weightsLeftOpponent[boardState[i]] = false;
            onetorque += (-1 - (i - boardLen/2)) * boardState[i];
            threetorque += (-3 - (i - boardLen/2)) * boardState[i];
            oppLeft--;
            break;
        }
    }
}

//int GameState::heuristic() const{
    // [TODO]
//    return hvalue;
//}
unsigned long long int ggg = 0;

// returns pair of gameValue, bestMove. gameValue is max achievable value if bestMove is chosen., returns pair(some value, (-100, -100)) for terminal nodes
std::pair<int, std::pair<int, int> > GameState::alphaBetaSearch(GameState gameState, int depth, int alpha, int beta, bool maximisingPlayer){
    //gameState.printBoard();
    //std::cout << depth << " " << alpha << " " << beta << std::endl;
    if(ggg % 100000 == 0){
        std::cout << ggg << std::endl;
        //std::cout << depth << " " << alpha << " " << beta << " " << maximisingPlayer << std::endl;
    }
    ggg++;
    if(depth == 0) //|| gameState.terminalNode())
        return std::make_pair(gameState.hvalue, std::make_pair(-100, -100));
    int value; std::pair<int, int> bestPlay;
    auto nextMoves = gameState.nextMoves();
    // terminal node
    if(nextMoves.size() == 0)
        return std::make_pair(gameState.hvalue, std::make_pair(-100, -100));
    // alpha beta
    if(maximisingPlayer){
        value = std::numeric_limits<int>::min();
        if(depth > 2){
            volatile bool flag=false;
            //#pragma omp parallel for shared(flag)
            for(size_t i=0; i<nextMoves.size(); ++i){
                if(flag) continue;
                auto& nextMove = nextMoves[i];
                auto next = alphaBetaSearch(std::get<2>(nextMove), depth - 1, alpha, beta, false);
                if(value < next.first){
                    value = next.first;
                    bestPlay.first = std::get<0>(nextMove); bestPlay.second = std::get<1>(nextMove);
                }
                alpha = std::max(alpha, value);
                if(alpha >= beta)
                    flag=true;
            }
        }else{
            for(size_t i=0; i<nextMoves.size(); ++i){
                auto& nextMove = nextMoves[i];
                auto next = alphaBetaSearch(std::get<2>(nextMove), depth - 1, alpha, beta, false);
                if(value < next.first){
                    value = next.first;
                    bestPlay.first = std::get<0>(nextMove); bestPlay.second = std::get<1>(nextMove);
                }
                alpha = std::max(alpha, value);
                if(alpha >= beta)
                    break;
            }
        }
    }else{
        value = std::numeric_limits<int>::max();
        if(depth > 2){
            volatile bool flag=false;
            //#pragma omp parallel for shared(flag)
            for(size_t i=0; i<nextMoves.size(); ++i){
                if(flag) continue;
                auto& nextMove = nextMoves[i];
                auto next = alphaBetaSearch(std::get<2>(nextMove), depth - 1, alpha, beta, true);
                if(value > next.first){
                    value = next.first;
                    bestPlay.first = std::get<0>(nextMove); bestPlay.second = std::get<1>(nextMove);
                }
                beta = std::min(beta, value);
                if(alpha >= beta)
                    flag = true;
            }
        }else{
            for(size_t i=0; i<nextMoves.size(); ++i){
                auto& nextMove = nextMoves[i];
                auto next = alphaBetaSearch(std::get<2>(nextMove), depth - 1, alpha, beta, true);
                if(value > next.first){
                    value = next.first;
                    bestPlay.first = std::get<0>(nextMove); bestPlay.second = std::get<1>(nextMove);
                }
                beta = std::min(beta, value);
                if(alpha >= beta)
                    break;
            }
        }
    }
    return std::make_pair(value, bestPlay);
}

std::pair<int, int> GameState::play(int depth){
    std::pair<int, int> pair;
    ggg = 0;
    
    pair = alphaBetaSearch(*this, depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true).second;

    if(state == ADDITION)
        add(pair);
    else
        remove(pair);
    //-100 index denotes no solution found
    // -1 weight denotes removal
    return pair;
}

void GameState::printBoard(){
    std::cout << "game phase: ";
    if(state == ADDITION)
        std::cout << "addition" << std::endl;
    else
        std::cout << "removal" << std::endl;

    std::cout << "wts left self: ";
    for(int i=1; i<=gameK; ++i)
        std::cout << weightsLeftSelf[i] << " ";
    std::cout << std::endl;

    std::cout << "wts left opponent: ";
    for(int i=1; i<=gameK; ++i)
        std::cout << weightsLeftOpponent[i] << " ";
    std::cout << std::endl;

    std::cout << "boardState: ";
    for(int i=0; i<=boardLen; ++i)
        std::cout << weights[i] << " ";
    std::cout << std::endl;

    std::cout << "toruqe from -1: " << onetorque << std::endl;
    std::cout << "toruqe from -3: " << threetorque << std::endl;

    std::cout << "---------------------------------------" << std::endl;
}  


// [TODO] improve this function
std::vector<std::tuple<int, int, GameState> > GameState::nextMoves() const{
    std::vector<std::tuple<int, int, GameState> > nextMoveV;
    if(state == ADDITION){
        for(int i=0; i<=boardLen; ++i){
            if(weights[i] == 0){
                for(int j=gameK; j >= 1; j--){
                    std::pair<int, int> tempMove(i - boardLen/2, j);
                    if(legalToAdd(tempMove)){
                        GameState nextMove = *this;
                        nextMove.add(tempMove);
                        std::swap(nextMove.selfLeft, nextMove.oppLeft);
                        std::swap(nextMove.weightsLeftSelf, nextMove.weightsLeftOpponent);
                        nextMoveV.emplace_back(std::make_tuple(i - boardLen/2, j, nextMove));
                        //nextMoveV.push_back(tempMove);
                    }  
                }
            }
        }
    }else{
        for(int i=0; i<=boardLen; ++i){
            if(weights[i] != 0){
                std::pair<int, int> tempMove(i - boardLen/2, weights[i]);
                if(legalToRemove(tempMove)){
                    GameState nextMove = *this;
                    nextMove.remove(tempMove);
                    std::swap(nextMove.selfLeft, nextMove.oppLeft);
                    std::swap(nextMove.weightsLeftSelf, nextMove.weightsLeftOpponent);
                    nextMoveV.emplace_back(std::make_tuple(i - boardLen/2, -1, nextMove));
                        //nextMoveV.push_back(tempMove);
                }
            }
        }
    }
    return nextMoveV;
}
}

int main(){
    rtt::GameState gamestate;
    int x;
    std::cin >> x;
    gamestate.play(x);
    return 0;
}
