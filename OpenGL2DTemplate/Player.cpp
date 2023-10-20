
#include <windows.h>
#include <glut.h>
#include "Player.h"
#include <cmath>

Player::Player() : x(0.0f), y(0.0f) {}

void Player::display() {
    const float HEAD_RADIUS = 0.02;
    const float BODY_LENGTH = 0.1;
    const float ARM_LENGTH = 0.05;
    const float LEG_LENGTH = 0.05;
    const float SHOE_WIDTH = 0.02;

    glClear(GL_COLOR_BUFFER_BIT);

    //float x = 0.0, y = 0.0;

    // Drawing the head
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0, 0.5, 0.5);
    glVertex2f(x, y + BODY_LENGTH);
    for (int i = 0; i <= 360; i++) {
        float theta = i * 3.142 / 180;
        glVertex2f(HEAD_RADIUS * cos(theta) + x, HEAD_RADIUS * sin(theta) + y + BODY_LENGTH);
    }
    glEnd();

    // Drawing sunglasses
    glBegin(GL_QUADS);
    glColor3f(0.0, 0.0, 0.0);
    glVertex2f(x - HEAD_RADIUS / 2, y + BODY_LENGTH + HEAD_RADIUS / 2);
    glVertex2f(x + HEAD_RADIUS / 2, y + BODY_LENGTH + HEAD_RADIUS / 2);
    glVertex2f(x + HEAD_RADIUS / 2, y + BODY_LENGTH + HEAD_RADIUS / 3);
    glVertex2f(x - HEAD_RADIUS / 2, y + BODY_LENGTH + HEAD_RADIUS / 3);
    glEnd();

    // Drawing the body
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);
    glVertex2f(x, y + BODY_LENGTH - HEAD_RADIUS);
    glVertex2f(x, y);
    glEnd();

    // Drawing arms
    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0);
    glVertex2f(x, y + BODY_LENGTH - HEAD_RADIUS - ARM_LENGTH);
    glVertex2f(x - ARM_LENGTH, y + BODY_LENGTH - HEAD_RADIUS - ARM_LENGTH);
    glVertex2f(x, y + BODY_LENGTH - HEAD_RADIUS - ARM_LENGTH);
    glVertex2f(x + ARM_LENGTH, y + BODY_LENGTH - HEAD_RADIUS - ARM_LENGTH);
    glEnd();

    // Drawing legs
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex2f(x, y);
    glVertex2f(x - LEG_LENGTH / 2, y - LEG_LENGTH);
    glVertex2f(x, y);
    glVertex2f(x + LEG_LENGTH / 2, y - LEG_LENGTH);
    glEnd();

    // Drawing shoes
    glBegin(GL_QUADS);
    glColor3f(0.5, 0.35, 0.05);
    glVertex2f(x - LEG_LENGTH / 2, y - LEG_LENGTH);
    glVertex2f(x - LEG_LENGTH / 2 - SHOE_WIDTH, y - LEG_LENGTH);
    glVertex2f(x - LEG_LENGTH / 2 - SHOE_WIDTH, y - LEG_LENGTH - 0.01);
    glVertex2f(x - LEG_LENGTH / 2, y - LEG_LENGTH - 0.01);

    glVertex2f(x + LEG_LENGTH / 2, y - LEG_LENGTH);
    glVertex2f(x + LEG_LENGTH / 2 + SHOE_WIDTH, y - LEG_LENGTH);
    glVertex2f(x + LEG_LENGTH / 2 + SHOE_WIDTH, y - LEG_LENGTH - 0.01);
    glVertex2f(x + LEG_LENGTH / 2, y - LEG_LENGTH - 0.01);
    glEnd();

    glutSwapBuffers();
}





void Player::move(char direction) {
    float nextX = x;
    float nextY = y;

    switch (direction) {
    case 'w': nextY = y + 0.1f; break;
    case 's': nextY = y - 0.1f; break;
    case 'a': nextX = x - 0.1f; break;
    case 'd': nextX = x + 0.1f; break;
    }

    // Check boundaries before actually moving the player.
    // Assuming the window boundaries are -1 and 1.
    // You might need to adjust these values based on your setup.
    if (nextX > -1 && nextX < 1) {
        x = nextX;
    }
    if (nextY > -1 && nextY < 1) {
        y = nextY;
    }

}
