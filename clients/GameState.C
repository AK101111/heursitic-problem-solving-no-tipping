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
}

/*GameState& GameState::operator=(GameState&& other) noexcept{
    if (this!=&other){
        selfLeft = other.selfLeft;
        oppLeft = other.oppLeft;
        weights = std::move(other.weights);
        weightsLeftSelf = std::move(other.weightsLeftSelf);
        weightsLeftOpponent = std::move(other.weightsLeftOpponent);
        state = other.state;
        onetorque = other.onetorque;
        threetorque = other.threetorque;
    }
    return *this;
}*/


bool GameState::legalToAdd(const std::pair<int, int>& indexWeight) const{
    return (weights[indexWeight.first + (boardLen/2)] == 0) && weightsLeftSelf[indexWeight.second] && balanceWith(indexWeight);
}

bool GameState::legalToRemove(const std::pair<int, int>& indexWeight) const{
    return (weights[indexWeight.first + (boardLen/2)] != 0) && !weightsLeftSelf[indexWeight.second] && balanceWithout(indexWeight);
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

    onetorque += (-1 -indexWeight.first) * indexWeight.second;
    threetorque += (-3 -indexWeight.first) * indexWeight.second;

    selfLeft--;
}
// always check lagality of removing before calling this
void GameState::remove(const std::pair<int, int>& indexWeight){
    weights[indexWeight.first + (boardLen/2)] = 0;
    weightsLeftSelf[indexWeight.second] = true;

    onetorque -= (-1 -indexWeight.first) * indexWeight.second;
    threetorque -= (-3 -indexWeight.first) * indexWeight.second;

    selfLeft++;
}

// absorbs game state from server
void GameState::absorb(const std::vector<int>& boardState){
    for(size_t i=0; i<boardState.size(); ++i){
        if(boardState[i] == 0 && weights[i] != 0){
            // removal
            weights[i] = 0;
            weightsLeftOpponent[weights[i]] = true;
            onetorque -= (-1 - (i - boardLen/2)) * weights[i];
            threetorque -= (-3 - (i - boardLen/2)) * weights[i]; 
            oppLeft++;
            break;
        }else if(boardState[i] != 0 && weights[i] == 0){
            // addition
            weights[i] = boardState[i];
            weightsLeftOpponent[boardState[i]] = false;
            onetorque += (-1 - (i - boardLen/2)) * boardState[i];
            threetorque += (-3 - (i - boardLen/2)) * boardState[i];
            oppLeft--;
            break;
        }
    }
}

int GameState::heuristic() const{
    // [TODO]
    return 0;
}

// returns pair of gameValue, bestMove. gameValue is max achievable value if bestMove is chosen., returns pair(some value, (-100, -100)) for terminal nodes
std::pair<int, std::pair<int, int>> alphaBetaSearch(GameState gameState, int depth, int alpha, int beta, bool  maximisingPlayer){
    if(depth == 0) //|| gameState.terminalNode())
        return std::make_pair(gameState.heuristic(), std::make_pair(-100, -100));
    int value; std::pair<int, int> bestPlay;
    auto nextMoves = gameState.nextMoves();
    // terminal node
    if(nextMoves.size() == 0)
        return std::make_pair(gameState.heuristic(), std::make_pair(-100, -100));
    // alpha beta
    if(maximisingPlayer){
        value = std::numeric_limits<int>::min();
        for(auto& nextMove: nextMoves){
            auto next = alphaBetaSearch(nextMove.second, depth - 1, alpha, beta, false);
            if(value < next.first){
                value = next.first;
                bestPlay = nextMove.first;
            }
            alpha = std::max(alpha, value);
            if(alpha >= beta)
                break;
        }
    }else{
        value = std::numeric_limits<int>::max();
        for(auto& nextMove: nextMoves){
            auto next = alphaBetaSearch(nextMove.second, depth - 1, alpha, beta, true);
            if(value < next.first){
                value = next.first;
                bestPlay = nextMove.first;
            }
            beta = std::max(beta, value);
            if(alpha >= beta)
                break;
        }
    }
    return std::make_pair(value, bestPlay);
}

std::pair<int, int> GameState::play(){
    auto pair = alphaBetaSearch(*this, depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true).second;
    if(pair.first == -100){
        // change to random
        return std::make_pair(0,0);
    }
    return pair.second;
}

void GameState::print() const{
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

bool GameState::terminalNode(){
    // [TODO]
    return true;
}

std::vector<std::pair<std::pair<int, int>, GameState>> GameState::nextMoves() const{
    std::vector<std::pair<std::pair<int, int>, GameState> > nextMoveV;
    if(state == ADDITION){
        for(int i=0; i<=boardLen; ++i){
            if(weights[i] == 0){
                for(int j=1; j<=gameK; ++j){
                    std::pair<int, int> tempMove(i - boardLen/2, j);
                    if(legalToAdd(tempMove)){
                        GameState nextMove = *this;
                        std::swap(nextMove.selfLeft, nextMove.oppLeft);
                        std::swap(nextMove.weightsLeftSelf, nextMove.weightsLeftOpponent);
                        nextMove.state = REMOVAL;
                        nextMoveV.emplace_back(std::make_pair(std::make_pair(i - boardLen/2, j), nextMove));
                        //nextMoveV.push_back(tempMove);
                    }   
                }
            }
        }
    }else{
        for(int i=0; i<=boardLen; ++i){
            if(weights[i] != 0){
                for(int j=1; j<=gameK; ++j){
                    std::pair<int, int> tempMove(i - boardLen/2, j);
                    if(legalToRemove(tempMove)){
                        GameState nextMove = *this;
                        std::swap(nextMove.selfLeft, nextMove.oppLeft);
                        std::swap(nextMove.weightsLeftSelf, nextMove.weightsLeftOpponent);
                        nextMove.state = ADDITION;
                        nextMoveV.emplace_back(std::make_pair(std::make_pair(i - boardLen/2, j), nextMove));
                        //nextMoveV.push_back(tempMove);
                    }
                }
            }
        }
    }
    return nextMoveV;
}
}

int main(){
    rtt::GameState gamestate;
    gamestate.print();
    return 0;
}
