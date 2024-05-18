#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fonction.h"

uint8_t frame_check_sequence(uint8_t message) {
    uint16_t m = (uint16_t) message << 8; // on ajoute 9-1 zéros à la fin du message
    // printf("crc 8 m: ");
    // print_bits16(m);
    uint16_t polynome = POLYNOME << 7; // Align the polynomial to the left

    for (int i = 0; i < 8; i++) {
        if (m & 0b1000000000000000) { // Si le bit le plus à gauche est 1
            m ^= polynome; // XOR avec le polynôme
        }
        m <<= 1; // Décaler le message à gauche
        // print_bits16(m);
    }
    // printf("fin crc 8\n");
    return (uint8_t)(m >> 8); // Retourner le CRC (les 8 bits de gauche)
}

uint8_t crc8(uint16_t message)
{
    // printf("crc 8 m: ");
    // print_bits16(message);
    uint16_t polynome = POLYNOME << 7; // Align the polynomial to the left

    for (int i = 0; i < 8; i++) {
        if (message & 0x8000) { // Si le bit le plus à gauche est 1
            message ^= polynome; // XOR avec le polynôme
        }
        message <<= 1; // Décaler le message à gauche
        // print_bits16(message);
    }
    // printf("fin crc 8\n");
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
