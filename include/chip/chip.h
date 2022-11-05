#ifndef CHIP_H
#define CHIP_H

#include "chip_datatype.h"
#include "chip_specifications.h"
#include "chip_instructions.h"

// chip display scale
uint8 scaling;

// chip frame rate
uint8 frame_rate;

// chip frame counter
uint64 frame;

void util_chip_init();
uint8 util_chip_load_ROM(const char *);
void util_chip_execute(uint16);

uint8 alpha(uint32);
uint8 red(uint32);
uint8 green(uint32);
uint8 blue(uint32);

#endif