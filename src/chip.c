#include <chip/chip.h>

#include <stdio.h>
#include <time.h>

/**
 * @brief Initialize chip
 */
void util_chip_init()
{
    // initialize memory
    for (uint16 i = 0; i < 0x1000; i++)
        memory[i] = 0;

    // initialize stack
    for (uint8 i = 0; i < 0x10; i++)
        stack[i] = 0;

    // initialize registers
    for (uint8 i = 0; i < 0x10; i++)
        V[i] = 0;

    // initialize program counter
    PC = 0x200;

    // initialize stack pointer
    SP = 0;

    // initialize address register
    I = 0;

    // initialize timers
    delay_timer = 0;
    sound_timer = 0;

    // initialize display (clear)
    for (uint8 y = 0; y < 0x20; y++)
        for (uint8 x = 0; x < 0x40; x++)
            display[y][x] = 0;

    uint8 default_font[0x50] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,
        0x20, 0x60, 0x20, 0x20, 0x70,
        0xF0, 0x10, 0xF0, 0x80, 0xF0,
        0xF0, 0x10, 0xF0, 0x10, 0xF0,
        0x90, 0x90, 0xF0, 0x10, 0x10,
        0xF0, 0x80, 0xF0, 0x10, 0xF0,
        0xF0, 0x80, 0xF0, 0x90, 0xF0,
        0xF0, 0x10, 0x20, 0x40, 0x40,
        0xF0, 0x90, 0xF0, 0x90, 0xF0,
        0xF0, 0x90, 0xF0, 0x10, 0xF0,
        0xF0, 0x90, 0xF0, 0x90, 0x90,
        0xE0, 0x90, 0xE0, 0x90, 0xE0,
        0xF0, 0x80, 0x80, 0x90, 0xF0,
        0xE0, 0x90, 0x90, 0x90, 0xE0,
        0xF0, 0x80, 0xF0, 0x80, 0xF0,
        0xF0, 0x80, 0xF0, 0x80, 0x80};

    // load font
    for (uint8 i = 0; i < 0x50; i++)
        memory[i] = default_font[i];

    // initialize emulated keyboard
    for (int i = 0; i < 16; i++)
        key_state[i] = 0;

    // initialize random value
    next = time(0);
}

/**
 * @brief Load a ROM from fileName path
 *
 * @param fileName the file's path to grab the ROM from
 * @return 1 if error occurred, 0 otherwise 
 */
uint8 util_chip_load_ROM(const char *fileName)
{
    FILE *file = fopen(fileName, "rb");

    if (file == 0)
    {
        perror("Failed to load ROM.\n");
        return 1;
    }

    fread(memory + 0x200, sizeof(memory), 1, file);

    fclose(file);

    return 0;
}

/**
 * @brief Given an opcode, executes the associated instruction
 *
 * @param opcode the opcode of the instruction to execute
 * @param logFile the output file for debugging
 */
void util_chip_execute(uint16 opcode)
{
    PC += 2;

    switch ((opcode & 0xF000) >> 12)
    {
    case 0x0:
        switch (opcode & 0x0FFF)
        {
        case 0x0E0:
            CLS();
            break;
        case 0x0EE:
            RET();
            break;
        default:
            SYS(opcode & 0x0FFF);
            break;
        }
        break;
    case 0x1:
        JP(opcode & 0x0FFF);
        break;
    case 0x2:
        CALL(opcode & 0x0FFF);
        break;
    case 0x3:
        SE((opcode & 0x0F00) >> 8, opcode & 0x00FF);
        break;
    case 0x4:
        SNE((opcode & 0x0F00) >> 8, opcode & 0x00FF);
        break;
    case 0x5:
        switch (opcode & 0x000F)
        {
        case 0:
            SE2((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
            break;
        default:
            break;
        }
        break;
    case 0x6:
        LD((opcode & 0x0F00) >> 8, opcode & 0x00FF);
        break;
    case 0x7:
        ADD((opcode & 0x0F00) >> 8, opcode & 0x00FF);
        break;
    case 0x8:
        switch (opcode & 0x000F)
        {
        case 0x0:
            LD2((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
            break;
        case 0x1:
            OR((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
            break;
        case 0x2:
            AND((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
            break;
        case 0x3:
            XOR((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
            break;
        case 0x4:
            ADD2((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
            break;
        case 0x5:
            SUB((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
            break;
        case 0x6:
            SHR((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
            break;
        case 0x7:
            SUBN((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
            break;
        case 0xE:
            SHL((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
            break;
        default:
            break;
        }
        break;
    case 0x9:
        switch (opcode & 0x000F)
        {
        case 0:
            SNE2((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
            break;
        default:
            break;
        }
        break;
    case 0xA:
        LD3(opcode & 0x0FFF);
        break;
    case 0xB:
        JP2(opcode & 0x0FFF);
        break;
    case 0xC:
        RND((opcode & 0x0F00) >> 8, opcode & 0x00FF);
        break;
    case 0xD:
        DRW((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4, opcode & 0x000F);
        break;
    case 0xE:
        switch (opcode & 0x00FF)
        {
        case 0x9E:
            SKP((opcode & 0x0F00) >> 8);
            break;
        case 0xA1:
            SKNP((opcode & 0x0F00) >> 8);
            break;
        default:
            break;
        }
        break;
    case 0xF:
        switch (opcode & 0x00FF)
        {
        case 0x07:
            LD4((opcode & 0x0F00) >> 8);
            break;
        case 0x0A:
            LD5((opcode & 0x0F00) >> 8);
            break;
        case 0x15:
            LDDT((opcode & 0x0F00) >> 8);
            break;
        case 0x18:
            LDST((opcode & 0x0F00) >> 8);
            break;
        case 0x1E:
            ADDI((opcode & 0x0F00) >> 8);
            break;
        case 0x29:
            LDF((opcode & 0x0F00) >> 8);
            break;
        case 0x33:
            LDB((opcode & 0x0F00) >> 8);
            break;
        case 0x55:
            LDI((opcode & 0x0F00) >> 8);
            break;
        case 0x65:
            LD6((opcode & 0x0F00) >> 8);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

/**
 * Given an hexadecimal color, return the alpha channel
 *
 * @param color
 * @return 8-bit alpha channel
 */
uint8 alpha(uint32 color)
{
    return color >> 24 & 0xFF;
}

/**
 * Given an hexadecimal color, return the red channel
 *
 * @param color
 * @return 8-bit red channel
 */
uint8 red(uint32 color)
{
    return color >> 16 & 0xFF;
}

/**
 * Given an hexadecimal color, return the green channel
 *
 * @param color
 * @return 8-bit green channel
 */
uint8 green(uint32 color)
{
    return color >> 8 & 0xFF;
}

/**
 * Given an hexadecimal color, return the blue channel
 *
 * @param color
 * @return 8-bit blue channel
 */
uint8 blue(uint32 color)
{
    return color & 0xFF;
}