#include <list>
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;
// breath first

// http://www.frayn.net/beowulf/theory.html
/*
class node(){
	private:
		chessboard* bPtr;
		list<chessboard> boards;
}
*/
const int MATEVALUE = 300;
const int DEPTH = 3;
struct move {
	int x, y, fromY, fromX;
	char piece;
	int ev;
};
class chessboard { 
	protected:
		char board[8][8];
	int fromX, fromY;
	struct move {
		int x, y, fromY, fromX;
		char piece;
		int ev;
	} loc;
	move lastmove;
	list<move> possible;
	list<move> blackpossible;
	move bestmove;

public:
	chessboard(char b[][8]);
	void printboard(); // prints the board
	void addloc(int x, int y, int fromX, int fromY, bool kingtrett = false); // adds a board location to possible
	bool isvalid(int x, int y); // move is inside the board
	void repeat(int piecesquares[][2], int size, int x, int y, char side); // repeat testing of moves
	void possibleMoves(char side); // fill possible list with possible moves
	void move(char piece, int x, int y); // move a piece
	void move(char piece, int x, int y, int fromX, int fromY); // move a piece
	void printpossible(); // print all possible moves
	int eval(char side, int evalPoss); // get evaluation of current possistion
	int retBest(char side); // do minimax on the possible moves
	void printBest(); // print all moves with evaluation
	void printBestMove(); // print the best move
	int callPossible(int depth, char paramSide); // engine

	bool checktest(char side);

	bool checkstop(char side);
};
chessboard::chessboard(char b[][8]){
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			board[y][x] = b[y][x];
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
void chessboard::addloc(int x, int y, int fromX, int fromY, bool kingtrett) {
	loc.x = x; loc.y = y;
	if (board[fromY][fromX] == 'p' && fromY == 1 && y == 0 && board[y][x] == '0') { // autoqueen mofo
		loc.piece = 'q';
	}
	else if (board[fromY][fromX] == 'P' && fromY == 6 && y == 7 && board[y][x] == '0'){
		loc.piece = 'Q';
	}
	else {
		loc.piece = board[fromY][fromX];
	}

	loc.fromX = fromX;
	loc.fromY = fromY;
	possible.push_back(loc);
}
bool chessboard::isvalid(int x, int y) {
	return (y >= 0 && y < 8 && x >= 0 && x < 8);
}
void chessboard::repeat(int piecesquares[][2] ,int size,  int x, int y, char side) {
	int tempY, tempX;
	bool valid;
	bool rightSide = (islower(board[y][x]) && side == 'B') || (isupper(board[y][x]) && side == 'W');
	char square = '0';
	if (rightSide) {
		for (int i = 0; i < size; i++) {
			tempY = y + piecesquares[i][0]; tempX = x + piecesquares[i][1];
			valid = isvalid(tempX, tempY);
			if (valid) {
				square = board[tempY][tempX];
				while (square == '0' && valid) {
					addloc(tempX, tempY, x, y);
					tempY += piecesquares[i][0]; tempX += piecesquares[i][1];
					valid = isvalid(tempX, tempY);
					if (valid) { square = board[tempY][tempX]; } // må teste om tempy og tempx er utenfor
				}
				if (islower(square) && side == 'W' && isupper(board[y][x]) && square != 'k') { // error free code since 1981
					addloc(tempX, tempY, x, y); // taking a black piece as white
				}
				if (isupper(square) && side == 'B' && islower(board[y][x]) && square != 'K') { // no taking of king
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
	int pawnsquares[2][2] = { {1,-1}, {1,1} }; // POTENTIAL REFACTORING
	char square = '0';
	int tempX, tempY;
	bool valid = false;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (isupper(board[y][x]) && side == 'W') { // hvit
				if (board[y][x] == 'P') { // hvit bonde
					if (board[y + 1][x] == '0') { // bonde 1 fram
						addloc(x, y + 1, x, y);
						if (y == 1 && board[y + 2][x] == '0') { // bonde 2 fram
							addloc(x, y + 2, x, y);
						}
					}
					if (islower(board[y + 1][x + 1]) && board[y + 1][x + 1] != 'k' && isvalid(x+1, y+1)) {
						addloc(x + 1, y + 1, x, y);
					}
					if (islower(board[y + 1][x - 1]) && board[y + 1][x + 1] != 'k' && isvalid(x-1, y+1)) {
						addloc(x - 1, y + 1, x, y);
					}
				}
			}
			if (board[y][x] == 'N' || board[y][x] == 'n') { // knight moves the same way independent of side
				repeat(knightsquares, 8, x, y, side);
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
			if (board[y][x] == 'k' || board[y][x] == 'K') { // knight moves the same way independent of side
				for (int i = 0; i < 8; i++) {
					tempY = y + kingsquares[i][0]; tempX = x + kingsquares[i][1];
					if (tempY >= 0 && tempY < 8 && tempX >= 0 && tempX < 8) {
						square = board[tempY][tempX];
						if ((square == '0' || islower(square)) && side == 'W' && isupper(board[y][x]) && board[tempY][tempX] != 'k') {
							addloc(tempX, tempY, x, y);
						}
						if ((square == '0' || isupper(square)) && side == 'B' && islower(board[y][x]) && board[tempY][tempX] != 'K') {
							addloc(tempX, tempY, x, y);
						}
					}
				}
			}
			
			if (islower(board[y][x]) && side == 'B') { // svart
				if (board[y][x] == 'p') {
					if (board[y - 1][x] == '0') { // bonde 1 fram
						addloc(x, y-1, x, y);
						if (y == 6 && board[y - 2][x] == '0') { // bonde 2 fram
							addloc(x, y-2, x, y);
						}
					}
					if (isupper(board[y - 1][x + 1]) && board[y - 1][x + 1] != 'k'  && isvalid(x + 1, y - 1)) {
						addloc(x + 1, y - 1, x, y);
					}
					if (isupper(board[y - 1][x - 1]) && board[y - 1][x - 1] != 'k'  && isvalid(x - 1, y - 1)) {
						addloc(x - 1, y - 1, x, y);
					}
				}
			}
		}
	}
}

void chessboard::move(char piece, int x, int y){
	board[fromY][fromX] = '0';
	board[y][x] = piece;
}
void chessboard::move(char piece, int x, int y, int fromX, int fromY) {
	lastmove.fromX = fromX;
	lastmove.fromY = fromY;
	lastmove.x = x;
	lastmove.y = y;
	lastmove.piece = piece;
	board[fromY][fromX] = '0';
	board[y][x] = piece;
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

int chessboard::eval(char side, int evalPoss) {
	//chessboard::printboard();
	char pieces[6] = { 'k','q','b','n','r', 'p' };
	//char count[6] = { 0,0,0,0,0,0 };
	int value[6] = { 200, 10, 3, 3, 5, 1 };
	int blackev = ((side == 'W')? 0.1*evalPoss : 0.1*possible.size());
	int whiteev = ((side == 'B')? 0.1*evalPoss : 0.1*possible.size());
	int ev;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (board[y][x] != '0') {
				for (int i = 0; i < 6; i++) {
					if (tolower(board[y][x]) == pieces[i]) {
						(isupper(board[y][x])) ? (whiteev += value[i]) : (blackev += value[i]);
						//count[i] += 1;
						break;
					}
				}
			}
		}
	}
	ev = whiteev - blackev;
	return ev;
}
int chessboard::retBest(char side) {
	int minmax = 0;
	int index = 0;

	if (side == 'W') minmax = INT_MIN;
	else minmax = INT_MAX;
	for (auto i = possible.begin(); i != possible.end(); i++) {
		if (side == 'W') {
			minmax = ((*i).ev >= minmax) ? (*i).ev : minmax;
		}
		else {
			minmax = ((*i).ev <= minmax) ? (*i).ev : minmax;
		}
		
	}
	return minmax;
}
void chessboard::printBest() {
	int count = 0;
	for (auto i = possible.begin(); i != possible.end(); i++) {
		count += 1;
		cout << "(x, y): " << (*i).x << ", " << (*i).y;
		cout << " piece: " << (*i).piece << " to square:  " << board[(*i).y][(*i).x];
		cout << " from (x, y): " << (*i).fromX << ", " << (*i).fromY;
		cout << " ev: " << (*i).ev;
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
int chessboard::callPossible(int depth, char paramSide){ // populates this class boards, and the moves with EV
	chessboard* moveBoard;
	chessboard* tempBoard;
	bool kingtreat = false;
	bool mate = false;
	char side;
	int d = depth - 1;

	if (paramSide == 'B') {
		bestmove.ev = INT_MIN;
	}
	else {
		bestmove.ev = INT_MAX;
	}
	for (auto i = possible.begin(); i != possible.end(); i++) {
		moveBoard = new chessboard(board); tempBoard = new chessboard(board);
		moveBoard->move((*i).piece, (*i).x, (*i).y, (*i).fromX, (*i).fromY);
		tempBoard->move((*i).piece, (*i).x, (*i).y, (*i).fromX, (*i).fromY);
		
		side = ((islower((*i).piece)) ? 'W' : 'B');
		
		kingtreat = tempBoard->checktest(side);
		if (!kingtreat) { // normal
			moveBoard->possibleMoves(side);
		}
		else {// has a check
			mate = moveBoard->checkstop(side);
			cout << "check" << endl;
			if (mate) {
				cout << "mate" << endl;
				if (d == 0) {
					(*i).ev = MATEVALUE + ((DEPTH - d) * 2);
				}
				else {
					(*i).ev = INT_MAX;
				}
				(*i).ev *= (islower((*i).piece)) ? -1 : 1;
			}
		}
		if (!mate) {
			if (d == 0) {
				(*i).ev = moveBoard->eval(side, possible.size());
			}
			else {
				moveBoard->callPossible(d, side);
				(*i).ev = moveBoard->retBest(side);
			}
		}

		if (((*i).ev > bestmove.ev) && side == 'W') bestmove = (*i); // compensates for which player turn it is
		else if (((*i).ev < bestmove.ev) && side == 'B') bestmove = (*i);
	}
	return 1;
}
bool chessboard::checktest(char side) {
	bool kingtreat = false;

	possible.clear();
	possibleMoves((side == 'W') ? 'B' : 'W');
	for (auto b = possible.begin(); b != possible.end(); b++) {
		if (board[(*b).y][(*b).x] == 'k' || board[(*b).y][(*b).x] == 'K') {
			kingtreat = true;
			break;
		}
	}
	possible.clear();

	return kingtreat;
}
bool chessboard::checkstop(char side){
	chessboard* boardPtr;
	possibleMoves(side);
	auto i = possible.begin();
	while(i != possible.end()){
		boardPtr = new chessboard(board);
		boardPtr->move((*i).piece, (*i).x, (*i).y, (*i).fromX, (*i).fromY);
		if (boardPtr->checktest(side)) { // hardly optimal
			possible.erase(i++);
		}
		else {
			++i;
		}
	}
	return possible.empty();
}
char startboard[8][8] = { {'R','N','B','K','Q','B','N','R'},
						  {'P','P','P','P','P','P','P','P'},
					      {'0','0','0','0','0','0','0','0'},
					      {'0','0','0','0','0','0','0','0'},
					      {'0','0','0','0','0','0','0','0'},
					      {'0','0','0','0','0','0','0','0'},
					      {'p','p','p','p','p','p','p','p'},
					      {'r','n','b','q','k','b','n','r'} };

char testboard[8][8] = {  {'0','0','0','0','0','0','K','0'},
						  {'0','P','0','0','0','P','n','P'},
						  {'Q','0','0','0','R','0','0','0'},
						  {'0','0','0','P','0','0','0','0'},
						  {'0','0','0','0','0','0','0','0'},
						  {'p','p','b','N','p','0','p','0'},
						  {'k','0','0','0','0','0','0','p'},
						  {'0','r','0','q','0','r','0','0'} };
int main() {
	chessboard board(testboard);
	char side = 'W';
	board.possibleMoves(side);
	board.callPossible(DEPTH, side);
	board.printboard();
	board.printBest();
	board.printBestMove();
	cout << "finished" << endl;

}