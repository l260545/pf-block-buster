// =====================================================================
//  BLOCK BUSTER  -  PF Mini Project  (C++ + SFML 3)
//  Team: Huba Awais, Madeeha Mohsin , Areeha Ali 
// =====================================================================

#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
#include <cstdlib>   // rand, srand
#include <ctime>     // time
#include <optional>
using namespace std;

// ---------------------------------------------------------------------
// 1) NAMED CONSTANTS  (Feature 10: no magic numbers)
//    Values "Game values" 
// ---------------------------------------------------------------------
const int WINDOW_W = 600;
const int WINDOW_H = 700;
const int HUD_H = 50;          // upar wali score/lives strip

const int ROWS = 8;           // bricks ki rows
const int COLS = 10;          // bricks ke columns
const int BRICK_W = 60;
const int BRICK_H = 25;
const int BRICKS_Y = 100;         // bricks y = 100 se shuru

const int PADDLE_W = 100;   // normal paddle width
const int PADDLE_WIDE_W = 150;   // power-up "W" wali width
const int PADDLE_H = 14;
const int PADDLE_Y = 650;   // paddle ka top edge
const int PADDLE_START_X = 250;
const int PADDLE_SPEED = 8;     // pixels har frame

const int BALL_SIZE = 12;  // ball ka box 12 x 12
const int BALL_ON_PADDLE_Y = 638; // paddle ke bilkul upar (650 - 12)
const int BALL_SPEED_Y = 5;   // dy = 5 ya -5

const int START_LIVES = 3;
const int POINTS_HIT = 10;      // brick ko hit kiya
const int POINTS_BREAK = 50;      // hit se brick toot gayi

const int POWERUP_W = 24;     // girne wala power-up box
const int POWERUP_H = 12;
const int POWERUP_SPEED = 3;      // 3 pixels har frame
const int WIDE_FRAMES = 600;    // 10 seconds x 60 frames

// Game states (Feature 7)
const int MENU = 0;
const int PLAYING = 1;
const int GAME_OVER = 2;          // game over YA you win (playerWon batata hai)

// Ek-baar-press wali keys (event loop se aati hain)
const int NO_KEY = 0;
const int KEY_LAUNCH = 1;         // Space
const int KEY_ENTER = 2;
const int KEY_ESCAPE = 3;
const int KEY_UP = 4;
const int KEY_DOWN = 5;

// Menu options (order: New Game, Continue, Exit)
const int OPTION_NEW_GAME = 0;
const int OPTION_CONTINUE = 1;
const int OPTION_EXIT = 2;
const int MENU_OPTIONS = 3;

// ---------------------------------------------------------------------
// 2) COLOURS  - sirf yahan change karo, poori game ka look badal jayega
//    sf::Color(red, green, blue)  har value 0 se 255
// ---------------------------------------------------------------------
const sf::Color COLOR_BACKGROUND(16, 20, 45);     // dark navy
const sf::Color COLOR_HUD(28, 33, 70);     // HUD strip
const sf::Color COLOR_TEXT(235, 235, 245);  // normal text
const sf::Color COLOR_HIGHLIGHT(70, 225, 255);   // selected menu option
const sf::Color COLOR_WARNING(255, 85, 110);   // error / "No saved game"
const sf::Color COLOR_PADDLE(70, 225, 255);   // cyan paddle
const sf::Color COLOR_BALL(255, 255, 255);  // white ball
const sf::Color COLOR_LIFE(255, 85, 110);   // lives ke circles

// index = hits left:  0 = no brick, 1 = green, 2 = yellow, 3 = red
const sf::Color BRICK_COLORS[4] = {
    sf::Color(25, 25, 45),     // 0: no brick (draw nahi hoti)
    sf::Color(60, 220, 150),   // 1: one hit left   (green)
    sf::Color(255, 210, 60),   // 2: two hits left  (yellow)
    sf::Color(255, 85, 110)    // 3: three hits left (red)
};

// ---------------------------------------------------------------------
// 3) GAME VARIABLES  (poori game ki "state")
// ---------------------------------------------------------------------
int bricks[ROWS][COLS];       // level.txt se aati hain

int  gameState = MENU;   // TEMPORARY testing ke liye.
int  menuChoice = OPTION_NEW_GAME;  // menu mein kaunsa option selected hai
bool showNoSave = false;            // "No saved game" dikhana hai?
bool levelLoaded = false;            // level.txt mila ya nahi
bool playerWon = false;            // GAME_OVER screen: true = YOU WIN

int score = 0;
int lives = START_LIVES;

int paddleX = PADDLE_START_X;
int paddleWidth = PADDLE_W;

int  ballX = 0, ballY = 0;    // ball ke box ka top-left corner
int  dx = 0, dy = 0;          // ball ki speed (x aur y mein)
bool ballLaunched = false;    // false = ball paddle pe ruki hai
const int BALL_SPEED_X = 3;       // dx = 3 ya -3 (left/right speed)
const int POWERUP_CHANCE = 5;     // 1 in 5 chance power-up gire
// Bonus power-up
bool powerUpFalling = false;
int  powerUpX = 0, powerUpY = 0;
char powerUpType = 'W';       // 'W' = wide paddle, '+' = extra life
int  wideTimer = 0;           // kitne frames wide paddle baqi hai

// Input
int  pressedKey = NO_KEY;     // is frame mein kaunsi key ek dafa press hui
bool leftHeld = false;       // Left ya A dabi hui hai?
bool rightHeld = false;       // Right ya D dabi hui hai?

// Text ke liye font
sf::Font font;
bool fontLoaded = false;

// ---------------------------------------------------------------------
// 4) FUNCTION LIST  (prototypes) - har function ek kaam karta hai
// ---------------------------------------------------------------------
// SFML / setup
bool loadFont();
void handleInput(sf::RenderWindow& window);
void updateGame(sf::RenderWindow& window);
void drawGame(sf::RenderWindow& window);
void drawText(sf::RenderWindow& window, const string& message, float x, float y, unsigned int size, sf::Color color);
void drawTextCentered(sf::RenderWindow& window, const string& message, float y, unsigned int size, sf::Color color);

// Drawing + menu (code: sfml_parts_to_paste.cpp)
void drawBricks(sf::RenderWindow& window);
void drawErrorScreen(sf::RenderWindow& window); 
void drawMenu(sf::RenderWindow& window);        
void drawEndScreen(sf::RenderWindow& window);   
void drawPaddle(sf::RenderWindow& window);      
void drawBall(sf::RenderWindow& window);        
void drawPowerUp(sf::RenderWindow& window);     
void drawHUD(sf::RenderWindow& window);         

// Game logic
bool loadLevel();                               
void startNewGame();                          
void updateMenu(sf::RenderWindow& window);      
void updateGameOver();                          
void updatePlaying();                           
void movePaddle();                              
void resetBallOnPaddle();                    
void launchBall();                     
void moveBall();                                
bool bounceOnPaddle();                        
void saveGame();                               
bool loadSavedGame();                          
void clearSave();                               
void tryDropPowerUp(int row, int col);          
void updatePowerUp();                           
bool cornerInBrickArea(int cx, int cy);         
bool hitBricks();                               
void loseLife();                                
void checkWinOrLose();                          

// =====================================================================
// 5) MAIN  
// =====================================================================
int main()
{
    srand((unsigned)time(0));   // har dafa alag random numbers

    sf::RenderWindow window(sf::VideoMode({ (unsigned)WINDOW_W, (unsigned)WINDOW_H }), "Block Buster");
    window.setFramerateLimit(60);   

    fontLoaded = loadFont();
    levelLoaded = loadLevel();      // file nahi mili to error screen dikhegi

    // GAME LOOP (Feature 1): input -> update -> draw, 60 dafa har second
    while (window.isOpen())
    {
        handleInput(window);
        updateGame(window);
        drawGame(window);
    }
    return 0;
}

// =====================================================================
// 6) SFML PART 
// =====================================================================

// Font load karo. Pehle project folder dekho, phir Windows ka Arial.
bool loadFont()
{
    if (font.openFromFile("arial.ttf"))
        return true;
    if (font.openFromFile("C:/Windows/Fonts/arial.ttf"))
        return true;
    return false;
}

// INPUT: keys aur window events parhna. Yahan sirf "kaunsi key" save hoti
// hai; us key se kya hoga woh update functions decide karte hain.
void handleInput(sf::RenderWindow& window)
{
    pressedKey = NO_KEY;   // har frame naye sire se

    // Ek-baar-press wali keys + window ka X button
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Space)       pressedKey = KEY_LAUNCH;
            else if (key->code == sf::Keyboard::Key::Enter)  pressedKey = KEY_ENTER;
            else if (key->code == sf::Keyboard::Key::Escape) pressedKey = KEY_ESCAPE;
            else if (key->code == sf::Keyboard::Key::Up)     pressedKey = KEY_UP;
            else if (key->code == sf::Keyboard::Key::Down)   pressedKey = KEY_DOWN;
        }
    }

    // Dabi hui keys (paddle ke liye): har frame seedha check
    leftHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
    rightHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
}

// UPDATE: har state sirf apna kaam karti hai (switch-case, Feature 7)
void updateGame(sf::RenderWindow& window)
{
    if (!levelLoaded)        // level.txt nahi mili: kuch update nahi karna
        return;

    switch (gameState)
    {
    case MENU:      updateMenu(window); break;
    case PLAYING:   updatePlaying();    break;
    case GAME_OVER: updateGameOver();   break;
    }
}

// DRAW: clear -> sab draw -> display
void drawGame(sf::RenderWindow& window)
{
    if (!window.isOpen())
        return;

    window.clear(COLOR_BACKGROUND);

    if (!levelLoaded)
    {
        drawErrorScreen(window);
    }
    else
    {
        switch (gameState)
        {
        case MENU:
            drawMenu(window);
            break;
        case PLAYING:
            drawHUD(window);
            drawBricks(window);
            drawPaddle(window);
            drawBall(window);
            drawPowerUp(window);
            break;
        case GAME_OVER:
            drawEndScreen(window);
            break;
        }
    }

    window.display();
}

// Text likhne ka helper: (x, y) pe message
void drawText(sf::RenderWindow& window, const string& message, float x, float y, unsigned int size, sf::Color color)
{
    if (!fontLoaded)
        return;
    sf::Text text(font, message, size);
    text.setFillColor(color);
    text.setPosition({ x, y });
    window.draw(text);
}

// Text ko screen ke beech (horizontally) mein likhna
void drawTextCentered(sf::RenderWindow& window, const string& message, float y, unsigned int size, sf::Color color)
{
    if (!fontLoaded)
        return;
    sf::Text text(font, message, size);
    text.setFillColor(color);
    float width = text.getLocalBounds().size.x;
    text.setPosition({ (WINDOW_W - width) / 2.f, y });
    window.draw(text);
}

// =====================================================================
// 7) DRAWING  
// =====================================================================
void drawBricks(sf::RenderWindow& window)
{
    // Ek hi rectangle banao, har brick pe uska colour aur jagah badlo.
    // 2 pixel chhota, taake bricks ke beech gap nazar aaye.
    sf::RectangleShape brick({ BRICK_W - 2.f, BRICK_H - 2.f });

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            int hitsLeft = bricks[r][c];
            if (hitsLeft <= 0 || hitsLeft > 3)   // 0 = brick nahi hai
                continue;

            brick.setFillColor(BRICK_COLORS[hitsLeft]);        // hits ke hisaab se colour
            brick.setPosition({ (float)(c * BRICK_W + 1),        // x = c * 60 (+1 gap)
                               (float)(BRICKS_Y + r * BRICK_H + 1) }); // y = 100 + r * 25
            window.draw(brick);
        }
    }
}
void drawErrorScreen(sf::RenderWindow& window)
{
    drawTextCentered(window, "ERROR", 260.f, 48, COLOR_WARNING);
    drawTextCentered(window, "level.txt was not found.", 340.f, 22, COLOR_TEXT);
    drawTextCentered(window, "Put level.txt next to main.cpp", 380.f, 18, COLOR_TEXT);
    drawTextCentered(window, "and run the game again.", 405.f, 18, COLOR_TEXT);
}
void drawMenu(sf::RenderWindow& window)
{
    drawTextCentered(window, "BLOCK BUSTER", 150.f, 52, COLOR_HIGHLIGHT);

    string options[MENU_OPTIONS] = { "New Game", "Continue", "Exit" };

    for (int i = 0; i < MENU_OPTIONS; i++)
    {
        float y = 290.f + i * 60.f;
        if (i == menuChoice)
            drawTextCentered(window, "> " + options[i] + " <", y, 32, COLOR_HIGHLIGHT);
        else
            drawTextCentered(window, options[i], y, 28, COLOR_TEXT);
    }

    if (showNoSave)
        drawTextCentered(window, "No saved game", 500.f, 22, COLOR_WARNING);

    drawTextCentered(window, "Up/Down to choose, Enter to select", 630.f, 16, COLOR_TEXT);
}
void drawEndScreen(sf::RenderWindow& window)
{
    if (playerWon)
        drawTextCentered(window, "YOU WIN", 230.f, 60, BRICK_COLORS[1]);
    else
        drawTextCentered(window, "GAME OVER", 230.f, 60, COLOR_WARNING);

    drawTextCentered(window, "Final score: " + to_string(score), 340.f, 30, COLOR_TEXT);
    drawTextCentered(window, "Press Enter", 430.f, 24, COLOR_HIGHLIGHT);
}
void drawPaddle(sf::RenderWindow& window)
{
    // paddleWidth use kiya (100 ya power-up mein 150)
    sf::RectangleShape paddle({ (float)paddleWidth, (float)PADDLE_H });
    paddle.setFillColor(COLOR_PADDLE);
    paddle.setPosition({ (float)paddleX, (float)PADDLE_Y });
    window.draw(paddle);
}
void drawBall(sf::RenderWindow& window)
{
    sf::CircleShape ball(BALL_SIZE / 2.f);   // radius 6 -> 12 x 12 box
    ball.setFillColor(COLOR_BALL);
    ball.setPosition({ (float)ballX, (float)ballY }); // box ka top-left corner
    window.draw(ball);
}
void drawPowerUp(sf::RenderWindow& window) { /* Member B, Saturday (bonus): paste */ }
void drawHUD(sf::RenderWindow& window)
{
    // upar ki 50 px strip
    sf::RectangleShape strip({ (float)WINDOW_W, (float)HUD_H });
    strip.setFillColor(COLOR_HUD);
    strip.setPosition({ 0.f, 0.f });
    window.draw(strip);

    // score
    drawText(window, "Score: " + to_string(score), 15.f, 12.f, 22, COLOR_TEXT);

    // lives
    drawText(window, "Lives:", 420.f, 12.f, 22, COLOR_TEXT);
    if (lives > 5)
    {
        drawText(window, to_string(lives), 495.f, 12.f, 22, COLOR_LIFE);
        return;
    }
    sf::CircleShape life(7.f);
    life.setFillColor(COLOR_LIFE);
    for (int i = 0; i < lives; i++)
    {
        life.setPosition({ 495.f + i * 20.f, 18.f });
        window.draw(life);
    }
}

// =====================================================================
// 8) GAME LOGIC  - 
// =====================================================================

// ----------------------------- MEMBER A -----------------------------

// Feature 2: level.txt ko bricks[8][10] mein parhna.

bool loadLevel()
{
    ifstream fin("level.txt");
    if (!fin)
        return false;
    else
    {
        int rows, col;
        fin >> rows >> col;
        for (int i = 0;i < rows;i++)
        {
            for (int j = 0;j < col;j++)
                fin >> bricks[i][j];
        }
    }
    fin.close();
    return true;   
}

// Feature 7: New Game hamesha fresh start
void startNewGame()
{
    score = 0;
    lives = START_LIVES;  
    levelLoaded = loadLevel();    // taake bricks file se dobara aayen.    
    paddleX = PADDLE_START_X;
    paddleWidth = PADDLE_W;
    powerUpFalling = false;
    ballLaunched = false;
    resetBallOnPaddle();
    wideTimer = 0;
    gameState = PLAYING;
}

// Feature 7: menu ke keys 
void updateMenu(sf::RenderWindow& window)
{
    if (pressedKey == KEY_UP)
    {
        menuChoice--;
        if (menuChoice < 0)                       // sab se upar wale se upar nahi
            menuChoice = 0;
        showNoSave = false;
    }
    else if (pressedKey == KEY_DOWN)
    {
        menuChoice++;
        if (menuChoice > MENU_OPTIONS - 1)        // Exit se neeche nahi
            menuChoice = MENU_OPTIONS - 1;
        showNoSave = false;
    }
    else if (pressedKey == KEY_ENTER)
    {
        switch (menuChoice)
        {
        case OPTION_NEW_GAME:
            showNoSave = false;
            startNewGame();                       // score 0, 3 lives, fresh bricks, PLAYING
            break;

        case OPTION_CONTINUE:
            if (loadSavedGame())                  // save mila aur parh liya
            {
                showNoSave = false;
                gameState = PLAYING;
            }
            else                                  // koi save nahi
            {
                showNoSave = true;
            }
            break;

        case OPTION_EXIT:
            window.close();                       // game band
            break;
        }
    }
}

// Feature 7: Enter -> menu 
void updateGameOver()
{
    if (pressedKey == KEY_ENTER)
    {
        clearSave();                    // khatam game continue na ho sake 
        menuChoice = OPTION_NEW_GAME;   // menu "New Game" pe khule
        showNoSave = false;
        gameState = MENU;
    }
}

// PLAYING state har frame: functions ko sahi order mein call karna
void updatePlaying()
{
    if (pressedKey == KEY_ESCAPE)
    {
        saveGame();         // Esc: game save kar ke menu pe jao
        gameState = MENU;
        return;
  }
    movePaddle();
    if (ballLaunched == false)           // Ball abhi paddle pe hai: paddle ke saath chalao, Space pe launch
    {
        resetBallOnPaddle();
        if (pressedKey == KEY_LAUNCH)
            launchBall();
    }
    else
        moveBall();          // Ball chal rahi hai
    updatePowerUp();
    checkWinOrLose();
}

// ----------------------------- MEMBER B -----------------------------

// Feature 3: paddle 8 px left/right, window se bahar nahi
void movePaddle()
{
    if (leftHeld == true)
        paddleX -= PADDLE_SPEED;
    if (rightHeld == true)
       paddleX+= PADDLE_SPEED;
    if (paddleX < 0)             // paddle ko window ke andar rakho
        paddleX = 0;
    if (paddleX + paddleWidth > WINDOW_W)          // paddle ko window ke andar rakho
        paddleX = WINDOW_W - paddleWidth;
}

// Feature 3: 

void resetBallOnPaddle()
{
    ballX = paddleX + (paddleWidth - BALL_SIZE) / 2;
    ballY = BALL_ON_PADDLE_Y;
}

// Feature 3: 
void launchBall()
{
    dy = -BALL_SPEED_Y;
    if (rand() % 2 == 0)
    {
        dx = 3
             ;          // thora right
    }
    else
        dx = -3;          //thora left
    ballLaunched = true;
}

// Feature 4: 
void moveBall()
{
    ballX+= dx;
    if (ballX < 0 || ballX + BALL_SIZE > WINDOW_W)
    {
        ballX -= dx;
        dx = -dx;
    }
    else if (hitBricks() == true)
    {
        ballX -= dx;
        dx = -dx;
    }
    ballY += dy;
    if (ballY < HUD_H)
    {
        ballY -= dy;
        dy = -dy;
    }
    else if (hitBricks() == true)
    {
        ballY -= dy;
        dy = -dy;
    }
    else
        bounceOnPaddle();
    if (ballY > WINDOW_H)
        loseLife();
}

// Feature 4: paddle se takrayi (sirf neeche jaate hue) -> true
// Feature 4: paddle se takrayi (sirf neeche jaate hue) -> true
bool bounceOnPaddle()
{
    if (dy < 0)                       // ball upar ja rahi hai: paddle ignore karo
        return false;

    int ballBottom = ballY + BALL_SIZE;

    // ball ka neeche wala edge paddle ki patti ke andar hai?
    if (ballBottom < PADDLE_Y || ballBottom > PADDLE_Y + PADDLE_H)
        return false;

    // ball paddle ke left aur right edge ke beech hai?
    if (ballX + BALL_SIZE < paddleX || ballX > paddleX + paddleWidth)
        return false;

    // paddle pe lagi: ball ko paddle ke upar rakho aur upar bhejo
    ballY = BALL_ON_PADDLE_Y;
    dy = -BALL_SPEED_Y;

    // paddle ke kis hisse pe lagi?
    int hitPoint = (ballX + BALL_SIZE / 2) - paddleX;   // 0 se paddleWidth tak

    if (hitPoint < paddleWidth / 3)              // left wala hissa
        dx = -BALL_SPEED_X;
    else if (hitPoint > paddleWidth * 2 / 3)     // right wala hissa
        dx = BALL_SPEED_X;
    // beech wala hissa: dx same rahega

    return true;
}

// Feature 8: score, lives aur 8 rows savegame.txt mein
void saveGame()
{
    // TODO (Member B, Friday)
}

// Feature 8: savegame.txt parhna. Kuch nahi mila -> false
bool loadSavedGame()
{
    // TODO (Member B, Friday)
    return false;
}

// Feature 8: savegame.txt khaali karna (ofstream khol ke band)
void clearSave()
{
    // TODO (Member B, Friday)
}

// Bonus: brick tooti -> 1 in 5 chance power-up gire
void tryDropPowerUp(int row, int col)
{
    // TODO (Member B, Saturday, bonus)
}

// Bonus: power-up girna, catch karna, wide timer chalana
void updatePowerUp()
{
    // TODO (Member B, Saturday, bonus)
}

// ----------------------------- MEMBER C -----------------------------

// Feature 5: corner brick area ke andar hai? 
bool cornerInBrickArea(int cx, int cy)
{
    if (cy >= BRICKS_Y && cy < BRICKS_Y + ROWS * BRICK_H && cx >= 0 && cx < WINDOW_W)
        return true;
    else
    return false;
}

// Feature 5:
bool hitBricks()
{
    int cornerX[4] = { ballX,ballX + BALL_SIZE - 1,ballX,ballX + BALL_SIZE - 1 };
    int cornerY[4] = { ballY,ballY,ballY + BALL_SIZE - 1,ballY + BALL_SIZE - 1 };
    int hitRow[4], hitCol[4], hitcount = 0;
    for (int i = 0;i < 4;i++)
    {
        if (cornerInBrickArea(cornerX[i], cornerY[i]) == false)
            continue;
        int row = (cornerY[i] - BRICKS_Y) / BRICK_H;
        int col = cornerX[i] / BRICK_W;
        if (bricks[row][col] == 0)
            continue;
        bool alreadyInList = false;
        for (int j = 0;j < hitcount;j++)
        {
            if (hitRow[j] == row && hitCol[j] == col)
                alreadyInList = true;
        }
        if (alreadyInList == false)
        {
            hitRow[hitcount] = row;
            hitCol[hitcount] = col;
            hitcount++;
        }
    } 
    for (int k = 0;k < hitcount;k++)
    {
        bricks[hitRow[k]][hitCol[k]]--;
        if (bricks[hitRow[k]][hitCol[k]]==0)
        {
            score += POINTS_BREAK;
            tryDropPowerUp(hitRow[k], hitCol[k]);
        }
        else
            score += POINTS_HIT;

    }
    return hitcount>0;
}

// Feature 6: ball neeche gir gayi -> ek life kam, ball wapas paddle pe
void loseLife()
{
   
    lives--; // 1 life kam
    ballLaunched = false; // ball ruk jaye
    dx = 0;
    dy = 0;
    if (lives <= 0)
    {
     // Sari lives khatam -> GAME OVER, jeete nahi
     playerWon = false;
     gameState = GAME_OVER;
     }
    else
     {
     // Life bachi hai -> ball wapas paddle pe
     resetBallOnPaddle();
     }
}

// Feature 6: saari bricks 0 -> YOU WIN; lives 0 -> GAME OVER
void checkWinOrLose()
{
 
 // 1. Pehle check karo: kya sari bricks toot gayi?
 bool allBricksGone = true;
for (int r = 0; r < ROWS; r++)
{
    for (int c = 0; c < COLS; c++)
    {
    if (bricks[r][c] > 0)
        {
            allBricksGone = false;
        }
    }
}

if (allBricksGone == true)
{
    playerWon = true; // YOU WIN
    gameState = GAME_OVER;
    return;
}
// 2. Kya lives khatam ho gayi?
if (lives <= 0)
    {
        playerWon = false; // GAME OVER
        gameState = GAME_OVER;
    }
}
