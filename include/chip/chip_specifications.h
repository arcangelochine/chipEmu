#ifndef CHIP_SPECIFICATIONS_H
#define CHIP_SPECIFICATIONS_H

#include "chip_datatype.h"

// chip memory
uint8 memory[0x1000];

// chip stack
uint16 stack[0x10];

// chip registers
uint8 V[0x10];

// chip program counter
uint16 PC;

// chip stack pointer
uint8 SP;

// chip address register
uint16 I;

// chip delay timer
uint8 delay_timer;

// chip sound timer
uint8 sound_timer;

// chip display
uint8 display[0x20][0x40];

// chip emulated keyboard: 1 for down, 0 for up
uint8 key_state[0x10];

// chip random value
uint8 next;

#endif