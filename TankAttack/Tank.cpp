#include "Tank.h"
#include "Map.h"
#include "Cell.h"
#include "raylib.h"
#include <cstdlib>
#include <queue>
#include <vector>
#include <ctime>
#include <chrono>
#include <thread>

void Tank::updatePosition(int user, Cell& current, Cell& target) {
    if (user == 0) {
        current.isObstacle = false;
        current.hasTank0 = false;
        target.hasTank0 = true;
    }
    else {
        current.isObstacle = false;
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

    std::vector<int> path;
    int current = tgIdx;

    while (current != -1) {
        path.push_back(current);
        current = parent[current];
    }

    std::reverse(path.begin(), path.end());

    for (int i = 0; i < path.size(); i++) {
        int idx = path[i];
        int row = idx / Map::COLS;
        int col = idx % Map::COLS;
        map.path.push_back(&map.grid[row][col]);
    }
	map.moving = true;
}


void Tank::moveDijkstra(int user, Map& map, Cell& start, Cell& target) {
    map.path.clear();

    int total = Map::ROWS * Map::COLS;
    int startIdx = start.row * Map::COLS + start.col;
    int targetIdx = target.row * Map::COLS + target.col;

    std::vector<int> dist(total, 300);
    std::vector<int> parent(total, -1);
    std::vector<bool> visited(total, false);

    dist[startIdx] = 0;
    for (int i = 0; i < total; i++) 
    {
        int curr = -1;
        int minDist = 300;

        for (int j = 0; j < total; j++) 
        {
            if (!visited[j] && dist[j] < minDist) 
            {
                minDist = dist[j];
                curr = j;
            }
        }
        if (curr == -1)
            break;
        visited[curr] = true;
        for (int j = 0; j < total; j++) 
        {
            if (map.adjacency[curr][j] && !visited[j]) 
            {
                int newDist = dist[curr] + 1;
                if (newDist < dist[j]) 
                {
                    dist[j] = newDist;
                    parent[j] = curr;
                }
            }
        }
    }

    std::vector<int> path;
    int current = targetIdx;
    while (current != -1)
    {
        path.push_back(current);
        current = parent[current];
    }
    std::reverse(path.begin(), path.end());

    for (int i = 0; i < path.size(); i++) 
    {
        int idx = path[i];
        int row = idx / Map::COLS;
        int col = idx % Map::COLS;
        map.path.push_back(&map.grid[row][col]);
    }
    map.moving = true;
}

void Tank::moveRandom(int user, Map& map, Cell& start, Cell& target) {
    map.path.clear();
    map.path.push_back(&map.grid[row][col]);

    const int radius = 5;
    int currRow = row;
    int currCol = col;

    int dRow = (target.row > currRow) ? 1 : (target.row < currRow) ? -1 : 0;
    int dCol = (target.col > currCol) ? 1 : (target.col < currCol) ? -1 : 0;

    bool blocked = false;
    while (currRow != target.row || currCol != target.col) 
    {
        int nextRow = currRow;
        int nextCol = currCol;

        if (currRow != target.row) nextRow += dRow;
        else if (currCol != target.col) nextCol += dCol;

        if (nextRow < 0 || nextRow >= Map::ROWS || nextCol < 0 || nextCol >= Map::COLS || map.grid[nextRow][nextCol].isObstacle || map.grid[nextRow][nextCol].hasTank0 || map.grid[nextRow][nextCol].hasTank1)
        {
            blocked = true;
            break;
        }

        map.path.push_back(&map.grid[nextRow][nextCol]);
        currRow = nextRow;
        currCol = nextCol;
    }

    if (!blocked) 
    {
        map.moving = true;
        return;
    }

    int randRow = currRow;
    int randCol = currCol;
    bool valid = false;

    while (!valid) 
    {
        randRow = currRow + (rand() % (2 * radius + 1)) - radius;
        randCol = currCol + (rand() % (2 * radius + 1)) - radius;

        if (randRow >= 0 && randRow < Map::ROWS && randCol >= 0 && randCol < Map::COLS && !map.grid[randRow][randCol].isObstacle && !map.grid[randRow][randCol].hasTank0 && !map.grid[randRow][randCol].hasTank1) valid = true;
    }

    while (currRow != randRow || currCol != randCol) 
    {
        int nextRow = currRow;
        int nextCol = currCol;

        if (currRow < randRow) nextRow++;
        else if (currRow > randRow) nextRow--; 
        else if (currCol < randCol) nextCol++;
        else if (currCol > randCol) nextCol--;

        if (map.grid[nextRow][nextCol].isObstacle || map.grid[nextRow][nextCol].hasTank0 || map.grid[nextRow][nextCol].hasTank1) break;
        map.path.push_back(&map.grid[nextRow][nextCol]);
        currRow = nextRow;
        currCol = nextCol;
    }

    dRow = (target.row > currRow) ? 1 : (target.row < currRow) ? -1 : 0;
    dCol = (target.col > currCol) ? 1 : (target.col < currCol) ? -1 : 0;

    while (currRow != target.row || currCol != target.col) 
    {
        int nextRow = currRow;
        int nextCol = currCol;

        if (currRow != target.row) nextRow += dRow;
        else if (currCol != target.col) nextCol += dCol;

        if (nextRow < 0 || nextRow >= Map::ROWS || nextCol < 0 || nextCol >= Map::COLS || map.grid[nextRow][nextCol].isObstacle || map.grid[nextRow][nextCol].hasTank0 || map.grid[nextRow][nextCol].hasTank1) break;
        map.path.push_back(&map.grid[nextRow][nextCol]);
        currRow = nextRow;
        currCol = nextCol;
    }
    map.moving = true;
}