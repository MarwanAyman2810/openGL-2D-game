#ifndef PLAYER_H
#define PLAYER_H

class Player {
public:
    float x, y;

    Player();
    void display();
    void move(char direction);
private:
    const float HEAD_RADIUS = 0.05;
    const float BODY_LENGTH = 0.3;
    const float ARM_LENGTH = 0.05;
    const float LEG_LENGTH = 0.1;
    const float SHOE_WIDTH = 0.03;
};

#endif // PLAYER_H

