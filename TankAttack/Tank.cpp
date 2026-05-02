#include "Tank.h"
#include "Cell.h"
#include "raylib.h"
#include <cstdlib>
#include "Map.h"
#include <queue>
#include <vector>

void Tank::updatePosition(int user, Cell& current, Cell& target) {
	row = target.row;
	col = target.col;

    if (user == 0) {
        current.hasTank0 = false;
        target.hasTank0 = true;
    } else {
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