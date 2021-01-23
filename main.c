#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// STRUCTURES DE DONNÉES
typedef struct Vol Vol;
typedef struct Liste_vols Liste_vols;

typedef struct Aero Aero;
typedef struct Liste_aeros Liste_aeros;

typedef struct Ville Ville;
typedef struct Liste_villes Liste_villes;

struct Vol {
    // Destination
    Aero* destination;
    // Dates départ
    int annee_depart;
    int mois_depart;
    int jour_depart;
    int heure_depart;
    int minute_depart;
    // Dates arrivée
    int annee_arrivee;
    int mois_arrivee;
    int jour_arrivee;
    int heure_arrivee;
    int minute_arrivee;
    // Suivant
    Vol* vol_suivant;
};
struct Liste_vols {
    Vol* tete_vols;
};

struct Aero {
    // Est
    char nom_aeroport[50];
    char code_fs[5];
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

// Parcours l'arborescence, si la ville n'existe pas, la crée, puis crée l'aéroport
void inserer_aeroport(Liste_villes* liste_villes, char nom_ville[50], char code_fs[5], char nom_aeroport[50], char nom_pays[50], char decalage_utc[10]) {
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
        printf("Erreur de lecture du fichier\n");
        exit(0);
    }

    // Initialisation des variables :
        // de la ville
        Liste_villes* liste_villes = malloc(sizeof(Liste_villes));

        // de parcours
        char caractere_courant = fgetc(fichier_aeroports);

        // de reconnaissance des données
        int validation_nom_ville = 0; // city":"Cali"
        int compteur_nom_ville = 0;
        int validation_code_fs = 0; // fs":"CLO
        int validation_nom_aeroport = 0; // name":"Mukhaizna Airport"
        int compteur_nom_aeroport = 0;
        int validation_nom_pays = 0; // countryName":"Oman"
        int compteur_nom_pays = 0;
        int validation_decalage_utc = 0; // utcOffsetHours":-6.0,
        int compteur_utc = 0; // Permet de savoir à quelle position écrire la valeur du décalage utc dans le tableau correspondant

        // d'enregistrement temporaire (le temps de la lecture) des données d'un aéroport
        char nom_ville[50];
        char code_fs[6];
        char nom_aeroport[50];
        char nom_pays[50];
        char decalage_utc[10];

    // Parcours tout le fichier
    while (caractere_courant != ']' && caractere_courant != EOF) {
        // On enregistre les données d'un aeroport à chaque '}'
        if (caractere_courant == '}') {
            inserer_aeroport(liste_villes, nom_ville, code_fs, nom_aeroport, nom_pays, decalage_utc);
            // printf("fs : %s : %ld\n", code_fs, sizeof(code_fs));
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
        if (validation_code_fs == 7) { // Si fin de lecture
            code_fs[2] = caractere_courant;
            validation_code_fs = 0;
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

// Un nouveau vol est détecté par "arrivalAirportFsCode"
void recenser_vols(Liste_villes* liste_villes) {
    FILE* fichier_vols = fopen("Avions_Bdd/20160206_flightstatus.json", "r");
    if (fichier_vols == NULL) {
        printf("Erreur de lecture du fichier\n");
        exit(0);
    }
    fseek(fichier_vols, 13, SEEK_SET);
    
    // Initialisation des variables
        // de parcours
        char caractere_courant = fgetc(fichier_vols);
        // de reconnaissance des données
        int validation_code_fs_arrivee = 0; // arrivalAirportFsCode":"AMS
        int validation_date_arrivee = 0; // Utc":"2016-02-06T06:30:00.000Z
        int validation_code_fs_depart = 0; // departureAirportFsCode":"AAL
        int validation_date_depart = 0; // Utc":"2016-02-06T05:05:00.000Z
        int arrivee_ou_depart = 0; // 0 : arrivée / 1 : départ

        // d'enregistrement temporaire (le temps de la lecture) des données d'un aéroport
        char code_fs_arrivee[6];
        char date_arrivee[25];
        char code_fs_depart[6];
        char date_depart[25];

        int nb_vols = 0;
    
    while (caractere_courant != EOF) {
        // FS ARRIVEE
        if (validation_code_fs_arrivee == 25) { // Si fin de lecture
            code_fs_arrivee[2] = caractere_courant;

            // TODO: enregistrer le vol
            validation_code_fs_arrivee = 0;
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
        if (validation_code_fs_depart == 27) { // Si fin de lecture
            code_fs_depart[2] = caractere_courant;
            validation_code_fs_depart = 0;
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

    fclose(fichier_vols);
}

void afficher_liste(Liste_villes liste) {
    int nb_aeroports = 0;
    int nb_villes = 0;
    Ville* ville = liste.tete_villes;
    while (ville != NULL)
    {
        Aero* aeroport = ville->liste_aeroports->tete_aeros;
        while (aeroport != NULL) {
            printf("%s   %s   %s\n", aeroport->code_fs, aeroport->nom_aeroport, aeroport->ville->nom_ville);
            nb_aeroports++;
            aeroport = aeroport->aero_suivant;
        }
        // printf("%s\n", ville->nom_ville);
        ville = ville->ville_suivante;
        nb_villes++;
    }
    printf("%d\n", nb_villes);
}


int main(int argc, char const *argv[])
{
    Liste_villes* liste_villes = lister_aeroports();

    clock_t start = clock();
    // afficher_liste(*liste_villes);
    recenser_vols(liste_villes);
    clock_t end = clock() - start;
    printf("%ld ms\n", (end*1000/CLOCKS_PER_SEC));

    // char nombre[10] = "8.92\0";
    // float nombree = atof(nombre);
    // printf("char : %s -> int : %f\n", nombre, nombree);

    return 0;
}
