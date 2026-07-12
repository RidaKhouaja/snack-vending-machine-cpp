#include "Automat.h"
#include <iostream>
#include <iomanip>

Automat::Automat(int numSlots, int numProductsPerSlot)
    : numSlot(numSlots), numProductsPerSlot(numProductsPerSlot), cached(nullptr) {
    m_slots = new Slot*[numSlot];
    for (int i = 0; i < numSlot; ++i) {
        m_slots[i] = nullptr;
    }
}

Automat::~Automat() {
    for (int i = 0; i < numSlot; ++i) {
        delete m_slots[i];
    }
    delete[] m_slots;
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
        if (m_slots[i] == nullptr) {
            m_slots[i] = slot;
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
        if (m_slots[i] != nullptr && m_slots[i]->getId() == slotId) {
            cached = m_slots[i];
            return m_slots[i];
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
        if (m_slots[i] != nullptr) {
            m_slots[i]->setNumProducts(numProductsPerSlot);
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
        if (m_slots[i] != nullptr) {
            empty = false;
            std::cout << "| " << std::setw(4) << m_slots[i]->getId() << " | "
                       << std::setw(20) << std::left << m_slots[i]->getProductName()
                       << std::right << " | " << std::setw(6) << m_slots[i]->getPrice()
                       << "   | " << std::setw(7) << m_slots[i]->getNumProducts() << " |\n";
        }
    }
    if (empty) {
        std::cout << "|            (aucun emplacement configure)          |\n";
    }
    std::cout << "+------+----------------------+----------+---------+\n";
}

std::vector<SlotInfo> Automat::getAllSlotsInfo() const {
    std::vector<SlotInfo> infos;
    for (int i = 0; i < numSlot; ++i) {
        if (m_slots[i] != nullptr) {
            infos.push_back({m_slots[i]->getId(), m_slots[i]->getProductName(),
                              m_slots[i]->getPrice(), m_slots[i]->getNumProducts()});
        }
    }
    return infos;
}

int Automat::selectionnerProduit() {
    return keypad.getSelection();
}

int Automat::insererPiece() {
    return coinSlot.updateCoinAmount();
}

bool Automat::essayerAjouterPiece(int valeur) {
    return coinSlot.ajouterPiece(valeur);
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

std::string Automat::rendreMonnaieText(int prix) {
    return coinSlot.returnCoinsText(prix);
}