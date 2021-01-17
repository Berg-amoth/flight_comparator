#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// STRUCTURES
typedef struct Vol Vol;
typedef struct Aero Aero;

struct Vol
{
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
};

struct Aero
{
    char nom_aeroport[50];
    char code[10];
    char ville[50];
    char pays[50];
    int decalage_utc;
    Vol* liste_vols;
};


// TRADUCTION JSON
// Recuperation des aeroports
Aero* initialiser_aeroports() {
    Aero* liste_aeroports = malloc(sizeof(Aero));

    Aero* aeroport_courant = malloc(sizeof(*aeroport_courant));



    return liste_aeroports;
}


// VÉRIFICATION DATES


// DIJSKTRA


// MAIN
int main(int argc, char const *argv[])
{
    // Liste des aeroports (uniquement ceux dont on connait les vols)
    Aero* aeroports = initialiser_aeroports();






    // TESTS
    Aero* aeroport1 = malloc(sizeof(Aero));
    strcpy(aeroport1->nom_aeroport, "Charles de Gaulle");

    Aero* aeroport2 = malloc(sizeof(Aero));
    strcpy(aeroport2->nom_aeroport, "Bordeaux");

    Vol* vol1 = malloc(sizeof(Vol));
    vol1->destination = aeroport2;
    vol1->annee_depart = 2020;
    vol1->mois_depart = 10;
    vol1->jour_depart = 12;
    vol1->heure_depart = 15;

    printf("%d\n", vol1->annee_depart);
    
    
    return 0;
}
