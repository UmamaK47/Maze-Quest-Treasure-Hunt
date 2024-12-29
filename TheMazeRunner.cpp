#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <ctime>
#include <cstdlib>
#include <map>
#include <array>
#include <iostream>

using namespace sf;
using namespace std;

enum GameState { MAIN_MENU, HOW_TO_PLAY, START_GAME, EXIT };

struct Node {
    int x, y;
    bool visited;
    bool walls[4]; // Top, Right, Bottom, Left walls

    Node(int x, int y) : x(x), y(y), visited(false) {
        walls[0] = walls[1] = walls[2] = walls[3] = true;
    }
};

class Graph {
private:
    map<pair<int, int>, Node*> nodes;
    int COLUMNS, ROWS;
    Texture entranceTexture, exitTexture, visitedTexture;

public:
    Graph(int cols, int rows) : COLUMNS(cols), ROWS(rows) {
        for (int x = 0; x < cols; ++x) {
            for (int y = 0; y < rows; ++y) {
                nodes[{x, y}] = new Node(x, y);
            }
        }

        // Load textures
        if (!entranceTexture.loadFromFile("D:/TheMazeRunner/Graphics/idle/body/tile000.png")) {
            cerr << "Failed to load entrance texture" << endl;
        }
        if (!exitTexture.loadFromFile("D:/TheMazeRunner/Graphics/treasure.png")) {
            cerr << "Failed to load exit texture" << endl;
        }
        if (!visitedTexture.loadFromFile("D:/TheMazeRunner/Graphics/idle/body/tile005.png")) {
            cerr << "Failed to load visited texture" << endl;
        }
    }

    ~Graph() {
        for (auto& pair : nodes) {
            delete pair.second;
        }
    }

    Node* getNode(int x, int y) const {
        if (x >= 0 && x < COLUMNS && y >= 0 && y < ROWS) {
            return nodes.at({ x, y });
        }
        return nullptr;
    }

    void removeWalls(Node* current, Node* neighbor) {
        int dx = neighbor->x - current->x;
        int dy = neighbor->y - current->y;

        if (dx == 1) {
            current->walls[1] = false;
            neighbor->walls[3] = false;
        }
        else if (dx == -1) {
            current->walls[3] = false;
            neighbor->walls[1] = false;
        }
        else if (dy == 1) {
            current->walls[2] = false;
            neighbor->walls[0] = false;
        }
        else if (dy == -1) {
            current->walls[0] = false;
            neighbor->walls[2] = false;
        }
    }

    void addNeighborsToFrontier(Node* node, Node* frontier[], int& frontierSize) {
        static const array<pair<int, int>, 4> directions = { {{-1, 0}, {1, 0}, {0, -1}, {0, 1}} };

        for (auto& dir : directions) {
            int nx = node->x + dir.first;
            int ny = node->y + dir.second;
            Node* neighbor = getNode(nx, ny);

            if (neighbor && !neighbor->visited) {
                bool alreadyInFrontier = false;
                for (int i = 0; i < frontierSize; ++i) {
                    if (frontier[i] == neighbor) {
                        alreadyInFrontier = true;
                        break;
                    }
                }

                if (!alreadyInFrontier) {
                    frontier[frontierSize++] = neighbor;
                }
            }
        }
    }

    Node* getRandomVisitedNeighbor(Node* node) {
        static const array<pair<int, int>, 4> directions = { {{-1, 0}, {1, 0}, {0, -1}, {0, 1}} };
        Node* visitedNeighbors[4];
        int count = 0;

        for (auto& dir : directions) {
            int nx = node->x + dir.first;
            int ny = node->y + dir.second;
            Node* neighbor = getNode(nx, ny);

            if (neighbor && neighbor->visited) {
                visitedNeighbors[count++] = neighbor;
            }
        }

        if (count > 0) {
            return visitedNeighbors[rand() % count];
        }
        return nullptr;
    }

    void draw_maze(RenderWindow& window, int cellSize, Vector2f offset, Node* current = nullptr, bool generationCompleted = false) {
        window.clear(Color::Black);

        for (auto& pair : nodes) {
            Node* node = pair.second;

            int x = offset.x + node->x * cellSize;
            int y = offset.y + node->y * cellSize;

            if (node->walls[0]) drawLine(window, x, y, x + cellSize, y);
            if (node->walls[1]) drawLine(window, x + cellSize, y, x + cellSize, y + cellSize);
            if (node->walls[2]) drawLine(window, x, y + cellSize, x + cellSize, y + cellSize);
            if (node->walls[3]) drawLine(window, x, y, x, y + cellSize);

            if (node->x == 0 && node->y == 0) {
                Sprite sprite(entranceTexture);
                sprite.setPosition(x, y);
                sprite.setScale(cellSize / static_cast<float>(entranceTexture.getSize().x),
                    cellSize / static_cast<float>(entranceTexture.getSize().y));
                window.draw(sprite);
            }

            if (node->x == COLUMNS - 1 && node->y == ROWS - 1) {
                Sprite sprite(exitTexture);
                sprite.setPosition(x, y);
                sprite.setScale(cellSize / static_cast<float>(exitTexture.getSize().x),
                    cellSize / static_cast<float>(exitTexture.getSize().y));
                window.draw(sprite);
            }

            if (node == current && !generationCompleted) {
                Sprite sprite(visitedTexture);
                sprite.setPosition(x, y);
                sprite.setScale(cellSize / static_cast<float>(visitedTexture.getSize().x),
                    cellSize / static_cast<float>(visitedTexture.getSize().y));
                window.draw(sprite);
            }
        }
    }

    void drawLine(RenderWindow& window, int x1, int y1, int x2, int y2) {
        Vertex line[] = {
            Vertex(Vector2f(x1, y1), Color::White),
            Vertex(Vector2f(x2, y2), Color::White)
        };
        window.draw(line, 2, Lines);
    }

    void reset() {
        for (auto& pair : nodes) {
            Node* node = pair.second;
            node->visited = false;
            node->walls[0] = node->walls[1] = node->walls[2] = node->walls[3] = true;
        }
    }
};

int main() {
    const int COLUMNS = 20;
    const int ROWS = 10;
    const int CELL_SIZE = 40;

    GameState gameState = MAIN_MENU;

    VideoMode vm(1920, 1080);
    RenderWindow window(vm, "THE MAZE RUNNER");
    window.setFramerateLimit(60);

    Vector2f offset((vm.width - COLUMNS * CELL_SIZE) / 2.0f, (vm.height - ROWS * CELL_SIZE) / 2.0f);

    Graph graph(COLUMNS, ROWS);

    Node* frontier[1000];
    int frontierSize = 0;

    Node* currentNode = graph.getNode(0, 0);
    currentNode->visited = true;
    graph.addNeighborsToFrontier(currentNode, frontier, frontierSize);

    Vector2i playerPosition(0, 0);
    Texture playerTexture;
    if (!playerTexture.loadFromFile("D:/TheMazeRunner/Graphics/idle/body/tile000.png")) {
        cerr << "Failed to load player texture" << endl;
        return -1;
    }

    Sprite playerSprite(playerTexture);
    float scaleFactor = 2.5f;
    playerSprite.setScale(
        (CELL_SIZE * scaleFactor) / static_cast<float>(playerTexture.getSize().x),
        (CELL_SIZE * scaleFactor) / static_cast<float>(playerTexture.getSize().y)
    );

    SoundBuffer generating_maze_sound_buffer, game_start_buffer, treasure_collected_buffer;
    Sound maze_sound, game_start_sound, treasure_collected_sound;

    generating_maze_sound_buffer.loadFromFile("D:/TheMazeRunner/Sound/mazegenerating.wav");
    maze_sound.setBuffer(generating_maze_sound_buffer);
    game_start_buffer.loadFromFile("D:/TheMazeRunner/Sound/game-start.wav");
    game_start_sound.setBuffer(game_start_buffer);
    treasure_collected_buffer.loadFromFile("D:/TheMazeRunner/Sound/achievement.wav");
    treasure_collected_sound.setBuffer(treasure_collected_buffer);

    // Load font
    Font font;
    if (!font.loadFromFile("D:/TheMazeRunner/Font/ArcadeClassic.ttf")) {
        cerr << "Failed to load font" << endl;
        return -1;
    }

    Text welcomeText, playAgainText, gameStartText;

    bool gameStarted = false;
    bool welcomeDisplayed = false;
    float welcomeStartTime = 0.0f; // Time to display the welcome message
    float gameStartTime = 0;  // Time the message will be displayed

    bool gameCompleted = false;
    maze_sound.play();
    game_start_sound.play();

    bool upPressed = false, downPressed = false, leftPressed = false, rightPressed = false;

    // MAIN MENU
    const int MENU_ITEMS = 3;
    Text menu[MENU_ITEMS];
    string options[MENU_ITEMS] = { "Start Game", "How to Play", "Exit" };
    for (int i = 0; i < MENU_ITEMS; ++i) {
        menu[i].setFont(font);
        menu[i].setString(options[i]);
        menu[i].setCharacterSize(50);
        menu[i].setFillColor(Color::White);
        menu[i].setPosition(vm.width / 2 - menu[i].getLocalBounds().width / 2, 300 + i * 70);
    }
    int selectedItem = 0;
    menu[selectedItem].setFillColor(Color::Red);

    // "How to Play" setup
    Text howToPlayText;
    howToPlayText.setFont(font);
    howToPlayText.setString(
        "Navigate  through  the  maze  to   get  to  the  treasure \n\n"
        "Get  to  the  treasure  before  time  runs  out!!\n\n "
        "Use  arrow  keys  to  move\n\n\n"
        "Press  Escape  to  return  to  the  Main  Menu");
    howToPlayText.setCharacterSize(40);
    howToPlayText.setFillColor(Color::White);
    howToPlayText.setPosition(200, 200);

    //Time Bar
    RectangleShape timeBar;
    float timeBarStartWidth = 400;
    float timeBarHeight = 80;
    timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
    timeBar.setFillColor(Color::Red);
    timeBar.setOutlineThickness(5);
    timeBar.setOutlineColor(Color::Yellow);
    timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 9);

    bool timerStarted = false;
    Time gameTimeTotal;
    float timeRemaining = 30.0f;  // Start the game with 60 seconds
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

    

    Clock gameClock;  // SFML clock to track time

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed /* || Keyboard::isKeyPressed(Keyboard::Escape)*/) {
                window.close();
            }

            if (event.type == Event::KeyReleased) {
                if (event.key.code == Keyboard::Up) upPressed = false;
                if (event.key.code == Keyboard::Down) downPressed = false;
                if (event.key.code == Keyboard::Left) leftPressed = false;
                if (event.key.code == Keyboard::Right) rightPressed = false;
            }

            if (gameState == MAIN_MENU) {
                if (event.type == Event::KeyPressed) {
                    if (event.key.code == Keyboard::Up) {
                        menu[selectedItem].setFillColor(Color::White);
                        selectedItem = (selectedItem - 1 + MENU_ITEMS) % MENU_ITEMS;
                        menu[selectedItem].setFillColor(Color::Red);
                    }
                    else if (event.key.code == Keyboard::Down) {
                        menu[selectedItem].setFillColor(Color::White);
                        selectedItem = (selectedItem + 1) % MENU_ITEMS;
                        menu[selectedItem].setFillColor(Color::Red);
                    }
                    else if (event.key.code == Keyboard::Enter) {
                        if (selectedItem == 0)
                            gameState = START_GAME;
                        else if (selectedItem == 1)
                            gameState = HOW_TO_PLAY;
                        else if (selectedItem == 2)
                            window.close();
                    }
                }
            }
            else if (gameState == HOW_TO_PLAY) {
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
                    gameState = MAIN_MENU;
                }
            }
            else if (gameState == START_GAME) {
                // Placeholder for maze game logic
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
                    gameState = MAIN_MENU;
                }
            }
        }
        if (gameState == MAIN_MENU) {
            window.clear();
            for (int i = 0; i < MENU_ITEMS; ++i) {
                window.draw(menu[i]);
            }
        }
        else if (gameState == HOW_TO_PLAY) {
            window.clear();
            window.draw(howToPlayText);
        }
        else if (gameState == START_GAME) {
            window.clear();
            bool mazeGenerated = false;

            if (!gameCompleted) {
                // Maze generation logic
                if (frontierSize > 0) {
                    int randIndex = rand() % frontierSize;
                    currentNode = frontier[randIndex];
                    frontier[randIndex] = frontier[--frontierSize];

                    Node* visitedNeighbor = graph.getRandomVisitedNeighbor(currentNode);
                    if (visitedNeighbor) {
                        graph.removeWalls(currentNode, visitedNeighbor);
                        currentNode->visited = true;
                        graph.addNeighborsToFrontier(currentNode, frontier, frontierSize);
                    }
                }
                else {
                    mazeGenerated = true;
                    if (!timerStarted) {
                        timerStarted = true;  // Start the timer after maze is generated
                        gameClock.restart();
                    }
                }

                // Check if time has run out
                if (timeRemaining <= 0) {
                    gameCompleted = true;  // Mark game as completed
                    timeRemaining = 0;     // Clamp time to 0
                    treasure_collected_sound.stop(); // Ensure no treasure sound plays
                }

                // Movement logic
                if (Keyboard::isKeyPressed(Keyboard::Up) && !upPressed) {
                    Node* current = graph.getNode(playerPosition.x, playerPosition.y);
                    if (current && !current->walls[0]) {
                        playerPosition.y -= 1;
                    }
                    upPressed = true;
                }

                if (Keyboard::isKeyPressed(Keyboard::Down) && !downPressed) {
                    Node* current = graph.getNode(playerPosition.x, playerPosition.y);
                    if (current && !current->walls[2]) {
                        playerPosition.y += 1;
                    }
                    downPressed = true;
                }

                if (Keyboard::isKeyPressed(Keyboard::Left) && !leftPressed) {
                    Node* current = graph.getNode(playerPosition.x, playerPosition.y);
                    if (current && !current->walls[3]) {
                        playerPosition.x -= 1;
                    }
                    leftPressed = true;
                }

                if (Keyboard::isKeyPressed(Keyboard::Right) && !rightPressed) {
                    Node* current = graph.getNode(playerPosition.x, playerPosition.y);
                    if (current && !current->walls[1]) {
                        playerPosition.x += 1;
                    }
                    rightPressed = true;
                }

                // Check if player reaches the treasure
                if (playerPosition.x == COLUMNS - 1 && playerPosition.y == ROWS - 1) {
                    treasure_collected_sound.play();
                    gameCompleted = true; // Set game to completed when treasure is reached
                }

                // Drawing the maze and player sprite
                graph.draw_maze(window, CELL_SIZE, offset, currentNode, frontierSize == 0);
                playerSprite.setPosition(
                    offset.x + playerPosition.x * CELL_SIZE - (CELL_SIZE * (scaleFactor - 1)) / 2,
                    offset.y + playerPosition.y * CELL_SIZE - (CELL_SIZE * (scaleFactor - 1)) / 2
                );
                window.draw(playerSprite);
            }
           
            else {
                if (timeRemaining <= 0) {
                    Text outOfTimeText;
                    outOfTimeText.setFont(font);
                    outOfTimeText.setString("You Are Out of Time!!\nPress Enter to Play Again!\nPress ESC to go back to Main Menu");
                    outOfTimeText.setCharacterSize(50);
                    outOfTimeText.setFillColor(Color::Red);
                    outOfTimeText.setPosition(1920 / 2 - outOfTimeText.getLocalBounds().width / 2, 1080 / 2 - outOfTimeText.getLocalBounds().height / 2);
                    window.draw(outOfTimeText);
                }
                else {
                    Text winText;
                    winText.setFont(font);
                    winText.setString("Congratulations!!\nPress Enter to Play Again!\nPress ESC to go back to Main Menu");
                    winText.setCharacterSize(50);
                    winText.setFillColor(Color::White);
                    winText.setPosition(1920 / 2 - winText.getLocalBounds().width / 2, 1080 / 2 - winText.getLocalBounds().height / 2);
                    window.draw(winText);
                }

                // Key input to restart or go back to main menu
                if (Keyboard::isKeyPressed(Keyboard::Enter)) {
                    graph.reset();
                    frontierSize = 0;
                    currentNode = graph.getNode(0, 0);
                    currentNode->visited = true;
                    graph.addNeighborsToFrontier(currentNode, frontier, frontierSize);
                    playerPosition = Vector2i(0, 0);
                    gameCompleted = false;
                    timeRemaining = 30.0f; // Reset time
                    timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight)); // Reset the time bar to its original width
                    maze_sound.play();
                    timerStarted = false; // Reset the timer start flag
                }

                if (Keyboard::isKeyPressed(Keyboard::Escape)) {
                    gameState = MAIN_MENU;
                }
            }
            // Update time only if the timer has started
            if (mazeGenerated && timerStarted) {
                float deltaTime = gameClock.restart().asSeconds();
                timeRemaining -= deltaTime;

                if (timeRemaining < 0) timeRemaining = 0;  // Clamp time to zero

                timeBar.setSize(Vector2f(timeBarStartWidth * (timeRemaining / 30.0f), timeBarHeight)); // Update time bar
            }

            window.draw(timeBar);
}
        window.display();
    }

    return 0;
}



