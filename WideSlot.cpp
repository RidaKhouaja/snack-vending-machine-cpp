#include "WideSlot.h"

WideSlot::WideSlot(int slotId, const std::string& productName, int price, int motorId1, int motorId2)
    : Slot(slotId, productName, price), motor1(new Motor(motorId1)), motor2(new Motor(motorId2)) {}

WideSlot::~WideSlot() {
    delete motor1;
    delete motor2;
}

void WideSlot::drop() {
    motor1->trigger();
    motor2->trigger();
    decrementerStock();
}
