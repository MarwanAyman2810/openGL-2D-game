#include <Windows.h>
#include <glut.h>
#include <ctime>
#include <cstdlib>
#include <string>
#include <thread> 


// Window size
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Player properties
 int PLAYER_SIZE = 30;
 int PLAYER_STEP = 10;
int playerX = WINDOW_WIDTH / 2;
int playerY = WINDOW_HEIGHT / 2;
int playerHealth = 5; // Full health

// Obstacle properties
const int OBSTACLE_SIZE = 30;
const int NUM_OBSTACLES = 6;
int obstacleX[NUM_OBSTACLES] = { 100, 300, 500, 200, 400, 600 };
int obstacleY[NUM_OBSTACLES] = { 100, 200, 300, 400, 500, 100 };
//diappear powerup
const int DISAPPEAR_SIZE = 30;
int disappearX;
int disappearY;
bool disappearActive = true; // Active when on screen and not collected
bool obstaclesDisappeared = false; // True when obstacles are temporarily disappeared
int disappearEffectTimer = 0; // Timer to track how long the effect lasts

// Collectable properties
const int COLLECTABLE_SIZE = 30;
const int NUM_COLLECTABLES = 5;
int collectableX[NUM_COLLECTABLES];
int collectableY[NUM_COLLECTABLES];
// Shrink and SpeedUp powerup properties
const int SHRINK_SPEEDUP_SIZE = 30;
int shrinkSpeedUpX;
int shrinkSpeedUpY;
bool shrinkSpeedUpActive = true; // Active when on screen and not collected
int shrinkSpeedUpEffectTimer = 0; // Timer to track how long the effect lasts
const int SHRINK_SIZE = 15; // New size of the player when shrunk
const int SPEED_BOOST = 20; // Speed increment of the player

// Score
int score = 0;
const int FENCE_WIDTH = 5;

void drawFence() {
    glColor3f(0.0f, 0.0f, 0.0f); // Black color

    // Top fence
    glRecti(1, 1, WINDOW_WIDTH, FENCE_WIDTH);
    // Bottom fence
    glRecti(1, WINDOW_HEIGHT - FENCE_WIDTH, WINDOW_WIDTH, WINDOW_HEIGHT);
    // Left fence
    glRecti(1, 1, FENCE_WIDTH, WINDOW_HEIGHT);
    // Right fence
    glRecti(WINDOW_WIDTH - FENCE_WIDTH, 1, WINDOW_WIDTH, WINDOW_HEIGHT);
}
const int GOAL_SIZE = 60; // You can adjust the size of the goal triangle

void drawGoal() {
    glColor3f(1, 1, 0); // Yellow color
    glBegin(GL_TRIANGLES);
    glVertex2i(FENCE_WIDTH, WINDOW_HEIGHT - FENCE_WIDTH);
    glVertex2i(FENCE_WIDTH, WINDOW_HEIGHT - FENCE_WIDTH - GOAL_SIZE);
    glVertex2i(FENCE_WIDTH + GOAL_SIZE, WINDOW_HEIGHT - FENCE_WIDTH);
    glEnd();
}
void initializeDisappear() {
    srand(time(0) + 1); // Seed random number generator
    disappearX = FENCE_WIDTH + rand() % (WINDOW_WIDTH - DISAPPEAR_SIZE - 2 * FENCE_WIDTH);
    disappearY = FENCE_WIDTH + rand() % (WINDOW_HEIGHT - DISAPPEAR_SIZE - 2 * FENCE_WIDTH);
}
void initializeShrinkSpeedUp() {
    srand(time(0) + 2); // Seed random number generator
    shrinkSpeedUpX = FENCE_WIDTH + rand() % (WINDOW_WIDTH - SHRINK_SPEEDUP_SIZE - 2 * FENCE_WIDTH);
    shrinkSpeedUpY = FENCE_WIDTH + rand() % (WINDOW_HEIGHT - SHRINK_SPEEDUP_SIZE - 2 * FENCE_WIDTH);
}
void drawDisappear() {
    if (disappearActive) {
        glColor3f(1, 1, 0); // Yellow color
        glRecti(disappearX, disappearY, disappearX + DISAPPEAR_SIZE, disappearY + DISAPPEAR_SIZE);
    }
}

void drawShrinkSpeedUp() {
    if (shrinkSpeedUpActive) {
        glColor3f(0.5f, 0, 0.5f); // Purple color
        glRecti(shrinkSpeedUpX, shrinkSpeedUpY, shrinkSpeedUpX + SHRINK_SPEEDUP_SIZE, shrinkSpeedUpY + SHRINK_SPEEDUP_SIZE);
    }
}

void drawPlayer() {
    glColor3f(0, 0, 1); // Blue color
    glRecti(playerX, playerY, playerX + PLAYER_SIZE, playerY + PLAYER_SIZE);
}

void drawObstacles() {
    if (!obstaclesDisappeared) {
        glColor3f(1, 0, 0); // Red color
        for (int i = 0; i < NUM_OBSTACLES; i++) {
            glRecti(obstacleX[i], obstacleY[i], obstacleX[i] + OBSTACLE_SIZE, obstacleY[i] + OBSTACLE_SIZE);
        }
    }
}


void drawHealthBar() {
    int offsetX = FENCE_WIDTH + 2; // Adding an offset to be inside the fence
    int offsetY = FENCE_WIDTH + 2; // Adding an offset to be inside the fence
    int barWidth = 100; // You can adjust the width of the health bar
    int barHeight = 20; // You can adjust the height of the health bar

    // Drawing the red bar (full health)
    glColor3f(1, 0, 0); // Red color
    glRecti(offsetX, offsetY, offsetX + barWidth, offsetY + barHeight);

    // Drawing the green bar (current health)
    glColor3f(0, 1, 0); // Green color
    glRecti(offsetX, offsetY, offsetX + (barWidth * playerHealth) / 5, offsetY + barHeight); // Assuming full health is 5
}
void checkDisappearCollision() {
    if (disappearActive &&
        playerX < disappearX + DISAPPEAR_SIZE &&
        playerX + PLAYER_SIZE > disappearX &&
        playerY < disappearY + DISAPPEAR_SIZE &&
        playerY + PLAYER_SIZE > disappearY) {

        disappearActive = false;
        obstaclesDisappeared = true;
        disappearEffectTimer = 5; // The effect lasts for 5 seconds
    }
}
void checkShrinkSpeedUpCollision() {
    if (shrinkSpeedUpActive &&
        playerX < shrinkSpeedUpX + SHRINK_SPEEDUP_SIZE &&
        playerX + PLAYER_SIZE > shrinkSpeedUpX &&
        playerY < shrinkSpeedUpY + SHRINK_SPEEDUP_SIZE &&
        playerY + PLAYER_SIZE > shrinkSpeedUpY) {

        shrinkSpeedUpActive = false;
        shrinkSpeedUpEffectTimer = 5; // The effect lasts for 5 seconds
    }
}
void updateDisappearEffect() {
    static int lastTime = glutGet(GLUT_ELAPSED_TIME);
    int currentTime = glutGet(GLUT_ELAPSED_TIME);

    if (obstaclesDisappeared && currentTime - lastTime >= 1000) {
        disappearEffectTimer--;
        lastTime = currentTime;

        if (disappearEffectTimer <= 0) {
            obstaclesDisappeared = false;
        }
    }
}
void updateShrinkSpeedUpEffect() {
    static int lastTime = glutGet(GLUT_ELAPSED_TIME);
    int currentTime = glutGet(GLUT_ELAPSED_TIME);

    if (shrinkSpeedUpEffectTimer > 0 && currentTime - lastTime >= 1000) {
        shrinkSpeedUpEffectTimer--;
        lastTime = currentTime;

        if (shrinkSpeedUpEffectTimer <= 0) {
            PLAYER_SIZE = 30; // Resetting the player size to original size
            PLAYER_STEP = 10; // Resetting the player speed to original speed
        }
        else {
            PLAYER_SIZE = SHRINK_SIZE; // Setting the player size to smaller size
            PLAYER_STEP = SPEED_BOOST; // Increasing the player's speed
        }
    }
}


bool checkCollision() {
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        if (playerX < obstacleX[i] + OBSTACLE_SIZE &&
            playerX + PLAYER_SIZE > obstacleX[i] &&
            playerY < obstacleY[i] + OBSTACLE_SIZE &&
            playerY + PLAYER_SIZE > obstacleY[i]) {
            return true;
        }
    }
    return false;
}
bool checkGoalCollision() {
    int goalX = FENCE_WIDTH;
    int goalY = WINDOW_HEIGHT - FENCE_WIDTH - GOAL_SIZE;

    // Simple collision check considering the triangle as a square for simplicity
    return playerX < goalX + GOAL_SIZE && playerX + PLAYER_SIZE > goalX &&
        playerY < goalY + GOAL_SIZE && playerY + PLAYER_SIZE > goalY;
}

void drawGameOverScreen() {
    glClearColor(1.0, 0.0, 0.0, 1.0); // Set background color to red
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0, 1.0, 1.0); // Set text color to white
    glRasterPos2i(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2);
    std::string loseText = "You lose!";
    for (char c : loseText) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }

    glutSwapBuffers();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    exit(0);
}
void drawWinScreen() {
    glClearColor(0.0, 1.0, 0.0, 1.0); // Set background color to green
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0, 1.0, 1.0); // Set text color to white
    glRasterPos2i(WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2);
    std::string winText = "Congratulations, you win!";
    for (char c : winText) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }

    glutSwapBuffers();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    exit(0);
}
int gameTimer = 40;
void drawTimer() {
    static int lastTime = glutGet(GLUT_ELAPSED_TIME); // Static variable to keep track of the last time this function was called
    int currentTime = glutGet(GLUT_ELAPSED_TIME); // Get current time

    if (currentTime - lastTime >= 1000) { // If 1 second has passed
        gameTimer--; // Decrement the game timer
        lastTime = currentTime; // Update the last time to the current time
    }

    glColor3f(0, 0, 0); // Black color
    glRasterPos2i(WINDOW_WIDTH - 100, 30);

    std::string timerText = "Time: " + std::to_string(gameTimer);
    for (char c : timerText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // If timer reaches 0, end the game
    if (gameTimer <= 0) {
        drawGameOverScreen();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        exit(0);
    }
    glutPostRedisplay();
}


void handleCollision() {
    if (checkCollision() || playerX <= FENCE_WIDTH || playerX + PLAYER_SIZE >= WINDOW_WIDTH - FENCE_WIDTH ||
        playerY <= FENCE_WIDTH || playerY + PLAYER_SIZE >= WINDOW_HEIGHT - FENCE_WIDTH) {
        playerHealth--;
        // Reset player position
        playerX = WINDOW_WIDTH / 2;
        playerY = WINDOW_HEIGHT / 2;
        if (playerHealth <= 0) {
            drawGameOverScreen();
            std::this_thread::sleep_for(std::chrono::seconds(3));
            exit(0);
        }
        glutSwapBuffers();
    }
    if (checkGoalCollision())
    {
        drawWinScreen();
    }
}
void initializeCollectables() {
    srand(time(0)); // Seed random number generator
    for (int i = 0; i < NUM_COLLECTABLES; i++) {
        collectableX[i] = FENCE_WIDTH + rand() % (WINDOW_WIDTH - COLLECTABLE_SIZE - 2 * FENCE_WIDTH);
        collectableY[i] = FENCE_WIDTH + rand() % (WINDOW_HEIGHT - COLLECTABLE_SIZE - 2 * FENCE_WIDTH);
    }
}

void drawCollectables() {
    glColor3f(0, 1, 0); // Green color
    for (int i = 0; i < NUM_COLLECTABLES; i++) {
        if (collectableX[i] != -1 && collectableY[i] != -1) {
            glRecti(collectableX[i], collectableY[i],
                collectableX[i] + COLLECTABLE_SIZE, collectableY[i] + COLLECTABLE_SIZE);
        }
    }
}

void drawScore() {
    glColor3f(0, 0, 0); // Black color

    // Positioning text at a specific location within the window
    glRasterPos2i(WINDOW_WIDTH / 2 - 50, 30);

    // Drawing the "Score: " text
    std::string scoreLabel = "Score: ";
    for (char c : scoreLabel) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // Drawing the numeric score value next to the "Score: " text
    std::string scoreText = std::to_string(score);
    for (char c : scoreText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}


void checkCollectableCollision() {
    for (int i = 0; i < NUM_COLLECTABLES; i++) {
        if (playerX < collectableX[i] + COLLECTABLE_SIZE &&
            playerX + PLAYER_SIZE > collectableX[i] &&
            playerY < collectableY[i] + COLLECTABLE_SIZE &&
            playerY + PLAYER_SIZE > collectableY[i]) {
            // Player collected a collectable
            collectableX[i] = -1;
            collectableY[i] = -1;
            score++;
        }
    }
}


void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w': // Move up
        playerY -= PLAYER_STEP;
        break;
    case 'a': // Move left
        playerX -= PLAYER_STEP;
        break;
    case 's': // Move down
        playerY += PLAYER_STEP;
        break;
    case 'd': // Move right
        playerX += PLAYER_STEP;
        break;
    }

    checkCollectableCollision();
    handleCollision();// Check for collision with collectables here
    checkDisappearCollision();
    updateDisappearEffect();
    checkShrinkSpeedUpCollision();
    updateShrinkSpeedUpEffect();
    glutPostRedisplay();
}

void display() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // White background
    glClear(GL_COLOR_BUFFER_BIT);
    drawFence();
    drawPlayer();
    drawObstacles();
    drawCollectables();
    drawHealthBar();
    drawScore();
    drawGoal();
    drawTimer();
    drawDisappear();
    drawShrinkSpeedUp();

    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Stickman Game");

    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    initializeCollectables();
    initializeDisappear();
    initializeShrinkSpeedUp();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}