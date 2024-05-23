#ifndef FONCTION_H
#define FONCTION_H

#define POLYNOME 0b10111001 // P(x) sans le terme x^8

uint8_t frame_check_sequence(uint8_t message);

uint8_t crc8(uint16_t message);

void print_bits8(uint8_t byte);

void print_bits16(uint16_t word);

#endif