//
// Created by ounols on 19. 8. 7.
//

#pragma once


class TouchNode {
public:
    TouchNode();

    ~TouchNode();

    void SetPosition(int x, int y) {
        this->x = x;
        this->y = y;
    }

    int GetX() const {
        return x;
    }

    int GetY() const {
        return y;
    }

    int getNum() const;

    void setNum(int num);

    float getTime() const;

    void setTime(float time);

private:
    int x = -1;
    int y = -1;

    int num = 0;
    float time = 0;

};



