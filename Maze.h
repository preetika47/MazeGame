// Maze.h
#ifndef GAME_ENTITY_H // Start of include guard
#define GAME_ENTITY_H

// A simple base class for game objects with position and symbol
class GameEntity {
protected: // Protected allows derived classes (Player, Enemy) to access directly
    int x, y;
    char symbol;

public:
    // Constructor to initialize position and symbol
    GameEntity(int startX, int startY, char sym) : x(startX), y(startY), symbol(sym) {}

    // Virtual destructor is good practice for base classes, though not strictly needed here if no complex cleanup
    virtual ~GameEntity() = default; // Use default destructor

    // Method to update the entity's position
    // Note: We make x and y accessible directly in MazeGame for simplicity in the provided solution,
    // but stricter encapsulation would use setters or friend classes.
    void move(int dx, int dy) {
        x += dx;
        y += dy;
    }

    // Getter methods for position and symbol
    int getX() const { return x; } // Mark getters as const
    int getY() const { return y; } // Mark getters as const
    char getSymbol() const { return symbol; } // Mark getters as const

    // Friend declaration (optional): Allows MazeGame direct access to protected/private members if needed
    // friend class MazeGame; // Uncomment this if MazeGame needs to directly modify x, y of Player/Enemy

    // Make MazeGame::resetLevel and MazeGame::loadGame friends to allow direct modification of x, y
    friend class MazeGame; // Grant friendship to the entire class for simplicity here
};

// Player class, derived from GameEntity
class Player : public GameEntity {
public:
    // Constructor specifically for Player, setting symbol to 'P'
    Player(int startX, int startY) : GameEntity(startX, startY, 'P') {}
};

// Enemy class, derived from GameEntity
class Enemy : public GameEntity {
public:
    // Constructor specifically for Enemy, setting symbol to 'X'
    Enemy(int startX, int startY) : GameEntity(startX, startY, 'X') {}
};

#endif // GAME_ENTITY_H  // <--- ADD THIS LINE
