/*
 * @file botTemplate.cpp
 * @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
 * @date 2010-02-04
 * Template for users to create their own bots
 */

#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
#include <vector>
#include <cmath>
#include <ctime>
#define BOARD_SIZE 8
#define MAXPLAYER 1
#define MINPLAYER -1
#define MAXDEPTH 5
#define NINF -1e15
#define INF 1e15
typedef long long int ll;
using namespace std;
using namespace Desdemona;

class MyBot : public OthelloPlayer
{
    public:
        MyBot(Turn turn);
        virtual Move play(const OthelloBoard &board);
    private:
        double alphaBeta(OthelloBoard &board, int ptype, double alpha, double beta, int depth);
        Coin myColor;
        Move bestMove;
        clock_t begin;
        vector<vector<int>> weights; // weights for diff positions
        vector<vector<int>> neighbours; // neighbour positions
        vector<vector<int>> corners; // corner positions
        static bool comparer(const pair<Move,double>& x, const pair<Move, double>& y);
        vector<vector<char>> getLabelGrid(OthelloBoard& board, Turn t); // returns a labelled grid; 'a' for my coins, 'b' for opp coins, 'd' for empty cells
        bool isFrontier(vector<vector<char>>& lboard, int a, int b);
        double evalGrid(OthelloBoard& board);
};

MyBot::MyBot(Turn turn) 
    : OthelloPlayer( turn ), bestMove{-1,-1}
{
    // weights for diff positions
    weights = { { 20, -3, 11, 8, 8, 11, -3, 20 }, 
                { -3, -7, 0, 1, 1, 0, -7, -3 },
    			{ 11, 0, 2, 2, 2, 2, 0, 11 }, 
                { 8, 1, 2, 1, 1, 2, 1, 8 },
                { 8, 1, 2, 1, 1, 2, 1, 8 }, 
                { 11, 0, 2, 2, 2, 2, 0, 11 },
    	        { -3, -7, 0, 1, 1, 0, -7, -3 }, 
                { 20, -3, 11, 8, 8, 11, -3, 20 } };

    // moves to neighbours [relative coordinates]
    neighbours = {{-1, -1}, {-1,0}, {-1,1}, {0,-1}, {0,1}, {1, -1}, {1,0}, {1, 1}};
    
    // corner positions
    corners = {{0,0}, {0,7}, {7,0}, {7,7}};
    myColor = turn;
}

vector<vector<char>> MyBot::getLabelGrid(OthelloBoard& board, Turn t) {
    vector<vector<char>> lboard(8);
    for(int i=0;i<8;++i) {
        for(int j=0;j<8;++j) {
            Coin cur = board.get(i,j);
            if (cur == t) lboard[i].push_back('a');
            else if (cur == other(t)) lboard[i].push_back('b');
            else lboard[i].push_back('d');
        }
    }
    return lboard;
}

bool MyBot::isFrontier(vector<vector<char>>& lboard, int a, int b) {
    for (auto nc : neighbours) {
        int x = a + nc[0], y = b + nc[1];
        if (x >= 0 && x < 8 && y >= 0 && y < 8 && lboard[x][y] == 'e') {
            return true;
        }
    }
    return false;
}

double MyBot::evalGrid(OthelloBoard& oboard) {

    vector<vector<char>> lboard = getLabelGrid(oboard, myColor);
    // Alice is player 1 whose score is evaluated.
    int aliceCoins = 0, aliceFrontierCoins = 0;
    int bobCoins = 0, bobFrontierCoins = 0;

    double coinweightscore = 0.0;
    for(int i=0;i<8;++i) {
        for(int j=0;j<8;++j) {
            char cell = lboard[i][j];
            if (cell == 'a') {
                coinweightscore += weights[i][j];
                aliceCoins += 1;
            } else if (cell == 'b') {
                coinweightscore -= weights[i][j];
                bobCoins += 1;
            }

            // if cell is not empty
            if (cell != 'e' and isFrontier(lboard, i, j)) {
                if (cell == 'a') {
                    ++aliceFrontierCoins;
                } else if (cell == 'b') {
                    ++bobFrontierCoins;
                }
            }
        }
    }

    // high number of coins is good.
    double coindiffscore = 0.0;
    if (aliceCoins > bobCoins) coindiffscore = 100.0 * aliceCoins / (aliceCoins + bobCoins);
    else if (bobCoins > aliceCoins) coindiffscore = -100.0 * bobCoins / (aliceCoins + bobCoins);

    // high number of frontier coins is not good as they are vulnerable.
    double coinfrontierdiffscore = 0.0;
    if (aliceFrontierCoins > bobFrontierCoins) coinfrontierdiffscore = -100.0 * aliceFrontierCoins / (aliceFrontierCoins + bobFrontierCoins);
    else if (bobFrontierCoins > aliceFrontierCoins) coinfrontierdiffscore = 100.0 * bobFrontierCoins / (aliceFrontierCoins + bobFrontierCoins);

    // mobility score 
    double mobilityscore = 0.0;
    int aliceMoves = oboard.getValidMoves(myColor).size();
    int bobMoves = oboard.getValidMoves(other(myColor)).size();
    if (aliceMoves > bobMoves) mobilityscore = 100.0 * aliceMoves / (aliceMoves + bobMoves);
    else if (bobMoves > aliceMoves) mobilityscore = -100.0 * bobMoves / (aliceMoves + bobMoves);

    // corner occupancy score
    double cornerscore = 0.0;
    int aliceCorners = 0, bobCorners = 0;
    for(auto p : corners) {
        int x = p[0], y = p[1];
        if (lboard[x][y] == 'a') ++aliceCorners;
        else if (lboard[x][y] == 'b') ++bobCorners;
    }
    cornerscore = 25.0 * (aliceCorners - bobCorners);

    // near corner score. Being near corners is bad as Bob may capture it in his turn :(
    double nearcornerscore = 0.0;
    int aliceNearCorners = 0, bobNearCorners = 0;
    for(auto p : corners) {
        int x = p[0], y = p[1];
        if (lboard[x][y] != 'e') continue;
        for(auto q : neighbours) {
            int i = x + q[0], j = y + q[1];
            if (i >= 0 && j >= 0 && i < 8 && j < 8) {
                if (lboard[i][j] == 'a') ++aliceNearCorners;
                else if (lboard[i][j] == 'b') ++bobNearCorners;
            }
        }
    }
    nearcornerscore = -10.0 * (aliceNearCorners - bobNearCorners);

    double finalscore = 85 * cornerscore + 38 * nearcornerscore + 9 * mobilityscore + 8 * coinfrontierdiffscore + 
                        coinweightscore + coindiffscore;
    return finalscore;
}

double MyBot::alphaBeta(OthelloBoard &board, int ptype, double alpha, double beta, int depth) {

    Coin pColor = ptype == MAXPLAYER ? myColor : other(myColor);

    clock_t now = clock();
    if ((double)(now-begin)/CLOCKS_PER_SEC > 1.95) {
        if (ptype == MAXPLAYER) {
            return 2*INF;
        } else {
            return 2*NINF;
        }
    }

    if (depth == 0) {
        return evalGrid(board);
    }

    list<Move> moves = board.getValidMoves(pColor);
    // handling skipped turns
    if (moves.size() == 0) {
        OthelloBoard bcopy = board;
        return alphaBeta(bcopy, -ptype, alpha, beta, depth - 1);
    }

    double value;
    if (ptype == MAXPLAYER) {
        value = NINF;
        for (auto it = moves.begin(); it != moves.end(); ++it) {
            OthelloBoard bcopy = board;
            bcopy.makeMove(pColor, *it);
            double moveValue = alphaBeta(bcopy, -ptype, alpha, beta, depth - 1);
            if (moveValue > value) {
                value = moveValue;
                if (depth == MAXDEPTH)
                    bestMove = *it;
            }

            if (value >= beta) break;
            alpha = max(value, alpha);
        }
        
    } else {
        value = INF;
        for (auto it = moves.begin(); it != moves.end(); ++it) {
            OthelloBoard bcopy = board;
            bcopy.makeMove(pColor, *it);
            double moveValue = alphaBeta(bcopy, -ptype, alpha, beta, depth - 1);

            if (moveValue < value) {
                value = moveValue;
                if (depth == MAXDEPTH)
                    bestMove = *it;
            }
            
            if (value <= alpha) break;
            beta = min(value, beta);
        }
    }
    return value;
}

Move MyBot::play(const OthelloBoard &board)
{
    OthelloBoard boardcopy = board;
    begin = clock();
    bestMove = Move(-1,-1);
    alphaBeta(boardcopy, MAXPLAYER, NINF, INF, MAXDEPTH);
    return bestMove;
}

// The following lines are _very_ important to create a bot module for Desdemona

extern "C"
{
    OthelloPlayer *createBot(Turn turn)
    {
        return new MyBot(turn);
    }

    void destroyBot(OthelloPlayer *bot)
    {
        delete bot;
    }
}
