#include <chip/chip_datatype.h>
#include <chip/chip_instructions.h>
#include <chip/chip_specifications.h>

/**
 * 0nnn - Jump to a machine code routine at nnn.
 *
 * @param addr the address to jump to
 */
void SYS(uint16 addr)
{
    PC = addr;
}

/**
 * 00E0 - Clear the display.
 */
void CLS()
{
    for (int i = 0; i < 32; i++)
        for (int j = 0; j < 64; j++)
            display[i][j] = 0;
}

/**
 * 00EE - Return from a subroutine
 *
 * The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
 */
void RET()
{
    PC = stack[SP--];
}

/**
 * 1nnn - Jump to location nnn.
 *
 * The interpreter sets the program counter to nnn.
 *
 * @param addr the address to jump to
 */
void JP(uint16 addr)
{
    PC = addr;
}

/**
 * 2nnn - Call subroutine at nnn.
 *
 * The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.
 *
 * @param addr the address of the subroutine to call
 */
void CALL(uint16 addr)
{
    stack[++SP] = PC;
    PC = addr;
}

/**
 * 3xkk - Skip next instruction if Vx = kk.
 *
 * The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.
 *
 * @param reg the register to compare
 * @param val the value to compare
 */
void SE(uint8 reg, uint8 val)
{
    if (V[reg] == val)
        PC += 2;
}

/**
 * 4xkk - Skip next instruction if Vx != kk.
 *
 * The interpreter compares register Vx to kk, and if they are not equal, increments the program counter by 2.
 * @param reg the register to compare
 * @param val the value to compare
 */
void SNE(uint8 reg, uint8 val)
{
    if (V[reg] != val)
        PC += 2;
}

/**
 * 5xy0 - Skip next instruction if Vx = Vy.
 *
 * The interpreter compares register Vx to register Vy, and if they are equal, increments the program counter by 2.
 *
 * @param regX the first register to compare
 * @param regY the second register to compare
 */
void SE2(uint8 regX, uint8 regY)
{
    if (V[regX] == V[regY])
        PC += 2;
}

/**
 * 6xkk - Set Vx = kk.
 *
 * The interpreter puts the value kk into register Vx.
 *
 * @param reg the register to store the value in
 * @param val the value to load in the register
 */
void LD(uint8 reg, uint8 val)
{
    V[reg] = val;
}

/**
 * 7xkk - Set Vx = Vx + kk.
 *
 * Adds the value kk to the value of register Vx, then stores the result in Vx.
 *
 * @param reg the register to store the value in
 * @param val the value to add to the register
 */
void ADD(uint8 reg, uint8 val)
{
    V[reg] += val;
}

/**
 * 8xy0 - Set Vx = Vy.
 *
 * Stores the value of register Vy in register Vx.
 *
 * @param regX the register to store the value in
 * @param regY the register to grab the value from
 */
void LD2(uint8 regX, uint8 regY)
{
    V[regX] = V[regY];
}

/**
 * 8xy1 - Set Vx = Vx OR Vy.
 *
 * Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
 *
 * @param regX the register to store the value in
 * @param regY the register to grab the value from
 */
void OR(uint8 regX, uint8 regY)
{
    V[regX] |= V[regY];
    V[0xF] = 0;
}

/**
 * 8xy2 - Set Vx = Vx AND Vy.
 *
 * Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
 *
 * @param regX the register to store the value in
 * @param regY the register to grab the value from
 */
void AND(uint8 regX, uint8 regY)
{
    V[regX] &= V[regY];
    V[0xF] = 0;
}

/**
 * 8xy3 - Set Vx = Vx XOR Vy.
 *
 * Performs a bitwise XOR on the values of Vx and Vy, then stores the result in Vx.
 *
 * @param regX the register to store the value in
 * @param regY the register to grab the value from
 */
void XOR(uint8 regX, uint8 regY)
{
    V[regX] ^= V[regY];
    V[0xF] = 0;
}

/**
 * 8xy4 - Set Vx = Vx + Vy, set VF = carry.
 *
 * The values of Vx and Vy are added together.
 * If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0.
 * Only the lowest 8 bits of the result are kept, and stored in Vx.
 *
 * @param regX the register to store the value in
 * @param regY the register to grab the value from
 */
void ADD2(uint8 regX, uint8 regY)
{
    uint8 overflow = 0;
    if ((uint16)V[regX] + (uint16)V[regY] > 255)
        overflow = 1;

    V[regX] += V[regY];

    V[0xF] = overflow;
}

/**
 * 8xy5 - Set Vx = Vx - Vy, set VF = NOT borrow.
 *
 * If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
 *
 * @param regX the register to store the value in
 * @param regY the register to grab the value from
 */
void SUB(uint8 regX, uint8 regY)
{
    uint8 underflow = 0;
    if (V[regX] > V[regY])
        underflow = 1;

    V[regX] -= V[regY];

    V[0xF] = underflow;
}

/**
 * 8xy6 - Set Vx = Vy SHR 1.
 *
 * If the least-significant bit of Vy is 1, then VF is set to 1, otherwise 0. Then Vy is divided by 2 and the result is stored in Vx.
 *
 * @param regX the register to store the value in
 * @param regY the register to grab the value from
 */
void SHR(uint8 regX, uint8 regY)
{
    uint8 lsb = 0;
    if (V[regY] & 0x01)
        lsb = 1;

    V[regX] = V[regY] >> 1;

    V[0xF] = lsb;
}

/**
 * 8xy7 - Set Vx = Vy - Vx, set VF = NOT borrow.
 *
 * If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
 *
 * @param regX the register to store the value in
 * @param regY the register to grab the value from
 */
void SUBN(uint8 regX, uint8 regY)
{
    uint8 not_borrow = 0;
    if (V[regY] > V[regX])
        not_borrow = 1;

    V[regX] = V[regY] - V[regX];

    V[0xF] = not_borrow;
}

/**
 * 8xyE - Set Vx = Vx SHL 1.
 *
 * If the most-significant bit of Vy is 1, then VF is set to 1, otherwise 0. Then Vy is multiplied by 2 and the result is stored in Vx.
 *
 * @param regX the register to store the value in
 * @param regY the register to grab the value from
 */
void SHL(uint8 regX, uint8 regY)
{
    uint8 msb = 0;
    if (V[regY] & 0x80)
        msb = 1;

    V[regX] = V[regY] << 1;

    V[0xF] = msb;
}

/**
 * 9xy0 - Skip next instruction if Vx != Vy.
 *
 * The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
 *
 * @param regX the first register to compare
 * @param regY the second register to compare
 */
void SNE2(uint8 regX, uint8 regY)
{
    if (V[regX] != V[regY])
        PC += 2;
}

/**
 * Annn - Set I = nnn.
 *
 * The value of register I is set to nnn.
 *
 * @param addr the address to set in I
 */
void LD3(uint16 addr)
{
    I = addr;
}

/**
 * Bnnn - Jump to location nnn + V0.
 *
 * The program counter is set to nnn plus the value of V0.
 *
 * @param addr the address to jump to
 */
void JP2(uint16 addr)
{
    // PC = addr + V[(addr & 0xF0) >> 4];
    PC = addr + V[0x0];
}

/**
 * Cxkk - Set Vx = random byte AND kk.
 *
 * The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. The results are stored in Vx.
 *
 * @param reg the register to store the value in
 * @param val the value to & to the value
 */
void RND(uint8 reg, uint8 val)
{
    next = next * 4097 + 127;
    V[reg] = (next % 0x100) & val;
}

/**
 * Dxyn - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
 *
 * The interpreter reads n bytes from memory, starting at the address stored in I.
 * These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
 * Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
 * If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen.
 *
 * @param regX the register with x coordinate
 * @param regY the register with y coordinate
 * @param n number of sprites to draw
 */
void DRW(uint8 regX, uint8 regY, uint8 n)
{
    if (V[regX] > 63 || V[regY] > 32)
        return;

    uint8 vx = V[regX] % 64;
    uint8 vy = V[regY] % 32;
    V[0xF] = 0;

    for (uint8 y = 0; y < n; y++)
    {
        uint8 row = memory[I + y];
        for (uint8 x = 0; x < 8; x++)
        {
            if (vx + x > 63 || vy + y > 31)
                break;
            uint8 pixel = (row & (1 << (7 - x))) >> (7 - x);
            if (display[vy + y][vx + x] && pixel)
                V[0xF] = 1;
            display[vy + y][vx + x] ^= pixel;
        }
    }
}

/**
 * Ex9E - Skip next instruction if key with the value of Vx is pressed.
 *
 * Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.
 *
 * @param reg the register with key value
 */
void SKP(uint8 reg)
{
    if (key_state[V[reg]])
        PC += 2;
}

/**
 * ExA1 - Skip next instruction if key with the value of Vx is not pressed.
 *
 * Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
 *
 * @param reg the register with key value
 */
void SKNP(uint8 reg)
{
    if (!key_state[V[reg]])
        PC += 2;
}

/**
 * Fx07 - Set Vx = delay timer value.
 *
 * The value of DT is placed into Vx.
 *
 * @param reg the register to store the value in
 */
void LD4(uint8 reg)
{
    V[reg] = delay_timer;
}

/**
 * Fx0A - Wait for a key press, store the value of the key in Vx.
 *
 * All execution stops until a key is pressed, then the value of that key is stored in Vx.
 *
 * @param reg the register to store the value in
 */
void LD5(uint8 reg)
{
    V[reg] = 0;
    for (int i = 0; i < 16; i++)
        if (key_state[i] && !key_prev[i])
        {
            V[reg] = i;
            return;
        }
    PC -= 2;
}

/**
 * Fx15 - Set delay timer = Vx.
 *
 * DT is set equal to the value of Vx.
 *
 * @param reg the register to store the value in
 */
void LDDT(uint8 reg)
{
    delay_timer = V[reg];
}

/**
 * Fx18 - Set sound timer = Vx.
 *
 * ST is set equal to the value of Vx.
 *
 * @param reg the register to grab the value from
 */
void LDST(uint8 reg)
{
    sound_timer = V[reg];
}

/**
 * Fx1E - Set I = I + Vx.
 *
 * The values of I and Vx are added, and the results are stored in I.
 *
 * @param reg the register to grab the value from
 */
void ADDI(uint8 reg)
{
    I += V[reg];
}

/**
 * Fx29 - Set I = location of sprite for digit Vx.
 *
 * The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx.
 *
 * @param reg the register to grab the value from
 */
void LDF(uint8 reg)
{
    I = V[reg] * 5;
}

/**
 * Fx33 - Store BCD representation of Vx in memory locations I, I+1, and I+2.
 *
 * The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.
 *
 * @param reg the register to grab the value from
 */
void LDB(uint8 reg)
{
    memory[I] = V[reg] / 100;
    memory[I + 1] = (V[reg] % 100) / 10;
    memory[I + 2] = V[reg] % 10;
}

/**
 * Fx55 - Store registers V0 through Vx in memory starting at location I.
 *
 * The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
 *
 * @param reg the register to go through
 */
void LDI(uint8 reg)
{
    for (int i = 0; i <= reg; i++)
    {
        memory[I] = V[i];
        I++;
    }
}

/**
 * Fx65 - Read registers V0 through Vx from memory starting at location I.
 *
 * The interpreter reads values from memory starting at location I into registers V0 through Vx.
 *
 * @param reg the register to go through
 */
void LD6(uint8 reg)
{
    for (int i = 0; i <= reg; i++)
    {
        V[i] = memory[I];
        I++;
    }
}
