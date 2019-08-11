//
// Created by ounols on 19. 8. 7.
//

#include "TouchNode.h"

TouchNode::TouchNode() {

}

TouchNode::~TouchNode() {

}

int TouchNode::getNum() const {
    return num;
}

void TouchNode::setNum(int num) {
    TouchNode::num = num;
}

float TouchNode::getTime() const {
    return time;
}

void TouchNode::setTime(float time) {
    TouchNode::time = time;
}
