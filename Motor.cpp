#include "Motor.h"
#include <iostream>

Motor::Motor(int id) : id(id) {}

void Motor::trigger() const {
    std::cout << "   [Moteur " << id << "] rotation de la spirale..." << std::endl;
}

int Motor::getId() const {
    return id;
}
