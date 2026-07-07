#ifndef WIDESLOT_H
#define WIDESLOT_H

#include "Slot.h"
#include "Motor.h"

/**
 * WideSlot : emplacement "large" equipe de deux moteurs en spirale
 * (necessaire pour les produits plus lourds/volumineux).
 */
class WideSlot : public Slot {
private:
    Motor* motor1;
    Motor* motor2;

public:
    WideSlot(int slotId, const std::string& productName, int price, int motorId1, int motorId2);
    ~WideSlot() override;

    void drop() override;
};

#endif // WIDESLOT_H
