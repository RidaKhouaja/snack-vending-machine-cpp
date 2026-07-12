#include "CoinSlot.h"
#include <iostream>
#include <limits>

const int CoinSlot::coinValues[4] = {10, 5, 2, 1};

CoinSlot::CoinSlot() : coinAmount(0) {}

int CoinSlot::updateCoinAmount() {
    int piece;
    std::cout << "Inserez une piece (10, 5, 2 ou 1 DH) ou -1 pour annuler : ";
    std::cin >> piece;

    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Saisie invalide.\n";
        return updateCoinAmount();
    }

    if (piece == -1) {
        return -1;
    }

    bool valide = false;
    for (int i = 0; i < NUM_COIN_VALUES; ++i) {
        if (coinValues[i] == piece) {
            valide = true;
            break;
        }
    }

    if (!valide) {
        std::cout << "Piece non reconnue par la machine.\n";
        return updateCoinAmount();
    }

    coinAmount += piece;
    return piece;
}

bool CoinSlot::ajouterPiece(int valeur) {
    for (int i = 0; i < NUM_COIN_VALUES; ++i) {
        if (coinValues[i] == valeur) {
            coinAmount += valeur;
            return true;
        }
    }
    return false;
}

int CoinSlot::getCoinAmount() const {
    return coinAmount;
}

void CoinSlot::clear() {
    coinAmount = 0;
}

void CoinSlot::returnCoins(int prix) {
    std::cout << returnCoinsText(prix);
}

std::string CoinSlot::returnCoinsText(int prix) {
    int monnaie = coinAmount - prix;
    std::string texte;

    if (monnaie <= 0) {
        texte = "Aucune monnaie a rendre.\n";
        clear();
        return texte;
    }

    texte = "Monnaie rendue (" + std::to_string(monnaie) + " DH) :\n";
    for (int i = 0; i < NUM_COIN_VALUES; ++i) {
        int nb = monnaie / coinValues[i];
        if (nb > 0) {
            texte += "  " + std::to_string(nb) + " x " + std::to_string(coinValues[i]) + " DH\n";
            monnaie -= nb * coinValues[i];
        }
    }
    clear();
    return texte;
}