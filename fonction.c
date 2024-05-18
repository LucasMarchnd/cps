#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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

uint8_t crc8(uint8_t message) {
    uint16_t m = (uint16_t) message << 8; // Multiplier par X^8
    uint16_t polynome = POLYNOME << 7; // Align the polynomial to the left

    for (int i = 0; i < 8; i++) {
        if (m & 0x8000) { // Si le bit le plus à gauche est 1
            m ^= polynome; // XOR avec le polynôme
        }
        m <<= 1; // Décaler le message à gauche
    }

    return (uint8_t)(m >> 8); // Retourner le CRC (les 8 bits de gauche)
}

void print_bits8(uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (byte >> i) & 0x01);
    }
    printf("\n");
}

void print_bits16(uint16_t word) {
    for (int i = 15; i >= 0; i--) {
        printf("%d", (word >> i) & 0x01);
    }
    printf("\n");
}