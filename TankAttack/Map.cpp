#include "Map.h"
#include "raylib.h"
#include <cstdlib>
#include <ctime>
#include <cstdio>

Map::Map() {
    int total = ROWS * COLS;
    adjacency = new bool* [total];
    for (int i = 0; i < total; i++) {
        adjacency[i] = new bool[total];
        for (int j = 0; j < total; j++)
            adjacency[i][j] = false;
    }
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            grid[r][c] = Cell(r, c);
    pathSize = 0;
    bulletPathSize = 0;
    showBulletPath = false;
}

Map::~Map() {
    int total = ROWS * COLS;
    for (int i = 0; i < total; i++)
        delete[] adjacency[i];
    delete[] adjacency;
}

int Map::toIndex(int row, int col) const {
    return row * COLS + col;
}

bool Map::isObstacle(int row, int col) const {
    return grid[row][col].isObstacle;
}

void Map::generate() {
    srand((unsigned int)time(nullptr));
    do {
        for (int r = 0; r < ROWS; r++)
            for (int c = 0; c < COLS; c++) {
                grid[r][c].isObstacle = false;
                grid[r][c].hasTank0 = false;
                grid[r][c].hasTank1 = false;
            }
        for (int r = 0; r < ROWS; r++)
            for (int c = 0; c < COLS; c++)
                if (rand() % 100 < 30)
                    grid[r][c].isObstacle = true;
        buildAdjacency();
    } while (!isFullyConnected());

    int placed = 0;
    while (placed < 4) {
        int r = rand() % ROWS;
        int c = rand() % COLS;
        if (!grid[r][c].isObstacle && !grid[r][c].hasTank0 && !grid[r][c].hasTank1) {
            switch (placed) {
            case 0: grid[r][c].hasTank0 = true; tanks[0] = Tank(r, c, 0, 100, BLUE);    break;
            case 1: grid[r][c].hasTank0 = true; tanks[1] = Tank(r, c, 0, 100, RED);     break;
            case 2: grid[r][c].hasTank1 = true; tanks[2] = Tank(r, c, 1, 100, SKYBLUE); break;
            case 3: grid[r][c].hasTank1 = true; tanks[3] = Tank(r, c, 1, 100, YELLOW);  break;
            }
            placed++;
        }
    }
}

void Map::buildAdjacency() {
    int total = ROWS * COLS;
    for (int i = 0; i < total; i++)
        for (int j = 0; j < total; j++)
            adjacency[i][j] = false;

    int dr[] = { -1, 1, 0, 0 };
    int dc[] = { 0, 0, -1, 1 };

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (grid[r][c].isObstacle) continue;
            if (grid[r][c].hasTank0 || grid[r][c].hasTank1) continue; // tanque bloquea
            for (int d = 0; d < 4; d++) {
                int nr = r + dr[d];
                int nc = c + dc[d];
                if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS)
                    if (!grid[nr][nc].isObstacle &&
                        !grid[nr][nc].hasTank0 &&
                        !grid[nr][nc].hasTank1)
                        adjacency[toIndex(r, c)][toIndex(nr, nc)] = true;
            }
        }
    }
}

bool Map::isFullyConnected() const {
    int total = ROWS * COLS;
    int start = -1;
    for (int i = 0; i < total; i++) {
        int r = i / COLS, c = i % COLS;
        if (!grid[r][c].isObstacle) { start = i; break; }
    }
    if (start == -1) return false;

    bool* visited = new bool[total];
    for (int i = 0; i < total; i++) visited[i] = false;

    int* q = new int[total];
    int front = 0, back = 0;
    q[back++] = start;
    visited[start] = true;

    while (front < back) {
        int curr = q[front++];
        for (int j = 0; j < total; j++) {
            if (adjacency[curr][j] && !visited[j]) {
                visited[j] = true;
                q[back++] = j;
            }
        }
    }

    bool connected = true;
    for (int i = 0; i < total; i++) {
        int r = i / COLS, c = i % COLS;
        if (!grid[r][c].isObstacle && !visited[i]) {
            connected = false;
            break;
        }
    }
    delete[] visited;
    delete[] q;
    return connected;
}

void Map::shootBullet(int fromRow, int fromCol, int targetRow, int targetCol, int tankIdx) {
    bulletPathSize = 0;
    showBulletPath = true;

    int dRow = targetRow - fromRow;
    int dCol = targetCol - fromCol;
    int stepR = (dRow == 0) ? 0 : (dRow > 0 ? 1 : -1);
    int stepC = (dCol == 0) ? 0 : (dCol > 0 ? 1 : -1);
    if (stepR == 0 && stepC == 0) return;

    auto esPared = [&](int r, int c) -> bool {
        if (r < 0 || r >= ROWS || c < 0 || c >= COLS) return true;
        return grid[r][c].isObstacle;
        };

    // Ahora esTanque NO excluye el origen — cualquier tanque cuenta
    // excepto en el primer paso (cuando cr==fromRow && cc==fromCol)
    auto esTanque = [&](int r, int c) -> int {
        for (int t = 0; t < 4; t++) {
            if (tanks[t].health <= 0) continue;
            if (tanks[t].row == r && tanks[t].col == c)
                return t;
        }
        return -1;
        };

    int cr = fromRow, cc = fromCol;
    bool rebotado = false;
    bool primerPaso = true;

    while (true) {
        if (bulletPathSize < ROWS * COLS * 4)
            bulletPath[bulletPathSize++] = &grid[cr][cc];

        // Revisar tanque en celda actual, ignorar solo en el primer paso
        if (!primerPaso) {
            int t = esTanque(cr, cc);
            if (t != -1) {
                int dmg = (t == 0 || t == 2) ? 25 : 50;
                tanks[t].health -= dmg;
                if (tanks[t].health < 0) tanks[t].health = 0;
                return;
            }
        }
        primerPaso = false;

        int nextR = cr + stepR;
        int nextC = cc + stepC;

        auto bloqueada = [&](int r, int c) -> bool {
            if (esPared(r, c)) return true;
            return esTanque(r, c) != -1;
            };

        bool bloqR = bloqueada(nextR, cc);
        bool bloqC = bloqueada(cr, nextC);
        bool bloqDiag = bloqueada(nextR, nextC);

        if (bloqDiag) {
            // Verificar si el bloqueo es un tanque para hacerle daño
            int t = esTanque(nextR, nextC);
            if (t == -1 && bloqR) t = esTanque(nextR, cc);
            if (t == -1 && bloqC) t = esTanque(cr, nextC);
            if (t != -1) {
                int dmg = (t == 0 || t == 2) ? 25 : 50;
                tanks[t].health -= dmg;
                if (tanks[t].health < 0) tanks[t].health = 0;
                return;
            }

            if (rebotado) return;
            rebotado = true;

            if (bloqR && bloqC) { stepR = -stepR; stepC = -stepC; }
            else if (bloqR) { stepR = -stepR; }
            else if (bloqC) { stepC = -stepC; }
            else { stepR = -stepR; stepC = -stepC; }
            continue;
        }

        cr = nextR;
        cc = nextC;
    }
}

void Map::updateMovement() {
    static float timer = 0;
    timer += GetFrameTime();
    if (!moving || timer < 0.1f) return;
    timer = 0;

    if (pathSize > 1) {
        Cell* current = path[0];
        Cell* next = path[1];

        if (selectedTank < 2) {
            current->hasTank0 = false;
            next->hasTank0 = true;
        }
        else {
            current->hasTank1 = false;
            next->hasTank1 = true;
        }
        tanks[selectedTank].row = next->row;
        tanks[selectedTank].col = next->col;

        for (int i = 0; i < pathSize - 1; i++)
            path[i] = path[i + 1];
        pathSize--;

        buildAdjacency(); // reconstruir con nueva posicion del tanque
    }
    else {
        moving = false;
        pathSize = 0;
        selectedTank = -1;
        actionMode = 0;
        buildAdjacency(); // reconstruir al terminar
    }
}

void Map::drawHUD() {
    int barY = ROWS * CELL_SIZE;
    DrawRectangle(0, barY, COLS * CELL_SIZE, BAR_HEIGHT, BLACK);

    const char* turnText = (player == 0) ? "Turno: Jugador 1" : "Turno: Jugador 2";
    DrawText(turnText, 10, barY + 8, 16, WHITE);

    const char* names[] = { "Azul", "Rojo", "Celeste", "Amarillo" };
    for (int t = 0; t < 4; t++) {
        char buf[32];
        if (tanks[t].health <= 0)
            snprintf(buf, sizeof(buf), "%s: DESTRUIDO", names[t]);
        else
            snprintf(buf, sizeof(buf), "%s: %d%%", names[t], tanks[t].health);
        DrawText(buf, 10 + t * 220, barY + 30, 14, tanks[t].color);
    }
}

void Map::drawActionButtons() {
    if (!isSelected || selectedTank == -1) return;

    int barY = ROWS * CELL_SIZE;
    int btnW = 120, btnH = 36;
    int startX = COLS * CELL_SIZE - 3 * (btnW + 10) - 10;
    int btnY = barY + 12;

    const char* labels[] = { "Mover", "Disparar", "Power-up" };
    Color colors[] = { GREEN, RED, PURPLE };

    for (int i = 0; i < 3; i++) {
        int x = startX + i * (btnW + 10);
        Color btnColor = (actionMode == i + 1) ? YELLOW : colors[i];
        DrawRectangle(x, btnY, btnW, btnH, btnColor);
        DrawText(labels[i], x + 20, btnY + 10, 16, WHITE);
    }
}

void Map::draw() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            Color color = grid[r][c].isObstacle ? DARKGRAY : BEIGE;
            DrawRectangle(c * CELL_SIZE, r * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1, color);
        }
    }

    // Trazo de movimiento
    for (int i = 0; i < pathSize; i++) {
        DrawRectangle(
            path[i]->col * CELL_SIZE + CELL_SIZE / 4,
            path[i]->row * CELL_SIZE + CELL_SIZE / 4,
            CELL_SIZE / 2, CELL_SIZE / 2,
            Fade(GREEN, 0.5f)
        );
    }

    // Trazo de bala
    if (showBulletPath) {
        for (int i = 0; i < bulletPathSize; i++) {
            DrawRectangle(
                bulletPath[i]->col * CELL_SIZE + CELL_SIZE / 3,
                bulletPath[i]->row * CELL_SIZE + CELL_SIZE / 3,
                CELL_SIZE / 3, CELL_SIZE / 3,
                Fade(ORANGE, 0.8f)
            );
        }
    }

    // Tanques
    for (int t = 0; t < 4; t++) {
        if (tanks[t].health <= 0) continue;
        DrawRectangle(
            tanks[t].col * CELL_SIZE + 4,
            tanks[t].row * CELL_SIZE + 4,
            CELL_SIZE - 8, CELL_SIZE - 8,
            tanks[t].color
        );
        // Mostrar vida sobre el tanque
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", tanks[t].health);
        DrawText(buf,
            tanks[t].col * CELL_SIZE + 8,
            tanks[t].row * CELL_SIZE + 8,
            14, WHITE);
    }

    // Highlight seleccion
    if (highlightRow >= 0 && highlightCol >= 0) {
        Rectangle rect = {
            (float)(highlightCol * CELL_SIZE),
            (float)(highlightRow * CELL_SIZE),
            (float)(CELL_SIZE - 1),
            (float)(CELL_SIZE - 1)
        };
        DrawRectangleLinesEx(rect, 3, GREEN);
    }

    drawHUD();
    drawActionButtons();
}

void Map::buildAdjacencyForTank(int tankIdx) {
    int total = ROWS * COLS;
    for (int i = 0; i < total; i++)
        for (int j = 0; j < total; j++)
            adjacency[i][j] = false;

    int dr[] = { -1, 1, 0, 0 };
    int dc[] = { 0, 0, -1, 1 };

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (grid[r][c].isObstacle) continue;
            // Saltar otros tanques, pero NO el tanque que se mueve
            bool esTanqueMoviendo = (tanks[tankIdx].row == r && tanks[tankIdx].col == c);
            if (!esTanqueMoviendo && (grid[r][c].hasTank0 || grid[r][c].hasTank1)) continue;

            for (int d = 0; d < 4; d++) {
                int nr = r + dr[d];
                int nc = c + dc[d];
                if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS) {
                    if (grid[nr][nc].isObstacle) continue;
                    // La celda destino no puede tener otro tanque
                    bool esDestinoTanqueMoviendo = (tanks[tankIdx].row == nr && tanks[tankIdx].col == nc);
                    if (!esDestinoTanqueMoviendo && (grid[nr][nc].hasTank0 || grid[nr][nc].hasTank1)) continue;
                    adjacency[toIndex(r, c)][toIndex(nr, nc)] = true;
                }
            }
        }
    }
}

bool Map::change() {
    return false;
}