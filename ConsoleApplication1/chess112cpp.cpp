#include <list>
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;
// se trekkene som var beste
// notajon input fra spiller
// forskjell på ulik spill stadie i ev. åpning er shit..
// eval som underklasse (protected)
// eval endrer seg selv.. AI!?
// http://www.frayn.net/beowulf/theory.html

const int MATEVALUE = 300;
const int DEPTH = 3;
const int MOBILITY = 0;
const int PIECESQUARE = 0;
bool normalflow = false;
int pronecount = 0;
int hits = 0;

struct chessmove {
	int x, y, fromY, fromX;
	char piece;
	float ev;
	chessboard* ptr;
	bool operator<(const chessmove & a);
	bool operator==(const chessmove& a);
};
enum stage { opening, midgame, endgame };

class chessboard { 
	protected:
	char board[8][8];
	char Aboard[8][8];
	int fromX, fromY;
	bool kingtreat = false;
	chessmove loc;
	list<chessmove> possible;
	list<chessmove> temppossible;
	chessmove bestmove;

public:
	chessboard(char b[][8]);
	chessboard(char b[][8], char Ab[][8]);
	void printboard(); // prints the board
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
	void getBest(float value, chessboard* bPtr = nullptr); // print all moves with evaluation
	void printBest();
	void printBestMove(); // print the best move
	int callPossible(int depth, char paramSide, int alfa); // engine

	bool checktest(char side);

	bool checkstop(char side);
	void doBestMove(); // does the bestmove...
	void posClear();
	void Aclear(char side = 'x');
};
chessboard::chessboard(char b[][8]){
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			board[y][x] = b[y][x];
			Aboard[y][x] = '0';
		}
	}
}
chessboard::chessboard(char b[][8], char Ab[][8]) {
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			board[y][x] = b[y][x];
			Aboard[y][x] = Ab[y][x];
		}
	}
}
void chessboard::printboard() {
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			cout << board[y][x];
		}
		cout << endl;
	}
	cout << endl;
}
void chessboard::addloc(int x, int y, int fromX, int fromY, char piece) {
	char pieces[4] = { 'q','b','n','r'};
	bool kingtrett = false;
	char square = '0';
	char side = isupper(board[fromY][fromX])? 'W' : 'B';

	loc.x = x; loc.y = y;
	Aboard[y][x] = side;

	if (board[fromY][fromX] == 'p' && fromY == 1 && y == 0 && board[y][x] == '0') { // pawn turns to new piece
		for (int i = 0; i < 4; i++) {
			addloc(x, y, fromX, fromY, pieces[i]);
		}
	}
	else if (board[fromY][fromX] == 'P' && fromY == 6 && y == 7 && board[y][x] == '0') {
		for (int i = 0; i < 4; i++) {
			addloc(x, y, fromX, fromY, toupper(pieces[i]));
		}
	}
	else {
		if (piece == 'x') {
			loc.piece = board[fromY][fromX];
		}
		else {
			loc.piece = piece;
		}
	}

	loc.fromX = fromX;
	loc.fromY = fromY;
	possible.push_back(loc);
}
void chessboard::updateAboard(int x, int y, char side, char newSQ) {
	if (Aboard[y][x] == ((side == 'W') ? 'B' : 'W')) {
		Aboard[y][x] = newSQ;
	}
	else {
		if (Aboard[y][x] == side) Aboard[y][x] = 'X';
		else Aboard[y][x] = side;
	}
}
bool chessboard::isvalid(int x, int y, int fromX, int fromY) {
	bool inside = (y >= 0 && y < 8 && x >= 0 && x < 8);
	bool takingRight = false;
	char side = (isupper(board[fromY][fromX])) ? 'W' : 'B';
	bool selfCheck = false;
	if (inside) {
		if (side == 'W') {
			takingRight = islower(board[y][x]) || board[y][x] == '0';
		}
		else {
			takingRight = isupper(board[y][x]) || board[y][x] == '0';
		}
		if (board[fromY][fromX] == 'k' || board[fromY][fromX] == 'K') {
			if (Aboard[y][x] == ((side == 'W') ? 'B' : 'W') || Aboard[y][x] == 'X') {
				selfCheck = true;
			}
			else {
				selfCheck = false;
			}
		}
	}
	if (!takingRight && inside) {
		updateAboard(x, y, side);
	}
	return inside && !selfCheck && takingRight;
}
void chessboard::repeat(int piecesquares[][2] ,int size,  int x, int y, char side, bool loop) {
	int tempY, tempX;
	bool valid;
	bool rightSide = (islower(board[y][x]) && side == 'B') || (isupper(board[y][x]) && side == 'W');
	char square = '0';
	int count = 0;
	if (rightSide) {
		for (int i = 0; i < size; i++) {
			count = 0;
			tempY = y + piecesquares[i][0]; tempX = x + piecesquares[i][1]; // does one for the non looping ones, aka hourse, king
			valid = isvalid(tempX, tempY, x, y);
			if (valid) {
				square = board[tempY][tempX];
				addloc(tempX, tempY, x, y); count++;
				while (square == '0' && valid && loop) {
					if (isvalid(tempX+ piecesquares[i][1], tempY+ piecesquares[i][0], x, y) ) {
						valid = true;
						tempY += piecesquares[i][0]; tempX += piecesquares[i][1];
						addloc(tempX, tempY, x, y); count++;
						square = board[tempY][tempX];
					}
					else {
						valid = false;
					}
				}
				if (islower(square) && side == 'W' && isupper(board[y][x])) { // add:  && square != 'k'
					addloc(tempX, tempY, x, y); // taking a black piece as white
				}
				if (isupper(square) && side == 'B' && islower(board[y][x])) { // add:  && square != 'K'
					addloc(tempX, tempY, x, y); // taking a white piece as black
				}
			}
		}
	}
}
void chessboard::possibleMoves(char side) {
	int knightsquares[8][2] = { {2,-1},{2,1},{1,2},{-1,2},{-2,1},{-2,-1},{-1,-2}, {1,-2} };
	int kingsquares[8][2] = { {1,-1} ,{1,0} ,{1,1} ,{0,1} ,{-1,-1} ,{-1,0} ,{-1,-1} ,{0,-1} };
	int bishopsquares[4][2] = { {1,-1}, {1, 1}, {-1,1}, {-1, -1} };
	int rooksquares[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1}};
	int pawnsquares[2][2] = { {1,-1}, {1,1} };
	char square = '0';
	int offset = 0;
	int pawnstart = 0;
	int tempX, tempY;
	bool valid = false;

	possible.clear();

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (board[y][x] != '0') {
				if (board[y][x] == 'P' || board[y][x] == 'p') { // 24 - 30
					offset = (board[y][x] == 'P') ? 1 : -1; // directon pawn moves
					if ((side == 'W' && offset == 1) || (side == 'B' && offset == -1)) {
						pawnstart = (offset == 1) ? 1 : 6;
						if (board[y + offset][x] == '0') {
							addloc(x, y + offset, x, y);
							if (y == pawnstart && board[y + 2 * offset][x] == '0') {
								addloc(x, y + 2 * offset, x, y);
							}
						}
						for (int i = 0; i < 2; i++) {
							tempX = x + pawnsquares[i][1];
							tempY = y + offset * pawnsquares[i][0];
							if (isvalid(tempX, tempY, x, y)) {
								square = board[tempY][tempX];
								if (isupper(square) && square != 'k' && offset == -1) {
									addloc(tempX, tempY, x, y);
								}
								if (islower(square) && square != 'k' && offset == 1) {
									addloc(tempX, tempY, x, y);
								}
							}
						}
					}
				}
				if (board[y][x] == 'N' || board[y][x] == 'n') { // knight moves the same way independent of side
					repeat(knightsquares, 8, x, y, side, false);
				}
				if (board[y][x] == 'B' || board[y][x] == 'b') {
					repeat(bishopsquares, 4, x, y, side);
				}
				if (board[y][x] == 'R' || board[y][x] == 'r') {
					repeat(rooksquares, 4, x, y, side);
				}
				if (board[y][x] == 'Q' || board[y][x] == 'q') {
					repeat(rooksquares, 4, x, y, side);
					repeat(bishopsquares, 4, x, y, side);
				}
				if (board[y][x] == 'K' || board[y][x] == 'k') { // knight moves the same way independent of side
					repeat(kingsquares, 8, x, y, side, false);
				}
			}
		}
	}
	Aclear((side == 'W') ? 'B' : 'W'); // clear opposite attackboard
}

void chessboard::move(char piece, int x, int y){
	board[fromY][fromX] = '0';
	board[y][x] = piece;
}
void chessboard::move(char piece, int x, int y, int fromX, int fromY) {
	board[fromY][fromX] = '0';
	board[y][x] = piece;
	updateAboard(x, y, (isupper(piece)) ? 'B' : 'W', '0'); // empty aboard when piece moves
}

void chessboard::printpossible(){
	int count = 0;
	for (auto i = possible.begin(); i != possible.end(); i++) {
		count += 1;
		cout << "(x, y): " << (*i).x << ", " << (*i).y;
		cout << " piece: " << (*i).piece << " to square:  " << board[(*i).y][(*i).x];
		cout << " from (x, y): " << (*i).fromX << ", " << (*i).fromY;
		cout << endl;
	}
	cout << "possible moves: " << count << endl;
}

float chessboard::eval(char side, int evalPoss) { // https://www.chessprogramming.org/Simplified_Evaluation_Function
	char pieces[6] = { 'k','q','b','n','r', 'p' };
	float value[6] = { 200, 10, 3, 3, 5, 1 };
	float count[6] = { 0, 0, 0, 0, 0, 0 };
	float blackev = ((side == 'W') ? round(MOBILITY*evalPoss) : round(MOBILITY*possible.size()));
	float whiteev = ((side == 'B') ? round(MOBILITY*evalPoss) : round(MOBILITY*possible.size()));
	int pawnscore[8][8] = { {0,  0,  0,  0,  0,  0,  0,  0},
							{50, 50, 50, 50, 50, 50, 50, 50},
							{10, 10, 20, 30, 30, 20, 10, 10},
							{ 5,  5, 10, 50, 50, 10,  5,  5},
							{ 0,  0,  0, 50, 50,  0,  0,  0},
							{ 5,  5, 10,  20,  20,-10,  10,  5},
							{ 5, 10, 10,-20,-20, 10, 10,  5},
							{ 0,  0,  0,  0,  0,  0,  0,  0} };
	int knightscore[8][8] = { {-50,-40,-30,-30,-30,-30,-40,-50},
							  {-40,-20,  0,  0,  0,  0,-20,-40}, // B
							  {-30,  0, 10, 15, 15, 10,  0,-30},
							  {-30,  5, 15, 20, 20, 15,  5,-30},
							  {-30,  0, 15, 20, 20, 15,  0,-30},
							  {-30,  5, 7, 15, 15, 7,  5,-30}, // 5 10 15 15 10 5
							  {-40, -20,  0,  5,  5,  0,-20,-40}, // W
							  {-50, -40, -30,-30,-30,-30,-40,-50} };
	int bishopscore[8][8] = { {-20,-10,-10,-10,-10,-10,-10,-20 },
								{10,  0,  0,  0,  0,  0,  0,-10 },
								{-10,  0,  5, 10, 10,  5,  0,-10 },
								{-10,  5,  5, 10, 10,  5,  5,-10 },
								{-10,  0, 10, 10, 10, 10,  0,-10 },
								{-10, 10, 10, 10, 10, 10, 10,-10 },
								{-10,  5,  0,  0,  0,  0,  5,-10 },
								{-20,-10,-10,-10,-10,-10,-10,-20 } };
	int rookscore[8][8] = { {0,  0,  0,  0,  0,  0,  0,  0 },
							{ 5, 10, 10, 10, 10, 10, 10,  5 },
							{ -5,  0,  0,  0,  0,  0,  0, -5 },
							{ -5,  0,  0,  0,  0,  0,  0, -5 },
							{ -5,  0,  0,  0,  0,  0,  0, -5 },
							{ -5,  0,  0,  0,  0,  0,  0, -5 },
							{ -5,  0,  0,  0,  0,  0,  0, -5 },
							{ 0,  0,  0,  5,  5,  0,  0,  0 }};
	int queenscore[8][8] = { {-20,-10,-10, -5, -5,-10,-10,-20},
							{-10,  0,  0,  0,  0,  0,  0,-10},
							{-10,  0,  5,  5,  5,  5,  0,-10},
							{ -5,  0,  5,  5,  5,  5,  0, -5},
							{  0,  0,  5,  5,  5,  5,  0, -5},
							{-10,  5,  5,  5,  5,  5,  0,-10},
							{-10,  0,  5,  0,  0,  5,  0,-10},
							{-20,-10,-10, -5, -5,-10,-10,-20} };
	float ev = 0;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (board[y][x] != '0') {
				for (int i = 0; i < 6; i++) {
					if (tolower(board[y][x]) == pieces[i]) {
						(isupper(board[y][x])) ? (whiteev += value[i]) : (blackev += value[i]);
						if (tolower(board[y][x]) == 'p') {
							if (isupper(board[y][x])) {
								whiteev += PIECESQUARE*pawnscore[7- y][x];
							}
							else {
								blackev += PIECESQUARE *pawnscore[y][x];
							}
						}
						if (tolower(board[y][x]) == 'n') {
							if (isupper(board[y][x])) {
								whiteev += PIECESQUARE *knightscore[7 - y][x];
							}
							else {
								blackev += PIECESQUARE *knightscore[y][x];
							}
						}
						if (tolower(board[y][x]) == 'b') {
							if (isupper(board[y][x])) {
								whiteev += PIECESQUARE *bishopscore[7 - y][x];
							}
							else {
								blackev += PIECESQUARE *bishopscore[y][x];
							}
						}
						if (tolower(board[y][x]) == 'r') {
							if (isupper(board[y][x])) {
								whiteev += PIECESQUARE *rookscore[7 - y][x];
							}
							else {
								blackev += PIECESQUARE*rookscore[y][x];
							}
						}
						if (tolower(board[y][x]) == 'q') {
							if (isupper(board[y][x])) {
								whiteev += PIECESQUARE *queenscore[7 - y][x];
							}
							else {
								blackev += PIECESQUARE *queenscore[y][x];
							}
						}
						break;
					}
				}
			}
		}
	}
	ev = whiteev - blackev;
	return ev;
}
float chessboard::retBest(char side) {
	float minmax = 0;
	int index = 0;

	if (side == 'B') minmax = INT_MIN;
	else minmax = INT_MAX;
	for (auto i = possible.begin(); i != possible.end(); i++) {
		if (side == 'B') {
			minmax = ((*i).ev >= minmax) ? (*i).ev : minmax;
		}
		else {
			minmax = ((*i).ev <= minmax) ? (*i).ev : minmax;
		}
		
	}
	return minmax;
}
void chessboard::getBest(float value, chessboard* bPtr) {
	for (auto i = possible.begin(); i != possible.end(); i++) {
		if (value == (*i).ev) {
			bestmove = (*i);
			bestmove.ptr = bPtr;
		}
	}
}
void chessboard::printBest() {
	int count = 0;
	for (auto i = possible.begin(); i != possible.end(); i++) {
		count += 1;
		cout << "(x, y): " << (*i).x << ", " << (*i).y;
		cout << " piece: " << (*i).piece << " to square:  " << board[(*i).y][(*i).x];
		cout << " from (x, y): " << (*i).fromX << ", " << (*i).fromY;
		cout << "  ev: " << (*i).ev;
		cout << endl;
	}
	cout << "possible moves: " << count << endl;
}
void chessboard::printBestMove() {
	cout << "(x, y): " << bestmove.x << ", " << bestmove.y;
	cout << " piece: " << bestmove.piece << " to square:  " << board[bestmove.y][bestmove.x];
	cout << " from (x, y): " << bestmove.fromX << ", " << bestmove.fromY;
	cout << " ev: " << bestmove.ev;
	cout << endl;
}
bool chessmove::operator<(const chessmove& a) {
	return ev < a.ev;
}
bool chessmove::operator==(const chessmove & a)
{
	return (x == a.x) && (y == a.y) && (fromY == a.fromY) && (fromX == a.fromX) && (piece == a.piece);
}
int chessboard::callPossible(int depth, char paramSide, int alfa){ // populates this class boards, and the moves with EV
	chessboard* moveBoard;
	chessboard* checkBoard;
	bool mate = false;
	char side;
	int d = depth - 1;

	possible.unique();
	possible.sort();
	if (paramSide == 'W') possible.reverse();

	
	for (auto i = possible.begin(); i != possible.end(); i++) {
		moveBoard = new chessboard(board, Aboard);
		checkBoard = new chessboard(board, Aboard);

		if (tolower(board[(*i).y][(*i).x]) != 'k') { // never actually take the king
			moveBoard->move((*i).piece, (*i).x, (*i).y, (*i).fromX, (*i).fromY);
			side = ((islower((*i).piece)) ? 'W' : 'B'); // father node

			*checkBoard = *moveBoard;
			if (checkBoard->checktest(side)) { // am i in check?
				mate = moveBoard->checkstop(side);
				if (mate) {
					(*i).ev = MATEVALUE * ((side == 'B') ? -1 : 1);
				}
			}
			else{
				moveBoard->possibleMoves(side);
			}
			if (!mate) { // normal way
				if (d == 0) {
					(*i).ev = moveBoard->eval(side, possible.size());
				}
				else {
					moveBoard->callPossible(d, side, alfa);
					(*i).ev = moveBoard->retBest(side); // W = minst mulig ev B = høyst mulig ev
					//moveBoard->getBest((*i).ev); needs to store best move somehow.. boardptr of best move is lost..
					alfa = (*i).ev;
				}
			}
			if ((*i).ev < alfa && side == 'W') { // når hvit = svart possible, minst mulig ev.
				pronecount += 1;
				break;
			}
			if ((*i).ev > alfa && side == 'B') { // når svart = hvit possible, høyst ev.
				pronecount += 1;
				break;
			}
			hits += 1;
		}
	}
	return 1;
}
bool chessboard::checktest(char side) { // VELDIG ineffektivt much
	bool kingtreat = false; // 5ms

	temppossible = possible;
	possible.clear();
	possibleMoves( ((side == 'W')? 'B' : 'W'));
	for (auto b = possible.begin(); b != possible.end(); b++) {
		if (board[(*b).y][(*b).x] == 'k' || board[(*b).y][(*b).x] == 'K') { // pålegger at repeat ikke bruker square != 'k'
			kingtreat = true;
			break;
		}
	}
	possible.clear();
	possible = temppossible;
	temppossible.clear();

	return kingtreat;
}
bool chessboard::checkstop(char side){
	chessboard* boardPtr;
	possible.clear();
	possibleMoves(side);
	possible.unique();
	auto i = possible.begin();
	while(i != possible.end()){
		boardPtr = new chessboard(board);
		boardPtr->move((*i).piece, (*i).x, (*i).y, (*i).fromX, (*i).fromY);
		if (boardPtr->checktest(side)) {
			possible.erase(i++);
		}
		else {
			++i;
		}
	}
	return possible.empty();
}
void chessboard::doBestMove() {
	cout << "(x, y): " << bestmove.x << ", " << bestmove.y;
	cout << " piece: " << bestmove.piece << " to square:  " << board[bestmove.y][bestmove.x];
	cout << " from (x, y): " << bestmove.fromX << ", " << bestmove.fromY;
	cout << " ev: " << bestmove.ev;
	cout << "  BESTMOVE" << endl;
	move(bestmove.piece, bestmove.x, bestmove.y, bestmove.fromX, bestmove.fromY);
}
void chessboard::posClear() {
	possible.clear();
}
void chessboard::Aclear(char side) {
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if(Aboard[y][x] == side || side == 'x') Aboard[y][x] = '0';
			if (Aboard[y][x] == 'X' && side == 'W') Aboard[y][x] = 'B';
			if (Aboard[y][x] == 'X' && side == 'B') Aboard[y][x] = 'W';
		}
	}
}
char startboard[8][8] = { {'R','N','B','K','Q','B','N','R'},
						  {'P','P','P','P','P','P','P','P'},
					      {'0','0','0','0','0','0','0','0'},
					      {'0','0','0','0','0','0','0','0'},
					      {'0','0','0','0','0','0','0','0'},
					      {'0','0','0','0','0','0','0','0'},
					      {'p','p','p','p','p','p','p','p'},
					      {'r','n','b','q','k','b','n','r'} };

char blackpawn[8][8] = {  {'0','0','0','0','K','0','0','0'},
						  {'0','0','0','0','0','0','0','0'},
						  {'0','0','0','0','0','0','0','0'},
						  {'0','0','0','0','0','0','0','0'},
						  {'0','0','0','0','0','0','0','0'},
						  {'0','0','0','0','0','0','0','0'},
						  {'0','0','0','0','0','r','0','0'},
						  {'0','0','0','r','0','k','0','0'} };

char testboard[8][8] = {  {'0','0','0','0','0','0','K','0'}, // W best: R 4,4
						  {'0','P','0','0','0','P','n','P'},
						  {'Q','0','0','0','R','0','0','0'},
						  {'0','0','0','P','0','0','0','0'},
						  {'0','0','0','0','0','0','0','0'},
						  {'p','p','b','N','0','0','p','0'},
						  {'k','0','0','0','0','0','0','p'},
						  {'0','r','0','q','0','r','0','0'} };
int main() {
	chessboard board(testboard);
	char side = 'W';

	board.possibleMoves( ((side == 'W') ? 'B' : 'W') ); // fill attackboard (Aboard)
	board.posClear(); // clear possibleMoves

	int movecount = 0;
	while (true) {
		movecount += 1;
		board.possibleMoves(side); // do the sides possible moves
		board.callPossible(DEPTH, side, (side == 'W') ? INT_MIN : INT_MAX); // search best move

		board.getBest(board.retBest( ((side == 'W')? 'B': 'W')  )); // get bestmove
		board.printBestMove(); // print bestmove
		board.doBestMove(); // do the best move
		board.printBest();
		board.printboard(); // print the board
		board.posClear(); // clear possible moves

		side = (side == 'W') ? 'B' : 'W'; // change side 
		board.printBest();
		cout << ((float)pronecount / (float)hits)*100.0F  << "  Hits: " << hits << " move: " <<  movecount/2 << endl;
		if (hits == 0) break;
		hits = pronecount = 0;
	}
	cout << "finished" << endl;
}