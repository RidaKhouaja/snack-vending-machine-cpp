#include "Slot.h"

Slot::Slot(int slotId, const std::string& productName, int price)
    : productName(productName), id(slotId), numProduits(0), prix(price) {}

Slot::~Slot() {}

void Slot::decrementerStock() {
    if (numProduits > 0) {
        numProduits--;
    }
}

std::string Slot::getProductName() const {
    return productName;
}

void Slot::setProductName(const std::string& name) {
    productName = name;
}

int Slot::getNumProducts() const {
    return numProduits;
}

void Slot::setNumProducts(int num) {
    numProduits = num;
}

int Slot::getPrice() const {
    return prix;
}

void Slot::setPrice(int price) {
    prix = price;
}

int Slot::getId() const {
    return id;
}
