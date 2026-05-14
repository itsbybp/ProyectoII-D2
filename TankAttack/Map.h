#pragma once
#include "Cell.h"
#include "Tank.h"
#include <cstdio>

class Map {
public:
    static const int ROWS = 15;
    static const int COLS = 19;
    static const int CELL_SIZE = 50;
    static const int BAR_HEIGHT = 70;
    static const int LEFT_PANEL = 200;
    static const int RIGHT_PANEL = 160;

	float timeLimit = 10.0f;
	float timeRemaining = 10.0f;

	bool gameOver = false;

    void buildAdjacency();
    void buildAdjacencyForTank(int tankIdx);

    int highlightRow = -1;
    int highlightCol = -1;
    bool isSelected = false;
    int selectedTank = -1;
    int player = 0;
    bool moving = false;
    int actionMode = 0; // 0=ninguno, 1=mover, 2=disparar, 3=powerup

	int tanksPlayer1 = 2;
	int tanksPlayer2 = 2;

    Cell* path[ROWS * COLS];
    int pathSize = 0;

    Cell* bulletPath[ROWS * COLS * 4];
    int bulletPathSize = 0;
    bool showBulletPath = false;

    bool** adjacency;
    Cell grid[ROWS][COLS];
    Tank tanks[4];

    bool showPowerUpMsg = false;
    float powerUpMsgTimer = 0.0f;
    char powerUpMsgText[64] = "";

    int extraTurns = 0;

    // Power-up "en uso" por tanque (-1 = ninguno)
    // Usamos PowerUpType como int, -1 = sin poder en uso
    int usingPowerUp[4] = { -1, -1, -1, -1 };

    // Flag para shootBullet
    bool usePowerAttack = false;
    bool usePrecisionAttack = false;

    Map();
    ~Map();
    void generate();
    void updateMovement();
    bool isObstacle(int row, int col) const;
    void draw();
    bool change();
    void shootBullet(int fromRow, int fromCol, int targetRow, int targetCol, int tankIdx);
    void shootBulletAStar(int fromRow, int fromCol, int targetRow, int targetCol, int tankIdx);
    void tryGivePowerUp();
    void applyPowerUp(int tankIdx);
    void endTurn();

    // Offset del mapa en pantalla
    int mapOffsetX() const { return LEFT_PANEL; }
    int mapOffsetY() const { return 0; }

private:
    int toIndex(int row, int col) const;
    bool isFullyConnected() const;
    void drawHUD();
    void drawActionButtons();
    void drawPowerUpPanel();
    void drawPowerUpNotification();
};