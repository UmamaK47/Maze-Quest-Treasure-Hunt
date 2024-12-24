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
  
//GAME LOOP STARTS HERE
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed || Keyboard::isKeyPressed(Keyboard::Escape)) {
                window.close();
            }
        }
    }

    return 0;
}
