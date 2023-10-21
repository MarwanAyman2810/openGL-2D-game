#include <Windows.h>
#include <glut.h>
#include <ctime>
#include <cstdlib>
#include <string>

// Window size
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Player properties
const int PLAYER_SIZE = 30;
const int PLAYER_STEP = 10;
int playerX = WINDOW_WIDTH / 2;
int playerY = WINDOW_HEIGHT / 2;
int playerHealth = 2; // Full health

// Obstacle properties
const int OBSTACLE_SIZE = 30;
const int NUM_OBSTACLES = 6;
int obstacleX[NUM_OBSTACLES] = { 100, 300, 500, 200, 400, 600 };
int obstacleY[NUM_OBSTACLES] = { 100, 200, 300, 400, 500, 100 };


// Collectable properties
const int COLLECTABLE_SIZE = 30;
const int NUM_COLLECTABLES = 5;
int collectableX[NUM_COLLECTABLES];
int collectableY[NUM_COLLECTABLES];

// Score
int score = 0;



void drawPlayer() {
    glColor3f(0, 0, 1); // Blue color
    glRecti(playerX, playerY, playerX + PLAYER_SIZE, playerY + PLAYER_SIZE);
}

void drawObstacles() {
    glColor3f(1, 0, 0); // Red color
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        glRecti(obstacleX[i], obstacleY[i], obstacleX[i] + OBSTACLE_SIZE, obstacleY[i] + OBSTACLE_SIZE);
    }
}

void drawHealthBar() {
    glColor3f(0, 1, 0); // Green color
    glRecti(10, 10, 10 + (200 * playerHealth), 30);
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

void handleCollision() {
    if (checkCollision()) {
        playerHealth--;
        // Reset player position
        playerX = WINDOW_WIDTH / 2;
        playerY = WINDOW_HEIGHT / 2;
        if (playerHealth <= 0) {
            // Game over logic here
            exit(0);
        }
    }
}
void initializeCollectables() {
    srand(time(0)); // Seed random number generator
    for (int i = 0; i < NUM_COLLECTABLES; i++) {
        collectableX[i] = rand() % (WINDOW_WIDTH - COLLECTABLE_SIZE);
        collectableY[i] = rand() % (WINDOW_HEIGHT - COLLECTABLE_SIZE);
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
    glRasterPos2i(WINDOW_WIDTH / 2 - 10, 30);
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
    glutPostRedisplay();
}

void display() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // White background
    glClear(GL_COLOR_BUFFER_BIT);

    drawPlayer();
    drawObstacles();
    drawCollectables();
    drawHealthBar();
    drawScore();

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
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
