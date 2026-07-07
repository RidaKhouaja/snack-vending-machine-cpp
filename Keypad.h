#ifndef KEYPAD_H
#define KEYPAD_H

/**
 * Keypad simule le clavier utilisé par l'acheteur pour choisir un produit.
 */
class Keypad {
public:
    // Demande le numero de l'emplacement souhaite (-1 pour annuler)
    int getSelection() const;
};

#endif // KEYPAD_H
