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
  //DEFINING MAZE DIMENSIONS
    const int COLUMNS = 20;
    const int ROWS = 10;
    const int CELL_SIZE = 40;
//SETTING CONSOLE SCREEN
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
    
  //adding sound features 
     // Flags to track key press states
 bool upPressed = false, downPressed = false, leftPressed = false, rightPressed = false;

 SoundBuffer generating_maze_sound_buffer, game_start_buffer, treasure_collected_buffer;
 Sound maze_sound, game_start_sound, treasure_collected_sound;

 generating_maze_sound_buffer.loadFromFile("D:/DSA/TheMazeRunner/Sounds/cool-sound.wav");
 maze_sound.setBuffer(generating_maze_sound_buffer);
 game_start_buffer.loadFromFile("D:/DSA/TheMazeRunner/Sounds/game-start.wav");
 game_start_sound.setBuffer(game_start_buffer);
 treasure_collected_buffer.loadFromFile("D:/DSA/TheMazeRunner/Sounds/achievement.wav");
 treasure_collected_sound.setBuffer(treasure_collected_buffer);

    //palying sounds by calling it
      maze_sound.play();
  game_start_sound.play();
    
//GAME LOOP STARTS HERE
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed || Keyboard::isKeyPressed(Keyboard::Escape)) {
                window.close();
            }
            
        }
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
//drawing the maze on the console screen
graph.draw_maze(window, CELL_SIZE, offset, currentNode, frontierSize == 0);
    }
    

    return 0;
}
