#ifndef SLOT_H
#define SLOT_H

#include <string>

/**
 * Classe abstraite Slot
 * Représente un emplacement générique du distributeur : un produit, son prix,
 * la quantité disponible. La façon de faire tomber le produit (un ou deux
 * moteurs) est déléguée aux classes dérivées SmallSlot et WideSlot.
 */
class Slot {
protected:
    std::string productName;
    int id;
    int numProduits;
    int prix;

    // Décrémente le stock (utilitaire partagé par les sous-classes, appelé
    // depuis leur implémentation de drop() pour éviter la duplication)
    void decrementerStock();

public:
    Slot(int slotId, const std::string& productName, int price);
    virtual ~Slot();

    // Méthode abstraite : chaque type d'emplacement définit sa propre
    // façon de déclencher son ou ses moteurs
    virtual void drop() = 0;

    std::string getProductName() const;
    void setProductName(const std::string& name);

    int getNumProducts() const;
    void setNumProducts(int num);

    int getPrice() const;
    void setPrice(int price);

    int getId() const;
};

#endif // SLOT_H
