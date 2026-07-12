#ifndef COINSLOT_H
#define COINSLOT_H

#include <string>

/**
 * CoinSlot simule le monnayeur du distributeur.
 *
 * Adaptation : l'enonce original raisonne en euros/centimes (2 euros = 200
 * cents ... 1 cent). Pour rester coherent avec les prix en DH entiers de la
 * Partie 1, coinValues utilise directement des pieces de 10, 5, 2 et 1 DH
 * (de la plus haute a la plus basse), ce qui suit exactement le meme principe.
 */
class CoinSlot {
private:
    int coinAmount;
    static const int coinValues[4];
    static const int NUM_COIN_VALUES = 4;

public:
    CoinSlot();

    // Invite l'utilisateur a inserer une piece (ou -1 pour annuler l'achat).
    // Ajoute la piece a coinAmount si elle est valide. Retourne la valeur saisie.
    int updateCoinAmount();

    // Version non-bloquante pour une interface graphique : ajoute directement
    // une piece (deja choisie via un bouton, par ex.) sans passer par cin.
    // Retourne false si la valeur n'est pas une denomination valide.
    bool ajouterPiece(int valeur);

    int getCoinAmount() const;

    // Reinitialise coinAmount a 0
    void clear();

    // Calcule et affiche la monnaie a rendre par rapport au prix passe en parametre
    void returnCoins(int prix);

    // Meme calcul que returnCoins(), mais retourne le texte au lieu de l'afficher
    // (reutilise par returnCoins() en interne, et utile pour une interface graphique)
    std::string returnCoinsText(int prix);
};

#endif // COINSLOT_H