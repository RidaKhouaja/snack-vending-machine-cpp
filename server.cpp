#include "httplib.h"
#include "Automat.h"

#include <cstdlib>
#include <ctime>
#include <sstream>
#include <mutex>

// ---------------------------------------------------------------------
// Etat global du "kiosque" : une seule transaction client a la fois,
// ce qui correspond exactement au comportement d'un vrai distributeur
// physique (un seul acheteur devant la machine a un instant donne).
// ---------------------------------------------------------------------
enum class EtatSession { IDLE, COLLECTING };

static Automat g_automate(6, 10);
static EtatSession g_etat = EtatSession::IDLE;
static int g_slotSelectionne = -1;
static int g_prixCourant = 0;
static std::mutex g_mutex; // protege l'etat partage contre les requetes concurrentes

// --- Petits utilitaires de construction JSON (evite une dependance externe) ---

static std::string jsonEscape(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"' || c == '\\') out += '\\';
        out += c;
    }
    return out;
}

static std::string produitsToJson() {
    std::vector<SlotInfo> infos = g_automate.getAllSlotsInfo();
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < infos.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{"
            << "\"id\":" << infos[i].id << ","
            << "\"nom\":\"" << jsonEscape(infos[i].productName) << "\","
            << "\"prix\":" << infos[i].price << ","
            << "\"stock\":" << infos[i].stock
            << "}";
    }
    oss << "]";
    return oss.str();
}

static std::string etatToJson(const std::string& message) {
    std::ostringstream oss;
    oss << "{"
        << "\"etat\":\"" << (g_etat == EtatSession::IDLE ? "IDLE" : "COLLECTING") << "\","
        << "\"slotSelectionne\":" << g_slotSelectionne << ","
        << "\"prixCourant\":" << g_prixCourant << ","
        << "\"montantInsere\":" << g_automate.getMontantInsere() << ","
        << "\"produits\":" << produitsToJson() << ","
        << "\"message\":\"" << jsonEscape(message) << "\""
        << "}";
    return oss.str();
}

// Extrait une valeur entiere simple d'un corps JSON du type {"cle":123}
// (suffisant ici car nos requetes n'envoient qu'un seul champ numerique)
static int extraireEntier(const std::string& body, const std::string& cle, int valeurParDefaut = -999999) {
    std::string motif = "\"" + cle + "\"";
    size_t pos = body.find(motif);
    if (pos == std::string::npos) return valeurParDefaut;
    pos = body.find(':', pos);
    if (pos == std::string::npos) return valeurParDefaut;
    ++pos;
    while (pos < body.size() && (body[pos] == ' ' || body[pos] == '\t')) ++pos;
    try {
        return std::stoi(body.substr(pos));
    } catch (...) {
        return valeurParDefaut;
    }
}

// Extrait une valeur chaine simple d'un corps JSON du type {"cle":"texte"}
static std::string extraireChaine(const std::string& body, const std::string& cle) {
    std::string motif = "\"" + cle + "\"";
    size_t pos = body.find(motif);
    if (pos == std::string::npos) return "";
    pos = body.find(':', pos);
    if (pos == std::string::npos) return "";
    pos = body.find('"', pos);
    if (pos == std::string::npos) return "";
    size_t debut = pos + 1;
    size_t fin = body.find('"', debut);
    if (fin == std::string::npos) return "";
    return body.substr(debut, fin - debut);
}

static void initialiserAutomate() {
    g_automate.addSlot("Chips Nature", 5, 1, 101, 102);  // WideSlot (2 moteurs)
    g_automate.addSlot("Chocolat", 8, 2, 103);             // SmallSlot (1 moteur)
    g_automate.addSlot("Biscuits", 6, 3, 104);             // SmallSlot
    g_automate.addSlot("Eau Minerale", 4, 4, 105, 106);    // WideSlot (2 moteurs)
    g_automate.fillAll();
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    initialiserAutomate();

    httplib::Server serveur;

    // Sert les fichiers statiques (index.html, css, js) depuis ./public
    serveur.set_mount_point("/", "./public");

    // --- Etat courant (produits + session en cours) ---
    serveur.Get("/api/etat", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(g_mutex);
        res.set_content(etatToJson(""), "application/json");
    });

    // --- Selection d'un produit (etape IDLE -> COLLECTING) ---
    serveur.Post("/api/selectionner", [](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(g_mutex);
        int id = extraireEntier(req.body, "id");

        if (g_etat != EtatSession::IDLE) {
            res.set_content(etatToJson("Une transaction est deja en cours."), "application/json");
            return;
        }
        if (!g_automate.isAvailable(id)) {
            res.set_content(etatToJson("Produit indisponible ou emplacement inexistant."), "application/json");
            return;
        }

        g_slotSelectionne = id;
        g_prixCourant = g_automate.getPrice(id);
        g_etat = EtatSession::COLLECTING;

        res.set_content(etatToJson("Produit selectionne. Inserez vos pieces."), "application/json");
    });

    // --- Insertion d'une piece ---
    serveur.Post("/api/inserer", [](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(g_mutex);
        int valeur = extraireEntier(req.body, "valeur");

        if (g_etat != EtatSession::COLLECTING) {
            res.set_content(etatToJson("Aucune selection en cours."), "application/json");
            return;
        }
        if (!g_automate.essayerAjouterPiece(valeur)) {
            res.set_content(etatToJson("Piece non reconnue par la machine."), "application/json");
            return;
        }

        if (g_automate.getMontantInsere() < g_prixCourant) {
            res.set_content(etatToJson("Piece inseree."), "application/json");
            return;
        }

        // Montant suffisant : on declenche la distribution (etats DROP puis RETURN_MONEY)
        bool succes = false;
        const int MAX_TENTATIVES = 3;
        std::string journal;
        for (int tentative = 1; tentative <= MAX_TENTATIVES && !succes; ++tentative) {
            g_automate.dropSlot(g_slotSelectionne);
            succes = g_automate.verifierChute();
            if (!succes) {
                journal += "Echec de detection (tentative " + std::to_string(tentative) +
                           "/" + std::to_string(MAX_TENTATIVES) + "). ";
            }
        }

        int prixEffectif = g_prixCourant;
        if (succes) {
            journal += "Produit distribue avec succes. ";
        } else {
            journal += "Echec apres " + std::to_string(MAX_TENTATIVES) + " tentatives, remboursement integral. ";
            prixEffectif = 0;
        }

        journal += g_automate.rendreMonnaieText(prixEffectif);

        g_etat = EtatSession::IDLE;
        g_slotSelectionne = -1;
        g_prixCourant = 0;

        res.set_content(etatToJson(journal), "application/json");
    });

    // --- Annulation de l'achat en cours ---
    serveur.Post("/api/annuler", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (g_etat != EtatSession::COLLECTING) {
            res.set_content(etatToJson("Aucune transaction a annuler."), "application/json");
            return;
        }
        std::string texte = g_automate.rendreMonnaieText(0);
        g_etat = EtatSession::IDLE;
        g_slotSelectionne = -1;
        g_prixCourant = 0;
        res.set_content(etatToJson("Achat annule. " + texte), "application/json");
    });

    // --- Administration : ajouter un emplacement ---
    serveur.Post("/api/admin/ajouter", [](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(g_mutex);
        std::string type = extraireChaine(req.body, "type"); // "etroit" ou "large"
        std::string nom = extraireChaine(req.body, "nom");
        int prix = extraireEntier(req.body, "prix");
        int id = extraireEntier(req.body, "id");
        int moteur1 = extraireEntier(req.body, "moteur1");

        bool succes;
        if (type == "large") {
            int moteur2 = extraireEntier(req.body, "moteur2");
            succes = g_automate.addSlot(nom, prix, id, moteur1, moteur2);
        } else {
            succes = g_automate.addSlot(nom, prix, id, moteur1);
        }

        res.set_content(etatToJson(succes ? "Emplacement ajoute." : "Echec de l'ajout (id deja utilise ou automate plein)."),
                         "application/json");
    });

    // --- Administration : modifier un emplacement ---
    serveur.Post("/api/admin/modifier", [](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(g_mutex);
        int id = extraireEntier(req.body, "id");
        std::string nom = extraireChaine(req.body, "nom");
        int prix = extraireEntier(req.body, "prix");
        g_automate.changeSlot(id, nom, prix);
        res.set_content(etatToJson("Emplacement modifie."), "application/json");
    });

    // --- Administration : remplir un emplacement ---
    serveur.Post("/api/admin/remplir", [](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(g_mutex);
        int id = extraireEntier(req.body, "id");
        int qte = extraireEntier(req.body, "quantite");
        g_automate.fill(id, qte);
        res.set_content(etatToJson("Emplacement rempli."), "application/json");
    });

    // --- Administration : tout remplir ---
    serveur.Post("/api/admin/remplirTout", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_automate.fillAll();
        res.set_content(etatToJson("Tous les emplacements ont ete remplis."), "application/json");
    });

    std::cout << "Serveur demarre : ouvrez http://localhost:8080 dans votre navigateur\n";
    serveur.listen("localhost", 8080);

    return 0;
}