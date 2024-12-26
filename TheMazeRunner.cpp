#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <ctime>
#include <iostream>

using namespace sf;
using namespace std;
// Node structure for each cell in the maze
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
        if (!entranceTexture.loadFromFile("D:/DSA/maze/Graphics/idle/body/tile000.png")) {
            cerr << "Failed to load entrance texture" << endl;
        }
        if (!exitTexture.loadFromFile("D:/DSA/maze/Graphics/treasure.png")) {
            cerr << "Failed to load exit texture" << endl;
        }
        if (!visitedTexture.loadFromFile("D:/DSA/maze/Graphics/idle/body/tile005.png")) {
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

        window.display();
    }

    void drawLine(RenderWindow& window, int x1, int y1, int x2, int y2) {
        Vertex line[] = {
            Vertex(Vector2f(x1, y1), Color::White),
            Vertex(Vector2f(x2, y2), Color::White)
        };
        window.draw(line, 2, Lines);
    }

   
};

int main() {
    const int COLUMNS = 20;
    const int ROWS = 10;
    const int CELL_SIZE = 40;

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
    if (!playerTexture.loadFromFile("D:/DSA/TheMazeRunner/Graphics/idle/body/tile000.png")) {
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

    generating_maze_sound_buffer.loadFromFile("D:/DSA/TheMazeRunner/Sounds/cool-sound.wav");
    maze_sound.setBuffer(generating_maze_sound_buffer);
    game_start_buffer.loadFromFile("D:/DSA/TheMazeRunner/Sounds/game-start.wav");
    game_start_sound.setBuffer(game_start_buffer);
    treasure_collected_buffer.loadFromFile("D:/DSA/TheMazeRunner/Sounds/achievement.wav");
    treasure_collected_sound.setBuffer(treasure_collected_buffer);

    // Load font
    Font font;
    if (!font.loadFromFile("D:/DSA/TheMazeRunner/Font/ArcadeClassic.ttf")) {
        cerr << "Failed to load font" << endl;
        return -1;
    }

    Text welcomeText, playAgainText, gameStartText;

    // Create "Welcome" message

    welcomeText.setFont(font);
    welcomeText.setString("Welcome To The Maze Runner");
    welcomeText.setCharacterSize(50);
    welcomeText.setFillColor(Color::White);
    welcomeText.setPosition(vm.width / 2 - welcomeText.getLocalBounds().width / 2,
        vm.height / 2 - welcomeText.getLocalBounds().height / 2);

    // Create rectangle box for "Welcome" text
    RectangleShape welcomeBox(Vector2f(welcomeText.getLocalBounds().width + 40, welcomeText.getLocalBounds().height + 40));
    welcomeBox.setFillColor(Color::Transparent);
    welcomeBox.setOutlineColor(Color::Black);
    welcomeBox.setOutlineThickness(2);
    welcomeBox.setPosition(welcomeText.getPosition().x - 20, welcomeText.getPosition().y - 20);

    // Create "Game Starts" message

    gameStartText.setFont(font);
    gameStartText.setString("Game Starts!");
    gameStartText.setCharacterSize(50);
    gameStartText.setFillColor(Color::White);
    gameStartText.setPosition(vm.width / 2 - gameStartText.getLocalBounds().width/2,
      vm.height / 2 - gameStartText.getLocalBounds().height/2); 

    // Create rectangle box for "Game Starts" text
    RectangleShape gameStartBox(Vector2f(gameStartText.getLocalBounds().width + 40, gameStartText.getLocalBounds().height + 40));
    gameStartBox.setFillColor(Color::Transparent);
    gameStartBox.setOutlineColor(Color::Black);
    gameStartBox.setOutlineThickness(2);
    gameStartBox.setPosition(gameStartText.getPosition().x - 20, gameStartText.getPosition().y - 20);


	// Create "Play Again?" message
    playAgainText.setFont(font);
    playAgainText.setString("Want  To  Play  Press  Enter  Otherwise  Press  Esacpe  Key");
    playAgainText.setCharacterSize(40);
    playAgainText.setFillColor(Color::Magenta);
    playAgainText.setPosition(vm.width / 2 - playAgainText.getLocalBounds().width / 2,
        vm.height / 2 - playAgainText.getLocalBounds().height / 2);

    // Create a rectangle box around the "Play again?" text
    RectangleShape box(Vector2f(playAgainText.getLocalBounds().width + 40, playAgainText.getLocalBounds().height + 40));  // Increased padding
    box.setFillColor(Color::Transparent);
    box.setOutlineColor(Color::Black);
    box.setOutlineThickness(2);

    // Position the box with the adjusted padding
    box.setPosition(playAgainText.getPosition().x - 20, playAgainText.getPosition().y - 20);  // Adjusted position
    
    bool gameStarted = false;
    bool welcomeDisplayed = false;
    float welcomeStartTime = 0.0f; // Time to display the welcome message
    float gameStartTime = 0;  // Time the message will be displayed

    bool gameCompleted = false;
    maze_sound.play();
    game_start_sound.play();

    bool upPressed = false, downPressed = false, leftPressed = false, rightPressed = false;


    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed || Keyboard::isKeyPressed(Keyboard::Escape)) {
                window.close();
            }

            if (event.type == Event::KeyReleased) {
                if (event.key.code == Keyboard::Up) upPressed = false;
                if (event.key.code == Keyboard::Down) downPressed = false;
                if (event.key.code == Keyboard::Left) leftPressed = false;
                if (event.key.code == Keyboard::Right) rightPressed = false;
            }
        }
		//welcome message shown
        if (!welcomeDisplayed) {
            welcomeStartTime += 1.0f / 60.0f;
            if (welcomeStartTime >= 3.0f) {  // Display welcome message for 2 seconds
                welcomeDisplayed = true;
            }

            window.clear();
            window.draw(welcomeBox);
            window.draw(welcomeText);
            window.display();
            continue;
        }


        //game start message shown
        if (!gameStarted) {
            gameStartTime += 1.0f / 60.0f;
            if (gameStartTime >= 2.0f) {  // Display for 3 seconds
                gameStarted = true;
            }

            window.clear();
            window.draw(gameStartBox);
            window.draw(gameStartText);
            window.display();
            continue;
        }
        if (!gameCompleted) {
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

            if (playerPosition.x == COLUMNS - 1 && playerPosition.y == ROWS - 1) {
                treasure_collected_sound.play();
                gameCompleted = true;
            }

            graph.draw_maze(window, CELL_SIZE, offset, currentNode, frontierSize == 0);
            playerSprite.setPosition(
                offset.x + playerPosition.x * CELL_SIZE - (CELL_SIZE * (scaleFactor - 1)) / 2,
                offset.y + playerPosition.y * CELL_SIZE - (CELL_SIZE * (scaleFactor - 1)) / 2
            );
            window.draw(playerSprite);
        }
        else {
            window.clear(Color::Black);

            // Draw the rectangle box around the text
            window.draw(box);

            // Draw the "Play again?" text
            window.draw(playAgainText);

            if (Keyboard::isKeyPressed(Keyboard::Enter)) {
                graph.reset();
                frontierSize = 0;
                currentNode = graph.getNode(0, 0);
                currentNode->visited = true;
                graph.addNeighborsToFrontier(currentNode, frontier, frontierSize);
                playerPosition = Vector2i(0, 0);
                gameCompleted = false;
                maze_sound.play();
            }

            if (Keyboard::isKeyPressed(Keyboard::Escape)) {
                window.close();
            }
        }

        window.display();
    }

    return 0;
}
