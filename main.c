#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

// STRUCTURES DE DONNÉES
typedef struct Vol Vol;
typedef struct Liste_vols Liste_vols;

typedef struct Aero Aero;
typedef struct Liste_aeros Liste_aeros;

typedef struct Ville Ville;
typedef struct Liste_villes Liste_villes;

typedef struct Noeud Noeud;

struct Vol {
    // Destination
    Aero* destination;
    // Dates
    int date_depart;
    int date_arrivee;
    // Suivant
    Vol* vol_suivant;
};
struct Liste_vols {
    Vol* tete_vols;
};

struct Aero {
    // Est
    char nom_aeroport[50];
    char code_fs[10];
    char decalage_utc[10];
    // Vient de
    Ville* ville;
    // Contient
    Liste_vols* liste_vols;
    // Suivant
    Aero* aero_suivant;
};
struct Liste_aeros {
    Aero* tete_aeros;
};

struct Ville {
    // Est
    char nom_ville[50];
    int num_ville; // Pour le choix utilisateur
    // Vient de
    char nom_pays[50];
    // Contient
    Liste_aeros* liste_aeroports;
    // Suivante
    Ville* ville_suivante;
};
struct Liste_villes {
    Ville* tete_villes;
};

struct Noeud {
    Noeud* noeud_precedent;
    Aero* aeroport;
    int etat;
    int date_arrivee; // 202012061925 -> 2020/12/06 à 19:25 -> DDHHmm
};


// FONCTIONS
// Parcours l'arborescence, si la ville n'existe pas, la crée, puis crée l'aéroport
void inserer_aeroport(Liste_villes* liste_villes, char nom_ville[50], int num_ville, char code_fs[10], char nom_aeroport[50], char nom_pays[50], char decalage_utc[10]) {
    // Création de l'aéroport
    Aero* aeroport = malloc(sizeof(Aero));
    strcpy(aeroport->code_fs, code_fs);
    strcpy(aeroport->decalage_utc, decalage_utc);
    aeroport->liste_vols = malloc(sizeof(Liste_vols));
    aeroport->liste_vols->tete_vols = NULL;
    if (strcmp(nom_aeroport, "") == 0) {
        strcpy(aeroport->nom_aeroport, nom_ville);
        strcat(aeroport->nom_aeroport, code_fs);
    } else {
        strcpy(aeroport->nom_aeroport, nom_aeroport);
    }

    // Ajout à la liste des aéroports de la ville
    Ville* curseur_ville = liste_villes->tete_villes;
    int cette_ville_existe = 0;
    Ville* ville;
    while (curseur_ville != NULL && cette_ville_existe == 0) {
        // LA VILLE EXISTE
        if (strcmp(curseur_ville->nom_ville, nom_ville) == 0) {
            cette_ville_existe = 1;
            ville = curseur_ville;
        }
        curseur_ville = curseur_ville->ville_suivante;
    }
    if (cette_ville_existe == 0) {
        ville = malloc(sizeof(Ville));
        strcpy(ville->nom_pays, nom_pays);
        strcpy(ville->nom_ville, nom_ville);
        ville->num_ville = num_ville;
        ville->ville_suivante = liste_villes->tete_villes;
        liste_villes->tete_villes = ville;
    }
    if (ville->liste_aeroports == NULL) {
        Liste_aeros* liste_aeroports_de_la_ville = malloc(sizeof(Liste_aeros));
        liste_aeroports_de_la_ville->tete_aeros = NULL;
        ville->liste_aeroports = liste_aeroports_de_la_ville;
    }
    aeroport->aero_suivant = ville->liste_aeroports->tete_aeros;
    ville->liste_aeroports->tete_aeros = aeroport;

    // On ajoute la ville à l'aéroport
    aeroport->ville = ville;
    
}

Liste_villes* lister_aeroports() {
    FILE* fichier_aeroports = fopen("Avions_Bdd/allAirports.json", "r");
    if (fichier_aeroports == NULL) {
        printf("Erreur de lecture du fichier aéroports\n");
        exit(0);
    }

    // Initialisation des variables :
        // de la ville
        Liste_villes* liste_villes = malloc(sizeof(Liste_villes));
        int num_ville = 0;

        // de parcours
        char caractere_courant = fgetc(fichier_aeroports);

        // de reconnaissance des données
        int validation_nom_ville = 0; // city":"Cali"
        int compteur_nom_ville = 0;
        int validation_code_fs = 0; // fs":"CLOX
        int validation_nom_aeroport = 0; // name":"Mukhaizna Airport"
        int compteur_nom_aeroport = 0;
        int validation_nom_pays = 0; // countryName":"Oman"
        int compteur_nom_pays = 0;
        int validation_decalage_utc = 0; // utcOffsetHours":-6.0,
        int compteur_utc = 0; // Permet de savoir à quelle position écrire la valeur du décalage utc dans le tableau correspondant

        // d'enregistrement temporaire (le temps de la lecture) des données d'un aéroport
        char nom_ville[50];
        char code_fs[10];
        char nom_aeroport[50];
        char nom_pays[50];
        char decalage_utc[10];

    // Parcours tout le fichier
    while (caractere_courant != ']' && caractere_courant != EOF) {
        // On enregistre les données d'un aeroport à chaque '}'
        if (caractere_courant == '}') {
            inserer_aeroport(liste_villes, nom_ville, num_ville++, code_fs, nom_aeroport, nom_pays, decalage_utc);
        }

        // NOM VILLE
        if (validation_nom_ville > 6 && caractere_courant == '"') { // Si fin de lecture
            nom_ville[compteur_nom_ville] = '\0';
            compteur_nom_ville = 0;
            validation_nom_ville = 0;
        }
        else if (validation_nom_ville > 6) {
            nom_ville[compteur_nom_ville] = caractere_courant;
            compteur_nom_ville++;
        }
        else if (caractere_courant == '"' && validation_nom_ville == 6) {
            validation_nom_ville++;
        }
        else if (caractere_courant == ':' && validation_nom_ville == 5) {
            validation_nom_ville++;
        }
        else if (caractere_courant == '"' && validation_nom_ville == 4) {
            validation_nom_ville++;
        }
        else if (caractere_courant == 'y' && validation_nom_ville == 3) {
            validation_nom_ville++;
        }
        else if (caractere_courant == 't' && validation_nom_ville == 2) {
            validation_nom_ville++;
        }
        else if (caractere_courant == 'i' && validation_nom_ville == 1) {
            validation_nom_ville++;
        }
        else if (caractere_courant == 'c' && validation_nom_ville == 0) {
            validation_nom_ville++;
        }
        else if (validation_nom_ville > 0) { // Le début de la chaine correspond mais le caractère actuel non 
            validation_nom_ville = 0;
        }

        // CODE FS (AEROPORT)
        if (validation_code_fs == 8) {
            if (caractere_courant == '"') {
                code_fs[3] = '\0';
            } else {
                code_fs[3] = caractere_courant;
                code_fs[4] = '\0';
            }
            validation_code_fs = 0;
        }
        else if (validation_code_fs == 7) {
            code_fs[2] = caractere_courant;
            validation_code_fs++;
        }
        else if (validation_code_fs == 6) {
            code_fs[1] = caractere_courant;
            validation_code_fs++;
        }
        else if (validation_code_fs == 5) {
            code_fs[0] = caractere_courant;
            validation_code_fs++;
        }
        else if (caractere_courant == '"' && validation_code_fs == 4) {
            validation_code_fs++;
        }
        else if (caractere_courant == ':' && validation_code_fs == 3) {
            validation_code_fs++;
        }
        else if (caractere_courant == '"' && validation_code_fs == 2) {
            validation_code_fs++;
        }
        else if (caractere_courant == 's' && validation_code_fs == 1) {
            validation_code_fs++;
        }
        else if (caractere_courant == 'f' && validation_code_fs == 0) {
            validation_code_fs++;
        }
        else if (validation_code_fs > 0) { // Le début de la chaine correspond mais le caractère actuel non 
            validation_code_fs = 0;
        }

        // NOM AEROPORT
        if (validation_nom_aeroport > 6 && caractere_courant == '"') { // Si fin de lecture
            nom_aeroport[compteur_nom_aeroport] = '\0';
            compteur_nom_aeroport = 0;
            validation_nom_aeroport = 0;
        }
        else if (validation_nom_aeroport > 6) {
            nom_aeroport[compteur_nom_aeroport] = caractere_courant;
            compteur_nom_aeroport++;
        }
        else if (caractere_courant == '"' && validation_nom_aeroport == 6) {
            validation_nom_aeroport++;
        }
        else if (caractere_courant == ':' && validation_nom_aeroport == 5) {
            validation_nom_aeroport++;
        }
        else if (caractere_courant == '"' && validation_nom_aeroport == 4) {
            validation_nom_aeroport++;
        }
        else if (caractere_courant == 'e' && validation_nom_aeroport == 3) {
            validation_nom_aeroport++;
        }
        else if (caractere_courant == 'm' && validation_nom_aeroport == 2) {
            validation_nom_aeroport++;
        }
        else if (caractere_courant == 'a' && validation_nom_aeroport == 1) {
            validation_nom_aeroport++;
        }
        else if (caractere_courant == 'n' && validation_nom_aeroport == 0) {
            validation_nom_aeroport++;
        }
        else if (validation_nom_aeroport > 0) { // Le début de la chaine correspond mais le caractère actuel non 
            validation_nom_aeroport = 0;
        }

        // NOM PAYS
        if (validation_nom_pays > 13 && caractere_courant == '"') { // Si fin de lecture
            nom_pays[compteur_nom_pays] = '\0';
            compteur_nom_pays = 0;
            validation_nom_pays = 0;
        }
        else if (validation_nom_pays > 13) {
            nom_pays[compteur_nom_pays] = caractere_courant;
            compteur_nom_pays++;
        }
        else if (caractere_courant == '"' && validation_nom_pays == 13) {
            validation_nom_pays++;
        }
        else if (caractere_courant == ':' && validation_nom_pays == 12) {
            validation_nom_pays++;
        }
        else if (caractere_courant == '"' && validation_nom_pays == 11) {
            validation_nom_pays++;
        }
        else if (caractere_courant == 'e' && validation_nom_pays == 10) {
            validation_nom_pays++;
        }
        else if (caractere_courant == 'm' && validation_nom_pays == 9) {
            validation_nom_pays++;
        }
        else if (caractere_courant == 'a' && validation_nom_pays == 8) {
            validation_nom_pays++;
        }
        else if (caractere_courant == 'N' && validation_nom_pays == 7) {
            validation_nom_pays++;
        }
        else if (caractere_courant == 'y' && validation_nom_pays == 6) {
            validation_nom_pays++;
        }
        else if (caractere_courant == 'r' && validation_nom_pays == 5) {
            validation_nom_pays++;
        }
        else if (caractere_courant == 't' && validation_nom_pays == 4) {
            validation_nom_pays++;
        }
        else if (caractere_courant == 'n' && validation_nom_pays == 3) {
            validation_nom_pays++;
        }
        else if (caractere_courant == 'u' && validation_nom_pays == 2) {
            validation_nom_pays++;
        }
        else if (caractere_courant == 'o' && validation_nom_pays == 1) {
            validation_nom_pays++;
        }
        else if (caractere_courant == 'c' && validation_nom_pays == 0) {
            validation_nom_pays++;
        }
        else if (validation_nom_pays > 0) { // Le début de la chaine correspond mais le caractère actuel non 
            validation_nom_pays = 0;
        }

        // DECALAGE UTC
        if (validation_decalage_utc > 15 && caractere_courant == ',') { // Si fin de lecture
            decalage_utc[compteur_utc] = '\0';
            compteur_utc = 0;
            validation_decalage_utc = 0;
        }
        else if (validation_decalage_utc > 15) {
            decalage_utc[compteur_utc] = caractere_courant;
            compteur_utc++;
        }
        else if (caractere_courant == ':' && validation_decalage_utc == 15) {
            validation_decalage_utc++;
        }
        else if (caractere_courant == '"' && validation_decalage_utc == 14) {
            validation_decalage_utc++;
        }
        else if (caractere_courant == 's' && validation_decalage_utc == 13) {
            validation_decalage_utc++;
        }
        else if (caractere_courant == 'r' && validation_decalage_utc == 12) {
            validation_decalage_utc++;
        }
        else if (caractere_courant == 'u' && validation_decalage_utc == 11) {
            validation_decalage_utc++;
        }
        else if (caractere_courant == 'o' && validation_decalage_utc == 10) {
            validation_decalage_utc++;
        }
        else if (caractere_courant == 'H' && validation_decalage_utc == 9) {
            validation_decalage_utc++;
        }
        else if (caractere_courant == 't' && validation_decalage_utc == 8) {
            validation_decalage_utc++;
        }
        else if (caractere_courant == 'e' && validation_decalage_utc == 7) {
            validation_decalage_utc++;
        }
        else if (caractere_courant == 's' && validation_decalage_utc == 6) {
            validation_decalage_utc++;
        }
        else if (caractere_courant == 'f' && validation_decalage_utc == 5) {
            validation_decalage_utc++;
        }
        else if (caractere_courant == 'f' && validation_decalage_utc == 4) {
            validation_decalage_utc++;
        }
        else if (caractere_courant == 'O' && validation_decalage_utc == 3) {
            validation_decalage_utc++;
        }
        else if (caractere_courant == 'c' && validation_decalage_utc == 2) {
            validation_decalage_utc++;
        }
        else if (caractere_courant == 't' && validation_decalage_utc == 1) {
            validation_decalage_utc++;
        }
        else if (caractere_courant == 'u' && validation_decalage_utc == 0) {
            validation_decalage_utc++;
        }
        else if (validation_decalage_utc > 0) { // Le début de la chaine correspond mais le caractère actuel non 
            validation_decalage_utc = 0;
        }

        // On récupère le prochain caractère
        caractere_courant = fgetc(fichier_aeroports);
    }

    fclose(fichier_aeroports);
    return liste_villes;
}

void inserer_vol(Liste_villes* liste_villes, char code_fs_arrivee[10], char date_arrivee[25], char code_fs_depart[10], char date_depart[25]) {
    int dnb_dest = 0;
    int dnb_src = 0;

    // Création du vol
    Vol* vol = malloc(sizeof(Vol));
    Aero* aeroport_depart;
    Aero* aeroport_arrivee;

    // Ajout des valeurs d'arrivée au vol
        // Année
        // char annee_arrivee[5];
        // for (int i = 0; i < 4; i++) {
        //     annee_arrivee[i] = date_arrivee[i];
        // }
        // vol->date_arrivee = atoi(annee_arrivee)*100000000;
        // // Mois
        // char mois_arrivee[3];
        // mois_arrivee[0] = date_arrivee[5];
        // mois_arrivee[1] = date_arrivee[6];
        // vol->date_arrivee += atoi(mois_arrivee)*1000000;
        // Jour
        char jour_arrivee[3];
        jour_arrivee[0] = date_arrivee[8];
        jour_arrivee[1] = date_arrivee[9];
        vol->date_arrivee += atoi(jour_arrivee)*1000;
        // Heure
        char heure_arrivee[3];
        heure_arrivee[0] = date_arrivee[11];
        heure_arrivee[1] = date_arrivee[12];
        vol->date_arrivee += atoi(heure_arrivee)*10;
        // Minute
        char minute_arrivee[3];
        minute_arrivee[0] = date_arrivee[14];
        // minute_arrivee[1] = date_arrivee[15];
        vol->date_arrivee += atoi(minute_arrivee);

    // Ajout des valeurs de départ au vol
        // Année
        // char annee_depart[5];
        // for (int i = 0; i < 4; i++) {
        //     annee_depart[i] = date_depart[i];
        // }
        // vol->date_depart = atoi(annee_depart)*100000000;
        // Mois
        // char mois_depart[3];
        // mois_depart[0] = date_depart[5];
        // mois_depart[1] = date_depart[6];
        // vol->date_depart += atoi(mois_depart)*1000000;
        // Jour
        char jour_depart[3];
        jour_depart[0] = date_depart[8];
        jour_depart[1] = date_depart[9];
        vol->date_depart += atoi(jour_depart)*1000;
        // Heure
        char heure_depart[3];
        heure_depart[0] = date_depart[11];
        heure_depart[1] = date_depart[12];
        vol->date_depart += atoi(heure_depart)*10;
        // Minute
        char minute_depart[3];
        minute_depart[0] = date_depart[14];
        // minute_depart[1] = date_depart[15];
        vol->date_depart += atoi(minute_depart);



    // Création des curseurs
    Ville* ville_curseur = liste_villes->tete_villes;
    Aero* aeroport_curseur = ville_curseur->liste_aeroports->tete_aeros;

    // Parcours de la liste à la recherche de l'aéroport de départ pour l'y insérer et celui d'arrivée pour insérer l'aéroport en destination du vol
    while (ville_curseur != NULL) {
        // Le curseur se place au début de la liste des aéroports de la ville actuelle
        aeroport_curseur = ville_curseur->liste_aeroports->tete_aeros;
        // Parcours de tous les aéroports
        while (aeroport_curseur != NULL) {
            // Si le code FS de l'aéroport actuel correspond à celui de l'aéroport d'arrivée du vol on l'enregistre
            if ((code_fs_arrivee[0] == aeroport_curseur->code_fs[0]) && (code_fs_arrivee[1] == aeroport_curseur->code_fs[1]) && (code_fs_arrivee[2] == aeroport_curseur->code_fs[2]) && (code_fs_arrivee[3] == aeroport_curseur->code_fs[3])) {
                if (dnb_dest < 1) {
                    aeroport_arrivee = aeroport_curseur;
                }
                dnb_dest++;
            }
            // Si le code FS de l'aéroport actuel correspond à celui de l'aéroport de dépard du vol on rajoute ce vol à la liste des vols de l'aéroport actuel
            if ((code_fs_depart[0] == aeroport_curseur->code_fs[0]) && (code_fs_depart[1] == aeroport_curseur->code_fs[1]) && (code_fs_depart[2] == aeroport_curseur->code_fs[2]) && (code_fs_depart[3] == aeroport_curseur->code_fs[3])) {
                if (dnb_src < 1) {
                    aeroport_depart = aeroport_curseur;
                }
                dnb_src++;
            }
            // Aéroport suivant
            aeroport_curseur = aeroport_curseur->aero_suivant;
        }
        // Ville suivante
        ville_curseur = ville_curseur->ville_suivante;
    }

    if (dnb_dest == 1 && dnb_src == 1) {
        vol->destination = aeroport_arrivee;
        vol->vol_suivant = aeroport_depart->liste_vols->tete_vols;
        aeroport_depart->liste_vols->tete_vols = vol;
    }
}

// Un nouveau vol est détecté par "arrivalAirportFsCode"
void lister_vols(Liste_villes* liste_villes) {
    FILE* fichier_vols = fopen("Avions_Bdd/20160206_flightstatus.json", "r");
    if (fichier_vols == NULL) {
        printf("Erreur de lecture du fichier vols\n");
        exit(0);
    }
    fseek(fichier_vols, 13, SEEK_SET);
    
    // Initialisation des variables
        // de parcours
        char caractere_courant = fgetc(fichier_vols);
        int num_vol = 0;
        // de reconnaissance des données
        int validation_code_fs_arrivee = 0; // arrivalAirportFsCode":"AMS
        int validation_date_arrivee = 0; // Utc":"2016-02-06T06:30:00.000Z
        int validation_code_fs_depart = 0; // departureAirportFsCode":"AAL
        int validation_date_depart = 0; // Utc":"2016-02-06T05:05:00.000Z
        int arrivee_ou_depart = 0; // 0 : arrivée / 1 : départ

        // d'enregistrement temporaire (le temps de la lecture) des données d'un aéroport
        char code_fs_arrivee[10];
        char date_arrivee[25];
        char code_fs_depart[10];
        char date_depart[25];
    
    while (caractere_courant != EOF) {
        // FS ARRIVEE
        if (validation_code_fs_arrivee == 26) { // Si fin de lecture
            if (caractere_courant == '"') {
                code_fs_arrivee[3] = '\0';
            } else {
                code_fs_arrivee[3] = caractere_courant;
                code_fs_arrivee[4] = '\0';
            }
            
            // Insertion du vol dans l'aéroport
            if (num_vol > 0) {
                inserer_vol(liste_villes, code_fs_arrivee, date_arrivee, code_fs_depart, date_depart);
            }
            num_vol++;
            printf("%3d/100\r", (int)(num_vol*100/60371));

            // On passe à la suite
            validation_code_fs_arrivee = 0;
        }
        else if (validation_code_fs_arrivee == 25) {
            code_fs_arrivee[2] = caractere_courant;
            validation_code_fs_arrivee++;
        }
        else if (validation_code_fs_arrivee == 24) {
            code_fs_arrivee[1] = caractere_courant;
            validation_code_fs_arrivee++;
        }
        else if (validation_code_fs_arrivee == 23) {
            code_fs_arrivee[0] = caractere_courant;
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == '"' && validation_code_fs_arrivee == 22) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == ':' && validation_code_fs_arrivee == 21) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == '"' && validation_code_fs_arrivee == 20) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 'e' && validation_code_fs_arrivee == 19) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 'd' && validation_code_fs_arrivee == 18) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 'o' && validation_code_fs_arrivee == 17) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 'C' && validation_code_fs_arrivee == 16) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 's' && validation_code_fs_arrivee == 15) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 'F' && validation_code_fs_arrivee == 14) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 't' && validation_code_fs_arrivee == 13) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 'r' && validation_code_fs_arrivee == 12) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 'o' && validation_code_fs_arrivee == 11) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 'p' && validation_code_fs_arrivee == 10) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 'r' && validation_code_fs_arrivee == 9) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 'i' && validation_code_fs_arrivee == 8) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 'A' && validation_code_fs_arrivee == 7) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 'l' && validation_code_fs_arrivee == 6) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 'a' && validation_code_fs_arrivee == 5) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 'v' && validation_code_fs_arrivee == 4) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 'i' && validation_code_fs_arrivee == 3) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 'r' && validation_code_fs_arrivee == 2) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 'r' && validation_code_fs_arrivee == 1) {
            validation_code_fs_arrivee++;
        }
        else if (caractere_courant == 'a' && validation_code_fs_arrivee == 0) {
            validation_code_fs_arrivee++;
        }
        else if (validation_code_fs_arrivee > 0) { // Le début de la chaine correspond mais le caractère actuel non 
            validation_code_fs_arrivee = 0;
        }
        
        // DATE arrivee (UTC)
        if (arrivee_ou_depart == 0) {
            if (validation_date_arrivee == 28) { // Si fin de lecture
                date_arrivee[22] = caractere_courant;
                arrivee_ou_depart = 1;
                validation_date_arrivee = 0;
            }
            else if (validation_date_arrivee == 27) {
                date_arrivee[21] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 26) {
                date_arrivee[20] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 25) {
                date_arrivee[19] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 24) {
                date_arrivee[18] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 23) {
                date_arrivee[17] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 22) {
                date_arrivee[16] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 21) {
                date_arrivee[15] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 20) {
                date_arrivee[14] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 19) {
                date_arrivee[13] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 18) {
                date_arrivee[12] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 17) {
                date_arrivee[11] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 16) {
                date_arrivee[10] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 15) {
                date_arrivee[9] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 14) {
                date_arrivee[8] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 13) {
                date_arrivee[7] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 12) {
                date_arrivee[6] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 11) {
                date_arrivee[5] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 10) {
                date_arrivee[4] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 9) {
                date_arrivee[3] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 8) {
                date_arrivee[2] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 7) {
                date_arrivee[1] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee == 6) {
                date_arrivee[0] = caractere_courant;
                validation_date_arrivee++;
            }
            else if (caractere_courant == '"' && validation_date_arrivee == 5) {
                validation_date_arrivee++;
            }
            else if (caractere_courant == ':' && validation_date_arrivee == 4) {
                validation_date_arrivee++;
            }
            else if (caractere_courant == '"' && validation_date_arrivee == 3) {
                validation_date_arrivee++;
            }
            else if (caractere_courant == 'c' && validation_date_arrivee == 2) {
                validation_date_arrivee++;
            }
            else if (caractere_courant == 't' && validation_date_arrivee == 1) {
                validation_date_arrivee++;
            }
            else if (caractere_courant == 'U' && validation_date_arrivee == 0) {
                validation_date_arrivee++;
            }
            else if (validation_date_arrivee > 0) { // Le début de la chaine correspond mais le caractère actuel non 
                validation_date_arrivee = 0;
            }
        }
        

        // FS DEPART        
        if (validation_code_fs_depart == 28) { // Si fin de lecture
            if (caractere_courant == '"') {
                code_fs_depart[3] = '\0';
            } else {
                code_fs_depart[3] = caractere_courant;
                code_fs_depart[4] = '\0';
            }
            validation_code_fs_depart = 0;
        }
        else if (validation_code_fs_depart == 27) {
            code_fs_depart[2] = caractere_courant;
            validation_code_fs_depart++;
        }
        else if (validation_code_fs_depart == 26) {
            code_fs_depart[1] = caractere_courant;
            validation_code_fs_depart++;
        }
        else if (validation_code_fs_depart == 25) {
            code_fs_depart[0] = caractere_courant;
            validation_code_fs_depart++;
        }
        else if (caractere_courant == '"' && validation_code_fs_depart == 24) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == ':' && validation_code_fs_depart == 23) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == '"' && validation_code_fs_depart == 22) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'e' && validation_code_fs_depart == 21) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'd' && validation_code_fs_depart == 20) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'o' && validation_code_fs_depart == 19) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'C' && validation_code_fs_depart == 18) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 's' && validation_code_fs_depart == 17) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'F' && validation_code_fs_depart == 16) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 't' && validation_code_fs_depart == 15) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'r' && validation_code_fs_depart == 14) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'o' && validation_code_fs_depart == 13) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'p' && validation_code_fs_depart == 12) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'r' && validation_code_fs_depart == 11) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'i' && validation_code_fs_depart == 10) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'A' && validation_code_fs_depart == 9) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'e' && validation_code_fs_depart == 8) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'r' && validation_code_fs_depart == 7) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'u' && validation_code_fs_depart == 6) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 't' && validation_code_fs_depart == 5) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'r' && validation_code_fs_depart == 4) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'a' && validation_code_fs_depart == 3) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'p' && validation_code_fs_depart == 2) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'e' && validation_code_fs_depart == 1) {
            validation_code_fs_depart++;
        }
        else if (caractere_courant == 'd' && validation_code_fs_depart == 0) {
            validation_code_fs_depart++;
        }
        else if (validation_code_fs_depart > 0) { // Le début de la chaine correspond mais le caractère actuel non 
            validation_code_fs_depart = 0;
        }

        // DATE DEPART
        if (arrivee_ou_depart == 1) {
            if (validation_date_depart == 28) { // Si fin de lecture
                date_depart[22] = caractere_courant;
                arrivee_ou_depart = 0;
                validation_date_depart = 0;
            }
            else if (validation_date_depart == 27) {
                date_depart[21] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 26) {
                date_depart[20] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 25) {
                date_depart[19] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 24) {
                date_depart[18] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 23) {
                date_depart[17] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 22) {
                date_depart[16] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 21) {
                date_depart[15] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 20) {
                date_depart[14] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 19) {
                date_depart[13] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 18) {
                date_depart[12] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 17) {
                date_depart[11] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 16) {
                date_depart[10] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 15) {
                date_depart[9] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 14) {
                date_depart[8] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 13) {
                date_depart[7] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 12) {
                date_depart[6] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 11) {
                date_depart[5] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 10) {
                date_depart[4] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 9) {
                date_depart[3] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 8) {
                date_depart[2] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 7) {
                date_depart[1] = caractere_courant;
                validation_date_depart++;
            }
            else if (validation_date_depart == 6) {
                date_depart[0] = caractere_courant;
                validation_date_depart++;
            }
            else if (caractere_courant == '"' && validation_date_depart == 5) {
                validation_date_depart++;
            }
            else if (caractere_courant == ':' && validation_date_depart == 4) {
                validation_date_depart++;
            }
            else if (caractere_courant == '"' && validation_date_depart == 3) {
                validation_date_depart++;
            }
            else if (caractere_courant == 'c' && validation_date_depart == 2) {
                validation_date_depart++;
            }
            else if (caractere_courant == 't' && validation_date_depart == 1) {
                validation_date_depart++;
            }
            else if (caractere_courant == 'U' && validation_date_depart == 0) {
                validation_date_depart++;
            }
            else if (validation_date_depart > 0) { // Le début de la chaine correspond mais le caractère actuel non 
                validation_date_depart = 0;
            }
        }

        caractere_courant = fgetc(fichier_vols);
    }
    // Insertion du dernier vol
    inserer_vol(liste_villes, code_fs_arrivee, date_arrivee, code_fs_depart, date_depart);

    printf("100/100 | Chargement terminé !\n");
    fclose(fichier_vols);
}

int afficher_villes_depart(Liste_villes liste_villes, char lettre) {
    // Formatage de la lettre pour accepter les majuscules et les minuscules
    if (lettre >= 'a' && lettre <= 'z') {
        lettre += 'A' - 'a';
    }
    char premiere_lettre_ville;

    // Recherche
    int il_y_a_au_moins_une_ville = 0;
    Ville* ville = liste_villes.tete_villes;
    while (ville != NULL) {
        premiere_lettre_ville = ville->nom_ville[0];
        if (premiere_lettre_ville >= 'a' && premiere_lettre_ville <= 'z') {
            premiere_lettre_ville += 'A' - 'a';
        }
        if (premiere_lettre_ville == lettre && ville->liste_aeroports->tete_aeros->liste_vols->tete_vols != NULL) {
            printf("%5d : %s (%s)\n", ville->num_ville, ville->nom_ville, ville->nom_pays);
            il_y_a_au_moins_une_ville = 1;
        }
        ville = ville->ville_suivante;
    }

    return il_y_a_au_moins_une_ville;
}

int afficher_villes_arrivee(Liste_villes liste_villes, char lettres[10]) {
    // Formatage de la lettre pour accepter les majuscules et les minuscules
    if (lettres[0] >= 'a' && lettres[0] <= 'z') {
        lettres[0] += 'A' - 'a';
    }
    if (lettres[1] >= 'a' && lettres[1] <= 'z') {
        lettres[1] += 'A' - 'a';
    }
    if (lettres[2] >= 'a' && lettres[2] <= 'z') {
        lettres[2] += 'A' - 'a';
    }
    lettres[3] = '\0';
    char premiere_lettre_ville[10];

    // Recherche des villes
    int il_y_a_au_moins_une_ville = 0;
    Ville* ville = liste_villes.tete_villes;
    while (ville != NULL) {
        premiere_lettre_ville[0] = ville->nom_ville[0];
        if (premiere_lettre_ville[0] >= 'a' && premiere_lettre_ville[0] <= 'z') {
            premiere_lettre_ville[0] += 'A' - 'a';
        }
        premiere_lettre_ville[1] = ville->nom_ville[1];
        if (premiere_lettre_ville[1] >= 'a' && premiere_lettre_ville[1] <= 'z') {
            premiere_lettre_ville[1] += 'A' - 'a';
        }
        premiere_lettre_ville[2] = ville->nom_ville[2];
        if (premiere_lettre_ville[2] >= 'a' && premiere_lettre_ville[2] <= 'z') {
            premiere_lettre_ville[2] += 'A' - 'a';
        }
        premiere_lettre_ville[3] = '\0';
        
        if (strcmp(premiere_lettre_ville, lettres) == 0) {
            printf("%5d : %s (%s)\n", ville->num_ville, ville->nom_ville, ville->nom_pays);
            il_y_a_au_moins_une_ville = 1;
        }
        ville = ville->ville_suivante;
    }

    return il_y_a_au_moins_une_ville;
}

// Permet de retrouver une ville à partir de son numéro
Ville* trouver_ville(Liste_villes liste_villes, int num_ville) {
    Ville* ville = liste_villes.tete_villes;
    while (ville != NULL && ville->num_ville != num_ville) {
        ville = ville->ville_suivante;
    }
    return ville;
}

int compter_nombre_aeroports_ville(Ville ville) {
    int nombre_aeroports = 0;
    Aero* aero_curseur = ville.liste_aeroports->tete_aeros;
    while (aero_curseur != NULL) {
        nombre_aeroports++;
        aero_curseur = aero_curseur->aero_suivant;
    }
    
    return nombre_aeroports;
}

int compter_nombre_aeroports(Liste_villes liste_villes) {
    int nombre_aeroports = 0;
    Ville* ville_curseur = liste_villes.tete_villes;
    Aero* aero_curseur;
    while (ville_curseur != NULL) {
        aero_curseur = ville_curseur->liste_aeroports->tete_aeros;
        while (aero_curseur != NULL) {
            nombre_aeroports++;
            aero_curseur = aero_curseur->aero_suivant;
        }
        ville_curseur = ville_curseur->ville_suivante;
    }

    return nombre_aeroports;
}

Aero* trouver_aeroport(Ville ville, int index) {
    Aero* aero_curseur = ville.liste_aeroports->tete_aeros;
    for (int i = 0; i < index; i++) {
        aero_curseur = aero_curseur->aero_suivant;
    }
    return aero_curseur;
}

int index_plus_petite_valeur(int* valeurs, int size) {
    int index_min = 0;
    int valeur_min = valeurs[0];
    for (int i = 1; i < size; i++) {
        if (valeurs[i] < valeur_min) {
            valeur_min = valeurs[i];
            index_min = i;
        }
    }

    return index_min;
}

int plus_petite_valeur(int* valeurs, int size) {
    int valeur_min = valeurs[0];
    for (int i = 1; i < size; i++) {
        if (valeurs[i] < valeur_min) {
            valeur_min = valeurs[i];
        }
    }

    return valeur_min;
}

int dijkstra(Liste_villes* liste_villes, char aeroport_depart_fs[10], char aeroport_arrivee_fs[10], int nb_escales_max, int afficher_chemin) {
    // INITIALISATION
    int nb_escales = 0;
    // Initialisation du noeud d'arrivée
    Noeud* destination;

    // Initialisation du noeud de parcours à la valeur de départ
    Noeud* noeud_actuel;

    // Initialisation du tableau des noeuds
    int nombre_aeroports = compter_nombre_aeroports(*liste_villes);
    Noeud* noeuds = (Noeud*)malloc(nombre_aeroports*sizeof(Noeud));
    int numero_aeroport = 0;
    Ville* ville_curseur = liste_villes->tete_villes;
    Aero* aero_curseur;
    while (ville_curseur != NULL) {
        aero_curseur = ville_curseur->liste_aeroports->tete_aeros;
        while (aero_curseur != NULL) {
            if (aero_curseur->code_fs[0] == aeroport_depart_fs[0] && aero_curseur->code_fs[1] == aeroport_depart_fs[1] && aero_curseur->code_fs[2] == aeroport_depart_fs[2] && aero_curseur->code_fs[3] == aeroport_depart_fs[3]) {
                noeuds[numero_aeroport].etat = 2;
                noeuds[numero_aeroport].date_arrivee = 05000; // Pour simplifier, on suppose que nous sommes au plus tard le 05/02/2016 à 00:00
                noeud_actuel = &noeuds[numero_aeroport];
            } else if (aero_curseur->code_fs[0] == aeroport_arrivee_fs[0] && aero_curseur->code_fs[1] == aeroport_arrivee_fs[1] && aero_curseur->code_fs[2] == aeroport_arrivee_fs[2] && aero_curseur->code_fs[3] == aeroport_arrivee_fs[3]) {
                noeuds[numero_aeroport].etat = 0;
                noeuds[numero_aeroport].date_arrivee = 22222;
                destination = &noeuds[numero_aeroport];
            } else {
                noeuds[numero_aeroport].etat = 0;
                noeuds[numero_aeroport].date_arrivee = 0;
            }
            noeuds[numero_aeroport].noeud_precedent = NULL;
            noeuds[numero_aeroport].aeroport = aero_curseur;
            
            numero_aeroport++;
            aero_curseur = aero_curseur->aero_suivant;
        }
        ville_curseur = ville_curseur->ville_suivante;
    }
    
    // ALGORITHME
    int fin_algo = 0;
    Vol* vol_curseur;
    while (fin_algo == 0) {
        nb_escales++;
        // On vérifie si on a trouvé le chemin le plus court
        for (int i = 0; i < nombre_aeroports; i++) {
            if (nb_escales >= nb_escales_max || (noeuds[i].etat == 1 && noeuds[i].date_arrivee >= destination->date_arrivee)) {
                fin_algo = 1;
            }
        }
        // Marque les aéroports accessibles depuis le noeud actuel
        vol_curseur = noeud_actuel->aeroport->liste_vols->tete_vols;
        while (vol_curseur != NULL) { // Parcours tous les vols du noeud
            if (noeud_actuel->date_arrivee < vol_curseur->date_depart) { // Si le vol part après notre arrivée dans l'aéroport actuel
                for (int i = 0; i < nombre_aeroports; i++) {
                    if (strcmp(vol_curseur->destination->code_fs, noeuds[i].aeroport->code_fs) == 0 && noeuds[i].etat < 2) {
                        if (noeuds[i].etat == 0) {
                            noeuds[i].etat = 1;
                            noeuds[i].date_arrivee = vol_curseur->date_arrivee;
                        } else if (vol_curseur->date_arrivee < noeuds[i].date_arrivee) {
                            noeuds[i].date_arrivee = vol_curseur->date_arrivee;
                        }
                        noeuds[i].noeud_precedent = noeud_actuel;
                    }
                }
            }
            vol_curseur = vol_curseur->vol_suivant;
        }
        // Recherche du prochain noeud à traiter (le plus petit parmi les état == 1)
        int index_min;
        int val_min = 22222;
        for (int i = 0; i < nombre_aeroports; i++) { // Parcours tous les noeuds
            if (noeuds[i].etat == 1 && noeuds[i].date_arrivee < val_min) {
                index_min = i;
                val_min = noeuds[i].date_arrivee;
            }
        }
        Noeud* noeud_temp = noeud_actuel;
        noeud_actuel = &noeuds[index_min];
        noeud_actuel->noeud_precedent = noeud_temp;
        noeud_actuel->etat = 2;
    }

    // if (destination->date_arrivee < 22222) {
    //     printf("Departure : %s | Arrival : %s | Date : %d | Nb it : %d\n", aeroport_depart_fs, aeroport_arrivee_fs, destination->date_arrivee, nb_escales);
    // }

    // Affichage du chemin
    if (afficher_chemin) {
        printf("--------------------------------\n");
        printf("Arrivée ");
        Noeud* noeud_curseur = destination;
        while (noeud_curseur != NULL) {
            printf("<- %s (%s) ", noeud_curseur->aeroport->nom_aeroport, noeud_curseur->aeroport->code_fs);
            noeud_curseur = noeud_curseur->noeud_precedent;
        }
        printf("<- départ\n");
    }

    // RETOUR
    return destination->date_arrivee;
}


// MAIN
int main(int argc, char const *argv[])
{
    printf("Bonjour, nous sommes le 04/02/2016. Il est 15h36. (Date et heure supposées pour l'exercice)\n");
    printf("Chargement des données :\n");
    // Chargement des données
    Liste_villes* liste_villes = lister_aeroports();
    lister_vols(liste_villes);
    int nb_escales_max;

    // Interaction utilisateur
    int calculer_vol = 0;
    int choix_utilisateur = -1;
    int num_ville_depart;
    int num_ville_arrivee;
    char lettre_ville_depart;
    char lettres_ville_arrivee[10];
    while (choix_utilisateur != 0) {
        if (choix_utilisateur == -1) {
            printf("Veuillez entrer la première lettre de la ville de départ : ");
            scanf(" %c", &lettre_ville_depart);
            printf("--------------------------------\n");
            if (afficher_villes_depart(*liste_villes, lettre_ville_depart) == 0) {
                printf("Il n'y a pas de vol prévu d'une ville commançant par la lettre : %c\n", lettre_ville_depart);
            }
            printf("--------------------------------\n");
            printf("   -1 : relancer une recherche\n");
            printf("    0 : quitter\n");
            printf("Veuillez indiquer votre choix : ");
            scanf("%d", &choix_utilisateur);
        } if (choix_utilisateur > 0) {
            // On enregistre le numéro de la ville de départ
            num_ville_depart = choix_utilisateur;
            // Interaction utilisateur
            printf("Veuillez entrer les trois premières lettres de la ville de destination : ");
            scanf("%s", lettres_ville_arrivee);
            lettres_ville_arrivee[3] = '\0';
            printf("--------------------------------\n");
            if (afficher_villes_arrivee(*liste_villes, lettres_ville_arrivee) == 0) {
                printf("Il n'y a pas de vol prévu vers une ville commançant par les lettres : %s\n", lettres_ville_arrivee);
            }
            printf("--------------------------------\n");
            printf("   -2 : relancer une recherche de destination\n");
            printf("   -1 : relancer une recherche complète\n");
            printf("    0 : quitter\n");
            printf("Veuillez indiquer votre choix : ");
            scanf("%d", &choix_utilisateur);
            if (choix_utilisateur > 0) {
                num_ville_arrivee = choix_utilisateur;
                // Interaction utilisateur
                printf("Nombre d'escales maximum : ");
                scanf("%d", &nb_escales_max);
                if (nb_escales_max > 10) {
                    nb_escales_max = 10;
                }
                
                calculer_vol = 1;
            }
        } if (calculer_vol == 1) { // Afin de gagner en efficacité, plusieurs processus calculerons le meilleur aéroport de départ et d'arrivée (car plusieurs aéroports par ville) à l'aide de l'algorithme de Dijkstra
            // Meilleur aéroport de départ et d'arrivée
            Aero* meilleur_aeroport_depart;
            Aero* meilleur_aeroport_arrivee;

            // Enregistre les villes de départ et d'arrivée
            Ville* ville_depart = trouver_ville(*liste_villes, num_ville_depart);
            Ville* ville_arrivee = trouver_ville(*liste_villes, num_ville_arrivee);

            // Trouver le nombre d'aéroports de départ : x
            int nombre_aeroports_depart = compter_nombre_aeroports_ville(*ville_depart);

            // Trouver le nombre d'aéroports d'arrivée : y
            int nombre_aeroports_arrivee = compter_nombre_aeroports_ville(*ville_arrivee);

            // Fork x fois
            __pid_t* pids_depart = malloc(nombre_aeroports_depart*sizeof(__pid_t));
            __pid_t pid_pere = getpid();
            Aero* aero_depart;
            int* dates_arrivee_chemin_x = malloc(nombre_aeroports_depart*sizeof(int));
            int date_arrivee_chemin_x;
            for (int i = 0; i < nombre_aeroports_depart; i++) {
                if (getpid() == pid_pere) {
                    pids_depart[i] = fork();
                    aero_depart = trouver_aeroport(*ville_depart, i);
                }
            }
            if (getpid() == pid_pere) { // Si père
                for (int i = 0; i < nombre_aeroports_depart; i++) {
                    waitpid(pids_depart[i], &date_arrivee_chemin_x, 0); // RECEPTION A : 2/2 - Plus court chemin par aéroport de départ
                    dates_arrivee_chemin_x[i] = WEXITSTATUS(date_arrivee_chemin_x);
                }
                // On connait désormais le meilleur aéroport de départ
                meilleur_aeroport_depart = trouver_aeroport(*ville_depart, index_plus_petite_valeur(dates_arrivee_chemin_x, nombre_aeroports_depart));
                // printf("Il vaut mieux partir de l'aéroport : %s\n", trouver_aeroport(*ville_depart, index_plus_petite_valeur(dates_arrivee_chemin_x, nombre_aeroports_depart))->nom_aeroport);
                // Recherche du meilleur aéroport d'arrivée
                //------------------------------------------------------------------------------------------------------
                __pid_t* pids_arrivee = malloc(nombre_aeroports_arrivee*sizeof(__pid_t));
                __pid_t pid_pere = getpid();
                Aero* aero_arrivee;
                int* dates_arrivee_chemin = malloc(nombre_aeroports_arrivee*sizeof(int));
                int date_arrivee_chemin;
                for (int i = 0; i < nombre_aeroports_arrivee; i++) {
                    if (getpid() == pid_pere) {
                        pids_arrivee[i] = fork();
                        aero_arrivee = trouver_aeroport(*ville_arrivee, i);
                    }
                }
                if (getpid() == pid_pere) { // Si père
                    for (int i = 0; i < nombre_aeroports_arrivee; i++) {
                        waitpid(pids_arrivee[i], &date_arrivee_chemin, 0); // RECEPTION B : 1/2 - Meilleur chemin pour chaque aéroport d'arrivée depuis l'aéroport actuel
                        dates_arrivee_chemin[i] = WEXITSTATUS(date_arrivee_chemin);
                    }
                    meilleur_aeroport_arrivee = trouver_aeroport(*ville_arrivee, index_plus_petite_valeur(dates_arrivee_chemin, nombre_aeroports_arrivee));
                    // printf("Il vaut mieux arriver à l'aéroport : %s\n", meilleur_aeroport_arrivee->nom_aeroport);

                    // TODO : Dijkstra final pour connaitre le meilleur itinéraire
                    int date_finale_arrivee = dijkstra(liste_villes, meilleur_aeroport_depart->code_fs, meilleur_aeroport_arrivee->code_fs, nb_escales_max, 1);
                    if (date_finale_arrivee == 22222) {
                        printf("Nous sommes navrés de vous apprendre qu'il n'existe pas de vol direct ou de correspondances vers cette destination actuellement.\n");
                    } else {
                        char date_affichee[12];
                        sprintf(date_affichee, "%d", date_finale_arrivee);
                        printf("En prenant cet itinéraire, vous arriverez le 0%c/02/2016 à %c%ch%c0.\n", date_affichee[0], date_affichee[1], date_affichee[2], date_affichee[3]);
                        printf("--------------------------------\n");
                    }

                    // FIN DU PROGRAMME////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    printf("Merci d'avoir utilisé nos services, à bientôt !\n");
                    choix_utilisateur = 0;
                    // FIN DU PROGRAMME////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                } else { // chaque fils -> Dijkstra s'effectue ici
                    exit((int)(dijkstra(liste_villes, meilleur_aeroport_depart->code_fs, aero_arrivee->code_fs, nb_escales_max, 0)/255)); // RETOUR B : 1/2 - Plus courte date de chaque chemin arrivant à cet aéroport
                }
                //------------------------------------------------------------------------------------------------------
            } else { // Chaque fils cherche ici le meilleur aéroport de départ en comparant leurs meilleurs chemins jusqu'à la ville de destination (en essayant chaque aéroport d'arrivée)
            //------------------------------------------------------------------------------------------------------
                __pid_t* pids_arrivee = malloc(nombre_aeroports_arrivee*sizeof(__pid_t));
                __pid_t pid_pere = getpid();
                Aero* aero_arrivee;
                int* dates_arrivee_chemin_y = malloc(nombre_aeroports_arrivee*sizeof(int));
                int date_arrivee_chemin_y;
                for (int i = 0; i < nombre_aeroports_arrivee; i++) {
                    if (getpid() == pid_pere) {
                        pids_arrivee[i] = fork();
                        aero_arrivee = trouver_aeroport(*ville_arrivee, i);
                    }
                }
                if (getpid() == pid_pere) { // Si père
                    for (int i = 0; i < nombre_aeroports_arrivee; i++) {
                        waitpid(pids_arrivee[i], &date_arrivee_chemin_y, 0); // RECEPTION A : 1/2 - Meilleur chemin pour chaque aéroport d'arrivée depuis l'aéroport actuel
                        dates_arrivee_chemin_y[i] = WEXITSTATUS(date_arrivee_chemin_y);
                    }
                    exit(plus_petite_valeur(dates_arrivee_chemin_y, nombre_aeroports_arrivee)); // RETOUR A : 2/2 - Le chemin le plus court parmis ceux partant de cet aéroport
                } else { // chaque fils -> Dijkstra s'effectue ici
                    exit((int)(dijkstra(liste_villes, aero_depart->code_fs, aero_arrivee->code_fs, nb_escales_max, 0)/255)); // RETOUR A : 1/2 - Plus courte date de chaque chemin arrivant à cet aéroport
                }
            //------------------------------------------------------------------------------------------------------
            }
        }   
    }

    return 0;
}




// char nombre[10] = "8.92\0";
    // float nombree = atof(nombre);
    // printf("char : %s -> int : %f\n", nombre, nombree);