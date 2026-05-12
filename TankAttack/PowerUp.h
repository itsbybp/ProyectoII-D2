#pragma once

enum class PowerUpType {
    DOBLE_TURNO,
    PRECISION_MOVIMIENTO,
    PRECISION_ATAQUE,
    PODER_ATAQUE
};

struct PowerUp {
    PowerUpType type;
    PowerUp* next;

    PowerUp(PowerUpType t) : type(t), next(nullptr) {}
};

// Cola FIFO manual
struct PowerUpQueue {
    PowerUp* front;
    PowerUp* back;
    int size;

    PowerUpQueue() : front(nullptr), back(nullptr), size(0) {}

    ~PowerUpQueue() {
        while (front) {
            PowerUp* tmp = front;
            front = front->next;
            delete tmp;
        }
    }

    void enqueue(PowerUpType t) {
        PowerUp* node = new PowerUp(t);
        if (!back) { front = back = node; }
        else { back->next = node; back = node; }
        size++;
    }

    PowerUpType dequeue() {
        PowerUp* tmp = front;
        PowerUpType t = tmp->type;
        front = front->next;
        if (!front) back = nullptr;
        delete tmp;
        size--;
        return t;
    }

    bool empty() const { return front == nullptr; }

    PowerUpType peek() const { return front->type; }

    const char* typeName(PowerUpType t) const {
        switch (t) {
        case PowerUpType::DOBLE_TURNO:          return "Doble Turno";
        case PowerUpType::PRECISION_MOVIMIENTO: return "Precision Mov";
        case PowerUpType::PRECISION_ATAQUE:     return "Precision Atq";
        case PowerUpType::PODER_ATAQUE:         return "Poder Ataque";
        default: return "?";
        }
    }
};