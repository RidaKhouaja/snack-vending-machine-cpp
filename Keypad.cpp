#include "Keypad.h"
#include <iostream>
#include <limits>

int Keypad::getSelection() const {
    int selection;
    std::cout << "Numero de l'emplacement souhaite (-1 pour annuler) : ";
    std::cin >> selection;

    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Saisie invalide.\n";
        return getSelection();
    }

    return selection;
}
