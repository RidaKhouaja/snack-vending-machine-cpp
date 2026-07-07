#ifndef MOTOR_H
#define MOTOR_H

/**
 * Classe Motor
 * Représente un moteur qui fait tourner la spirale d'un emplacement (slot)
 * pour pousser un produit vers le bac de sortie.
 */
class Motor {
private:
    int id;

public:
    explicit Motor(int id);

    // Déclenche la rotation du moteur (simulation console)
    void trigger() const;

    int getId() const;
};

#endif // MOTOR_H
