#include "Player.h"
#include <windows.h>
#include <glut.h>
#include "Player.h"

Player player;

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    player.display();
}

void processNormalKeys(unsigned char key, int xx, int yy) {
    player.move(key);
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000, 1000);
    //glutInitWindowPosition(1000, 1000);
    glutCreateWindow("Movable Stickman");
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutKeyboardFunc(processNormalKeys);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    //gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glutMainLoop();
    return 0;
}
