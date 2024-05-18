#ifndef FONCTION_H
#define FONCTION_H

#define POLYNOME 0b00000011 // P(x) sans le terme x^8

int calculate_parity(unsigned char byte);

uint8_t crc8(uint8_t message);

void print_bits8(uint8_t byte);

void print_bits16(uint16_t word);

#endif