#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fonction.h"

uint8_t frame_check_sequence(uint8_t message) {
    uint16_t m = (uint16_t) message << 8; // on ajoute 9-1 zéros à la fin du message
    
    uint16_t polynome = POLYNOME << 7;

    for (int i = 0; i < 8; i++) {
        if (m & 0b1000000000000000) {
            m ^= polynome; // XOR avec le polynôme
        }
        m <<= 1;
    }
    return (uint8_t)(m >> 8); // Retourner le  frame_check_sequence (les 8 bits de gauche)
}

uint8_t crc8(uint16_t message)
{
    uint16_t polynome = POLYNOME << 7;

    for (int i = 0; i < 8; i++) {
        if (message & 0x8000) { 
            message ^= polynome;
        }
        message <<= 1;
    }
    return (uint8_t)(message >> 8); // Retourner le CRC (les 8 bits de gauche)
}

void print_bits8(uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (byte >> i) & 0b1);
    }
    printf("\n");
}

void print_bits16(uint16_t word) {
    for (int i = 15; i >= 0; i--) {
        printf("%d", (word >> i) & 0b1);
    }
    printf("\n");
}
