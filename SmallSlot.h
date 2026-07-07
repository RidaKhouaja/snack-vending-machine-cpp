#ifndef SMALLSLOT_H
#define SMALLSLOT_H

#include "Slot.h"
#include "Motor.h"

/**
 * SmallSlot : emplacement "etroit" equipe d'un seul moteur en spirale.
 */
class SmallSlot : public Slot {
private:
    Motor* motor;

public:
    SmallSlot(int slotId, const std::string& productName, int price, int motorId);
    ~SmallSlot() override;

    void drop() override;
};

#endif // SMALLSLOT_H
