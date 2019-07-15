#include <list>
#include "chessmove.h"
using namespace std;

struct chessmove;
enum stage;
extern const int MATEVALUE;
extern const int DEPTH;
extern const int MOBILITY;
extern const int PIECESQUARE;
extern int pronecount;
extern int hits;
extern int maxhits;

class chessboard {
protected:
	char board[8][8];
	char Aboard[8][8];
	int fromX, fromY;
	bool kingtreat = false;
	chessmove loc;
	list <chessmove> possible;
	list <chessmove> temppossible;
	chessmove bestmove;

public:
	chessboard(char b[][8]);
	chessboard();
	chessboard(char b[][8], char Ab[][8]);
	void printboard(bool human = false); // prints the board
	void addloc(int x, int y, int fromX, int fromY, char piece = 'x'); // adds a board location to possible
	void updateAboard(int x, int y, char side, char newSQ = 'X');
	bool isvalid(int x, int y, int fromX, int fromY); // move is inside the board
	void repeat(int piecesquares[][2], int size, int x, int y, char side, bool loop = true); // repeat testing of moves
	void possibleMoves(char side); // fill possible list with possible moves
	void move(char piece, int x, int y); // move a piece
	void move(char piece, int x, int y, int fromX, int fromY); // move a piece
	void printpossible(); // print all possible moves
	float eval(char side, int evalPoss); // get evaluation of current possistion
	float retBest(char side); // do minimax on the possible moves
	void getBest(float value); // print all moves with evaluation
	void printBest();
	void printBestMove();
	void cleanPoss();
	// print the best move
	int callPossible(int depth, char paramSide, int alfa); // engine

	bool checktest(char side);

	bool checkstop(char side);
	void doBestMove(); // does the bestmove...
	void posClear();
	void Aclear(char side = 'x');
	void moveRandom();
};