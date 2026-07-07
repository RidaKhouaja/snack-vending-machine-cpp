#include "DropCheck.h"
#include <cstdlib>

bool DropCheck::productReleased() const {
    int r = rand() % 100; // valeur entre 0 et 99
    return r >= 10;       // 10% (0-9) => false, 90% (10-99) => true
}
