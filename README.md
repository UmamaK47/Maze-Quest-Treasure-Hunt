Maze Quest: Treasure Hunt

**Maze Quest** is a fun and challenging maze-navigation game where players solve randomized mazes to find a hidden treasure. Built in **C++** using the **SFML** library, this game features multiple difficulty levels, limited undo moves, and a timer to keep the gameplay exciting. Each game generates a unique maze, ensuring endless replayability.

**Features**

- **Randomized Mazes**: Each playthrough generates a unique maze using Prim's Algorithm.  
- **Three Difficulty Levels**:
  - Easy: 45 seconds, 20 undos.
  - Medium: 30 seconds, 10 undos.
  - Hard: 20 seconds, 5 undos.  
- **Player Movement**: Navigate the maze using arrow keys.  
- **Undo Functionality**: Backtrack moves, limited by the difficulty level.  
- **Countdown Timer**: Adds urgency and excitement to the game.  
- **Minimalist Design**: Black-and-white graphics with intuitive visuals.  
- **Sound Effects and Music**: Includes background music and event-driven sound effects.  

---

**Gameplay**

1. **Start**: Select a difficulty level from the main menu.  
2. **Navigate**: Use the arrow keys to find your way through the maze.  
3. **Win**: Reach the treasure before the timer runs out.  
4. **Lose**: If you run out of time or undo moves, it’s game over.  
5. **Replayability**: Randomized mazes ensure no two games are the same.

---

**Technical Details**

- **Language**: C++  
- **Library**: SFML (Simple and Fast Multimedia Library)  
- **Data Structures**: 
  - **Graphs**: Representing mazes for generation and validation.  
  - **Stacks**: For implementing undo functionality.  
- **Algorithm**: Randomized Prim's Algorithm for maze generation.  

---

**Prerequisites**:
   - Install SFML library (version 2.5 or later).  
   - A C++ compiler supporting C++11 or later.  

**Future Enhancements**

- Add a scoring system based on time and moves.  
- Include customizable maze sizes.  
- Introduce new themes and soundtracks.  

---

**Acknowledgments**

- **SFML**: For providing an excellent multimedia library.  
- **Online Tutorials**: For guidance on Prim's Algorithm and game state management.  
- **Asset Creators**: For free sprites, sounds, and fonts used in the game.  

Enjoy solving the maze and finding the treasure! 🎮✨
