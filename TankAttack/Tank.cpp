#include "Tank.h"
#include "Map.h"
#include "Cell.h"
#include "raylib.h"
#include <cstdlib>
#include <queue>
#include <vector>
#include <ctime>

void Tank::updatePosition(int user, Cell& current, Cell& target) {
    if (user == 0) {
        current.hasTank0 = false;
        target.hasTank0 = true;
    }
    else {
        current.hasTank1 = false;
        target.hasTank1 = true;
    }

    row = target.row;
    col = target.col;
}

void Tank::moveBFS(int user, Map& map, Cell& start, Cell& target) {
	int stIdx = start.row * Map::COLS + start.col;
	int tgIdx = target.row * Map::COLS + target.col;
	std::vector<int> parent(Map::ROWS * Map::COLS, -1);
    std::vector<char> visited(Map::ROWS * Map::COLS, 0);
	std::queue<int> q;

    visited[stIdx] = true;
    q.push(stIdx);

    bool found = false;

    while (!q.empty()) {
        int curr = q.front();
        q.pop();
        
        if (curr == tgIdx) {
            found = true;
            break;
		}

        for (int j = 0; j < Map::ROWS * Map::COLS; j++) {
            if (map.adjacency[curr][j] && !visited[j]) {
                visited[j] = true;
                parent[j] = curr;
                q.push(j);
            }
		}
	}

	updatePosition(user, start, target);
}


void Tank::moveDijkstra(int user, Map& map, Cell& start, Cell& target) {
    int newR = row;
    int newC = col;

    if (target.row > row) newR++;
    else if (target.row < row) newR--;
    else if (target.col > col) newC++;
    else if (target.col < col) newC--;

    if (newR >= 0 && newR < Map::ROWS && newC >= 0 && newC < Map::COLS && !map.grid[newR][newC].isObstacle) {
        if (user == 0) map.grid[row][col].hasTank0 = false; else map.grid[row][col].hasTank1 = false;
        row = newR; col = newC;
        if (user == 0) map.grid[row][col].hasTank0 = true; else map.grid[row][col].hasTank1 = true;
    }
}

void Tank::moveRandom(int user, Map& map, Cell& start, Cell& target) {
    const int radius = 5;

    int currRow = row;
    int currCol = col;

    int dRow = (target.row > currRow) ? 1 : (target.row < currRow) ? -1 : 0;
    int dCol = (target.col > currCol) ? 1 : (target.col < currCol) ? -1 : 0;

    bool blocked = false;

    while (currRow != target.row || currCol != target.col) {
        int nextRow = currRow;
        int nextCol = currCol;

        if (currRow != target.row) nextRow += dRow;
        else if (currCol != target.col) nextCol += dCol;

        if (nextRow < 0 || nextRow >= Map::ROWS || nextCol < 0 || nextCol >= Map::COLS || map.grid[nextRow][nextCol].isObstacle || map.grid[nextRow][nextCol].hasTank0 || map.grid[nextRow][nextCol].hasTank1) {
            blocked = true;
            break;
        }
        updatePosition(user, map.grid[currRow][currCol], map.grid[nextRow][nextCol]);
        currRow = nextRow;
        currCol = nextCol;
    }

	if (!blocked) return;

    int randRow = currRow + (rand() % (2 * radius + 1)) - radius;
    int randCol = currCol + (rand() % (2 * radius + 1)) - radius;

    if (randRow >= 0 && randRow < Map::ROWS && randCol >= 0 && randCol < Map::COLS && !map.grid[randRow][randCol].isObstacle && !map.grid[randRow][randCol].hasTank0 && !map.grid[randRow][randCol].hasTank1) {

        updatePosition(user, map.grid[currRow][currCol], map.grid[randRow][randCol]);

        currRow = randRow;
        currCol = randCol;
    }
    dRow = (target.row > currRow) ? 1 : (target.row < currRow) ? -1 : 0;
    dCol = (target.col > currCol) ? 1 : (target.col < currCol) ? -1 : 0; 

    while (currRow != target.row || currCol != target.col) { 
        int nextRow = currRow;
        int nextCol = currCol;

        if (currRow != target.row) nextRow += dRow;
        else if (currCol != target.col) nextCol += dCol;

        if (nextRow < 0 || nextRow >= Map::ROWS || nextCol < 0 || nextCol >= Map::COLS || map.grid[nextRow][nextCol].isObstacle || map.grid[nextRow][nextCol].hasTank0 || map.grid[nextRow][nextCol].hasTank1) break;
        updatePosition(user, map.grid[currRow][currCol], map.grid[nextRow][nextCol]);        
        currRow = nextRow;       
        currCol = nextCol; 
    }
}