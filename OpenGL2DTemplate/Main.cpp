#include <Windows.h>
#include <glut.h>
#include <ctime>
#include <cstdlib>
#include <string>
#include <thread> 
#include <cmath> 




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

float disappearRotationAngle = 0.0f;
float collectableRotationAngle[NUM_COLLECTABLES] = { 0.0f }; // Initialize rotation angles for each collectible
float shrinkSpeedUpRotationAngle = 0.0f;
// Shrink and SpeedUp powerup properties
const int SHRINK_SPEEDUP_SIZE = 30;
int shrinkSpeedUpX;
int shrinkSpeedUpY;
bool shrinkSpeedUpActive = true; // Active when on screen and not collected
int shrinkSpeedUpEffectTimer = 0; // Timer to track how long the effect lasts
const int SHRINK_SIZE = 15; // New size of the player when shrunk
const int SPEED_BOOST = 20; // Speed increment of the player

/// </summary>
float goalAngle = 0.0f;
float playerAngle = 0.0f;
static float rotationAngle = 0.0f;
// Score
int score = 0;
const int FENCE_WIDTH = 10;
void drawCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < num_segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
        float dx = r * cosf(theta);
        float dy = r * sinf(theta);
        glVertex2f(dx + cx, dy + cy);
    }
    glEnd();
}
void drawRect(int x, int y, int width, int height) {
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + width, y);
    glVertex2i(x + width, y + height);
    glVertex2i(x, y + height);
    glEnd();
}
void drawX(int x, int y, int size) {
    int thickness = size / 5; // Adjust the thickness of the X lines as per your preference

    glColor3f(0, 0, 0); // Black color for the X shape

    // Drawing the first part of the X (from top-left to bottom-right)
    glBegin(GL_QUADS);
    glVertex2i(x, y + thickness);
    glVertex2i(x + thickness, y);
    glVertex2i(x + size, y + size - thickness);
    glVertex2i(x + size - thickness, y + size);
    glEnd();

    // Drawing the second part of the X (from top-right to bottom-left)
    glBegin(GL_QUADS);
    glVertex2i(x + size - thickness, y);
    glVertex2i(x + size, y + thickness);
    glVertex2i(x, y + size - thickness);
    glVertex2i(x + thickness, y + size);
    glEnd();
}
void drawTriangle(int x, int y, float radius) {
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + radius); // top vertex
    glVertex2f(x + radius * sin(4.18879), y - radius * cos(4.18879)); // bottom right vertex
    glVertex2f(x - radius * sin(4.18879), y - radius * cos(4.18879)); // bottom left vertex
    glEnd();
}


void drawFence() {
    glColor3f(1.0f, 0.0f, 0.0f); // Red color

    // Parameters for the triangles
    int triangleBase = 10; // base of each small triangle

    // Drawing fence with triangles inside
    // Top fence
    glRecti(0, WINDOW_HEIGHT - FENCE_WIDTH, WINDOW_WIDTH, WINDOW_HEIGHT); // Drawing the red quad
    glColor3f(0.0f, 0.0f, 0.0f); // Black color for the triangles
    for (int i = 0; i < WINDOW_WIDTH; i += triangleBase) {
        glBegin(GL_TRIANGLES);
        glVertex2i(i, WINDOW_HEIGHT);
        glVertex2i(i + triangleBase, WINDOW_HEIGHT);
        glVertex2i(i + triangleBase / 2, WINDOW_HEIGHT - FENCE_WIDTH);
        glEnd();
    }

    // Bottom fence
    glColor3f(1.0f, 0.0f, 0.0f);
    glRecti(0, 0, WINDOW_WIDTH, FENCE_WIDTH);
    glColor3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < WINDOW_WIDTH; i += triangleBase) {
        glBegin(GL_TRIANGLES);
        glVertex2i(i, 0);
        glVertex2i(i + triangleBase, 0);
        glVertex2i(i + triangleBase / 2, FENCE_WIDTH);
        glEnd();
    }

    // Left fence
    glColor3f(1.0f, 0.0f, 0.0f);
    glRecti(0, 0, FENCE_WIDTH, WINDOW_HEIGHT);
    glColor3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < WINDOW_HEIGHT; i += triangleBase) {
        glBegin(GL_TRIANGLES);
        glVertex2i(0, i);
        glVertex2i(FENCE_WIDTH, i + triangleBase / 2);
        glVertex2i(0, i + triangleBase);
        glEnd();
    }

    // Right fence
    glColor3f(1.0f, 0.0f, 0.0f);
    glRecti(WINDOW_WIDTH - FENCE_WIDTH, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glColor3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < WINDOW_HEIGHT; i += triangleBase) {
        glBegin(GL_TRIANGLES);
        glVertex2i(WINDOW_WIDTH, i);
        glVertex2i(WINDOW_WIDTH - FENCE_WIDTH, i + triangleBase / 2);
        glVertex2i(WINDOW_WIDTH, i + triangleBase);
        glEnd();
    }
}

const int GOAL_SIZE = 50; // You can adjust the size of the goal triangle

void drawGoal() {

    glPushMatrix(); // Push the current matrix onto the stack
    glTranslatef(FENCE_WIDTH + GOAL_SIZE / 2, WINDOW_HEIGHT - FENCE_WIDTH - GOAL_SIZE / 2, 0.0f); // Translate to the center of the goal
    glRotatef(goalAngle, 0.0f, 0.0f, 1.0f); // Rotate around the z-axis
    glTranslatef(-FENCE_WIDTH - GOAL_SIZE / 2, -WINDOW_HEIGHT + FENCE_WIDTH + GOAL_SIZE / 2, 0.0f); // Translate back to the original position

    glColor3f(0, 0, 1); // Blue color
    // Drawing outer blue square
    glBegin(GL_QUADS);
    glVertex2i(FENCE_WIDTH, WINDOW_HEIGHT - FENCE_WIDTH);
    glVertex2i(FENCE_WIDTH, WINDOW_HEIGHT - FENCE_WIDTH - GOAL_SIZE);
    glVertex2i(FENCE_WIDTH + GOAL_SIZE, WINDOW_HEIGHT - FENCE_WIDTH - GOAL_SIZE);
    glVertex2i(FENCE_WIDTH + GOAL_SIZE, WINDOW_HEIGHT - FENCE_WIDTH);
    glEnd();

    // Setting coordinates relative to the blue square
    int houseBaseX = FENCE_WIDTH + GOAL_SIZE / 4;
    int houseBaseY = WINDOW_HEIGHT - FENCE_WIDTH - GOAL_SIZE / 4 * 3;
    int houseWidth = GOAL_SIZE / 2;
    int houseHeight = GOAL_SIZE / 2;

    glColor3f(0, 1, 0); // Green color

    // Drawing the main square part of the house
    glBegin(GL_QUADS);
    glVertex2i(houseBaseX, houseBaseY);
    glVertex2i(houseBaseX, houseBaseY + houseHeight);
    glVertex2i(houseBaseX + houseWidth, houseBaseY + houseHeight);
    glVertex2i(houseBaseX + houseWidth, houseBaseY);
    glEnd();

    glColor3f(1, 0, 0); // Red color
    // Drawing the roof as a triangle
    glBegin(GL_TRIANGLES);
    glVertex2i(houseBaseX, houseBaseY + houseHeight);
    glVertex2i(houseBaseX + houseWidth, houseBaseY + houseHeight);
    glVertex2i(houseBaseX + houseWidth / 2, houseBaseY + houseHeight + houseHeight / 2);
    glEnd();

    glColor3f(1, 1, 0); // Yellow color
    // Drawing the door as a rectangle
    glBegin(GL_QUADS);
    glVertex2i(houseBaseX + houseWidth / 4, houseBaseY);
    glVertex2i(houseBaseX + houseWidth / 4, houseBaseY + houseHeight / 2);
    glVertex2i(houseBaseX + houseWidth / 4 * 3, houseBaseY + houseHeight / 2);
    glVertex2i(houseBaseX + houseWidth / 4 * 3, houseBaseY);
    glEnd();

    glColor3f(0, 0, 0); // Black color
    // Drawing two lines as a cross on the door
    glBegin(GL_LINES);
    glVertex2i(houseBaseX + houseWidth / 4, houseBaseY);
    glVertex2i(houseBaseX + houseWidth / 4 * 3, houseBaseY + houseHeight / 2);
    glVertex2i(houseBaseX + houseWidth / 4 * 3, houseBaseY);
    glVertex2i(houseBaseX + houseWidth / 4, houseBaseY + houseHeight / 2);
    glEnd();

    glColor3f(1, 1, 1); // White color
    // Drawing a window as a polygon (hexagon in this case)
    glBegin(GL_POLYGON);
    int numSides = 6;
    float centerX = houseBaseX + houseWidth / 4 * 3;
    float centerY = houseBaseY + houseHeight / 4 * 3;
    float radius = houseWidth / 8;
    for (int i = 0; i < numSides; ++i) {
        float angle = i * (2 * 3.14159 / numSides);
        glVertex2f(centerX + radius * cos(angle), centerY + radius * sin(angle));
    }
    glEnd();
    glPopMatrix();
}
bool checkObjectOverlap(int x, int y, int size, int excludeIndex) {
    for (int i = 0; i < NUM_COLLECTABLES; i++) {
        if (i != excludeIndex &&
            x < collectableX[i] + COLLECTABLE_SIZE &&
            x + size > collectableX[i] &&
            y < collectableY[i] + COLLECTABLE_SIZE &&
            y + size > collectableY[i]) {
            return true;
        }
    }

    for (int i = 0; i < NUM_OBSTACLES; i++) {
        if (i != excludeIndex &&
            x < obstacleX[i] + OBSTACLE_SIZE &&
            x + size > obstacleX[i] &&
            y < obstacleY[i] + OBSTACLE_SIZE &&
            y + size > obstacleY[i]) {
            return true;
        }
    }


    return false;
}


// void drawDisappear() {
//     if (disappearActive) {
//         glColor3f(0, 0, 1); // Yellow color
//         glRecti(disappearX, disappearY, disappearX + DISAPPEAR_SIZE, disappearY + DISAPPEAR_SIZE);
//         glColor3f(1, 1, 0.5); // Light yellow color for cheese

//         glColor3f(1, 0, 0); // Dark red color for pepperoni
//         for (int i = 0; i < 4; i++) {
//             float angle = 2.0f * 3.1415926f * float(i) / 4.0f;
//             float dx = DISAPPEAR_SIZE / 4 * cosf(angle);
//             float dy = DISAPPEAR_SIZE / 4 * sinf(angle);
//             drawCircle(disappearX + DISAPPEAR_SIZE / 2 + dx,
//                 disappearY + DISAPPEAR_SIZE / 2 + dy,
//                 5, 10); // Small circles representing pepperoni
//         }
//     }
// }
void drawDisappear() {
    if (disappearActive) {
        // Increment the rotation angle on each frame
        disappearRotationAngle += 3.0f; // You can adjust the rotation speed

        // Ensure the rotation angle stays within the range [0, 360]
        if (disappearRotationAngle >= 360.0f) {
            disappearRotationAngle -= 360.0f;
        }

        glPushMatrix(); // Push the current matrix onto the stack

        // Calculate the center of the object
        float centerX = disappearX + DISAPPEAR_SIZE / 2;
        float centerY = disappearY + DISAPPEAR_SIZE / 2;

        // Translate to the center of the object
        glTranslatef(centerX, centerY, 0.0f);

        // Rotate around the Z-axis
        glRotatef(disappearRotationAngle, 0.0f, 0.0f, 1.0f);

        // Translate back to the original position
        glTranslatef(-centerX, -centerY, 0.0f);

        glColor3f(0, 0, 1); // Yellow color
        glRecti(disappearX, disappearY, disappearX + DISAPPEAR_SIZE, disappearY + DISAPPEAR_SIZE);
        glColor3f(1, 1, 0.5); // Light yellow color for cheese

        glColor3f(1, 0, 0); // Dark red color for pepperoni
        for (int i = 0; i < 4; i++) {
            float angle = 2.0f * 3.1415926f * float(i) / 4.0f;
            float dx = DISAPPEAR_SIZE / 4 * cosf(angle);
            float dy = DISAPPEAR_SIZE / 4 * sinf(angle);
            drawCircle(disappearX + DISAPPEAR_SIZE / 2 + dx,
                disappearY + DISAPPEAR_SIZE / 2 + dy,
                5, 10); // Small circles representing pepperoni
        }

        glPopMatrix(); // Restore the previous matrix from the stack
    }
}





void drawInnerCircle(int x, int y, float radius) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i++) {
        float degInRad = i * (3.14159 / 180);
        glVertex2f(cos(degInRad) * radius + x, sin(degInRad) * radius + y);
    }
    glEnd();
}

void drawTiltedZ() {
    int centerX = shrinkSpeedUpX + SHRINK_SPEEDUP_SIZE / 2;
    int centerY = shrinkSpeedUpY + SHRINK_SPEEDUP_SIZE / 2;
    int size = SHRINK_SPEEDUP_SIZE / 2; // Adjust the size of Z relative to your circle

    glBegin(GL_LINES);
    // Top-left to top-right
    glVertex2f(centerX - size / 2, centerY + size / 2);
    glVertex2f(centerX + size / 2, centerY + size / 2);

    // Top-right to bottom-left (tilted line)
    glVertex2f(centerX + size / 2, centerY + size / 2);
    glVertex2f(centerX - size / 2, centerY - size / 2);

    // Bottom-left to bottom-right
    glVertex2f(centerX - size / 2, centerY - size / 2);
    glVertex2f(centerX + size / 2, centerY - size / 2);
    glEnd();
}
// void drawShrinkSpeedUp() {
//     if (shrinkSpeedUpActive) {
//         glColor3f(0.5f, 0, 0.5f); // Purple color
//         glRecti(shrinkSpeedUpX, shrinkSpeedUpY, shrinkSpeedUpX + SHRINK_SPEEDUP_SIZE, shrinkSpeedUpY + SHRINK_SPEEDUP_SIZE);

//         // Draw circle
//         glColor3f(1, 1, 1); // White color
//         drawInnerCircle(shrinkSpeedUpX + SHRINK_SPEEDUP_SIZE / 2, shrinkSpeedUpY + SHRINK_SPEEDUP_SIZE / 2, SHRINK_SPEEDUP_SIZE / 2);

//         // Draw tilted Z
//         glColor3f(1, 0, 0); // Red color
//         drawTiltedZ();
//     }
// }

void drawShrinkSpeedUp() {
    if (shrinkSpeedUpActive) {
        // Update the rotation angle continuously
        shrinkSpeedUpRotationAngle += 5.0f; // You can adjust the rotation speed
        //drawTiltedZ();
        glColor3f(0.5f, 0, 0.5f); // Purple color
        //drawTiltedZ();
        glPushMatrix();
        //drawTiltedZ();
        glTranslatef(shrinkSpeedUpX + SHRINK_SPEEDUP_SIZE / 2, shrinkSpeedUpY + SHRINK_SPEEDUP_SIZE / 2, 0.0f);
        glRotatef(shrinkSpeedUpRotationAngle, 0.0f, 0.0f, 1.0f); // Rotate around the z-axis
        glTranslatef(-SHRINK_SPEEDUP_SIZE / 2, -SHRINK_SPEEDUP_SIZE / 2, 0.0f);
        //drawTiltedZ();

        // Draw the shrink power-up
        glRecti(0, 0, SHRINK_SPEEDUP_SIZE, SHRINK_SPEEDUP_SIZE);
        //drawTiltedZ();

        glPopMatrix();
        //glColor3f(1, 1, 1);
        //drawInnerCircle(SHRINK_SPEEDUP_SIZE / 2, SHRINK_SPEEDUP_SIZE / 2, SHRINK_SPEEDUP_SIZE / 2);
        //drawTiltedZ();
        glColor3f(1, 0, 0); // Red color
        drawTiltedZ();

        glPopMatrix();
    }
}


void drawPlayer() {
    glColor3f(0, 0, 1); // Blue color for square
    glRecti(playerX, playerY, playerX + PLAYER_SIZE, playerY + PLAYER_SIZE);

    // Drawing a circle inside the player square
    glColor3f(1, 1, 1); // White color for circle
    drawCircle(playerX + PLAYER_SIZE / 2, playerY + PLAYER_SIZE / 2, PLAYER_SIZE / 2, 100);
    int centerX = playerX + PLAYER_SIZE / 2;
    int centerY = playerY + PLAYER_SIZE / 2;
    float radius = PLAYER_SIZE / 2;
    glColor3f(0, 0, 0); // Green color
    drawTriangle(centerX, centerY, radius / sqrt(3));
    glColor3f(0, 0, 0); // Black color for sunglasses
    int eyeWidth = radius / 2;
    int eyeHeight = radius / 4;
    int eyeSpacing = eyeWidth / 1.25;

    glColor3f(0, 0, 0); // Black color for eyes
    drawRect(centerX - eyeSpacing / 2 - eyeWidth, playerY + radius / 2, eyeWidth, eyeHeight);  // Left eye
    drawRect(centerX + eyeSpacing / 2, playerY + radius / 2, eyeWidth, eyeHeight);            // Right eye
}


void drawObstacles() {
    if (!obstaclesDisappeared) {
        glColor3f(1, 0, 0); // Red color
        for (int i = 0; i < NUM_OBSTACLES; i++) {
            glRecti(obstacleX[i], obstacleY[i], obstacleX[i] + OBSTACLE_SIZE, obstacleY[i] + OBSTACLE_SIZE);
            glColor3f(0, 0, 0);
            drawX(obstacleX[i], obstacleY[i], OBSTACLE_SIZE);
            glColor3f(1, 0, 0);
            //checkObjectOverlap(obstacleX[i], obstacleY[i], OBSTACLE_SIZE, i);

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
    if (obstaclesDisappeared) { // If obstacles are disappeared, skip their collision check
        // You can add more logic here if you want to handle other collisions during disappearance
    }
    else if (checkCollision() || playerX <= FENCE_WIDTH || playerX + PLAYER_SIZE >= WINDOW_WIDTH - FENCE_WIDTH ||
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

    if (checkGoalCollision()) {
        drawWinScreen();
    }
}

void initializeObjects() {
    srand(time(0)); // Seed random number generator
    int i = 0;
    // Initialize collectables
    for (int i = 0; i < NUM_COLLECTABLES; i++) {
        do {
            collectableX[i] = FENCE_WIDTH + rand() % (WINDOW_WIDTH - COLLECTABLE_SIZE - 2 * FENCE_WIDTH);
            collectableY[i] = FENCE_WIDTH + rand() % (WINDOW_HEIGHT - COLLECTABLE_SIZE - 2 * FENCE_WIDTH);
        } while (checkObjectOverlap(collectableX[i], collectableY[i], COLLECTABLE_SIZE, i));
    }
    do {
        disappearX = FENCE_WIDTH + rand() % (WINDOW_WIDTH - DISAPPEAR_SIZE - 2 * FENCE_WIDTH);
        disappearY = FENCE_WIDTH + rand() % (WINDOW_HEIGHT - DISAPPEAR_SIZE - 2 * FENCE_WIDTH);
    } while (checkObjectOverlap(disappearX, disappearY, DISAPPEAR_SIZE, i));

    do {
        shrinkSpeedUpX = FENCE_WIDTH + rand() % (WINDOW_WIDTH - SHRINK_SPEEDUP_SIZE - 2 * FENCE_WIDTH);
        shrinkSpeedUpY = FENCE_WIDTH + rand() % (WINDOW_HEIGHT - SHRINK_SPEEDUP_SIZE - 2 * FENCE_WIDTH);
    } while (checkObjectOverlap(shrinkSpeedUpX, shrinkSpeedUpY, SHRINK_SPEEDUP_SIZE, i));

    // Initialize obstacles
    /*for (int i = 0; i < NUM_OBSTACLES; i++) {
        do {
            obstacleX[i] = FENCE_WIDTH + rand() % (WINDOW_WIDTH - OBSTACLE_SIZE - 2 * FENCE_WIDTH);
            obstacleY[i] = FENCE_WIDTH + rand() % (WINDOW_HEIGHT - OBSTACLE_SIZE - 2 * FENCE_WIDTH);
        } while (checkObjectOverlap(obstacleX[i], obstacleY[i], OBSTACLE_SIZE, -1));
    }*/
}



void drawPlusSign(int x, int y, int size) {
    int thickness = size / 5; // Adjust the thickness of the "+" lines

    // Drawing the horizontal part of the "+"
    glColor3f(0, 0, 0); // Black color for the "+" sign
    glBegin(GL_QUADS);
    glVertex2i(x + thickness, y + size / 2 - thickness / 2);
    glVertex2i(x + size - thickness, y + size / 2 - thickness / 2);
    glVertex2i(x + size - thickness, y + size / 2 + thickness / 2);
    glVertex2i(x + thickness, y + size / 2 + thickness / 2);
    glEnd();

    // Drawing the vertical part of the "+"
    glBegin(GL_QUADS);
    glVertex2i(x + size / 2 - thickness / 2, y + thickness);
    glVertex2i(x + size / 2 + thickness / 2, y + thickness);
    glVertex2i(x + size / 2 + thickness / 2, y + size - thickness);
    glVertex2i(x + size / 2 - thickness / 2, y + size - thickness);
    glEnd();
}
void drawCircle(int x, int y, int size) {
    const int segments = 100; // Number of segments to approximate a circle
    const double angleIncrement = 2.0 * 3.18 / segments;

    glColor3f(0, 0.75, 0); // Blue color for the circle
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; ++i) {
        double angle = i * angleIncrement;
        glVertex2i(x + size / 2 + cos(angle) * size / 2,
            y + size / 2 + sin(angle) * size / 2);
    }
    glEnd();
}

// void drawCollectables() {
//     for (int i = 0; i < NUM_COLLECTABLES; i++) {
//         glColor3f(0, 1, 0); // Green color for the square
//         drawRect(collectableX[i], collectableY[i], COLLECTABLE_SIZE, COLLECTABLE_SIZE); // Drawing the green square
//         drawCircle(collectableX[i], collectableY[i], COLLECTABLE_SIZE);
//         // Drawing the "+" sign inside the green square
//         drawPlusSign(collectableX[i], collectableY[i], COLLECTABLE_SIZE);
//         //drawCircle(collectableX[i], collectableY[i], COLLECTABLE_SIZE);
//     }
// }

void drawCollectables() {
    for (int i = 0; i < NUM_COLLECTABLES; i++) {
        glPushMatrix(); // Push the current matrix onto the stack
        glTranslatef(collectableX[i] + COLLECTABLE_SIZE / 2, collectableY[i] + COLLECTABLE_SIZE / 2, 0.0f);
        glRotatef(collectableRotationAngle[i], 0.0f, 0.0f, 1.0f); // Rotate around the z-axis
        glTranslatef(-COLLECTABLE_SIZE / 2, -COLLECTABLE_SIZE / 2, 0.0f);

        glColor3f(0, 1, 0); // Green color for the square
        drawRect(0, 0, COLLECTABLE_SIZE, COLLECTABLE_SIZE); // Drawing the green square
        drawCircle(0, 0, COLLECTABLE_SIZE);
        // Drawing the "+" sign inside the green square
        drawPlusSign(0, 0, COLLECTABLE_SIZE);

        glPopMatrix(); // Pop the matrix from the stack to undo the transformations
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
            collectableX[i] = -WINDOW_WIDTH; // Set the X position outside the window
            collectableY[i] = -WINDOW_HEIGHT;
            score++;
        }
    }
}
void Disappeartimer(int value) {
    updateDisappearEffect(); // Updating the disappear effect
    glutTimerFunc(1000, Disappeartimer, 0); // Calling this function again after 1000 milliseconds
}
void ShrinkSpeedUptimer(int value) {
    updateShrinkSpeedUpEffect();
    glutTimerFunc(1000, ShrinkSpeedUptimer, 0);
}



void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w': // Move up
        playerY -= PLAYER_STEP;
        playerAngle = 0.0f;
        break;
    case 'a': // Move left
        playerX -= PLAYER_STEP;
        playerAngle = -90.0f;
        break;
    case 's': // Move down
        playerY += PLAYER_STEP;
        playerAngle = 180.0f;
        break;
    case 'd': // Move right
        playerX += PLAYER_STEP;
        playerAngle = 90.0f; // Rotate the player to the right by 90 degrees
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

    // Increment the rotation angle

    rotationAngle += 3.0f; // Adjust the rotation speed as needed

    // Draw the grey square as the background with rotation
    glColor3f(0.5f, 0.5f, 0.5f); // Grey color
    int backgroundSize = 300; // Size of the grey square
    int backgroundX = (WINDOW_WIDTH - backgroundSize) / 2;
    int backgroundY = (WINDOW_HEIGHT - backgroundSize) / 2;

    glPushMatrix(); // Push the current matrix onto the stack
    glTranslatef(backgroundX + backgroundSize / 2, backgroundY + backgroundSize / 2, 0.0f); // Translate to the center of the square
    glRotatef(rotationAngle, 0.0f, 0.0f, 0.001f); // Rotate around the z-axis
    glTranslatef(-backgroundX - backgroundSize / 2, -backgroundY - backgroundSize / 2, 0.0f); // Translate back to the original position

    glBegin(GL_QUADS);
    glVertex2i(backgroundX, backgroundY);
    glVertex2i(backgroundX + backgroundSize, backgroundY);
    glVertex2i(backgroundX + backgroundSize, backgroundY + backgroundSize);
    glVertex2i(backgroundX, backgroundY + backgroundSize);
    glEnd();

    glPopMatrix(); // Pop the matrix back to the previous state

    drawFence();

    glPushMatrix(); // Push the current matrix onto the stack
    glTranslatef(playerX + PLAYER_SIZE / 2, playerY + PLAYER_SIZE / 2, 0.0f); // Translate to the center of the player square
    glRotatef(playerAngle, 0.0f, 0.0f, 1.0f); // Rotate around the z-axis
    glTranslatef(-playerX - PLAYER_SIZE / 2, -playerY - PLAYER_SIZE / 2, 0.0f); // Translate back to the original position

    drawPlayer(); // Draw the player square

    glPopMatrix(); // Pop the matrix back to the previous state

    drawObstacles();
    drawCollectables();
    for (int i = 0; i < NUM_COLLECTABLES; i++) {
        collectableRotationAngle[i] += 3.0f; // Increment the rotation angle
        if (collectableRotationAngle[i] >= 360.0f) {
            collectableRotationAngle[i] -= 360.0f; // Keep the angle within [0, 360)
        }
    }
    drawHealthBar();
    drawScore();
    goalAngle += 3.0f;
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
    glutTimerFunc(1000, Disappeartimer, 0);
    glutTimerFunc(1000, ShrinkSpeedUptimer, 0);
    //initializeCollectables();
    initializeObjects();
    //initializeDisappear();
    //initializeShrinkSpeedUp();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
