//#include<SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <fstream>
#include <cstring>

using namespace sf;
//using namespace std;


// Forward declarations
class BaseGame; //Have to use a bit main games will inherit
class GameBoy; //D
class Player; //it will be used
class User; //It will handle profiles changing name to USER FROM PROFILES
class SnakeGame;//D
class WordleGame;//D
class HangmanGame;//D
class GameManager; //it have no use
class Menu; //D
class InputHandler;//D
class Achievement;//Highest score
class Score;//All scores in it
class Word;//D
class HangmanFigure;//D
class Food;//D

// Constants
const int MAX_ACHIEVEMENTS = 10;
const int MAX_PROFILES = 5;
const int MAX_SNAKE_LENGTH = 100;
const int MAX_WORD_LENGTH = 20;

//Snake Game Variables
const int N = 40, M = 30; // Grid size
const int size = 14; // Size of each cell
const int w = size * N; // Window width
const int h = size * M; // Window height

//Wordle game variables
const int GRID_ROWS = 6, GRID_COLS = 5;
const int CELL_SIZE = 50;
const int WINDOW_WIDTH = 400, WINDOW_HEIGHT = 600;
const int DICTIONARY_SIZE = 6;

int pointss = 0;

//USERS-Profiles Constants
const int MAX_USERS = 5; // Maximum number of users
const std::string USER_DATA_FILE = "user_data.txt"; // File to store user credentials and scores

// Base Game Class
class BaseGame {
public:
    virtual void init() = 0; // Initialize the game
    virtual void update() = 0; // Update the game state
    virtual void render(sf::RenderWindow& window) = 0; // Render the game
    virtual void handleInput() = 0; // Handle user input
    virtual bool isGameOver() const = 0; // Check if the game is over
    virtual void reset() = 0; // Reset the game state
    virtual ~BaseGame() {}

protected:
    bool gameOver; // Indicates if the game is over
};

// Score Class
class Score {
public:
    Score() : score(0) {}
    void update(int points) { score += points; }
    int getScore() const { return score; }
private:
    int score;
};

// User Class
class User {
public:
    char username[50];
    char password[50];
    int highScore;
    int scoreCount;
    int scores[10];

    User() : highScore(0) {
        username[0] = '\0';
        password[0] = '\0';
    }

    void saveToFile(std::ofstream& outFile) {
        outFile << username << " " << password << " " << highScore << std::endl;
    }

    void loadFromFile(std::ifstream& inFile) {
        inFile >> username >> password >> highScore;
    }
};

// Helper function to convert integers to strings
std::string to_string(int num) {
    std::stringstream ss;
    ss << num;
    return ss.str();
}

// Represents a point in the grid
struct Point {
    int x, y;
};

// Represents a single segment of the snake
class SnakeSegment {
public:
    Point position;

    SnakeSegment(int x, int y) {
        position.x = x;
        position.y = y;
    }

    // Default constructor
    SnakeSegment() {
        position.x = -1; // Default invalid position
        position.y = -1; // Default invalid position
    }
};

// Represents the grid and other game components
class Grid {
public:
    int rows, cols, cellSize;
    Texture gridTexture, snakeTexture, fruitTexture;
    Sprite gridSprite, snakeSprite, fruitSprite;

    Grid(int rows, int cols, int cellSize)
        : rows(rows), cols(cols), cellSize(cellSize) {
        gridTexture.loadFromFile("white.png");
        snakeTexture.loadFromFile("purple.png");
        fruitTexture.loadFromFile("green.png");

        gridSprite.setTexture(gridTexture);
        snakeSprite.setTexture(snakeTexture);
        fruitSprite.setTexture(fruitTexture);
    }

    void drawGrid(RenderWindow& window) {
        for (int i = 0; i < cols; i++) {
            for (int j = 0; j < rows; j++) {
                gridSprite.setPosition(i * cellSize, j * cellSize);
                window.draw(gridSprite);
            }
        }
    }
};

// Manages the game logic and rendering
class SnakeGame {
private:
    Grid grid;
    SnakeSegment snake[100];  // Max snake size
    Point fruit;
    int direction, score, highestScore, snakeSize;
    float timer, delay;
    Font font; // Declare font
    Text scoreText, highestScoreText; // Declare score texts
    Text gameOverText; // Declare game over text
    bool isGameOver; // Track game over state
    Score s;

    void spawnFruit() {
        do {
            fruit.x = rand() % grid.cols;
            fruit.y = rand() % grid.rows;
        } while (isFruitOnSnake());
    }

    bool isFruitOnSnake() {
        for (int i = 0; i < snakeSize; i++) {
            if (snake[i].position.x == fruit.x && snake[i].position.y == fruit.y) {
                return true;
            }
        }
        return false;
    }

    void updateSnake() {
        if (snakeSize > 0) {
            // Move the snake segments in reverse order
            for (int i = snakeSize; i > 0; --i) {
                snake[i].position = snake[i - 1].position;
            }

            // Update head based on direction
            if (direction == 0) snake[0].position.y += 1; // Down
            if (direction == 1) snake[0].position.x -= 1; // Left
            if (direction == 2) snake[0].position.x += 1; // Right
            if (direction == 3) snake[0].position.y -= 1; // Up

            // Check fruit collision
            if (snake[0].position.x == fruit.x && snake[0].position.y == fruit.y) {
                snakeSize++;
                score++;
                if (score > highestScore) highestScore = score;
                if (score % 5 == 0 && score != 0) delay -= 0.01;
                spawnFruit();
            }

            // Wrap snake around screen
            if (snake[0].position.x >= grid.cols) snake[0].position.x = 0;
            if (snake[0].position.x < 0) snake[0].position.x = grid.cols - 1;
            if (snake[0].position.y >= grid.rows) snake[0].position.y = 0;
            if (snake[0].position.y < 0) snake[0].position.y = grid.rows - 1;

            // Check collision with itself
            for (int i = 1; i < snakeSize; i++) {
                if (snake[0].position.x == snake[i].position.x &&
                    snake[0].position.y == snake[i].position.y) {
                    gameOver();
                    break;
                }
            }
        }
    }
    void resetGame() {
        snakeSize = 2;
        snake[0] = SnakeSegment(10, 10);
        snake[1] = SnakeSegment(10, 11);
        score = 0;
        delay = 0.1;
        spawnFruit();
        isGameOver = false; // Reset game over state
    }

    void gameOver() {
        isGameOver = true; // Set game over state
        s.update(score);
        pointss += score;
        std::ofstream outFile("scores.txt", std::ios::app);
        if (outFile.is_open()) {
            outFile << "Player scored " << s.getScore() << " points in snake game!" << std::endl;
            outFile.close();
        }
        gameOverText.setString("Game Over! Press R to Restart");
        gameOverText.setPosition(grid.cols * grid.cellSize / 4, grid.rows * grid.cellSize / 2);
        gameOverText.setCharacterSize(30);
        gameOverText.setFillColor(Color::Red);
    }

    void updateScoreText() {
        scoreText.setString("Current Score: " + to_string(score));
        highestScoreText.setString("Highest Score: " + to_string(highestScore));

    }

public:
    SnakeGame(int rows, int cols, int cellSize)
        : grid(rows, cols, cellSize), direction(0), score(0), highestScore(0),
        snakeSize(2), timer(0), delay(0.1), isGameOver(false) {
        srand(time(0));
        spawnFruit();

        snake[0] = SnakeSegment(10, 10);
        snake[1] = SnakeSegment(10, 11);

        font.loadFromFile("ARIAL.ttf");
        scoreText.setFont(font);
        scoreText.setFillColor(Color::Black);
        scoreText.setPosition(10, 10);
        scoreText.setCharacterSize(18);

        highestScoreText.setFont(font);
        highestScoreText.setFillColor(Color::Black);
        highestScoreText.setPosition(400, 10);
        highestScoreText.setCharacterSize(18);

        gameOverText.setFont(font);
        gameOverText.setFillColor(Color::Red);
        gameOverText.setCharacterSize(30);
        gameOverText.setString(""); // Initially empty
    }

    void handleInput() {
        if (isGameOver) {
            // Restart the game if 'R' is pressed
            if (Keyboard::isKeyPressed(Keyboard::R)) {
                resetGame();
                gameOverText.setString(""); // Clear game over message
            }
            return; // Prevent further input processing
        }

        if (Keyboard::isKeyPressed(Keyboard::A) && direction != 2) direction = 1;
        if (Keyboard::isKeyPressed(Keyboard::D) && direction != 1) direction = 2;
        if (Keyboard::isKeyPressed(Keyboard::W) && direction != 0) direction = 3;
        if (Keyboard::isKeyPressed(Keyboard::S) && direction != 3) direction = 0;
    }

    void update(float timeElapsed) {
        if (!isGameOver) {
            timer += timeElapsed;
            if (timer > delay) {
                timer = 0;
                updateSnake();
            }
        }
        updateScoreText();
    }

    void draw(RenderWindow& window) {
        grid.drawGrid(window);

        for (int i = 0; i < snakeSize; i++) {
            grid.snakeSprite.setPosition(snake[i].position.x * grid.cellSize,
                snake[i].position.y * grid.cellSize);
            window.draw(grid.snakeSprite);
        }

        grid.fruitSprite.setPosition(fruit.x * grid.cellSize,
            fruit.y * grid.cellSize);
        window.draw(grid.fruitSprite);

        window.draw(scoreText);
        window.draw(highestScoreText);
        window.draw(gameOverText); // Draw game over text if applicable
    }

};

class WordDictionary {
private:
    const char* words[DICTIONARY_SIZE] = { "apple", "grape", "peach", "mango", "berry", "melon" };
public:
    const char* getRandomWord() {
        srand(static_cast<unsigned>(time(nullptr)));
        return words[rand() % DICTIONARY_SIZE];
    }
    bool isValidWord(const std::string& word) const {
        std::string lowerWord = toLowerCase(word);
        for (int i = 0; i < DICTIONARY_SIZE; i++) {
            if (lowerWord == words[i]) {
                return true;
            }
        }
        return false;
    }
    static std::string toLowerCase(const std::string& str) {
        std::string lowerStr = str;
        for (char& c : lowerStr) {
            c = std::tolower(c);
        }
        return lowerStr;
    }
};

class Letter {
public:
    char value;
    sf::Color color;

    Letter() : value(0), color(sf::Color::White) {}
};

class KeyboardClass {
private:
    sf::Color keyColors[26];
public:
    KeyboardClass() {
        resetColors();
    }
    void resetColors() {
        for (int i = 0; i < 26; i++) {
            keyColors[i] = sf::Color::White;
        }
    }
    void setKeyColor(char c, sf::Color color) {
        int index = c - 'A';
        if (index >= 0 && index < 26) {
            if (keyColors[index] != sf::Color::Green) { // Green is final
                keyColors[index] = color;
            }
            else if (keyColors[index] != sf::Color::Green && color == sf::Color::Green) {
                keyColors[index] = color;
            }
        }
    }
    sf::Color getKeyColor(char c) const {
        return keyColors[c - 'A'];
    }
    void draw(sf::RenderWindow& window, sf::Font& font) const {
        std::string keys = "QWERTYUIOPASDFGHJKLZXCVBNM";
        float x = 20, y = 450;
        int rowCounter = 0;

        for (int i = 0; i < 26; i++) {
            sf::RectangleShape key(sf::Vector2f(CELL_SIZE - 5, CELL_SIZE - 5));
            key.setPosition(x, y);
            key.setFillColor(keyColors[i]);
            window.draw(key);

            sf::Text letter(std::string(1, keys[i]), font, 20);
            letter.setPosition(x + 10, y + 10);
            letter.setFillColor(sf::Color::Black);
            window.draw(letter);

            x += CELL_SIZE;
            rowCounter++;
            if ((rowCounter == 10 && keys[i] == 'P') ||
                (rowCounter == 9 && keys[i] == 'L') ||
                (rowCounter == 7 && keys[i] == 'M')) {
                y += CELL_SIZE;
                x = 20;
                rowCounter = 0;
            }
        }
    }
};

class WordleGame {
private:
    Letter grid[GRID_ROWS][GRID_COLS];
    int attempts;
    bool gameOver;
    std::string targetWord;
    KeyboardClass keyboard;
    WordDictionary dictionary;
    Score s;

public:
    WordleGame() : attempts(0), gameOver(false) {
        resetGame();
    }

    void resetGame() {
        attempts = 0;
        gameOver = false;
        targetWord = dictionary.getRandomWord();
        keyboard.resetColors();
        for (int i = 0; i < GRID_ROWS; i++) {
            for (int j = 0; j < GRID_COLS; j++) {
                grid[i][j] = Letter();
            }
        }
    }

    void draw(sf::RenderWindow& window, sf::Font& font) {
        // Draw the grid
        for (int i = 0; i < GRID_ROWS; i++) {
            for (int j = 0; j < GRID_COLS; j++) {
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 5, CELL_SIZE - 5));
                cell.setPosition(j * CELL_SIZE + 50, i * CELL_SIZE + 50);
                cell.setFillColor(grid[i][j].color);
                window.draw(cell);

                if (grid[i][j].value != 0) {
                    sf::Text letter(std::string(1, grid[i][j].value), font, 20);
                    letter.setPosition(j * CELL_SIZE + 60, i * CELL_SIZE + 60);
                    letter.setFillColor(sf::Color::Black);
                    window.draw(letter);
                }
            }
        }
        keyboard.draw(window, font);
    }

    void handleGuess(const std::string& guess) {
        if (guess.size() != GRID_COLS || !dictionary.isValidWord(guess) || gameOver) {
            std::cout << "Invalid word!\n";
            return;
        }

        // Convert both guess and targetWord to uppercase for uniform comparison
        std::string normalizedTarget = dictionary.toLowerCase(targetWord);
        std::string normalizedGuess = dictionary.toLowerCase(guess);

        bool letterInWord[26] = { false };
        for (char c : normalizedTarget) {
            letterInWord[c - 'a'] = true;
        }

        for (int i = 0; i < GRID_COLS; i++) {
            char c = std::toupper(guess[i]);
            grid[attempts][i].value = c;

            if (normalizedTarget[i] == normalizedGuess[i]) {
                grid[attempts][i].color = sf::Color::Green;
                keyboard.setKeyColor(c, sf::Color::Green);
            }
            else if (letterInWord[std::tolower(c) - 'a']) {
                grid[attempts][i].color = sf::Color::Green;
                if (keyboard.getKeyColor(c) != sf::Color::Green) {
                    keyboard.setKeyColor(c, sf::Color::Green);
                }
            }
            else {
                grid[attempts][i].color = sf::Color::Green;
                if (keyboard.getKeyColor(c) == sf::Color::White) {
                    keyboard.setKeyColor(c, sf::Color::Green);
                }
            }
        }

        attempts++;
        if (dictionary.toLowerCase(guess) == normalizedTarget) {
            gameOver = true;
            std::cout << "You won!\n";
            pointss += 10;
            s.update(10);
            pointss += 10;
            std::ofstream outFile("scores.txt", std::ios::app);
            if (outFile.is_open()) {
                outFile << "Player scored " << 10 << " points in wordle game!" << std::endl;
                outFile.close();
            }
        }
        else if (attempts == GRID_ROWS) {
            gameOver = true;
            std::cout << "You lost! The word was " << targetWord << "\n";
            pointss += 0;
            s.update(0);
            pointss += 0;
            std::ofstream outFile("scores.txt", std::ios::app);
            if (outFile.is_open()) {
                outFile << "Player scored " << 0 << " points in wordle game! " << "LOST!!!" << std::endl;
                outFile.close();
            }
        }
    }

    bool isGameOver() const {
        return gameOver;
    }
};

class HangmanFigure {
public:
    void draw(sf::RenderWindow& window, int livesRemaining) {
        const int MAX_LIVES = 5;

        // Base structure
        sf::RectangleShape pole(sf::Vector2f(10, 200));
        pole.setFillColor(sf::Color::Black);
        pole.setPosition(50, 50);

        sf::RectangleShape beam(sf::Vector2f(100, 10));
        beam.setFillColor(sf::Color::Black);
        beam.setPosition(50, 50);

        sf::RectangleShape rope(sf::Vector2f(10, 50));
        rope.setFillColor(sf::Color::Black);
        rope.setPosition(140, 50);

        // Hangman parts
        sf::CircleShape head(20);
        head.setFillColor(sf::Color::Transparent);
        head.setOutlineColor(sf::Color::Black);
        head.setOutlineThickness(3);
        head.setPosition(125, 100);

        sf::RectangleShape body(sf::Vector2f(5, 70));
        body.setFillColor(sf::Color::Black);
        body.setPosition(142, 140);

        sf::RectangleShape leftArm(sf::Vector2f(40, 5));
        leftArm.setFillColor(sf::Color::Black);
        leftArm.setPosition(148, 160);
        leftArm.setRotation(45);

        sf::RectangleShape rightArm(sf::Vector2f(40, 5));
        rightArm.setFillColor(sf::Color::Black);
        rightArm.setPosition(144, 160);
        rightArm.setRotation(135);

        sf::RectangleShape leftLeg(sf::Vector2f(40, 5));
        leftLeg.setFillColor(sf::Color::Black);
        leftLeg.setPosition(148, 207);
        leftLeg.setRotation(45);

        sf::RectangleShape rightLeg(sf::Vector2f(40, 5));
        rightLeg.setFillColor(sf::Color::Black);
        rightLeg.setPosition(144, 207);
        rightLeg.setRotation(135);

        // Draw base structure
        window.draw(pole);
        window.draw(beam);
        window.draw(rope);

        // Draw parts based on remaining lives
        if(livesRemaining==5){}
        else if (livesRemaining == 4) window.draw(head);
        else if (livesRemaining == 3) { 
            window.draw(head);
            window.draw(body); 
        }
        else if (livesRemaining == 2) { 
            window.draw(head);
            window.draw(body);
            window.draw(leftArm); 
        }
        else if (livesRemaining == 1) { 
            window.draw(head);
            window.draw(body);
            window.draw(leftArm);
            window.draw(rightArm); 
        }
        else if (livesRemaining == 0) {
            window.draw(head);
            window.draw(body);
            window.draw(leftArm);
            window.draw(rightArm);
            window.draw(leftLeg);
            window.draw(rightLeg);
        }
        //if (livesRemaining <= MAX_LIVES - 5) return; // No parts drawn
        //if (livesRemaining > 0) window.draw(head);
        //if (livesRemaining > 1) window.draw(body);
        //if (livesRemaining > 2) window.draw(leftArm);
        //if (livesRemaining > 3) window.draw(rightArm);
        //if (livesRemaining > 4) {
        //    window.draw(leftLeg);
        //    window.draw(rightLeg);
        //}
    }
};

class Category {
private:
    std::string word;
    std::string word_obfuscated;

public:
    void loadWord(const std::string& newWord) {
        word = newWord;
        word_obfuscated = std::string(word.size(), '_');
    }

    std::string getWord() const {
        return word;
    }

    std::string getObfuscatedWord() const {
        return word_obfuscated;
    }

    void guess(char letter) {
        for (size_t i = 0; i < word.size(); ++i) {
            if (word[i] == letter) {
                word_obfuscated[i] = letter;
            }
        }
    }

    bool isWordGuessed() const {
        return word == word_obfuscated;
    }

    std::string getObfuscated() const {
        return word_obfuscated;
    }
};

class HangMan {
private:
    HangmanFigure figure;
    Category category;
    int lives;

public:
    HangMan() : lives(5) {}

    void reset(const std::string& newWord) {
        category.loadWord(newWord);
        lives = 5;
    }

    void guess(char letter) {
        if (category.getWord().find(letter) == std::string::npos) {
            lives--;
        }
        else {
            category.guess(letter);
        }
    }

    bool isGameOver() const {
        return lives <= 0;
    }

    bool isWin() const {
        return category.isWordGuessed();
    }

    int getLives() const {
        return lives;
    }

    std::string getObfuscatedWord() const {
        return category.getObfuscated();
    }

    void draw(sf::RenderWindow& window) {
        figure.draw(window, lives);
    }
};

// Game Manager Class
class GameManager {
public:
    void switchGame(BaseGame* game) {
        if (currentGame) {
            delete currentGame;
        }
        currentGame = game;
        currentGame->init();
    }

    void update() {
        if (currentGame) {
            currentGame->update();
        }
    }

    void render(sf::RenderWindow& window) {
        if (currentGame) {
            currentGame->render(window);
        }
    }

    BaseGame* getCurrentGame() const {
        return currentGame;
    }

private:
    BaseGame* currentGame = nullptr; // Pointer to the current game
};

// Function to create and return a button (rectangle with text)
sf::Text drawButton(sf::RenderWindow& window, sf::Font& font, const std::string& text, float x, float y, float width, float height) {
    // Button rectangle
    sf::RectangleShape button(sf::Vector2f(width, height));
    button.setFillColor(sf::Color(100, 100, 200));
    button.setPosition(x, y);
    window.draw(button);

    // Button text
    sf::Text buttonText;
    buttonText.setFont(font);
    buttonText.setString(text);
    buttonText.setCharacterSize(20);
    buttonText.setFillColor(sf::Color::White);

    // Center text within the button
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setPosition(
        x + (width - textBounds.width) / 2.f,
        y + (height - textBounds.height) / 2.f - 5.f
    );

    window.draw(buttonText);
    return buttonText;
}

class Menu {
private:
    bool showGameSelection = false; // Track whether to show game selection text
    User users[MAX_USERS]; // Array of users
    int userCount = 0; // Current number of users
    User currentUser; // Current logged-in user
    bool isLoggedIn = false; // Track if the user is logged in
    Score s;
    //sf::Music backgroundMusic; // Declare the music object

public:
    Menu() {
        loadUsers(); // Load users from file on initialization
        //if (!backgroundMusic.openFromFile("path/to/your/music.ogg")) { // Load your music file
        //    std::cerr << "Error loading music!" << std::endl;
        //}
        //backgroundMusic.setLoop(true); // Set the music to loop
        //backgroundMusic.play(); // Start playing the music
    }
    void display(sf::RenderWindow& window) {
        handleLoginSignup();
        window.setFramerateLimit(60);

        // Load the image
        sf::Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("main.png")) {
            std::cerr << "Error loading main.png" << std::endl;
            return;
        }
        sf::Sprite backgroundSprite(backgroundTexture);

        // Load font
        sf::Font font;
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Error loading font" << std::endl;
            return;
        }

        // Button parameters
        float buttonWidth = 200.f, buttonHeight = 50.f, spacing = 20.f;
        sf::Text buttons[4];
        std::string buttonTexts[] = { "Game", "Instructions", "Progress", "Exit" };

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                // Resizing the background to fit the screen
                if (event.type == sf::Event::Resized) {
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));
                    backgroundSprite.setScale(
                        static_cast<float>(event.size.width) / backgroundTexture.getSize().x,
                        static_cast<float>(event.size.height) / backgroundTexture.getSize().y
                    );
                }

                // Button interaction
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    for (int i = 0; i < 4; ++i) {
                        float x = (window.getSize().x - buttonWidth) / 2.f;
                        float y = 300 + i * (buttonHeight + spacing);
                        if (mousePos.x >= x && mousePos.x <= x + buttonWidth &&
                            mousePos.y >= y && mousePos.y <= y + buttonHeight) {
                            std::cout << buttonTexts[i] << " button clicked!" << std::endl;
                            if (buttonTexts[i] == "Game") {
                                showGameSelection = true; // Show game selection text
                                gameSelectionScreen(window, font); // Call the game selection screen
                            }
                            else if (buttonTexts[i] == "Instructions") {
                                showInstructions(window, font);
                            }
                            else if (buttonTexts[i] == "Progress") {
                                showProgress(window, font); // Show progress
                            }
                            else if (buttonTexts[i] == "Exit") {
                                window.close();
                            }
                        }
                    }
                }
            }

            // Render
            window.clear();
            window.draw(backgroundSprite);

            // Draw main menu buttons
            for (int i = 0; i < 4; ++i) {
                float x = (window.getSize().x - buttonWidth) / 2.f;
                float y = 300 + i * (buttonHeight + spacing);
                buttons[i] = drawButton(window, font, buttonTexts[i], x, y, buttonWidth, buttonHeight);
            }

            window.display();
        }
    }

    void gameSelectionScreen(sf::RenderWindow& window, sf::Font& font) {
        sf::Texture gamingTexture;
        if (!gamingTexture.loadFromFile("gaming.jpg")) {
            std::cerr << "Error loading gaming.png" << std::endl;
            return;
        }
        sf::Sprite gamingSprite(gamingTexture);

        // Scale the sprite to fit the window
        sf::Vector2u windowSize = window.getSize();
        gamingSprite.setScale(
            static_cast<float>(windowSize.x) / gamingTexture.getSize().x,
            static_cast<float>(windowSize.y) / gamingTexture.getSize().y
        );

        // Button parameters
        float buttonWidth = 200.f, buttonHeight = 50.f, spacing = 20.f;
        sf::Text buttons[4];
        std::string buttonTexts[] = { "Snake", "Wordle", "Hangman", "Back" };

        while (window.isOpen() && showGameSelection) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                // Button interaction
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    for (int i = 0; i < 4; ++i) {
                        float x = (window.getSize().x - buttonWidth) / 2.f;
                        float y = 300 + i * (buttonHeight + spacing);
                        if (mousePos.x >= x && mousePos.x <= x + buttonWidth &&
                            mousePos.y >= y && mousePos.y <= y + buttonHeight) {
                            std::cout << buttonTexts[i] << " button clicked!" << std::endl;
                            if (buttonTexts[i] == "Snake") {
                                startSnakeGame();
                            }
                            else if (buttonTexts[i] == "Wordle") {
                                startWordleGame();
                            }
                            else if (buttonTexts[i] == "Hangman") {
                                startHangmanGame();
                            }
                            else if (buttonTexts[i] == "Back") {
                                showGameSelection = false; // Go back to main menu
                            }
                        }
                    }
                }
            }

            // Render
            window.clear();
            window.draw(gamingSprite);

            // Draw game selection buttons
            for (int i = 0; i < 4; ++i) {
                float x = (window.getSize().x - buttonWidth) / 2.f;
                float y = 300 + i * (buttonHeight + spacing);
                buttons[i] = drawButton(window, font, buttonTexts[i], x, y, buttonWidth, buttonHeight);
            }

            window.display();
        }
    }

    void showInstructions(sf::RenderWindow& window, sf::Font& font) {
        sf::RenderWindow instructionsWindow(sf::VideoMode(800, 600), "Instructions");
        sf::Texture backgroundInstructionTexture;
        if (!backgroundInstructionTexture.loadFromFile("instructions.png")) {
            std::cerr << "Error loading instructions.png" << std::endl;
            return;
        }
        sf::Sprite backgroundInstructionSprite(backgroundInstructionTexture);

        // Load textures for game instructions
        sf::Texture snakeTexture, wordleTexture, hangmanTexture;
        if (!snakeTexture.loadFromFile("snakeInstructions.png") ||
            !wordleTexture.loadFromFile("wordleInstructions.png") ||
            !hangmanTexture.loadFromFile("hangmanInstructions.png")) {
            std::cerr << "Error loading game instruction images" << std::endl;
            return;
        }

        sf::Sprite gameInstructionSprite;
        bool showGameInstructions = false;
        std::string currentGame = "";

        // Create buttons for games with background color
        sf::RectangleShape snakeButtonBg(sf::Vector2f(200, 50));
        sf::RectangleShape wordleButtonBg(sf::Vector2f(200, 50));
        sf::RectangleShape hangmanButtonBg(sf::Vector2f(200, 50));

        snakeButtonBg.setFillColor(sf::Color::Green);
        wordleButtonBg.setFillColor(sf::Color::Blue);
        hangmanButtonBg.setFillColor(sf::Color::Red);

        sf::Text snakeButton("Snake", font, 30);
        sf::Text wordleButton("Wordle", font, 30);
        sf::Text hangmanButton("Hangman", font, 30);

        // Center buttons
        float buttonWidth = 200;
        float buttonHeight = 50;
        float centerX = (instructionsWindow.getSize().x - buttonWidth) / 2;
        snakeButtonBg.setPosition(centerX, 100);
        wordleButtonBg.setPosition(centerX, 200);
        hangmanButtonBg.setPosition(centerX, 300);

        snakeButton.setPosition(centerX + 10, 100);
        wordleButton.setPosition(centerX + 10, 200);
        hangmanButton.setPosition(centerX + 10, 300);

        while (instructionsWindow.isOpen()) {
            sf::Event event;
            while (instructionsWindow.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    instructionsWindow.close(); // Close the instructions window
                }

                // Check for button clicks
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    if (snakeButtonBg.getGlobalBounds().contains(mousePos)) {
                        currentGame = "Snake";
                        showGameInstructions = true;
                        gameInstructionSprite.setTexture(snakeTexture);
                    }
                    else if (wordleButtonBg.getGlobalBounds().contains(mousePos)) {
                        currentGame = "Wordle";
                        showGameInstructions = true;
                        gameInstructionSprite.setTexture(wordleTexture);
                    }
                    else if (hangmanButtonBg.getGlobalBounds().contains(mousePos)) {
                        currentGame = "Hangman";
                        showGameInstructions = true;
                        gameInstructionSprite.setTexture(hangmanTexture);
                    }
                }
            }

            // Render the instructions background
            instructionsWindow.clear(); // Clear the window
            instructionsWindow.draw(backgroundInstructionSprite); // Draw the instructions background

            // Draw game buttons with backgrounds
            instructionsWindow.draw(snakeButtonBg);
            instructionsWindow.draw(wordleButtonBg);
            instructionsWindow.draw(hangmanButtonBg);
            instructionsWindow.draw(snakeButton);
            instructionsWindow.draw(wordleButton);
            instructionsWindow.draw(hangmanButton);

            // If a game instruction is selected, display the corresponding image
            if (showGameInstructions) {
                gameInstructionSprite.setScale(
                    instructionsWindow.getSize().x / gameInstructionSprite.getLocalBounds().width,
                    instructionsWindow.getSize().y / gameInstructionSprite.getLocalBounds().height
                );
                instructionsWindow.draw(gameInstructionSprite);

                // Create a back button for returning to the instructions page
                sf::RectangleShape backButtonBg(sf::Vector2f(200, 50));
                backButtonBg.setFillColor(sf::Color::Yellow);
                backButtonBg.setPosition(10, 10); // Position at the top left

                sf::Text backButton("Back", font, 30);
                backButton.setPosition(20, 10); // Position text inside the button

                // Check for back button click
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

                    if (backButtonBg.getGlobalBounds().contains(mousePos)) {
                        showGameInstructions = false; // Hide game instructions and return to main instructions
                    }
                }

                // Draw back button
                instructionsWindow.draw(backButtonBg);
                instructionsWindow.draw(backButton);
            }

            instructionsWindow.display(); // Display what has been drawn
        }
    }

    void startSnakeGame() {
        // Logic to start the Snake game
        sf::RenderWindow gameWindow(sf::VideoMode(550, 400), "Snake Game");
        SnakeGame game(30, 40, 14); // Initialize SnakeGame

        Clock clock;

        while (gameWindow.isOpen()) {
            float timeElapsed = clock.restart().asSeconds();

            Event e;
            while (gameWindow.pollEvent(e)) {
                if (e.type == Event::Closed) gameWindow.close();
            }

            game.handleInput();
            game.update(timeElapsed);

            gameWindow.clear();
            game.draw(gameWindow);
            gameWindow.display();
        }
    }

    void startWordleGame() {
        // Logic to start the Wordle game
        sf::RenderWindow gameWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Wordle");
        sf::Font font;
        if (!font.loadFromFile("ARIAL.ttf")) {
            std::cerr << "Font load failed!\n";
            return;
        }

        WordleGame game;
        std::string currentGuess = "";

        while (gameWindow.isOpen()) {
            sf::Event event;
            while (gameWindow.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    gameWindow.close();
                }
                if (event.type == sf::Event::KeyPressed && !game.isGameOver()) {
                    if (event.key.code == sf::Keyboard::Enter && currentGuess.size() == GRID_COLS) {
                        game.handleGuess(currentGuess);
                        currentGuess = "";
                    }
                    else if (event.key.code == sf::Keyboard::BackSpace && !currentGuess.empty()) {
                        currentGuess.pop_back();
                    }
                    else if (currentGuess.size() < GRID_COLS && event.key.code >= sf::Keyboard::A && event.key.code <= sf::Keyboard::Z) {
                        currentGuess += static_cast<char>(event.key.code + 'A');
                    }
                }
            }

            gameWindow.clear(sf::Color::Black);
            game.draw(gameWindow, font);
            gameWindow.display();
        }
    }

    void showProgress(sf::RenderWindow& window, sf::Font& font) {
        sf::RenderWindow progressWindow(sf::VideoMode(800, 800), "Progress");
        while (progressWindow.isOpen()) {
            sf::Event event;
            while (progressWindow.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    progressWindow.close();
                }
            }

            progressWindow.clear(sf::Color::White);
            sf::Text title("Your Scores", font, 30);
            title.setFillColor(sf::Color::Black);
            title.setPosition(100, 20);
            progressWindow.draw(title);

            // Display scores
            //for (int i = 0; i < currentUser.scoreCount; i++) {
            sf::Text scoreText("Score : " + std::to_string(pointss), font, 20);
            scoreText.setFillColor(sf::Color::Black);
            scoreText.setPosition(50, 70 + 1 * 30);
            progressWindow.draw(scoreText);
            //}



            std::ifstream inFile("scores.txt");
            if (inFile.is_open()) {
                int i = 3; // Start at 0 for the first line
                char arr[100]; // Buffer for reading lines

                while (inFile.getline(arr, sizeof(arr))) {
                    // Create a text object for each line
                    sf::Text scoreText(arr, font, 20); // 20 is the character size
                    scoreText.setFillColor(sf::Color::Black);
                    scoreText.setPosition(50, 70 + i * 30); // Adjust the position for each line
                    progressWindow.draw(scoreText);
                    i++;
                }
                inFile.close();
            }
            else {
                std::cerr << "Unable to open file." << std::endl;
            }




            progressWindow.display();
        }
    }


    void handleLoginSignup() {
        int choice;
        do {

            std::cout << "You can only select 1,2 or 3" << std::endl;
            std::cout << "1. Login\n";
            std::cout << "2. Signup\n";
            std::cout << "3. Exit\n";
            std::cout << "Choose an option: ";
            std::cin >> choice;


            switch (choice) {
            case 1:
                login();
                break;
            case 2:
                signup();
                break;
            case 3:
                std::cout << "Exiting...\n";
                exit(0); // Exit the program
            default:
                std::cout << "Invalid choice. Please try again.\n";
            }
        } while (choice != 3 && !isLoggedIn); // Continue until logged in or exited
    }

    void login() {
        char username[50], password[50];
        std::cout << "Enter username: ";
        std::cin >> username;
        std::cout << "Enter password: ";
        std::cin >> password;

        for (int i = 0; i < userCount; i++) {
            if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
                currentUser = users[i]; // Set the current user
                isLoggedIn = true; // Set logged in status
                std::cout << "Login successful! Welcome, " << currentUser.username << "!\n";
                return;
            }
        }
        std::cout << "Invalid credentials. Please try again.\n";
    }

    void signup() {
        if (userCount >= MAX_USERS) {
            std::cout << "User  limit reached. Cannot signup.\n";
            return;
        }

        char username[50], password[50];
        std::cout << "Choose a username: ";
        std::cin >> username;
        std::cout << "Choose a password: ";
        std::cin >> password;

        for (int i = 0; i < userCount; i++) {
            if (strcmp(users[i].username, username) == 0) {
                std::cout << "Username already exists. Please try again.\n";
                return;
            }
        }

        // Create new user
        strcpy_s(users[userCount].username, username);
        strcpy_s(users[userCount].password, password);
        users[userCount].highScore = 0; // Initialize high score
        userCount++;

        // Save new user to file
        std::ofstream outFile(USER_DATA_FILE, std::ios::app);
        if (outFile.is_open()) {
            users[userCount - 1].saveToFile(outFile);
            outFile.close();
        }

        isLoggedIn = true; // Set logged in status
        std::cout << "Signup successful! Welcome, " << users[userCount - 1].username << "!\n";
    }

    void loadUsers() {
        std::ifstream inFile(USER_DATA_FILE);
        if (inFile.is_open()) {
            while (inFile.peek() != EOF && userCount < MAX_USERS) {
                users[userCount].loadFromFile(inFile);
                userCount++;
            }
            inFile.close();
        }
    }

    void startHangmanGame() {
        srand(static_cast<unsigned int>(time(NULL)));
        std::string wordlist[] = { "apple", "banana", "cherry", "date", "fig" }; // Sample word list
        int wordCount = sizeof(wordlist) / sizeof(wordlist[0]);

        sf::RenderWindow window(sf::VideoMode(600, 600), "Hangman");

        // Load font
        sf::Font font;
        if (!font.loadFromFile("ARIAL.ttf")) {
            std::cerr << "Failed to load font!" << std::endl;
            return;
        }

        // Text objects
        sf::Text text("", font, 40);
        text.setFillColor(sf::Color::Black);
        text.setPosition(300, 500);
        text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);

        sf::Text livesText("", font, 30);
        livesText.setFillColor(sf::Color::Black);
        livesText.setPosition(300, 50);
        livesText.setOrigin(livesText.getLocalBounds().width / 2, livesText.getLocalBounds().height / 2);

        sf::Text messageText("", font, 25);
        messageText.setFillColor(sf::Color::Red);
        messageText.setPosition(300, 300);
        messageText.setOrigin(messageText.getLocalBounds().width / 2, messageText.getLocalBounds().height / 2);

        // Background rectangle
        sf::RectangleShape background(sf::Vector2f(600, 600));
        background.setFillColor(sf::Color(200, 220, 255)); // Light blue background

        HangMan game;
        game.reset(wordlist[rand() % wordCount]);

        // Main game loop
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                else if (!game.isGameOver() && event.type == sf::Event::TextEntered) {
                    char guessed = static_cast<char>(event.text.unicode);
                    if (std::isalpha(guessed)) {
                        guessed = std::tolower(guessed);
                        game.guess(guessed);
                        text.setString(game.getObfuscatedWord());
                        if (game.isWin()) {
                            messageText.setString("You win! Press space to reset.");
                            s.update(10);
                            pointss += 10;
                            std::ofstream outFile("scores.txt", std::ios::app);
                            if (outFile.is_open()) {
                                outFile << "Player scored " << 10 << " points in hangman game!" << std::endl;
                                outFile.close();
                            }
                        }
                        else if (game.isGameOver()) {
                            pointss += 10;
                            messageText.setString("Game Over! Word: " + game.getObfuscatedWord() + ". Press space to reset.");
                            s.update(0);
                            pointss += 0;
                            std::ofstream outFile("scores.txt", std::ios::app);
                            if (outFile.is_open()) {
                                outFile << "Player scored " << 0 << " points in hangman game! " << "LOST!!!" << std::endl;
                                outFile.close();
                            }

                        }
                        livesText.setString("Lives: " + std::to_string(game.getLives()));
                    }
                }
                else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space) {
                    game.reset(wordlist[rand() % wordCount]);
                    text.setString(game.getObfuscatedWord());
                    livesText.setString("Lives: " + std::to_string(game.getLives()));
                    messageText.setString("");
                }
            }

            window.clear();
            window.draw(background);
            game.draw(window);
            window.draw(text);
            window.draw(livesText);
            window.draw(messageText);
            window.display();
        }
    }
};

// Input Handler Class
//class InputHandler
//{
//public:
//    void handleInput(GameManager& gameManager, sf::RenderWindow& window) {
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed) {
//                window.close();
//            }
//            if (event.type == sf::Event::KeyPressed) {
//                if (event.key.code == sf::Keyboard::Num1) {
//                    const int N = 40, M = 30, size = 14;
//
//                    RenderWindow window(VideoMode(size * N, size * M), "Snake Game!");
//                    SnakeGame game(M, N, size);
//
//                    Clock clock;
//
//                    while (window.isOpen()) {
//                        float timeElapsed = clock.restart().asSeconds();
//
//                        Event e;
//                        while (window.pollEvent(e)) {
//                            if (e.type == Event::Closed) window.close();
//                        }
//
//                        game.handleInput();
//                        game.update(timeElapsed);
//
//                        window.clear();
//                        game.draw(window);
//                        window.display();
//                    }
//                }
//                else if (event.key.code == sf::Keyboard::Num2) {
//                    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Wordle");
//                    sf::Font font;
//                    if (!font.loadFromFile("ARIAL.ttf")) {
//                        std::cerr << "Font load failed!\n";
//                        return;
//                    }
//
//                    WordleGame game;
//                    std::string currentGuess = "";
//
//                    while (window.isOpen()) {
//                        sf::Event event;
//                        while (window.pollEvent(event)) {
//                            if (event.type == sf::Event::Closed) {
//                                window.close();
//                            }
//                            if (event.type == sf::Event::KeyPressed && !game.isGameOver()) {
//                                if (event.key.code == sf::Keyboard::Enter && currentGuess.size() == GRID_COLS) {
//                                    game.handleGuess(currentGuess);
//                                    currentGuess = "";
//                                }
//                                else if (event.key.code == sf::Keyboard::BackSpace && !currentGuess.empty()) {
//                                    currentGuess.pop_back();
//                                }
//                                else if (currentGuess.size() < GRID_COLS && event.key.code >= sf::Keyboard::A && event.key.code <= sf::Keyboard::Z) {
//                                    currentGuess += static_cast<char>(event.key.code + 'A');
//                                }
//                            }
//                        }
//
//                        window.clear(sf::Color::Black);
//                        game.draw(window, font);
//                        window.display();
//                    }
//                }
//                else if (event.key.code == sf::Keyboard::Num3) {
//                    srand(static_cast<unsigned int>(time(NULL)));
//                    std::string wordlist[] = { "apple", "banana", "cherry", "date", "fig" }; // Sample word list
//                    int wordCount = sizeof(wordlist) / sizeof(wordlist[0]);
//
//                    sf::RenderWindow window(sf::VideoMode(600, 600), "Hangman");
//
//                    // Load font
//                    sf::Font font;
//                    if (!font.loadFromFile("ARIAL.ttf")) {
//                        std::cerr << "Failed to load font!" << std::endl;
//                        return;
//                    }
//
//                    // Text objects
//                    sf::Text text("", font, 40);
//                    text.setFillColor(sf::Color::Black);
//                    text.setPosition(300, 500);
//                    text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
//
//                    sf::Text livesText("", font, 30);
//                    livesText.setFillColor(sf::Color::Black);
//                    livesText.setPosition(300, 50);
//                    livesText.setOrigin(livesText.getLocalBounds().width / 2, livesText.getLocalBounds().height / 2);
//
//                    sf::Text messageText("", font, 25);
//                    messageText.setFillColor(sf::Color::Red);
//                    messageText.setPosition(300, 300);
//                    messageText.setOrigin(messageText.getLocalBounds().width / 2, messageText.getLocalBounds().height / 2);
//
//                    // Background rectangle
//                    sf::RectangleShape background(sf::Vector2f(600, 600));
//                    background.setFillColor(sf::Color(200, 220, 255)); // Light blue background
//
//                    HangMan game;
//                    game.reset(wordlist[rand() % wordCount]);
//
//                    // Main game loop
//                    while (window.isOpen()) {
//                        sf::Event event;
//                        while (window.pollEvent(event)) {
//                            if (event.type == sf::Event::Closed) {
//                                window.close();
//                            }
//                            else if (!game.isGameOver() && event.type == sf::Event::TextEntered) {
//                                char guessed = static_cast<char>(event.text.unicode);
//                                if (std::isalpha(guessed)) {
//                                    guessed = std::tolower(guessed);
//                                    game.guess(guessed);
//                                    text.setString(game.getObfuscatedWord());
//                                    if (game.isWin()) {
//                                        messageText.setString("You win! Press space to reset.");
//                                    }
//                                    else if (game.isGameOver()) {
//                                        messageText.setString("Game Over! Word: " + game.getObfuscatedWord() + ". Press space to reset.");
//                                    }
//                                    livesText.setString("Lives: " + std::to_string(game.getLives()));
//                                }
//                            }
//                            else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space) {
//                                game.reset(wordlist[rand() % wordCount]);
//                                text.setString(game.getObfuscatedWord());
//                                livesText.setString("Lives: " + std::to_string(game.getLives()));
//                                messageText.setString("");
//                            }
//                        }
//
//                        window.clear();
//                        window.draw(background);
//                        game.draw(window);
//                        window.draw(text);
//                        window.draw(livesText);
//                        window.draw(messageText);
//                        window.display();
//                    }
//                }
//
//                else {}
//            }
//        }
//    }
//};

// GameBoy Class
class GameBoy {
public:
    void run() {
        sf::RenderWindow window(sf::VideoMode(800, 600), "GameBoy");
        GameManager gameManager;
        Menu menu;
        //InputHandler inputHandler;

        // Start with the menu
        while (window.isOpen()) {
            //inputHandler.handleInput(gameManager, window);
            gameManager.update();

            window.clear();
            menu.display(window);
            gameManager.render(window);
            window.display();
        }
        //backgroundMusic.stop();
    }
};

// Main Program
int main() {
    GameBoy gameBoy;
    gameBoy.run();
    return 0;
}
