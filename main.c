#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// STRUCTURES
typedef struct Vol Vol;
typedef struct Aero Aero;

struct Vol
{
    Aero* destination;
    struct tm * date_depart;
    int annee_depart;
    int mois_depart;
    int jour_depart;
    int heure_depart;
    int minute_depart;
    int annee_arrivee;
    int mois_arrivee;
    int jour_arrivee;
    int heure_arrivee;
    int minute_arrivee;
};

struct Aero
{
    char nom_aeroport[50];
    Vol* liste_vols;
};


// TRADUCTION JSON


// VÉRIFICATION DATES


// DIJSKTRA


// MAIN
int main(int argc, char const *argv[])
{
    Aero* aeroport1 = malloc(sizeof(*aeroport1));
    strcpy(aeroport1->nom_aeroport, "Charles de Gaulle");

    Aero* aeroport2 = malloc(sizeof(*aeroport2));
    strcpy(aeroport2->nom_aeroport, "Bordeaux");

    Vol* vol1 = malloc(sizeof(*vol1));
    vol1->destination = aeroport2;
    vol1->annee_depart = 2020;
    vol1->mois_depart = 10;
    vol1->jour_depart = 12;
    vol1->heure_depart = 15;
    

    printf("%d %d\n", vol1->jour_depart, vol1->heure_depart);
    
    
    return 0;
}
