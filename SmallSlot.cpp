#include "SmallSlot.h"

SmallSlot::SmallSlot(int slotId, const std::string& productName, int price, int motorId)
    : Slot(slotId, productName, price), motor(new Motor(motorId)) {}

SmallSlot::~SmallSlot() {
    delete motor;
}

void SmallSlot::drop() {
    motor->trigger();
    decrementerStock();
}
