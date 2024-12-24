#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <ctime>
#include <iostream>

using namespace sf;
using namespace std;

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
