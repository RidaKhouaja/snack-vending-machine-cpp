#include "Automat.h"
#include <iostream>
#include <iomanip>

Automat::Automat(int numSlots, int numProductsPerSlot)
    : numSlot(numSlots), numProductsPerSlot(numProductsPerSlot), cached(nullptr) {
    slots = new Slot*[numSlot];
    for (int i = 0; i < numSlot; ++i) {
        slots[i] = nullptr;
    }
}

Automat::~Automat() {
    for (int i = 0; i < numSlot; ++i) {
        delete slots[i];
    }
    delete[] slots;
}

bool Automat::addSlot(Slot* slot) {
    if (slot == nullptr) {
        return false;
    }
    if (searchSlot(slot->getId()) != nullptr) {
        std::cout << "Erreur : l'emplacement " << slot->getId() << " existe deja.\n";
        delete slot;
        return false;
    }
    for (int i = 0; i < numSlot; ++i) {
        if (slots[i] == nullptr) {
            slots[i] = slot;
            return true;
        }
    }
    std::cout << "Erreur : l'automate est plein, impossible d'ajouter un emplacement.\n";
    delete slot;
    return false;
}

bool Automat::addSlot(const std::string& productName, int price, int slotId, int motorId) {
    return addSlot(new SmallSlot(slotId, productName, price, motorId));
}

bool Automat::addSlot(const std::string& productName, int price, int slotId, int motorId1, int motorId2) {
    return addSlot(new WideSlot(slotId, productName, price, motorId1, motorId2));
}

Slot* Automat::searchSlot(int slotId) {
    if (cached != nullptr && cached->getId() == slotId) {
        return cached;
    }
    for (int i = 0; i < numSlot; ++i) {
        if (slots[i] != nullptr && slots[i]->getId() == slotId) {
            cached = slots[i];
            return slots[i];
        }
    }
    return nullptr;
}

void Automat::changeSlot(int slotId, const std::string& name, int price) {
    Slot* s = searchSlot(slotId);
    if (s != nullptr) {
        s->setProductName(name);
        s->setPrice(price);
    } else {
        std::cout << "Erreur : emplacement " << slotId << " introuvable.\n";
    }
}

int Automat::getPieces(int slotId) {
    Slot* s = searchSlot(slotId);
    return (s != nullptr) ? s->getNumProducts() : 0;
}

bool Automat::isAvailable(int slotId) {
    Slot* s = searchSlot(slotId);
    return (s != nullptr) && (s->getNumProducts() > 0);
}

int Automat::getPrice(int slotId) {
    Slot* s = searchSlot(slotId);
    return (s != nullptr) ? s->getPrice() : -1;
}

bool Automat::dropSlot(int slotId) {
    Slot* s = searchSlot(slotId);
    if (s == nullptr || s->getNumProducts() <= 0) {
        return false;
    }
    s->drop(); // appel polymorphe : SmallSlot ou WideSlot
    return true;
}

void Automat::fillAll() {
    for (int i = 0; i < numSlot; ++i) {
        if (slots[i] != nullptr) {
            slots[i]->setNumProducts(numProductsPerSlot);
        }
    }
}

void Automat::fill(int slotId, int numProducts) {
    Slot* s = searchSlot(slotId);
    if (s != nullptr) {
        int newTotal = s->getNumProducts() + numProducts;
        if (newTotal > numProductsPerSlot) {
            newTotal = numProductsPerSlot;
        }
        s->setNumProducts(newTotal);
    } else {
        std::cout << "Erreur : emplacement " << slotId << " introuvable.\n";
    }
}

int Automat::getNumSlot() const {
    return numSlot;
}

void Automat::displaySlots() const {
    std::cout << "\n+------+----------------------+----------+---------+\n";
    std::cout << "|  N   | Produit              | Prix     | Stock   |\n";
    std::cout << "+------+----------------------+----------+---------+\n";
    bool empty = true;
    for (int i = 0; i < numSlot; ++i) {
        if (slots[i] != nullptr) {
            empty = false;
            std::cout << "| " << std::setw(4) << slots[i]->getId() << " | "
                       << std::setw(20) << std::left << slots[i]->getProductName()
                       << std::right << " | " << std::setw(6) << slots[i]->getPrice()
                       << "   | " << std::setw(7) << slots[i]->getNumProducts() << " |\n";
        }
    }
    if (empty) {
        std::cout << "|            (aucun emplacement configure)          |\n";
    }
    std::cout << "+------+----------------------+----------+---------+\n";
}

int Automat::selectionnerProduit() {
    return keypad.getSelection();
}

int Automat::insererPiece() {
    return coinSlot.updateCoinAmount();
}

int Automat::getMontantInsere() const {
    return coinSlot.getCoinAmount();
}

void Automat::reinitialiserMontant() {
    coinSlot.clear();
}

bool Automat::verifierChute() {
    return dropCheck.productReleased();
}

void Automat::rendreMonnaie(int prix) {
    coinSlot.returnCoins(prix);
}
