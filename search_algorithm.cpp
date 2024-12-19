//INTELLIGENT SYSTEMS - FINAL COMPETITION
//INTELLIGENT HUMANS GROUP

#pragma GCC optimize "O3,omit-frame-pointer,inline"
#include <ios>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <regex>
#include <list>

using namespace std;

int MYID;

unsigned int ROUND_SIMULATION = 8;
unsigned int BEAM_WIDTH = 150;

class Game{
public:
    enum MOVES {LEFT, RIGHT, UP, DOWN, STAY, B_LEFT, B_RIGHT, B_UP, B_DOWN, B_STAY };  
};

class Map{
/*
0 --> Floor
1 --> Box
2 --> Wall
3 --> Item 1
4 --> Item 2
*/
private:
    int map[11][13];
public:
    Map() {}

    Map(const Map& m){
        for(int i = 0; i < 11; i++){
            for(int j = 0; j < 13; j++){
                map[i][j] = m.map[i][j];
            }
        }
    }

    void addItem(int row, int column, const char item){
        int cell;
        if(item == '.')
            cell = 0;
        else if(item == '0') 
            cell = 1;
        else if(item == 'X')
            cell = 2;
        else if(item == '1')
            cell = 3;
        else if(item == '2')
            cell = 4;
        map[row][column] = cell;
    }

    void modifyItem(int row, int column, const int item){
        map[row][column] = item;
    }

    int getItem(int row, int column) const{
        return map[row][column];
    }
};

class Player{
private:
    int id;
    int x;
    int y;
    int numberOfBombs;
    int explosionRange;

public:
    Player(){}

    Player(int id, int x, int y, int nBombs, int eRange): id(id), x(x), y(y), numberOfBombs(nBombs), explosionRange(eRange) {}

    Player(const Player& player){
        id = player.id;
        x = player.x;
        y = player.y;
        numberOfBombs = player.numberOfBombs;
        explosionRange = player.explosionRange;
    }

    int getId() {return id;}
    int getnumberOfBombs() {return numberOfBombs;}
    int getExplosionRange() {return explosionRange;}
    pair<int, int> getPosition() {return make_pair(x, y);}
    int getX() {return x;}
    int getY() {return y;}

    void setPosition(int x, int y){
        this->x = x;
        this->y = y;
    }

    void setNumberOfBombs(int numberOfBombs){
        this->numberOfBombs = numberOfBombs;
    }

};

class Bomb{
private:
    int ownerId;
    int x;
    int y;
    int roundTimer;
    int explosionRange;
    Map map;
    vector<pair<int, int>> hotspots;

    bool thereIsABomb(int x, int y, const unordered_multimap<int, Bomb>& bombs){
        for(auto b: bombs)
            if(b.second.getX() == x && b.second.getY() == y)
                return true;
        return false;
    }

public: 
    Bomb(){}

    Bomb(int id, int x, int y, int timer, int eRange, const Map& ma, const unordered_multimap<int, Bomb>& bombs): ownerId(id), x(x), y(y), roundTimer(timer), explosionRange(eRange) {
        recomputeHotspot(map, bombs);
    }

    Bomb(const Bomb& bomb){
        ownerId = bomb.ownerId;
        x = bomb.x;
        y = bomb.y;
        roundTimer = bomb.roundTimer;
        explosionRange = bomb.explosionRange;
        hotspots = bomb.hotspots;
    }

    int getId() const {return ownerId;}
    int getroundTimer() const {return roundTimer;}
    int getExplosionRange() const {return explosionRange;}
    pair<int, int> getPosition() const{
        return make_pair(x, y);
    }

    int getX() const{
        return x;
    }
    int getY() const{
        return y;
    }

    void setRoundTimer(int timer){
        roundTimer = timer;
    }

    void incrementExplasionRange(){
        this->explosionRange += 1;
    }

    void recomputeHotspot(const Map& map, const unordered_multimap<int, Bomb>& bombs){
        hotspots.clear();
        for(int i = x; i > x - explosionRange; i--){
            if (i>=0){
                if (map.getItem(y, i) == 2)
                    break;
                if(i != x && thereIsABomb(i, y, bombs)){
                    hotspots.push_back(make_pair(i,y));
                    break;
                }  
                else if(map.getItem(y, i) != 0){
                    hotspots.push_back(make_pair(i,y));
                    break;
                }
                hotspots.push_back(make_pair(i,y));
            }
        }
        for(int i = x; i < x + explosionRange; i++){
            if (i<=12 && i != x){
                if (map.getItem(y, i) == 2)
                    break;
                else if(map.getItem(y, i) != 0 || thereIsABomb(i, y, bombs)){
                    hotspots.push_back(make_pair(i,y));
                    break;
                }
                hotspots.push_back(make_pair(i,y));
            }
        }
        for(int i = y; i > y - explosionRange; i--){
            if (i>=0 && i != y){
                if (map.getItem(i, x) == 2)
                    break;
                else if(map.getItem(i, x) != 0 || thereIsABomb(x, i, bombs)){
                    hotspots.push_back(make_pair(x,i));
                    break;
                }
                hotspots.push_back(make_pair(x,i));
            }
        }
        for(int i = y; i < y + explosionRange; i++){
            if (i<=10 && i != y){
                if (map.getItem(i, x) == 2)
                    break;
                else if(map.getItem(i, x) != 0 || thereIsABomb(x, i, bombs)){
                    hotspots.push_back(make_pair(x,i));
                    break;
                }
                hotspots.push_back(make_pair(x,i));
            }
        }
    }

    void recomputeTimer(const unordered_multimap<int, Bomb>& bombs){
        for (auto& b: bombs){
            for(auto explosionCell: b.second.getHotspots()){
                if(this->x == explosionCell.first && this->y == explosionCell.second){
                    if(b.second.getroundTimer() < this->roundTimer){
                        this->roundTimer = b.second.getroundTimer();
                    }  
                }
            }
        }
    }

    vector<pair<int,int>> getHotspots() const{
        return hotspots;
    }
};

class Item{
private:
    int x;
    int y;
    int type;
public:
    Item(){}

    Item(int x, int y, int type): x(x), y(y), type(type){}

    Item(const Item& item){
        x = item.x;
        y = item.y;
        type = item.type; 
    }

    int getX(){
        return x;
    }

    int getY(){
        return y;
    }

    int getType(){
        return type;
    }
};

//Game state
class State{
private:
    Map map;
    unordered_map<int, Player> players;
    unordered_multimap<int, Bomb> bombs;
    list<Item> items;
    float evaluation = 0;
    string move;
    State* parent;
    vector<State*> children;

    float pickItem(Player& player, list<Item>& items, unordered_multimap<int, Bomb>& bombs, const Map& map){
        float evaluation = 0;
        for (auto it = items.begin(); it != items.end(); ++it){
            if (it->getX() == player.getX() && it->getY() == player.getY()){
                if(it->getType() == 2){
                    player.setNumberOfBombs(player.getnumberOfBombs() + 1);
                    evaluation += 1;
                }
                else if(it->getType() == 1){
                    auto range = bombs.equal_range(MYID);
                    for (auto bomb = range.first; bomb != range.second; ++bomb) {
                        bomb->second.incrementExplasionRange();
                        bomb->second.recomputeHotspot(map, bombs);
                        bomb->second.recomputeTimer(bombs);
                    }
                    evaluation += 0.5;
                }
                it = items.erase(it);
            }
        }
        return evaluation;
    }

public:
    State(State* parent = nullptr): parent(parent){}

    bool operator<(const State& other){
        return this->evaluation < other.evaluation;
    }

    void setMap(Map map){
        this->map = map;
    }

    void setPlayers(unordered_map<int, Player> players){
        this->players = players;
    }

    void setBombs(unordered_multimap<int, Bomb> bombs){
        this->bombs = bombs;
    }

    void setItems(list<Item> items){
        this->items = items;
    }

    void setParent(State* parent){
        this->parent = parent;
    }

    void addChild(State* child){
        this->children.push_back(child);
    }

    vector<State*> getChildren() {
        return children;
    }

    State* getParent(){
        return this->parent;
    }

    string getMove(){
        return this->move;
    }

    Map getMap(){
        return map;
    }

    unordered_multimap<int, Bomb> getBombs() const{
        return bombs;
    }

    unordered_map<int, Player> getPlayer(){
        return players;
    }

    //Returns the actions that can be performed in that state for expanation
    vector<Game::MOVES> legalMoves(){
        vector<Game::MOVES> moves;
        moves.push_back(Game::MOVES::STAY);
        pair<int, int> playerPos = players[MYID].getPosition();
        bool canBomb = true;
        bool left = true;
        bool right = true;
        bool up = true;
        bool down = true;
        for (auto& bomb : bombs) {
            if(bomb.second.getPosition() == playerPos || players[MYID].getnumberOfBombs() == 0)
                canBomb = false;
            if(bomb.second.getX() == playerPos.first - 1 && bomb.second.getY() == playerPos.second)
                left = false;
            if(bomb.second.getX() == playerPos.first + 1 && bomb.second.getY() == playerPos.second)
                right = false;
            if(bomb.second.getX() == playerPos.first && bomb.second.getY() == playerPos.second - 1)
                up = false;
            if(bomb.second.getX() == playerPos.first && bomb.second.getY() == playerPos.second + 1)
                down = false;
        }
        if(canBomb)
            moves.push_back(Game::MOVES::B_STAY);
        if(playerPos.first > 0 && map.getItem(playerPos.second, playerPos.first - 1) == 0 && left){
            moves.push_back(Game::MOVES::LEFT);
            if(canBomb)
                moves.push_back(Game::MOVES::B_LEFT);
        }
        if(playerPos.first < 12 && map.getItem(playerPos.second, playerPos.first + 1) == 0 && right){
            moves.push_back(Game::MOVES::RIGHT);
            if(canBomb)
                moves.push_back(Game::MOVES::B_RIGHT);
        }
        if(playerPos.second > 0 && map.getItem(playerPos.second - 1, playerPos.first) == 0 && up){
            moves.push_back(Game::MOVES::UP);
            if(canBomb)
                moves.push_back(Game::MOVES::B_UP);
        }
        if(playerPos.second < 10 && map.getItem(playerPos.second + 1, playerPos.first) == 0 && down){
            moves.push_back(Game::MOVES::DOWN);
            if(canBomb)
                moves.push_back(Game::MOVES::B_DOWN);
        }
        return moves;
    }

    //State expansion by adding a child for each action
    void expand() {
        vector<Game::MOVES> moves = legalMoves();
        for (const auto& move : moves) {
            //Generete next state for each move
            State* child = new State();
            Map m = Map(map);
            unordered_map<int, Player> ps = players;
            unordered_multimap<int, Bomb> bs;
            list<Item> itms = items;
            unordered_multimap<int, Bomb> explodedBombs;

            unsigned int explodedBoxes = 0;
            for (auto& b: bombs){
                //Decreasing the bomb timer
                Bomb bomb = Bomb(b.second);
                bomb.setRoundTimer(bomb.getroundTimer() - 1);
                if(bomb.getroundTimer() == 0){
                    if(bomb.getId() == MYID){
                        ps[MYID].setNumberOfBombs(ps[MYID].getnumberOfBombs() + 1);
                        explodedBoxes += 1;
                    }
                    for(auto explosionCell: bomb.getHotspots()){
                        int mapItem = m.getItem(explosionCell.second, explosionCell.first);
                        if(mapItem != 0 && mapItem != 2){
                            if(mapItem == 3)
                                itms.push_back(Item(explosionCell.first, explosionCell.second, 1));
                            if(mapItem == 4)
                                itms.push_back(Item(explosionCell.first, explosionCell.second, 2));
                            m.modifyItem(explosionCell.second, explosionCell.first, 0);
                        }
                        for (auto it = itms.begin(); it != itms.end(); ++it) {
                            if (it->getX() == explosionCell.first && it->getY() == explosionCell.second) 
                                it = itms.erase(it);
                        }
                    }
                    explodedBombs.insert({bomb.getId(), bomb});
                }
                else{
                    bs.insert({bomb.getId(), bomb});
                } 
            }

            for(auto& b: bs){
                b.second.recomputeHotspot(m, bs);
                b.second.recomputeTimer(bs);
            }

            child->evaluation += explodedBoxes;

            int bombTimer = 8;
            for (auto& b: bs){
                for(auto explosionCell: b.second.getHotspots()){
                    if(ps[MYID].getX() == explosionCell.first && ps[MYID].getY() == explosionCell.second){
                        if(b.second.getroundTimer() < bombTimer)
                            bombTimer = b.second.getroundTimer();
                    }
                }
            }
            
            int x;
            int y;
            switch(move){
                case Game::MOVES::STAY:{
                    x = ps[MYID].getX();
                    y = ps[MYID].getY();

                    child->evaluation += pickItem(ps[MYID], itms, bs, m);

                    child->move = "MOVE " + to_string(x) + " " + to_string(y);
                    break;
                }
                case Game::MOVES::LEFT:{
                    x = ps[MYID].getX() - 1;
                    y = ps[MYID].getY();
                    ps[MYID].setPosition(x, y);

                    child->evaluation += pickItem(ps[MYID], itms, bs, m);

                    child->move = "MOVE " + to_string(x) + " " + to_string(y);
                    break;
                }
                case Game::MOVES::RIGHT:{
                    x = ps[MYID].getX() + 1;
                    y = ps[MYID].getY();
                    ps[MYID].setPosition(x, y);

                    child->evaluation += pickItem(ps[MYID], itms, bs, m);

                    child->move = "MOVE " + to_string(x) + " " + to_string(y);
                    break;
                }
                case Game::MOVES::UP:{
                    x = ps[MYID].getX();
                    y = ps[MYID].getY() - 1;
                    ps[MYID].setPosition(x, y);

                    child->evaluation += pickItem(ps[MYID], itms, bs, m);

                    child->move = "MOVE " + to_string(x) + " " + to_string(y);
                    break;
                }
                case Game::MOVES::DOWN:{
                    x = ps[MYID].getX();
                    y = ps[MYID].getY() + 1;
                    ps[MYID].setPosition(x, y);

                    child->evaluation += pickItem(ps[MYID], itms, bs, m);
                    
                    child->move = "MOVE " + to_string(x) + " " + to_string(y);
                    break;
                }
                case Game::MOVES::B_STAY:{
                    bs.insert({MYID, Bomb(MYID, ps[MYID].getX(), ps[MYID].getY(), bombTimer, 3, m, bs)});
                    ps[MYID].setNumberOfBombs(ps[MYID].getnumberOfBombs() - 1);
                    x = ps[MYID].getX();
                    y = ps[MYID].getY();

                    child->evaluation += pickItem(ps[MYID], itms, bs, m);

                    child->move = "BOMB " + to_string(x) + " " + to_string(y);
                    break;
                }
                case Game::MOVES::B_LEFT:{
                    bs.insert({MYID, Bomb(MYID, ps[MYID].getX(), ps[MYID].getY(), bombTimer, 3, m, bs)});
                    ps[MYID].setNumberOfBombs(ps[MYID].getnumberOfBombs() - 1);
                    x = ps[MYID].getX() - 1;
                    y = ps[MYID].getY();
                    ps[MYID].setPosition(x, y);

                    child->evaluation += pickItem(ps[MYID], itms, bs, m);

                    child->move = "BOMB " + to_string(x) + " " + to_string(y);
                    break;
                }
                case Game::MOVES::B_RIGHT:{
                    bs.insert({MYID, Bomb(MYID, ps[MYID].getX(), ps[MYID].getY(), bombTimer, 3, m, bs)});
                    ps[MYID].setNumberOfBombs(ps[MYID].getnumberOfBombs() - 1);
                    x = ps[MYID].getX() + 1;
                    y = ps[MYID].getY();
                    ps[MYID].setPosition(x, y);

                    child->evaluation += pickItem(ps[MYID], itms, bs, m);

                    child->move = "BOMB " + to_string(x) + " " + to_string(y);
                    break;
                }
                case Game::MOVES::B_UP:{
                    bs.insert({MYID, Bomb(MYID, ps[MYID].getX(), ps[MYID].getY(), bombTimer, 3, m, bs)});
                    ps[MYID].setNumberOfBombs(ps[MYID].getnumberOfBombs() - 1);
                    x = ps[MYID].getX();
                    y = ps[MYID].getY() - 1;
                    ps[MYID].setPosition(x, y);

                    child->evaluation += pickItem(ps[MYID], itms, bs, m);

                    child->move = "BOMB " + to_string(x) + " " + to_string(y);
                    break;
                }
                case Game::MOVES::B_DOWN:{
                    bs.insert({MYID, Bomb(MYID, ps[MYID].getX(), ps[MYID].getY(), bombTimer, 3, m, bs)});
                    ps[MYID].setNumberOfBombs(ps[MYID].getnumberOfBombs() - 1);
                    x = ps[MYID].getX();
                    y = ps[MYID].getY() + 1;
                    ps[MYID].setPosition(x, y);

                    child->evaluation += pickItem(ps[MYID], itms, bs, m);

                    child->move = "BOMB " + to_string(x) + " " + to_string(y);
                    break;
                }
            }

            for (auto& b: explodedBombs){
                for(auto explosionCell: b.second.getHotspots()){
                    if(x == explosionCell.first && y == explosionCell.second){
                        child->evaluation -= 1000;
                        break;
                    }
                }
            }

            child->setParent(this);
            this->addChild(child);
            child->setMap(m);
            child->setPlayers(ps);
            child->setBombs(bs);
            child->setItems(itms);
            child->heuristicEvaluation();
        }
    }

    bool thereIsABombNearby(int x, int y){
        for(auto b: bombs)
            if(b.second.getroundTimer() <= 5){ 
                for(auto explosionCell: b.second.getHotspots())
                    if(explosionCell.first == x && explosionCell.second == y)
                        return true;
            }
        return false;
    }

    void playerPositionEvaluation(){
        vector<int> distanceFromBox;
        vector<int> distanceFromItems;
        vector<int> distanceFromAvaibleItems;
        vector<int> distanceFromPlayers;
        int sumDist = 0;
        for(int row = 0; row < 11; row++){
            for(int col = 0; col < 13; col++){
                if(map.getItem(row, col) == 1 || map.getItem(row, col) == 3 || map.getItem(row, col) == 4 && !thereIsABombNearby(col, row)){
                    int dist = abs(players[MYID].getX() - col) + abs(players[MYID].getY() - row);
                    distanceFromBox.push_back(dist);
                    sumDist += dist;
                }
                if((map.getItem(row, col) == 3 || map.getItem(row, col) == 4)  && !thereIsABombNearby(col, row)){
                    int dist = abs(players[MYID].getX() - col) + abs(players[MYID].getY() - row);
                    distanceFromItems.push_back(dist);
                }
            }
        }
        for(auto item: items){
            if(!thereIsABombNearby(item.getX(), item.getY())){ 
                int dist = abs(players[MYID].getX() - item.getX()) + abs(players[MYID].getY() - item.getY());
                distanceFromAvaibleItems.push_back(dist);
            }
        }
        for(auto player: players){
            if(player.second.getId() != MYID){
                int dist = abs(players[MYID].getX() - player.second.getX()) + abs(players[MYID].getY() - player.second.getY());
                distanceFromPlayers.push_back(dist);
            }
        }
        if(distanceFromBox.size()>0) { 
            int minDist = *min_element(distanceFromBox.begin(), distanceFromBox.end());
            this->evaluation += (-3)*minDist;
        }
        if(distanceFromItems.size()>0) { 
            int minDist = *min_element(distanceFromItems.begin(), distanceFromItems.end());
            this->evaluation += (-0.5)*minDist;
        }
        if(distanceFromAvaibleItems.size()>0) { 
            int minDist = *min_element(distanceFromAvaibleItems.begin(), distanceFromAvaibleItems.end());
            this->evaluation += (-1.5)*minDist;
        }
        if(distanceFromPlayers.size()>0) { 
            int minDist = *min_element(distanceFromPlayers.begin(), distanceFromPlayers.end());
            this->evaluation += (-0.05)*minDist;
        }
        this->evaluation += (-0.2)*abs(players[MYID].getX() - 6) + abs(players[MYID].getY() - 5);
        this->evaluation -= sumDist;
    }

    void bombPositionEvaluation(){
        auto range = bombs.equal_range(MYID);
        for (auto bomb = range.first; bomb != range.second; ++bomb) {
            bool destroyAtLeastABox = false;
            for(auto explosionCell: bomb->second.getHotspots()){
                if(map.getItem(explosionCell.second, explosionCell.first) == 1){
                    this->evaluation += 40;
                    destroyAtLeastABox = true;
                }
                else if(map.getItem(explosionCell.second, explosionCell.first) == 3 || map.getItem(explosionCell.second, explosionCell.first) == 4){
                    this->evaluation += 45;
                    destroyAtLeastABox = true;
                }
                else
                    this->evaluation -= 0.2;
                
                for(auto item: items){
                    if(item.getX() == explosionCell.first && item.getY() == explosionCell.second)
                        this->evaluation -= -1;
                }
            }

            if(!destroyAtLeastABox)
                this->evaluation -= 10;
        }
    }

    void isSurvivable(){
        int minTimer = 8;
        bool exploded = false;
        for(auto& bomb: bombs){
            if(bomb.second.getroundTimer() <= 5){  
                for(auto explosionCell: bomb.second.getHotspots()){
                    if(players[MYID].getX() == explosionCell.first && players[MYID].getY() == explosionCell.second){
                        exploded = true;
                        if(bomb.second.getExplosionRange() < minTimer)
                            minTimer = bomb.second.getExplosionRange();
                    }
                }
            }
        }
        if(exploded)
            this->evaluation -= pow(0.90, minTimer) * 1000;
    }

    bool thereIsABomb(int x, int y){
        for(auto b: bombs)
            if(b.second.getX() == x && b.second.getY() == y)
                return true;
        return false;
    }

    bool thereIsAPlayer(int x, int y){
        for(auto p: players)
            if(p.second.getId() != MYID && p.second.getX() == x && p.second.getY() == y)
                return true;
        return false;
    }

    void isTrapped(){
        bool left = false;
        bool right = false;
        bool up = false;
        bool down = false;
        int x = players[MYID].getX();
        int y = players[MYID].getY();
        if(x > 0 && (map.getItem(y, x - 1) != 0 || thereIsABomb(x - 1, y) || thereIsAPlayer(x - 1, y)))
            left = true;
        if(x < 12 && (map.getItem(y, x + 1) != 0 || thereIsABomb(x + 1, y) || thereIsAPlayer(x + 1, y)))
            right = true;
        if(y > 0 && (map.getItem(y - 1, x) != 0 || thereIsABomb(x, y - 1) || thereIsAPlayer(x, y - 1)))
            up = true;
        if(y < 10 && (map.getItem(y + 1, x) != 0 || thereIsABomb(x, y + 1) || thereIsAPlayer(x, y + 1)))
            down = true;

        if(left && right && up && down)
            this->evaluation -= 200;
    }

    //State evaluation
    void heuristicEvaluation(){
        playerPositionEvaluation();
        bombPositionEvaluation();
        isSurvivable();
        isTrapped();
        this->evaluation += this->parent->evaluation;
    }

    float getEvaluation(){
        return this->evaluation;
    }

    ~State(){
        for (State* child: children)
            delete child; 
    }
};

State* createInitialState(Map map, unordered_map<int, Player> players, unordered_multimap<int, Bomb> bombs, list<Item> items){
    State* initialState = new State();
    initialState->setMap(map);
    initialState->setPlayers(players);
    initialState->setBombs(bombs);
    initialState->setItems(items);
    return initialState;
}

struct LessThanByEvaluation{
  bool operator()(State* state1, State* state2) const{
    return state1->getEvaluation() < state2->getEvaluation();
  }
};

string beamSearch(State* initialState){
    if (initialState == nullptr)
        return "NO MOVE";

    priority_queue<State*, vector<State*>, LessThanByEvaluation> queue;
    initialState->expand();
    for (State* child : initialState->getChildren()){
        queue.push(child);
    }
        
    int round = 1;
    while (round < ROUND_SIMULATION) {
        vector<State*> goodState;
        int queueSize = queue.size();
        for(int i = 0; i < BEAM_WIDTH && i < queueSize; i++){
            goodState.push_back(queue.top());
            queue.pop();
        }
        while (!queue.empty()) {
            queue.pop();
        }
        for(State* state: goodState){
            state->expand();
            for (State* child : state->getChildren()) {
                queue.push(child);
            }
        }
        goodState.clear();
        round++;
    }

    State* bestState = queue.top();
    string bestMove;
    for(int i = 1; i < ROUND_SIMULATION; i++){
        bestState = bestState->getParent();
        bestMove = bestState->getMove();
    }
    //Best action to perform
    return bestMove;
}

int main()
{
    int width;
    int height;
    int my_id;
    cin >> width >> height >> my_id; cin.ignore();

    MYID = my_id;

    unordered_map<int, Player> players;
    unordered_multimap<int, Bomb> bombs;
    list<Item> items;

    int entities;
    int x;
    int y;
    int entity_type;
    int owner;
    int param_1;
    int param_2;
    int round = 1;
    //Game loop
    while (1) {
        Map map = Map();
        //Map creation
        for (int i = 0; i < height; i++) {
            string row;
            cin >> row; cin.ignore();
            for (size_t j = 0; j < row.length(); j++) {
                map.addItem(i, j, row[j]);
            }
        }

        cin >> entities; cin.ignore();
        for (int i = 0; i < entities; i++) {
            cin >> entity_type >> owner >> x >> y >> param_1 >> param_2; cin.ignore();
            if(entity_type == 0){
                players[owner] = Player(owner, x, y, param_1, param_2);
            }
            else if(entity_type == 1){
                bombs.insert({owner, Bomb(owner, x, y, param_1, param_2, map, bombs)});
            }
            else if(entity_type == 2){
                items.push_back(Item(x, y, param_1));
            }
        }
        cerr << "Round " << round <<endl;
        auto startTime = std::chrono::high_resolution_clock::now();
        State* initialState = createInitialState(map, players, bombs, items);
        string move = beamSearch(initialState);
        cout << move << endl;
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        double milliseconds = duration.count();
        std::cerr << "Execution time: " << milliseconds << " ms" << std::endl;
        players.clear();
        bombs.clear();
        items.clear();
        delete initialState;
        round++;
    }
}