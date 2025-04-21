// Maze.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <ctime>
#include <string>    // Explicitly include string
#include <windows.h> // For Sleep() and console handles
#include <conio.h>   // For _getch()

#include "Maze.h"    // Include the header defining Player and Enemy

using namespace std;

// Windows specific getch alias
#define GETCH() _getch()

// Renamed from SIZE to avoid potential conflicts (e.g., with windows.h macros)
const int MAZE_DIMENSION = 10;

class MazeGame {
private:
    // Use the renamed constant for the maze dimensions
    char maze[MAZE_DIMENSION][MAZE_DIMENSION];
    Player* player;
    vector<Enemy*> enemies;
    int score;
    int moves;
    int level;
    int collectibles;
    HANDLE hConsole; // Handle for console colors
    int defaultColor; // Store default console color

    // Function to clear the console screen (Windows specific)
    void clearScreen() {
        COORD coordScreen = { 0, 0 };
        DWORD cCharsWritten;
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        DWORD dwConSize;

        if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
        dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
        if (!FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten)) return;
        if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
        if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten)) return;
        SetConsoleCursorPosition(hConsole, coordScreen);
    }

     // Function to set text color
    void setColor(int colorCode) {
        SetConsoleTextAttribute(hConsole, colorCode);
    }

    void initializeMaze() {
        // Use MAZE_DIMENSION
        for (int i = 0; i < MAZE_DIMENSION; i++) {
            for (int j = 0; j < MAZE_DIMENSION; j++) {
                 // Use MAZE_DIMENSION - 1 for bounds
                 if (i == 0 || i == MAZE_DIMENSION - 1 || j == 0 || j == MAZE_DIMENSION - 1 || (i%2 != 0 && j%2 != 0))
                    maze[i][j] = '#';
                else
                    maze[i][j] = ' ';
            }
        }
        maze[1][1] = ' '; // Ensure start is clear
        // Use MAZE_DIMENSION - 2 for exit position
        maze[MAZE_DIMENSION - 2][MAZE_DIMENSION - 2] = 'E';
        addCollectibles();
    }

    void addCollectibles() {
        static mt19937 gen(static_cast<unsigned int>(time(0)));
        // Use MAZE_DIMENSION - 2 for distribution bounds
        uniform_int_distribution<> dis(1, MAZE_DIMENSION - 2);

        collectibles = level + 2;
        for (int i = 0; i < collectibles; i++) {
            int r, c;
            do {
                r = dis(gen);
                c = dis(gen);
             // Use MAZE_DIMENSION - 2 for exit check
            } while (maze[r][c] != ' ' || (r == 1 && c == 1) || (r == MAZE_DIMENSION - 2 && c == MAZE_DIMENSION - 2));
            maze[r][c] = '*';
        }
    }

public:
    MazeGame() : score(0), moves(0), level(1), collectibles(0), defaultColor(7) { // Initialize defaultColor
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
             defaultColor = csbi.wAttributes; // Get actual default color
        }

        initializeMaze(); // Now maze is correctly declared
        player = new Player(1, 1); // Uses Player from Maze.h

        // Ensure enemy start position is clear before placing
        int start_enemy_x = 5;
        int start_enemy_y = 5;
        // Basic bounds check for initial enemy placement
        if (start_enemy_x >= 0 && start_enemy_x < MAZE_DIMENSION && start_enemy_y >= 0 && start_enemy_y < MAZE_DIMENSION) {
             if (maze[start_enemy_x][start_enemy_y] == '#') {
                 maze[start_enemy_x][start_enemy_y] = ' '; // Clear if wall
             }
             enemies.push_back(new Enemy(start_enemy_x, start_enemy_y)); // Uses Enemy from Maze.h
        } else {
             // Handle error or place enemy at a default valid spot if 5,5 is out of bounds
             if (maze[1][2] == ' ') enemies.push_back(new Enemy(1,2));
             else enemies.push_back(new Enemy(2,1)); // Failsafe
        }


    }

    ~MazeGame() {
        delete player;
        for (auto enemy : enemies) {
            delete enemy;
        }
        enemies.clear();
    }

    // --- Public Getters Added ---
    int getScore() const { return score; }
    int getPlayerX() const { return player->getX(); }
    int getPlayerY() const { return player->getY(); }
    int getCollectiblesRemaining() const { return collectibles; }
    char getCell(int x, int y) const {
        // Added bounds checking for safety
        if (x >= 0 && x < MAZE_DIMENSION && y >= 0 && y < MAZE_DIMENSION) {
            return maze[x][y];
        }
        return '#'; // Return wall if out-of-bounds request
    }
    // --- End Getters ---


    void display() {
        clearScreen();

        const int COLOR_WALL = 13;    // Magenta / Purple
        const int COLOR_PLAYER = 14;   // Yellow
        const int COLOR_ENEMY = 12;    // Bright Red
        const int COLOR_ITEM = 11;   // Bright Cyan / Yellowish
        const int COLOR_EXIT = 10;    // Bright Green

        cout << "--- Maze Game --- Level: " << level << " ---\n";

        // Use MAZE_DIMENSION
        for (int i = 0; i < MAZE_DIMENSION; i++) {
            for (int j = 0; j < MAZE_DIMENSION; j++) {
                bool drawn = false;
                if (i == player->getX() && j == player->getY()) {
                    setColor(COLOR_PLAYER);
                    cout << player->getSymbol() << " ";
                    drawn = true;
                } else {
                    for (auto enemy : enemies) {
                        if (i == enemy->getX() && j == enemy->getY()) {
                            setColor(COLOR_ENEMY);
                            cout << enemy->getSymbol() << " ";
                            drawn = true;
                            break;
                        }
                    }
                }

                if (!drawn) {
                    char cell = maze[i][j]; // Safe now: maze is accessible
                    if (cell == '#') setColor(COLOR_WALL);
                    else if (cell == '*') setColor(COLOR_ITEM);
                    else if (cell == 'E') setColor(COLOR_EXIT);
                    else setColor(defaultColor); // Empty space
                    cout << cell << " ";
                }
                 setColor(defaultColor); // Reset color after each cell
            }
            cout << endl;
        }
         setColor(defaultColor); // Ensure color is reset after the loop
        cout << "Score: " << score << " | Moves: " << moves << " | Collectibles left: " << collectibles << endl;
    }

    bool movePlayer(char direction) {
        int dx = 0, dy = 0;
        switch (tolower(direction)) {
            case 'w': dx = -1; break;
            case 's': dx = 1; break;
            case 'a': dy = -1; break;
            case 'd': dy = 1; break;
            default: return false;
        }

        int newX = player->getX() + dx;
        int newY = player->getY() + dy;

        // Use MAZE_DIMENSION for bounds check
        if (newX >= 0 && newX < MAZE_DIMENSION && newY >= 0 && newY < MAZE_DIMENSION && maze[newX][newY] != '#') {
            if (maze[newX][newY] == '*') {
                score += 10;
                collectibles--;
                maze[newX][newY] = ' '; // Clear collectible
            }
            player->move(dx, dy);
            moves++;
            // It might make more sense to move enemies *before* checking game state,
            // but we'll keep original logic for now.
            moveEnemies();
            return true;
        }
        return false;
    }

    void moveEnemies() {
        // Seed slightly differently or just once if preferred
        static mt19937 gen(static_cast<unsigned int>(time(0)) + 1);
        uniform_int_distribution<> dis(-1, 1);

        for (auto enemy : enemies) {
            int dx = 0, dy = 0, newX = 0, newY = 0;
            int attempts = 0;
            const int maxAttempts = 10; // Prevent infinite loops if stuck
            do {
                dx = dis(gen);
                dy = dis(gen);
                // Allow diagonal moves if dx and dy are both non-zero, but prevent standing still
                if (dx == 0 && dy == 0) continue;

                newX = enemy->getX() + dx;
                newY = enemy->getY() + dy;
                attempts++;

             // Check bounds using MAZE_DIMENSION and ensure it's not a wall or exit
            } while ( !(newX >= 0 && newX < MAZE_DIMENSION && newY >= 0 && newY < MAZE_DIMENSION && maze[newX][newY] != '#' && maze[newX][newY] != 'E') &&
                     attempts < maxAttempts);

            // Only move if a valid move was found within attempts
            if (attempts < maxAttempts) {
                 // Check for collision with player *before* moving enemy onto player space? Optional.
                 // if (newX == player->getX() && newY == player->getY()) { /* Handle potential immediate collision */ }
                enemy->move(dx, dy);
            }
        }
    }

    bool checkGameState() {
        // Check for collision with player
        for (auto enemy : enemies) {
            if (player->getX() == enemy->getX() && player->getY() == enemy->getY()) {
                // Use Windows API for color instead of potentially unsupported ANSI codes here
                setColor(12); // Bright Red
                cout << "\n--- Ouch! Caught by an enemy! ---" << endl;
                setColor(defaultColor);
                return false; // Game over
            }
        }

        // Check for reaching the exit
        // Use MAZE_DIMENSION-2 to correctly identify exit location defined in initializeMaze
        if (player->getX() == MAZE_DIMENSION - 2 && player->getY() == MAZE_DIMENSION - 2) {
             if (collectibles == 0) {
                setColor(10); // Bright Green
                cout << "\n--- Level " << level << " Complete! --- Moving to next level..." << endl;
                setColor(defaultColor);
                level++;
                resetLevel();
                Sleep(1500); // Pause
                return true; // Continue playing (next level)
             } else {
                 // Player is at exit, but hasn't collected everything (handled in main loop display now)
                 return true; // Continue playing (still on current level)
             }
        }
        return true; // Continue playing (no game-ending event occurred)
    }

    void resetLevel() {
        initializeMaze(); // Re-generates walls, exit, and collectibles
        player->x = 1; // Reset player position (direct access okay within class)
        player->y = 1;

        // Reposition existing enemies, ensuring they aren't in walls
        int enemy_start_x = 5;
        int enemy_start_y = 5;
        for (size_t i = 0; i < enemies.size(); ++i) {
             // Simple repositioning logic, could be improved
             int new_ex = (enemy_start_x + i) % (MAZE_DIMENSION - 2) + 1; // Spread them out a bit
             int new_ey = (enemy_start_y + i) % (MAZE_DIMENSION - 2) + 1;

             // Ensure the spot is clear
             while (maze[new_ex][new_ey] != ' ' || (new_ex == 1 && new_ey == 1)) {
                 new_ey++;
                 if (new_ey >= MAZE_DIMENSION - 1) {
                     new_ey = 1;
                     new_ex++;
                     if (new_ex >= MAZE_DIMENSION - 1) {
                         new_ex = 1; // Wrap around
                     }
                 }
             }
             enemies[i]->x = new_ex; // Direct access okay within class
             enemies[i]->y = new_ey;
             maze[new_ex][new_ey] = ' '; // Make sure spot is marked traversable if we cleared a wall
        }

        // Add new enemies based on level
        if (level >= 3 && enemies.size() < (level / 2) + 1) {
             int new_enemy_x = 3, new_enemy_y = 3;
             // Find an empty spot for the new enemy
             while(maze[new_enemy_x][new_enemy_y] != ' ' || (new_enemy_x == 1 && new_enemy_y == 1)) {
                 new_enemy_y++;
                 if(new_enemy_y >= MAZE_DIMENSION - 1) { new_enemy_y = 1; new_enemy_x++; }
                 if(new_enemy_x >= MAZE_DIMENSION - 1) new_enemy_x = 1; // Wrap around search
             }
             enemies.push_back(new Enemy(new_enemy_x, new_enemy_y));
             maze[new_enemy_x][new_enemy_y] = ' '; // Ensure spot is traversable
        }
        // moves = 0; // Optional: Reset moves per level? Currently cumulative.
    }

    void saveGame(const string& filename) {
        ofstream file(filename);
        if (file.is_open()) {
            file << level << " " << score << " " << moves << " " << collectibles << endl;
            file << player->getX() << " " << player->getY() << endl; // Use getters for consistency if desired, though direct access is fine here
            file << enemies.size() << endl;
            for(const auto& enemy : enemies) {
                file << enemy->getX() << " " << enemy->getY() << endl;
            }
            // Use MAZE_DIMENSION
            for (int i = 0; i < MAZE_DIMENSION; i++) {
                for (int j = 0; j < MAZE_DIMENSION; j++)
                    file << maze[i][j];
                file << endl;
            }
            file.close();
            cout << "\n--- Game Saved to " << filename << " ---" << endl;
            Sleep(1000);
        } else {
            cerr << "Error: Could not open file " << filename << " for saving!" << endl;
            Sleep(1000);
        }
    }

    void loadGame(const string& filename) {
        ifstream file(filename);
        if (file.is_open()) {
            // Temporary variables for reading
            int loadedLevel, loadedScore, loadedMoves, loadedCollectibles;
            int px, py;
            size_t enemyCount;

            // Read stats
            file >> loadedLevel >> loadedScore >> loadedMoves >> loadedCollectibles;
            if (file.fail()) { cerr << "Error reading game stats from save file." << endl; file.close(); return; }

            // Read player position
            file >> px >> py;
            if (file.fail()) { cerr << "Error reading player position from save file." << endl; file.close(); return; }

            // Read enemy count
            file >> enemyCount;
            if (file.fail()) { cerr << "Error reading enemy count from save file." << endl; file.close(); return; }

            // --- If all reads up to this point succeed, apply changes ---
            level = loadedLevel;
            score = loadedScore;
            moves = loadedMoves;
            collectibles = loadedCollectibles;
            player->x = px; // Direct access okay within class
            player->y = py;

            // Clear old enemies and load new ones
            for (auto enemy : enemies) { delete enemy; }
            enemies.clear();
            for(size_t i = 0; i < enemyCount; ++i) {
                int ex, ey;
                file >> ex >> ey;
                if (file.fail()) {
                    cerr << "Error reading enemy " << i << " position from save file." << endl;
                    // Clear potentially partially loaded enemies to avoid issues
                    for(auto enemy : enemies) { delete enemy; }
                    enemies.clear();
                    file.close();
                    return; // Abort load
                }
                enemies.push_back(new Enemy(ex, ey));
            }

            // Read maze grid
            string line;
            getline(file, line); // Consume the rest of the line after the last enemy coords
            // Use MAZE_DIMENSION
            for (int i = 0; i < MAZE_DIMENSION; i++) {
                 if (!getline(file, line) || line.length() < MAZE_DIMENSION) { // Check line length
                     cerr << "Error reading maze grid line " << i << " or line too short from save file." << endl;
                     file.close();
                     // Potentially revert game state or handle error more gracefully
                     return; // Abort load
                 }
                 for (int j = 0; j < MAZE_DIMENSION; j++) {
                    maze[i][j] = line[j];
                 }
            }

            file.close();
            cout << "\n--- Game Loaded from " << filename << " ---" << endl;
            Sleep(1000);
        } else {
            cerr << "Error: Could not open file " << filename << " for loading!" << endl;
            Sleep(1000);
        }
    }
}; // End of MazeGame class

int main() {
    MazeGame game;
    char input;
    bool running = true;

    // Welcome Message (using Windows API for color consistency)
    HANDLE hConsoleMain = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbiMain;
    int defaultColorMain = 7;
    if (GetConsoleScreenBufferInfo(hConsoleMain, &csbiMain)) {
        defaultColorMain = csbiMain.wAttributes;
    }
    auto SetColorMain = [&](int color) { SetConsoleTextAttribute(hConsoleMain, color); };

    SetColorMain(11); // Bright Cyan
    cout << "**********************************************************************************" << endl;
    cout << "                           Welcome to the Maze Game!                              " << endl;
    cout << "**********************************************************************************" << endl;
    SetColorMain(defaultColorMain);
    cout << "                 Guide your player (P) through the maze.                          " << endl;
    SetColorMain(15); // Bright White
    cout << "----------------------------------------------------------------------------------" << endl;
    SetColorMain(defaultColorMain);
    cout << "    * Use W to move up, A to move left, S to move down, D to move right.          " << endl;
    cout << "    * Collect * for 10 points, reach 'E' after collecting all * to level up.      " << endl;
    cout << "    * Avoid 'X' enemies! Collision is Game Over.                                  " << endl;
    cout << "    * Save game with 'V', load game with 'L', quit with 'Q'.                      " << endl;
    SetColorMain(15); // Bright White
    cout << "----------------------------------------------------------------------------------" << endl;
    // Highlight start prompt
    SetConsoleTextAttribute(hConsoleMain, BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); // White text on Blue background
    cout << " Press any key to start... ";
    SetColorMain(defaultColorMain); // Reset color
    cout << endl;
    GETCH(); // Wait for key press

    while (running) {
        game.display(); // Game handles its own colors during display

        // Display reminder if at exit but items remain - USE GETTERS
        int currentPX = game.getPlayerX();
        int currentPY = game.getPlayerY();
        // Use getCell and getCollectiblesRemaining getters
        if(game.getCell(currentPX, currentPY) == 'E' && game.getCollectiblesRemaining() > 0) {
             SetColorMain(14); // Yellow
             cout << "\nCollect all '*' before exiting!" << endl;
             SetColorMain(defaultColorMain);
        }

        SetColorMain(11); // Bright Cyan for prompt
        cout << "\nMove (w/a/s/d), Save (v), Load (l), Quit (q): ";
        SetColorMain(defaultColorMain);

        input = GETCH();
        // cout << input << endl; // Optional: Echo input back

        switch (tolower(input)) {
            case 'w':
            case 'a':
            case 's':
            case 'd':
                if (game.movePlayer(input)) {
                   // Check game state *after* potentially moving and enemies moving
                   running = game.checkGameState();
                }
                break;
            case 'v':
                game.saveGame("maze_save.txt");
                // Keep running = true after save
                break;
            case 'l':
                game.loadGame("maze_save.txt");
                // Keep running = true after load attempt
                break;
            case 'q':
                running = false;
                cout << "\nQuitting game..." << endl;
                break;
            default:
                 // cout << "\nInvalid key!" << endl; // Optional feedback
                 // Sleep(50); // Optional small delay
                 break; // Do nothing for invalid input
        }
    }

    // Game Over message (using main's color setting)
    SetConsoleTextAttribute(hConsoleMain, BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); // White text on Blue background
    cout << "\n\nGame Over! Final Score: ";
    SetColorMain(14 | BACKGROUND_BLUE); // Yellow on Blue background
    cout << game.getScore(); // Use getter for score
    SetColorMain(defaultColorMain); // Reset to default
    cout << endl << endl;


    cout << "Press any key to exit." << endl;
    GETCH();

    return 0;
}
