#ifndef AUTOMAT_H
#define AUTOMAT_H

#include <string>
#include <vector>
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
// Structure legere utilisee pour exposer l'etat des emplacements a une
// interface graphique sans exposer les Slot* (et leur polymorphisme) directement.
struct SlotInfo {
    int id;
    std::string productName;
    int price;
    int stock;
};

class Automat {
private:
    Slot** m_slots;
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

    // Version GUI-friendly de displaySlots() : retourne les infos plutot
    // que de les imprimer, pour alimenter un tableau/widget par exemple.
    std::vector<SlotInfo> getAllSlotsInfo() const;

    // --- Facade vers les composants materiels simules ---
    int selectionnerProduit();          // Keypad::getSelection() (mode console)
    int insererPiece();                  // CoinSlot::updateCoinAmount() (mode console)
    bool essayerAjouterPiece(int valeur); // CoinSlot::ajouterPiece() (mode GUI, non bloquant)
    int getMontantInsere() const;        // CoinSlot::getCoinAmount()
    void reinitialiserMontant();         // CoinSlot::clear()
    bool verifierChute();                // DropCheck::productReleased()
    void rendreMonnaie(int prix);        // CoinSlot::returnCoins() (mode console)
    std::string rendreMonnaieText(int prix); // CoinSlot::returnCoinsText() (mode GUI)
};

#endif // AUTOMAT_H