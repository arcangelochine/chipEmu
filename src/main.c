#include <stdio.h>
#include <string.h>

#include <chip/chip.h>
#include <SDL2/SDL.h>
#include <tinyfiledialogs.h>

#define PRIMARY_COLOR 0xFFDBCBD8
#define SECONDARY_COLOR 0xFF564787

// rom file name
char *rom_file;

// SDL window component
SDL_Window *window;

// SDL renderer component
SDL_Renderer *renderer;

// display vars
uint8 loop;
float delta_time;

uint8 util_sdl_init();
uint8 util_sdl_window_init();
uint8 util_sdl_renderer_init();

uint8 util_chip_reset();
void util_chip_open_rom();

void util_render();
uint8 util_keymap(SDL_Keycode);

// op code
uint16 op_code;

// key pressed
uint8 key;

int main()
{
    // set display scale
    scaling = 20;

    // set frame rate
    frame_rate = 60;

    // initialize SDL context
    if (util_sdl_init() || util_sdl_window_init() || util_sdl_renderer_init())
        return 1;

    // initialize rom file name
    rom_file = malloc(255 * sizeof(char));

    // initialize key
    key = 0xFF;

    util_chip_open_rom();
    if (util_chip_reset())
        return 1;

    loop = 1;

    uint64 a_clock = SDL_GetPerformanceCounter();
    uint64 b_clock = SDL_GetPerformanceCounter();

    while (loop)
    {
        a_clock = SDL_GetPerformanceCounter();
        delta_time = (float)(a_clock - b_clock) / SDL_GetPerformanceFrequency() * 1000;

        if (delta_time > 1000.0f / frame_rate)
        {
            SDL_UpdateWindowSurface(window);
            SDL_PumpEvents();

            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.window.event == SDL_WINDOWEVENT_CLOSE)
                    loop = 0;

                if (event.key.state == SDL_PRESSED)
                {
                    if (event.key.keysym.sym == SDLK_o)
                    {
                        util_chip_open_rom();
                        util_chip_reset();
                    }

                    if (event.key.keysym.sym == SDLK_i)
                        util_chip_reset();

                    key = util_keymap(event.key.keysym.sym);
                    if (key != 0xFF)
                        key_state[key] = 1;
                }

                if (event.key.state == SDL_RELEASED)
                {
                    key = util_keymap(event.key.keysym.sym);
                    if (key != 0xFF)
                        key_state[key] = 0;
                }
            }

            if (delay_timer > 0)
                delay_timer--;

            if (sound_timer > 0)
            {
                // TO-DO: ADD SOUND
                sound_timer--;
            }

            // fetch-execute cycle
            for (int i = 0; i < 10; i++)
            {
                frame++;
                op_code = (memory[PC] << 8) + memory[PC + 1];
                util_chip_execute(op_code);
            }

            for (uint8 i = 0; i < 0x10; i++)
                key_prev[i] = key_state[i];

            // render chip display
            util_render();

            SDL_RenderPresent(renderer);

            b_clock = SDL_GetPerformanceCounter();
        }
    }

    return 0;
}

/**
 * @brief Initialize SDL context
 * 
 * @return 1 if error occurred, 0 otherwise 
 */
uint8 util_sdl_init()
{
    // Init SDL subsystems
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    return 0;
}

/**
 * @brief Initialize SDL window
 * 
 * @return 1 if error occurred, 0 otherwise 
 */
uint8 util_sdl_window_init()
{
    // Create window
    window = SDL_CreateWindow("Chip-8", 100, 100, 64 * scaling, 32 * scaling, SDL_WINDOW_SHOWN);

    if (window == 0)
    {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    return 0;
}

/**
 * @brief Initialize SDL renderer
 * 
 * @return 1 if error occurred, 0 otherwise  
 */
uint8 util_sdl_renderer_init()
{
    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == 0)
    {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    return 0;
}

/**
 * @brief Set rom_file to the selected file's path
 * 
 */
void util_chip_open_rom()
{
    const char *extensions[1] = {"*.ch8"};
    const char *rom = tinyfd_openFileDialog("Open ROM", "roms/", 0, extensions, "chip-8 file", 0);

    if (rom != 0)
        strcpy(rom_file, rom);
}

/**
 * @brief 
 * 
 * @return 1 if error occurred, 0 otherwise  
 */
uint8 util_chip_reset()
{
    util_chip_init();

    if (rom_file == 0)
    {
        fprintf(stderr, "Error while loading ROM, quitting. . .\n");
        return 1;
    }

    if(util_chip_load_ROM(rom_file))
        return 1;
        
    return 0;
}

/**
 * @brief Render chip display
 * 
 */
void util_render()
{
    SDL_Rect *rect = malloc(sizeof rect);

    if (rect == 0)
    {
        fprintf(stderr, "Error while rendering: out of memory");
        return;
    }

    for (int yy = 0; yy < 32; yy++)
        for (int xx = 0; xx < 64; xx++)
        {
            rect->x = xx * scaling;
            rect->y = yy * scaling;
            rect->w = scaling;
            rect->h = scaling;

            if (display[yy][xx])
                SDL_SetRenderDrawColor(renderer,
                                       red(PRIMARY_COLOR),
                                       green(PRIMARY_COLOR),
                                       blue(PRIMARY_COLOR),
                                       alpha(PRIMARY_COLOR));
            else
                SDL_SetRenderDrawColor(renderer,
                                       red(SECONDARY_COLOR),
                                       green(SECONDARY_COLOR),
                                       blue(SECONDARY_COLOR),
                                       alpha(SECONDARY_COLOR));

            SDL_RenderFillRect(renderer, rect);
        }

    free(rect);
}

/**
 * @brief Map host's keyboard to cosmac-vip's keyboard
 * 
 * @param code 
 * @return key value 
 */
uint8 util_keymap(SDL_Keycode code)
{
    switch (code)
    {
    case SDLK_1:
        return 0x1;
        break;
    case SDLK_2:
        return 0x2;
        break;
    case SDLK_3:
        return 0x3;
        break;
    case SDLK_4:
        return 0xC;
        break;
    case SDLK_q:
        return 0x4;
        break;
    case SDLK_w:
        return 0x5;
        break;
    case SDLK_e:
        return 0x6;
        break;
    case SDLK_r:
        return 0xD;
        break;
    case SDLK_a:
        return 0x7;
        break;
    case SDLK_s:
        return 0x8;
        break;
    case SDLK_d:
        return 0x9;
        break;
    case SDLK_f:
        return 0xE;
        break;
    case SDLK_z:
        return 0xA;
        break;
    case SDLK_x:
        return 0x0;
        break;
    case SDLK_c:
        return 0xB;
        break;
    case SDLK_v:
        return 0xF;
        break;
    default:
        return 0xFF;
    }
}