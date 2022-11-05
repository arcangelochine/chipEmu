#ifndef CHIP_INSTRUCTIONS_H
#define CHIP_INSTRUCTIONS_H

#include "chip_datatype.h"

void SYS(uint16);
void CLS();
void RET();

void JP(uint16);

void CALL(uint16);

void SE(uint8, uint8);

void SNE(uint8, uint8);

void SE2(uint8, uint8);

void LD(uint8, uint8);

void ADD(uint8, uint8);

void LD2(uint8, uint8);
void OR(uint8, uint8);
void AND(uint8, uint8);
void XOR(uint8, uint8);
void ADD2(uint8, uint8);
void SUB(uint8, uint8);
void SHR(uint8, uint8);
void SUBN(uint8, uint8);
void SHL(uint8, uint8);

void SNE2(uint8, uint8);

void LD3(uint16);

void JP2(uint16);

void RND(uint8, uint8);

void DRW(uint8, uint8, uint8);

void SKP(uint8);
void SKNP(uint8);

void LD4(uint8);
void LD5(uint8);
void LDDT(uint8);
void LDST(uint8);
void ADDI(uint8);
void LDF(uint8);
void LDB(uint8);
void LDI(uint8);
void LD6(uint8);

#endif