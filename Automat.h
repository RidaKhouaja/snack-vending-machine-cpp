#ifndef AUTOMAT_H
#define AUTOMAT_H

#include <string>
#include "Slot.h"
#include "SmallSlot.h"
#include "WideSlot.h"
#include "CoinSlot.h"
#include "Keypad.h"
#include "DropCheck.h"

/**
 * Classe Automat
 * Représente le distributeur dans son ensemble : le tableau d'emplacements
 * (SmallSlot ou WideSlot, manipulés de façon polymorphe via Slot*), ainsi que
 * les composants matériels simulés CoinSlot, Keypad et DropCheck (composition).
 */
class Automat {
private:
    Slot** slots;
    int numSlot;
    int numProductsPerSlot;
    Slot* cached;

    CoinSlot coinSlot;
    Keypad keypad;
    DropCheck dropCheck;

public:
    Automat(int numSlots, int numProductsPerSlot);
    ~Automat();

    // Version generique : fait le vrai travail d'insertion (verification
    // d'unicite de l'id + recherche d'une case libre). Les deux overloads
    // ci-dessous construisent le bon sous-type puis delegent ici, evitant
    // toute duplication de code.
    bool addSlot(Slot* slot);

    // Emplacement etroit : un seul moteur
    bool addSlot(const std::string& productName, int price, int slotId, int motorId);

    // Emplacement large : deux moteurs
    bool addSlot(const std::string& productName, int price, int slotId, int motorId1, int motorId2);

    Slot* searchSlot(int slotId);
    void changeSlot(int slotId, const std::string& name, int price);

    int getPieces(int slotId);
    bool isAvailable(int slotId);
    int getPrice(int slotId);
    bool dropSlot(int slotId);

    void fillAll();
    void fill(int slotId, int numProducts);

    int getNumSlot() const;
    void displaySlots() const;

    // --- Facade vers les composants materiels simules ---
    int selectionnerProduit();          // Keypad::getSelection()
    int insererPiece();                  // CoinSlot::updateCoinAmount()
    int getMontantInsere() const;        // CoinSlot::getCoinAmount()
    void reinitialiserMontant();         // CoinSlot::clear()
    bool verifierChute();                // DropCheck::productReleased()
    void rendreMonnaie(int prix);        // CoinSlot::returnCoins()
};

#endif // AUTOMAT_H
