#ifndef DROPCHECK_H
#define DROPCHECK_H

/**
 * DropCheck simule les faisceaux infrarouges qui détectent si le produit
 * est bien tombé dans le bac de sortie.
 */
class DropCheck {
public:
    // Retourne true avec 90% de probabilite (produit detecte),
    // false avec 10% de probabilite (produit non detecte)
    bool productReleased() const;
};

#endif // DROPCHECK_H
