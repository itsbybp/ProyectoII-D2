#include "Map.h"
#include "raylib.h"
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cmath>

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
    for (int i = 0; i < 4; i++) powerUpEnUso[i] = -1;
    usarPoderAtaque = false;
    usarPrecisionAtaque = false;
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
            if (grid[r][c].hasTank0 || grid[r][c].hasTank1) continue;
            for (int d = 0; d < 4; d++) {
                int nr = r + dr[d], nc = c + dc[d];
                if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS)
                    if (!grid[nr][nc].isObstacle && !grid[nr][nc].hasTank0 && !grid[nr][nc].hasTank1)
                        adjacency[toIndex(r, c)][toIndex(nr, nc)] = true;
            }
        }
    }
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
            bool esMio = (tanks[tankIdx].row == r && tanks[tankIdx].col == c);
            if (!esMio && (grid[r][c].hasTank0 || grid[r][c].hasTank1)) continue;
            for (int d = 0; d < 4; d++) {
                int nr = r + dr[d], nc = c + dc[d];
                if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS) {
                    if (grid[nr][nc].isObstacle) continue;
                    bool esMioDest = (tanks[tankIdx].row == nr && tanks[tankIdx].col == nc);
                    if (!esMioDest && (grid[nr][nc].hasTank0 || grid[nr][nc].hasTank1)) continue;
                    adjacency[toIndex(r, c)][toIndex(nr, nc)] = true;
                }
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
        for (int j = 0; j < total; j++)
            if (adjacency[curr][j] && !visited[j]) { visited[j] = true; q[back++] = j; }
    }

    bool connected = true;
    for (int i = 0; i < total; i++) {
        int r = i / COLS, c = i % COLS;
        if (!grid[r][c].isObstacle && !visited[i]) { connected = false; break; }
    }
    delete[] visited; delete[] q;
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
    auto esTanque = [&](int r, int c) -> int {
        for (int t = 0; t < 4; t++) {
            if (tanks[t].health <= 0) continue;
            if (tanks[t].row == r && tanks[t].col == c) return t;
        }
        return -1;
        };

    int cr = fromRow, cc = fromCol;
    bool rebotado = false;
    bool primerPaso = true;

    while (true) {
        if (bulletPathSize < ROWS * COLS * 4)
            bulletPath[bulletPathSize++] = &grid[cr][cc];

        if (!primerPaso) {
            int t = esTanque(cr, cc);
            if (t != -1) {
                int dmg = usarPoderAtaque ? 100 : ((t == 0 || t == 2) ? 25 : 50);
                tanks[t].health -= dmg;
                if (tanks[t].health < 0) tanks[t].health = 0;
                usarPoderAtaque = false;
                return;
            }
        }
        primerPaso = false;

        int nextR = cr + stepR, nextC = cc + stepC;

        auto bloqueada = [&](int r, int c) -> bool {
            if (esPared(r, c)) return true;
            return esTanque(r, c) != -1;
            };

        bool bloqR = bloqueada(nextR, cc);
        bool bloqC = bloqueada(cr, nextC);
        bool bloqDiag = bloqueada(nextR, nextC);

        if (bloqDiag) {
            int t = esTanque(nextR, nextC);
            if (t == -1 && bloqR) t = esTanque(nextR, cc);
            if (t == -1 && bloqC) t = esTanque(cr, nextC);
            if (t != -1) {
                int dmg = usarPoderAtaque ? 100 : ((t == 0 || t == 2) ? 25 : 50);
                tanks[t].health -= dmg;
                if (tanks[t].health < 0) tanks[t].health = 0;
                usarPoderAtaque = false;
                return;
            }
            if (rebotado) { usarPoderAtaque = false; return; }
            rebotado = true;
            if (bloqR && bloqC) { stepR = -stepR; stepC = -stepC; }
            else if (bloqR) { stepR = -stepR; }
            else if (bloqC) { stepC = -stepC; }
            else { stepR = -stepR; stepC = -stepC; }
            continue;
        }
        cr = nextR; cc = nextC;
    }
}

void Map::shootBulletAStar(int fromRow, int fromCol, int targetRow, int targetCol, int tankIdx) {
    bulletPathSize = 0;
    showBulletPath = true;

    int total = ROWS * COLS;
    int startIdx = toIndex(fromRow, fromCol);
    int targetIdx = toIndex(targetRow, targetCol);

    // A* con heuristica Manhattan
    int* gCost = new int[total];
    int* hCost = new int[total];
    int* fCost = new int[total];
    int* parent = new int[total];
    bool* open = new bool[total];
    bool* closed = new bool[total];

    for (int i = 0; i < total; i++) {
        gCost[i] = 99999; hCost[i] = 0; fCost[i] = 99999;
        parent[i] = -1; open[i] = false; closed[i] = false;
    }

    gCost[startIdx] = 0;
    int tr = targetRow, tc = targetCol;
    auto heuristica = [&](int idx) -> int {
        int r = idx / COLS, c = idx % COLS;
        return abs(r - tr) + abs(c - tc);
        };
    hCost[startIdx] = heuristica(startIdx);
    fCost[startIdx] = hCost[startIdx];
    open[startIdx] = true;

    int dr[] = { -1,1,0,0 };
    int dc[] = { 0,0,-1,1 };

    while (true) {
        // Encontrar nodo con menor fCost en open
        int curr = -1, minF = 99999;
        for (int i = 0; i < total; i++)
            if (open[i] && fCost[i] < minF) { minF = fCost[i]; curr = i; }
        if (curr == -1 || curr == targetIdx) break;

        open[curr] = false;
        closed[curr] = true;

        int cr = curr / COLS, cc = curr % COLS;
        for (int d = 0; d < 4; d++) {
            int nr = cr + dr[d], nc = cc + dc[d];
            if (nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS) continue;
            if (grid[nr][nc].isObstacle) continue;
            int ni = toIndex(nr, nc);
            if (closed[ni]) continue;
            int ng = gCost[curr] + 1;
            if (ng < gCost[ni]) {
                gCost[ni] = ng;
                hCost[ni] = heuristica(ni);
                fCost[ni] = ng + hCost[ni];
                parent[ni] = curr;
                open[ni] = true;
            }
        }
    }

    // Reconstruir camino
    int* tmp = new int[total];
    int tmpSize = 0;
    int cur = targetIdx;
    while (cur != -1) { tmp[tmpSize++] = cur; cur = parent[cur]; }

    // El camino A* da celdas — la bala pasa por cada celda del camino
    // y aplica daño al llegar al objetivo
    for (int i = tmpSize - 1; i >= 0; i--) {
        int idx = tmp[i];
        int r = idx / COLS, c = idx % COLS;
        if (bulletPathSize < ROWS * COLS * 4)
            bulletPath[bulletPathSize++] = &grid[r][c];
    }

    // Aplicar daño al tanque en el objetivo si hay uno
    for (int t = 0; t < 4; t++) {
        if (tanks[t].health <= 0) continue;
        if (tanks[t].row == targetRow && tanks[t].col == targetCol) {
            int dmg = usarPoderAtaque ? 100 : ((t == 0 || t == 2) ? 25 : 50);
            tanks[t].health -= dmg;
            if (tanks[t].health < 0) tanks[t].health = 0;
            break;
        }
    }

    usarPoderAtaque = false;
    usarPrecisionAtaque = false;
    delete[] gCost; delete[] hCost; delete[] fCost;
    delete[] parent; delete[] open; delete[] closed; delete[] tmp;
}

void Map::tryGivePowerUp() {
    // Cambiar 30 por 100 para probar siempre
    if (rand() % 100 >= 30) return;

    // Solo tanques vivos del jugador actual
    int base = (player == 0) ? 0 : 2;
    // Filtrar tanques vivos
    int vivos[2], numVivos = 0;
    for (int i = base; i < base + 2; i++)
        if (tanks[i].health > 0) vivos[numVivos++] = i;
    if (numVivos == 0) return;

    int tankIdx = vivos[rand() % numVivos];

    PowerUpType types[] = {
        PowerUpType::DOBLE_TURNO,
        PowerUpType::PRECISION_MOVIMIENTO,
        PowerUpType::PRECISION_ATAQUE,
        PowerUpType::PODER_ATAQUE
    };
    PowerUpType chosen = types[rand() % 4];
    tanks[tankIdx].powerUps.enqueue(chosen);

    const char* names[] = { "Azul","Rojo","Celeste","Amarillo" };
    snprintf(powerUpMsgText, sizeof(powerUpMsgText),
        "Power-up para %s: %s",
        names[tankIdx],
        tanks[tankIdx].powerUps.typeName(chosen));
    showPowerUpMsg = true;
    powerUpMsgTimer = 3.0f;
}

void Map::applyPowerUp(int tankIdx) {
    if (tankIdx < 0 || tankIdx > 3) return;
    Tank& t = tanks[tankIdx];
    if (t.powerUps.empty()) return;
    if (tanks[tankIdx].health <= 0) return;

    // Verificar turno correcto
    if (tankIdx < 2 && player != 0) return;
    if (tankIdx >= 2 && player != 1) return;

    // No aplicar si ya hay uno en uso para este tanque
    if (powerUpEnUso[tankIdx] != -1) {
        snprintf(powerUpMsgText, sizeof(powerUpMsgText),
            "Ya hay un power-up en uso para este tanque!");
        showPowerUpMsg = true;
        powerUpMsgTimer = 2.0f;
        return;
    }

    PowerUpType pu = t.powerUps.dequeue();
    powerUpEnUso[tankIdx] = (int)pu;

    switch (pu) {
    case PowerUpType::DOBLE_TURNO:
        extraTurns += 2;
        powerUpEnUso[tankIdx] = -1; // se aplica inmediatamente
        snprintf(powerUpMsgText, sizeof(powerUpMsgText), "Doble turno activado!");
        break;
    case PowerUpType::PRECISION_MOVIMIENTO:
        t.precisionMovActiva = true;
        snprintf(powerUpMsgText, sizeof(powerUpMsgText), "Precision de movimiento lista!");
        break;
    case PowerUpType::PRECISION_ATAQUE:
        t.precisionAtqActiva = true;
        snprintf(powerUpMsgText, sizeof(powerUpMsgText), "Precision de ataque lista!");
        break;
    case PowerUpType::PODER_ATAQUE:
        t.poderAtqActivo = true;
        snprintf(powerUpMsgText, sizeof(powerUpMsgText), "Poder de ataque listo!");
        break;
    }

    showPowerUpMsg = true;
    powerUpMsgTimer = 2.5f;

    // Consumir turno
    endTurn();
}

void Map::endTurn() {
    if (extraTurns > 0) {
        extraTurns--;
    }
    else {
        player = (player == 0) ? 1 : 0;
    }
    isSelected = false;
    highlightRow = -1; highlightCol = -1;
    actionMode = 0;
    tryGivePowerUp();
}

void Map::updateMovement() {
    static float timer = 0;
    timer += GetFrameTime();
    if (!moving || timer < 0.1f) return;
    timer = 0;

    if (pathSize > 1) {
        Cell* current = path[0];
        Cell* next = path[1];
        if (selectedTank < 2) { current->hasTank0 = false; next->hasTank0 = true; }
        else { current->hasTank1 = false; next->hasTank1 = true; }
        tanks[selectedTank].row = next->row;
        tanks[selectedTank].col = next->col;
        for (int i = 0; i < pathSize - 1; i++) path[i] = path[i + 1];
        pathSize--;
        buildAdjacency();
    }
    else {
        moving = false;
        pathSize = 0;
        selectedTank = -1;
        actionMode = 0;
        buildAdjacency();
    }
}

void Map::drawHUD() {
    int mapW = COLS * CELL_SIZE;
    int mapH = ROWS * CELL_SIZE;
    int barY = mapH;
    int totalW = LEFT_PANEL + mapW + RIGHT_PANEL;

    DrawRectangle(0, barY, totalW, BAR_HEIGHT, BLACK);

    const char* turnText = (player == 0) ? "Turno: Jugador 1" : "Turno: Jugador 2";
    DrawText(turnText, LEFT_PANEL + 10, barY + 8, 16, WHITE);

    if (extraTurns > 0) {
        char buf[32];
        snprintf(buf, sizeof(buf), "(+%d turnos)", extraTurns);
        DrawText(buf, LEFT_PANEL + 200, barY + 8, 14, YELLOW);
    }

    const char* names[] = { "Azul","Rojo","Celeste","Amarillo" };
    int colW = mapW / 4;
    for (int t = 0; t < 4; t++) {
        char buf[32];
        if (tanks[t].health <= 0)
            snprintf(buf, sizeof(buf), "%s: DEST.", names[t]);
        else
            snprintf(buf, sizeof(buf), "%s: %d%%", names[t], tanks[t].health);
        DrawText(buf, LEFT_PANEL + t * colW + 5, barY + 32, 14, tanks[t].color);
    }
}

void Map::drawActionButtons() {
    int mapH = ROWS * CELL_SIZE;
    int mapW = COLS * CELL_SIZE;
    int panelX = LEFT_PANEL + mapW;

    DrawRectangle(panelX, 0, RIGHT_PANEL, mapH + BAR_HEIGHT, Color{ 30,30,30,255 });
    DrawText("ACCIONES", panelX + 10, 10, 14, WHITE);

    if (!isSelected || selectedTank == -1) {
        DrawText("Selecciona", panelX + 10, 40, 12, GRAY);
        DrawText("un tanque", panelX + 10, 55, 12, GRAY);
        return;
    }

    int btnW = RIGHT_PANEL - 20, btnH = 40;
    int btnX = panelX + 10;
    const char* labels[] = { "Mover","Disparar","Power-up" };
    Color colors[] = { GREEN, RED, PURPLE };

    for (int i = 0; i < 3; i++) {
        int btnY = 40 + i * (btnH + 10);
        Color c = (actionMode == i + 1) ? YELLOW : colors[i];
        DrawRectangle(btnX, btnY, btnW, btnH, c);
        DrawText(labels[i], btnX + 10, btnY + 12, 16, WHITE);
    }
}

void Map::drawPowerUpPanel() {
    int mapH = ROWS * CELL_SIZE;
    DrawRectangle(0, 0, LEFT_PANEL, mapH, Color{ 20,20,20,255 });

    if (!isSelected || selectedTank == -1) {
        // Sin seleccion: nombre del juego
        DrawText("TANK", 20, mapH / 2 - 40, 32, GOLD);
        DrawText("ATTACK!", 10, mapH / 2, 32, GOLD);
        return;
    }

    const char* names[] = { "Azul","Rojo","Celeste","Amarillo" };
    Tank& t = tanks[selectedTank];

    char header[32];
    snprintf(header, sizeof(header), "Tanque %s", names[selectedTank]);
    DrawText("Power-ups:", 10, 10, 15, WHITE);
    DrawText(header, 10, 28, 13, t.color);

    int y = 55;

    // En uso
    DrawText("En uso:", 10, y, 13, LIGHTGRAY);
    y += 17;
    if (powerUpEnUso[selectedTank] != -1) {
        PowerUpType pu = (PowerUpType)powerUpEnUso[selectedTank];
        DrawText(t.powerUps.typeName(pu), 10, y, 13, ORANGE);
    }
    else {
        DrawText("(ninguno)", 10, y, 13, GRAY);
    }
    y += 25;

    // Siguiente
    DrawText("Siguiente:", 10, y, 13, LIGHTGRAY);
    y += 17;
    if (!t.powerUps.empty()) {
        DrawText(t.powerUps.typeName(t.powerUps.peek()), 10, y, 13, YELLOW);
    }
    else {
        DrawText("(ninguno)", 10, y, 13, GRAY);
    }
    y += 25;

    // Resto de la cola
    DrawText("En cola:", 10, y, 13, LIGHTGRAY);
    y += 17;
    if (!t.powerUps.empty()) {
        PowerUp* curr = t.powerUps.front->next; // saltar el primero (ya es "siguiente")
        int idx = 2;
        while (curr && y < ROWS * CELL_SIZE - 40) {
            char buf[32];
            snprintf(buf, sizeof(buf), "%d. %s", idx, t.powerUps.typeName(curr->type));
            DrawText(buf, 10, y, 12, LIGHTGRAY);
            y += 16;
            curr = curr->next;
            idx++;
        }
    }
    else {
        DrawText("(vacia)", 10, y, 12, GRAY);
    }

    DrawText("Shift: usar PU", 10, ROWS * CELL_SIZE - 25, 12, DARKGREEN);
}

void Map::drawPowerUpNotification() {
    if (!showPowerUpMsg) return;
    powerUpMsgTimer -= GetFrameTime();
    if (powerUpMsgTimer <= 0) { showPowerUpMsg = false; return; }

    int w = 420, h = 50;
    int x = LEFT_PANEL + (COLS * CELL_SIZE - w) / 2;
    int y = ROWS * CELL_SIZE / 2 - 60;
    DrawRectangle(x, y, w, h, Fade(DARKBLUE, 0.92f));
    DrawRectangleLinesEx({ (float)x,(float)y,(float)w,(float)h }, 2, GOLD);
    DrawText(powerUpMsgText, x + 10, y + 15, 15, GOLD);
}

void Map::draw() {
    int ox = mapOffsetX();

    // Mapa
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            Color color = grid[r][c].isObstacle ? DARKGRAY : BEIGE;
            DrawRectangle(ox + c * CELL_SIZE, r * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1, color);
        }
    }

    // Trazo movimiento
    for (int i = 0; i < pathSize; i++)
        DrawRectangle(
            ox + path[i]->col * CELL_SIZE + CELL_SIZE / 4,
            path[i]->row * CELL_SIZE + CELL_SIZE / 4,
            CELL_SIZE / 2, CELL_SIZE / 2, Fade(GREEN, 0.5f));

    // Trazo bala
    if (showBulletPath)
        for (int i = 0; i < bulletPathSize; i++)
            DrawRectangle(
                ox + bulletPath[i]->col * CELL_SIZE + CELL_SIZE / 3,
                bulletPath[i]->row * CELL_SIZE + CELL_SIZE / 3,
                CELL_SIZE / 3, CELL_SIZE / 3, Fade(ORANGE, 0.8f));

    // Tanques
    for (int t = 0; t < 4; t++) {
        if (tanks[t].health <= 0) continue;
        DrawRectangle(
            ox + tanks[t].col * CELL_SIZE + 4,
            tanks[t].row * CELL_SIZE + 4,
            CELL_SIZE - 8, CELL_SIZE - 8, tanks[t].color);
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", tanks[t].health);
        DrawText(buf, ox + tanks[t].col * CELL_SIZE + 8, tanks[t].row * CELL_SIZE + 8, 14, WHITE);
    }

    // Highlight
    if (highlightRow >= 0 && highlightCol >= 0) {
        Rectangle rect = {
            (float)(ox + highlightCol * CELL_SIZE),
            (float)(highlightRow * CELL_SIZE),
            (float)(CELL_SIZE - 1), (float)(CELL_SIZE - 1)
        };
        DrawRectangleLinesEx(rect, 3, GREEN);
    }

    drawPowerUpPanel();
    drawActionButtons();
    drawPowerUpNotification();
    drawHUD();
}

bool Map::change() { return false; }