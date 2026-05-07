#include "Tank.h"
#include "Map.h"
#include "Cell.h"
#include "raylib.h"
#include <cstdlib>
#include <queue>
#include <vector>
#include <ctime>

void Tank::updatePosition(int user, Cell& current, Cell& target) {
	row = target.row;
	col = target.col;

    if (user == 0) {
        current.isObstacle = false;
        current.hasTank0 = false;
        target.hasTank0 = true;
    } else {
        current.isObstacle = false;
        current.hasTank1 = false;
        target.hasTank1 = true;
	}
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

    int currRow = start.row;
    int currCol = start.col;

    int dRow = (target.row > currRow) ? 1 : (target.row < currRow) ? -1 : 0;
    int dCol = (target.col > currCol) ? 1 : (target.col < currCol) ? -1 : 0;

    bool blocked = false;

    while (currRow != target.row || currCol != target.col) {
        int nextRow = currRow + dRow;
        int nextCol = currCol + dCol;
        if (nextRow < 0 || nextRow >= Map::ROWS || nextCol < 0 || nextCol >= Map::COLS || map.grid[nextRow][nextCol].isObstacle || map.grid[nextRow][nextCol].hasTank0 || map.grid[nextRow][nextCol].hasTank1) {
            blocked = true;
            break;
        }
        updatePosition(user, map.grid[currRow][currCol], map.grid[nextRow][nextCol]);
        currRow = nextRow;
        currCol = nextCol;
    }

	if (!blocked) return;

	srand((unsigned int)time(nullptr));

    bool valid = true;

    while (valid) {
        int position = (rand() % 3);
		bool validThrough[radius + 1] = { false };

        switch (position) {
        case 0:
            for (int i = radius; i > 0; i--) {
                if (!map.grid[currRow][currCol + i].isObstacle && !map.grid[currRow][currCol + i].hasTank0 && !map.grid[currRow][currCol + i].hasTank1) {
					validThrough[i] = true;
				}
            }
            for (int i = 1; i <= radius; i++) {
                if (validThrough[0] && !validThrough[i]) {
                    updatePosition(user, map.grid[currRow][currCol], map.grid[currRow][currCol + i - 1]);
                    currCol = currCol + i - 1;
                    valid = false;
                    break;
                }
			}
            break;
        case 1:
            for (int i = radius; i > 0; i--) {
                if (!map.grid[currRow + i][currCol].isObstacle && !map.grid[currRow + i][currCol].hasTank0 && !map.grid[currRow + i][currCol].hasTank1) {
                    validThrough[i] = true;
                }
            }
            for (int i = 1; i <= radius; i++) {
                if (validThrough[0] && !validThrough[i]) {
                    updatePosition(user, map.grid[currRow][currCol], map.grid[currRow + i - 1][currCol]);
					currRow = currRow + i - 1;
                    valid = false;
                    break;
                }
            }
            break;
        case 2:
            for (int i = radius; i > 0; i--) {
                if (!map.grid[currRow][currCol - i].isObstacle && !map.grid[currRow][currCol - i].hasTank0 && !map.grid[currRow][currCol - i].hasTank1) {
                    validThrough[i] = true;
                }
            }
            for (int i = 1; i <= radius; i++) {
                if (validThrough[0] && !validThrough[i]) {
                    updatePosition(user, map.grid[currRow][currCol], map.grid[currRow][currCol - i + 1]);
                    currCol = currCol - i + 1;
                    valid = false;
                    break;
                }
            }
            break;
        case 3:
            for (int i = radius; i > 0; i--) {
                if (!map.grid[currRow - i][currCol].isObstacle && !map.grid[currRow - i][currCol].hasTank0 && !map.grid[currRow - i][currCol].hasTank1) {
                    validThrough[i] = true;
                }
            }
            for (int i = 1; i <= radius; i++) {
                if (validThrough[0] && !validThrough[i]) {
                    updatePosition(user, map.grid[currRow][currCol], map.grid[currRow - i + 1][currCol]);
                    currRow = currRow - i + 1;
                    valid = false;
                    break;
                }
            }
            break;
        }
	}

    dRow = (target.row > currRow) ? 1 : (target.row < currRow) ? -1 : 0;
    dCol = (target.col > currCol) ? 1 : (target.col < currCol) ? -1 : 0;

    while (currRow != target.row || currCol != target.col) {
        int nextRow = currRow + dRow;
        int nextCol = currCol + dCol;
        if (nextRow < 0 || nextRow >= Map::ROWS || nextCol < 0 || nextCol >= Map::COLS || map.grid[nextRow][nextCol].isObstacle || map.grid[nextRow][nextCol].hasTank0 || map.grid[nextRow][nextCol].hasTank1) {
            break;
        }
        updatePosition(user, map.grid[currRow][currCol], map.grid[nextRow][nextCol]);
        currRow = nextRow;
        currCol = nextCol;
    }
}