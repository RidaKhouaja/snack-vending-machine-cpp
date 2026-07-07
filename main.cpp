#include <iostream>
#include <limits>
#include <cstdlib>
#include <ctime>
#include "Automat.h"

// Etats de l'automate (Figure 2 de l'enonce)
enum class EtatAutomate {
    IDLE,
    COLLECT_MONEY,
    DROP,
    RETURN_MONEY,
    END
};

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int lireEntier(const std::string& message) {
    int valeur;
    while (true) {
        std::cout << message;
        std::cin >> valeur;
        if (std::cin.eof()) {
            std::cout << "\nFin de saisie detectee. Fermeture du programme.\n";
            std::exit(0);
        }
        if (std::cin.fail()) {
            std::cout << "Saisie invalide, veuillez entrer un nombre entier.\n";
            clearInput();
        } else {
            return valeur;
        }
    }
}

void initialiserAutomate(Automat& automate) {
    automate.addSlot("Chips Nature", 5, 1, 101, 102);   // WideSlot (2 moteurs)
    automate.addSlot("Chocolat", 8, 2, 103);             // SmallSlot (1 moteur)
    automate.addSlot("Biscuits", 6, 3, 104);             // SmallSlot
    automate.addSlot("Eau Minerale", 4, 4, 105, 106);    // WideSlot (2 moteurs)
    automate.fillAll();
}

// ---------------------- Session client : machine a etats ----------------------
void sessionClient(Automat& automate) {
    EtatAutomate etat = EtatAutomate::IDLE;
    int slotSelectionne = -1;
    int prixCourant = 0;

    std::cout << "\n===== SESSION CLIENT =====\n";

    while (etat != EtatAutomate::END) {
        switch (etat) {

            case EtatAutomate::IDLE: {
                automate.displaySlots();
                int sel = automate.selectionnerProduit();

                if (sel == -1) {
                    std::cout << "Session terminee par l'utilisateur.\n";
                    etat = EtatAutomate::END;
                    break;
                }
                if (!automate.isAvailable(sel)) {
                    std::cout << "Produit indisponible ou emplacement inexistant.\n";
                    break; // on reste en IDLE
                }

                slotSelectionne = sel;
                prixCourant = automate.getPrice(sel);
                std::cout << "Prix du produit selectionne : " << prixCourant << " DH\n";
                etat = EtatAutomate::COLLECT_MONEY;
                break;
            }

            case EtatAutomate::COLLECT_MONEY: {
                int piece = automate.insererPiece();

                if (piece == -1) {
                    std::cout << "Achat annule.\n";
                    automate.rendreMonnaie(0); // rembourse tout ce qui a ete insere
                    etat = EtatAutomate::IDLE;
                    break;
                }

                std::cout << "Montant total insere : " << automate.getMontantInsere()
                          << " DH (prix : " << prixCourant << " DH)\n";

                if (automate.getMontantInsere() >= prixCourant) {
                    etat = EtatAutomate::DROP;
                }
                break;
            }

            case EtatAutomate::DROP: {
                std::cout << "\n--- Distribution du produit ---\n";
                bool succes = false;
                const int MAX_TENTATIVES = 3;

                for (int tentative = 1; tentative <= MAX_TENTATIVES && !succes; ++tentative) {
                    automate.dropSlot(slotSelectionne);
                    succes = automate.verifierChute();
                    if (!succes) {
                        std::cout << "Faisceaux infrarouges non interrompus (tentative "
                                  << tentative << "/" << MAX_TENTATIVES << "), nouvel essai...\n";
                    }
                }

                if (succes) {
                    std::cout << "Produit detecte dans le bac de sortie.\n";
                } else {
                    std::cout << "Echec de la distribution apres " << MAX_TENTATIVES
                              << " tentatives. Remboursement integral.\n";
                    prixCourant = 0; // remboursement complet, le produit n'a pas ete livre
                }
                etat = EtatAutomate::RETURN_MONEY;
                break;
            }

            case EtatAutomate::RETURN_MONEY: {
                automate.rendreMonnaie(prixCourant);
                etat = EtatAutomate::IDLE; // un nouvel achat peut commencer
                break;
            }

            default:
                etat = EtatAutomate::END;
                break;
        }
    }

    std::cout << "Fin de la session client.\n";
}

// ---------------------- Mode Administrateur ----------------------
void modeAdmin(Automat& automate) {
    int choix;
    do {
        std::cout << "\n===== MENU ADMINISTRATEUR =====\n";
        std::cout << "1. Afficher les emplacements\n";
        std::cout << "2. Ajouter un emplacement\n";
        std::cout << "3. Modifier un emplacement (nom/prix)\n";
        std::cout << "4. Remplir un emplacement\n";
        std::cout << "5. Remplir tous les emplacements\n";
        std::cout << "0. Retour au menu principal\n";
        choix = lireEntier("Votre choix : ");

        switch (choix) {
            case 1:
                automate.displaySlots();
                break;
            case 2: {
                int type = lireEntier("Type d'emplacement (1 = etroit/1 moteur, 2 = large/2 moteurs) : ");
                std::cin.ignore();
                std::string nom;
                std::cout << "Nom du produit : ";
                std::getline(std::cin, nom);
                int prix = lireEntier("Prix (DH) : ");
                int id = lireEntier("Numero de l'emplacement : ");

                if (type == 1) {
                    int motor = lireEntier("Id du moteur : ");
                    if (automate.addSlot(nom, prix, id, motor)) {
                        std::cout << "Emplacement etroit ajoute avec succes.\n";
                    }
                } else if (type == 2) {
                    int motor1 = lireEntier("Id du moteur 1 : ");
                    int motor2 = lireEntier("Id du moteur 2 : ");
                    if (automate.addSlot(nom, prix, id, motor1, motor2)) {
                        std::cout << "Emplacement large ajoute avec succes.\n";
                    }
                } else {
                    std::cout << "Type invalide.\n";
                }
                break;
            }
            case 3: {
                int id = lireEntier("Numero de l'emplacement a modifier : ");
                std::cin.ignore();
                std::string nom;
                std::cout << "Nouveau nom du produit : ";
                std::getline(std::cin, nom);
                int prix = lireEntier("Nouveau prix (DH) : ");
                automate.changeSlot(id, nom, prix);
                break;
            }
            case 4: {
                int id = lireEntier("Numero de l'emplacement a remplir : ");
                int qte = lireEntier("Nombre de produits a ajouter : ");
                automate.fill(id, qte);
                break;
            }
            case 5:
                automate.fillAll();
                std::cout << "Tous les emplacements ont ete remplis.\n";
                break;
            case 0:
                std::cout << "Retour au menu principal.\n";
                break;
            default:
                std::cout << "Choix invalide.\n";
        }
    } while (choix != 0);
}

// ---------------------- Menu principal ----------------------
int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    Automat automate(6, 10);
    initialiserAutomate(automate);

    int choix;
    do {
        std::cout << "\n=========================================\n";
        std::cout << "   DISTRIBUTEUR AUTOMATIQUE DE SNACKS\n";
        std::cout << "=========================================\n";
        std::cout << "1. Lancer une session client\n";
        std::cout << "2. Mode Administrateur (gerer le distributeur)\n";
        std::cout << "0. Quitter\n";
        choix = lireEntier("Votre choix : ");

        switch (choix) {
            case 1:
                sessionClient(automate);
                break;
            case 2:
                modeAdmin(automate);
                break;
            case 0:
                std::cout << "Au revoir !\n";
                break;
            default:
                std::cout << "Choix invalide.\n";
        }
    } while (choix != 0);

    return 0;
}
