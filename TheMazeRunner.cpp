#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>
#include <map>
#include <array>
#include <thread> // For sleep

using namespace sf;
using namespace std;

// Node structure for each cell in the maze
struct Node {
    int x, y;            // Coordinates
    bool visited;        // Visited flag
    bool walls[4];       // Top, Right, Bottom, Left walls

    Node(int x, int y) : x(x), y(y), visited(false) {
        walls[0] = walls[1] = walls[2] = walls[3] = true;  // All walls initially present
    }
};

class Graph {
private:
    map<pair<int, int>, Node*> nodes;  // Nodes mapped by coordinates
    int COLUMNS, ROWS;

public:
    // Constructor
    Graph(int cols, int rows) : COLUMNS(cols), ROWS(rows) {
        for (int x = 0; x < cols; ++x) {
            for (int y = 0; y < rows; ++y) {
                nodes[{x, y}] = new Node(x, y);
            }
        }
    }

    // Destructor
    ~Graph() {
        for (auto& pair : nodes) {
            delete pair.second;
        }
    }

    // Get node at coordinates
    Node* getNode(int x, int y) const {
        if (x >= 0 && x < COLUMNS && y >= 0 && y < ROWS) {
            return nodes.at({ x, y });
        }
        return nullptr;
    }

    // Remove walls between two cells
    void removeWalls(Node* current, Node* neighbor) {
        int dx = neighbor->x - current->x;
        int dy = neighbor->y - current->y;

        if (dx == 1) { // Neighbor is to the right
            current->walls[1] = false;
            neighbor->walls[3] = false;
        }
        else if (dx == -1) { // Neighbor is to the left
            current->walls[3] = false;
            neighbor->walls[1] = false;
        }
        else if (dy == 1) { // Neighbor is below
            current->walls[2] = false;
            neighbor->walls[0] = false;
        }
        else if (dy == -1) { // Neighbor is above
            current->walls[0] = false;
            neighbor->walls[2] = false;
        }
    }

    // Generate maze using Randomized Prim's Algorithm
    void generateMaze(RenderWindow& window, int cellSize, Vector2f offset) {
        srand(static_cast<unsigned int>(time(nullptr)));

        const int MAX_FRONTIER = 1000;
        Node* frontier[MAX_FRONTIER];
        int frontierSize = 0;

        // Start with the top-left corner
        Node* start = getNode(0, 0);
        start->visited = true;
        addNeighborsToFrontier(start, frontier, frontierSize);

        while (frontierSize > 0) {
            // Pick a random cell from the frontier
            int randIndex = rand() % frontierSize;
            Node* current = frontier[randIndex];
            frontier[randIndex] = frontier[--frontierSize];

            // Draw the current visiting node in red
            draw(window, cellSize, offset, current);

            // Find a random visited neighbor
            Node* visitedNeighbor = getRandomVisitedNeighbor(current);
            if (visitedNeighbor) {
                removeWalls(current, visitedNeighbor);
                current->visited = true;
                addNeighborsToFrontier(current, frontier, frontierSize);
            }

            // Slow down for animation
            this_thread::sleep_for(chrono::milliseconds(50));
        }

        // After maze generation is complete, redraw without red color
        draw(window, cellSize, offset, nullptr, true);
    }

    // Add unvisited neighbors to the frontier
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

    // Get a random visited neighbor
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

    // Draw the maze
    void draw(RenderWindow& window, int cellSize, Vector2f offset, Node* current = nullptr, bool generationCompleted = false) {
        window.clear(Color::Black); // Background color

        for (auto& pair : nodes) {
            Node* node = pair.second;

            // Draw the walls of the cell
            int x = offset.x + node->x * cellSize;
            int y = offset.y + node->y * cellSize;

            if (node->walls[0]) { // Top wall
                drawLine(window, x, y, x + cellSize, y);
            }
            if (node->walls[1]) { // Right wall
                drawLine(window, x + cellSize, y, x + cellSize, y + cellSize);
            }
            if (node->walls[2]) { // Bottom wall
                drawLine(window, x, y + cellSize, x + cellSize, y + cellSize);
            }
            if (node->walls[3]) { // Left wall
                drawLine(window, x, y, x, y + cellSize);
            }

            // Highlight the entrance in green
            if (node->x == 0 && node->y == 0) {
                RectangleShape cell(Vector2f(cellSize - 2, cellSize - 2));
                cell.setPosition(offset.x + node->x * cellSize + 1, offset.y + node->y * cellSize + 1);
                cell.setFillColor(Color::Green);
                window.draw(cell);
            }

            // Highlight the exit in blue
            if (node->x == COLUMNS - 1 && node->y == ROWS - 1) {
                RectangleShape cell(Vector2f(cellSize - 2, cellSize - 2));
                cell.setPosition(offset.x + node->x * cellSize + 1, offset.y + node->y * cellSize + 1);
                cell.setFillColor(Color::Blue);
                window.draw(cell);
            }

            // Only highlight the current node being visited in red if generation is ongoing
            if (node == current && !generationCompleted) {
                RectangleShape cell(Vector2f(cellSize - 2, cellSize - 2));
                cell.setPosition(offset.x + node->x * cellSize + 1, offset.y + node->y * cellSize + 1);
                cell.setFillColor(Color::Red);
                window.draw(cell);
            }
        }

        window.display();
    }

    // Helper to draw a line
    void drawLine(RenderWindow& window, int x1, int y1, int x2, int y2) {
        Vertex line[] = {
            Vertex(Vector2f(x1, y1), Color::White), // Wall color
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
    RenderWindow window(vm, "Randomized Prim's Maze Generator (Animated)");
    window.setFramerateLimit(60);

    Vector2f offset((vm.width - COLUMNS * CELL_SIZE) / 2.0f, (vm.height - ROWS * CELL_SIZE) / 2.0f);

    Graph graph(COLUMNS, ROWS);
    graph.generateMaze(window, CELL_SIZE, offset);

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
