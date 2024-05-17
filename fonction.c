#include <stdio.h>
#include <stdlib.h>
#include "fonction.h"

int calculate_parity(unsigned char octet) {
    int p = 0;

    // Parcourir les 8 bits de l'octet
    for (int i = 0; i < 8; i++) {
        // Vérifier si le i-ème bit est activé
        if ((octet >> i) & 1) {
            // Si oui, inverser le bit de parité
            p = !p;
        }
    }
    return p;
}