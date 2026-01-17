#include <allegro.h>
#include <iostream>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRAVITY 1.0
#define HORIZONTAL_SPEED 2.0

// Kötél adatok
#define ROPE_LENGTH 150
#define ROPE_SWING_SPEED 0.05

typedef struct {
    float x, y;
    float angle;
    float speed;
    int falling;
    float vx;
    float vy;
} Candy;

enum GameState { LOADING_SCREEN, LEVEL1, LEVEL2, LEVEL3 };

void draw_rope(BITMAP *buffer, int x1, int y1, int x2, int y2) {
    line(buffer, x1, y1, x2, y2, makecol(255, 255, 255));
}

void draw_loading_screen(BITMAP *buffer) {
    clear_to_color(buffer, makecol(0, 0, 0));
    textout_ex(buffer, font, "Valassz szintet", SCREEN_WIDTH / 2 - 55, 200, makecol(255, 255, 255), -1);
    rect(buffer, SCREEN_WIDTH / 2 - 50, 250, SCREEN_WIDTH / 2 + 50, 300, makecol(255, 255, 255));
    textout_ex(buffer, font, "1. szint", SCREEN_WIDTH / 2 - 28, 270, makecol(255, 255, 255), -1);
    rect(buffer, SCREEN_WIDTH / 2 - 50, 320, SCREEN_WIDTH / 2 + 50, 370, makecol(255, 255, 255));
    textout_ex(buffer, font, "2. szint", SCREEN_WIDTH / 2 - 28, 340, makecol(255, 255, 255), -1);
    rect(buffer, SCREEN_WIDTH / 2 - 50, 390, SCREEN_WIDTH / 2 + 50, 440, makecol(255, 255, 255));
    textout_ex(buffer, font, "3. szint", SCREEN_WIDTH / 2 - 28, 410, makecol(255, 255, 255), -1);
}

int main() {
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    show_mouse(screen);

    BITMAP *buffer = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
    BITMAP *background = load_bitmap("background.bmp", NULL);
    BITMAP *beka = load_bitmap("beka.bmp", NULL);
    BITMAP *candy = load_bitmap("candy.bmp", NULL);

    if (!background || !beka || !candy) {
        allegro_message("Error loading bitmaps!");
        return -1;
    }

    Candy candy_obj;
    candy_obj.x = SCREEN_WIDTH / 2;
    candy_obj.y = 100;
    candy_obj.angle = 0;
    candy_obj.speed = ROPE_SWING_SPEED;
    candy_obj.falling = 0;
    candy_obj.vx = 0;
    candy_obj.vy = 0;

    int rope_cut = 0;
    int game_over = 0;
    char* message = NULL;
    GameState game_state = LOADING_SCREEN;

    while (!key[KEY_ESC]) {
        if (game_state == LOADING_SCREEN) {
            draw_loading_screen(buffer);
            if (mouse_b & 1) {
                if (mouse_x > SCREEN_WIDTH / 2 - 50 && mouse_x < SCREEN_WIDTH / 2 + 50) {
                    if (mouse_y > 250 && mouse_y < 300) {
                        game_state = LEVEL1;
                    } else if (mouse_y > 320 && mouse_y < 370) {
                        game_state = LEVEL2;
                    } else if (mouse_y > 390 && mouse_y < 440) {
                        game_state = LEVEL3;
                    }
                }
            }
        }
        if(game_state == LEVEL1 && !game_over) {
            if (!candy_obj.falling) {
                candy_obj.angle += candy_obj.speed;
                if (candy_obj.angle > M_PI / 4 || candy_obj.angle < -M_PI / 4) {
                    candy_obj.speed = -candy_obj.speed;
                }

                candy_obj.x = SCREEN_WIDTH / 2 + ROPE_LENGTH * sin(candy_obj.angle);
                candy_obj.y = 50 + ROPE_LENGTH * cos(candy_obj.angle);
            } else {
                candy_obj.vy += GRAVITY;  // Növekvő gravitációs gyorsulás
                candy_obj.x += candy_obj.vx*5;  // Vízszintes mozgás
                candy_obj.y += candy_obj.vy;  // Függőleges mozgás

                // Ellenőrizzük, hogy a cukorka a béka szájába esett-e
                if (candy_obj.x + candy->w / 2 > 340 && candy_obj.x + candy->w / 2 < 340 + beka->w &&
                    candy_obj.y + candy->h > 550 && candy_obj.y + candy->h < 550 + beka->h) {
                    game_over = 1;
                    message = "Te nyertel!";
                }

                // Ellenőrizzük, hogy a cukorka kiesett-e a képernyőről
                else if (candy_obj.y > SCREEN_HEIGHT || candy_obj.x > SCREEN_WIDTH || candy_obj.x < 0) {
                    game_over = 1;
                    message = "Vesztettel!";
                }
            }

            if (mouse_b & 1 && !rope_cut && !game_over) {
                if (mouse_x > SCREEN_WIDTH / 2 - ROPE_LENGTH && mouse_x < candy_obj.x && mouse_y > 50 && mouse_y < candy_obj.y) {
                    rope_cut = 1;
                    candy_obj.falling = 1;
                    candy_obj.vx = HORIZONTAL_SPEED * sin(candy_obj.angle);
                    candy_obj.vy = HORIZONTAL_SPEED * cos(candy_obj.angle);
                }
            }

            clear_to_color(buffer, makecol(0, 0, 0));
            blit(background, buffer, 0, 0, 0, 0, background->w, background->h);
            masked_blit(beka, buffer, 0, 0, 340, 480, beka->w, beka->h);

            if (!rope_cut) {
                draw_rope(buffer, SCREEN_WIDTH / 2, 50, candy_obj.x + candy->w / 2, candy_obj.y);
            }

            masked_blit(candy, buffer, 0, 0, candy_obj.x, candy_obj.y, candy->w, candy->h);

            if (game_over) {
                textout_ex(buffer, font, message, 10, SCREEN_HEIGHT - 20, makecol(255, 255, 255), -1);
            }
        }
        if(game_state == LEVEL2) {
            if (!candy_obj.falling) {
                candy_obj.angle += candy_obj.speed;
                if (candy_obj.angle > M_PI / 4 || candy_obj.angle < -M_PI / 4) {
                    candy_obj.speed = -candy_obj.speed;
                }

                candy_obj.x = 150 + ROPE_LENGTH * sin(candy_obj.angle);
                candy_obj.y = 50 + ROPE_LENGTH * cos(candy_obj.angle);
            } else {
                candy_obj.vy += GRAVITY;  // Növekvő gravitációs gyorsulás
                candy_obj.x += candy_obj.vx*7;  // Vízszintes mozgás
                candy_obj.y += candy_obj.vy;  // Függőleges mozgás

                // Ellenőrizzük, hogy a cukorka a béka szájába esett-e
                if (candy_obj.x + candy->w / 2 > 340 && candy_obj.x + candy->w / 2 < 340 + beka->w &&
                    candy_obj.y + candy->h > 480 && candy_obj.y + candy->h < 480 + beka->h) {
                    game_over = 1;
                    message = "Te nyertel!";
                }

                // Ellenőrizzük, hogy a cukorka kiesett-e a képernyőről
                if (candy_obj.y > SCREEN_HEIGHT || candy_obj.x > SCREEN_WIDTH || candy_obj.x < 0) {
                    game_over = 1;
                    message = "Vesztettel!";
                }
            }

            if (mouse_b & 1 && !rope_cut && !game_over) {
                if (mouse_x > 150 - ROPE_LENGTH && mouse_x < candy_obj.x && mouse_y > 50 && mouse_y < candy_obj.y) {
                    rope_cut = 1;
                    candy_obj.falling = 1;
                    candy_obj.vx = HORIZONTAL_SPEED * sin(candy_obj.angle);
                    candy_obj.vy = HORIZONTAL_SPEED * cos(candy_obj.angle);
                }
            }

            clear_to_color(buffer, makecol(0, 0, 0));
            blit(background, buffer, 0, 0, 0, 0, background->w, background->h);
            masked_blit(beka, buffer, 0, 0, 340, 480, beka->w, beka->h);

            if (!rope_cut) {
                draw_rope(buffer, 150, 50, candy_obj.x + candy->w / 2, candy_obj.y);
            }

            masked_blit(candy, buffer, 0, 0, candy_obj.x, candy_obj.y, candy->w, candy->h);

            if (game_over) {
                textout_ex(buffer, font, message, 10, SCREEN_HEIGHT - 20, makecol(255, 255, 255), -1);
            }
        }
        if(game_state == LEVEL3) {
            if (!candy_obj.falling) {
                candy_obj.angle += candy_obj.speed;
                if (candy_obj.angle > M_PI / 4 || candy_obj.angle < -M_PI / 4) {
                    candy_obj.speed = -candy_obj.speed;
                }

                candy_obj.x = SCREEN_WIDTH -150 + ROPE_LENGTH * sin(candy_obj.angle);
                candy_obj.y = 50 + ROPE_LENGTH * cos(candy_obj.angle);
            } else {
                candy_obj.vy += GRAVITY;  // Növekvő gravitációs gyorsulás
                candy_obj.x += candy_obj.vx*7;  // Vízszintes mozgás
                candy_obj.y += candy_obj.vy;  // Függőleges mozgás

                // Ellenőrizzük, hogy a cukorka a béka szájába esett-e
                if (candy_obj.x + candy->w / 2 > 340 && candy_obj.x + candy->w / 2 < 340 + beka->w &&
                    candy_obj.y + candy->h > 480 && candy_obj.y + candy->h < 480 + beka->h) {
                    game_over = 1;
                    message = "Te nyertel!";
                }

                // Ellenőrizzük, hogy a cukorka kiesett-e a képernyőről
                if (candy_obj.y > SCREEN_HEIGHT || candy_obj.x > SCREEN_WIDTH || candy_obj.x < 0) {
                    game_over = 1;
                    message = "Vesztettel!";
                }
            }

            if (mouse_b & 1 && !rope_cut && !game_over) {
                if (mouse_x > SCREEN_WIDTH - ROPE_LENGTH && mouse_x < candy_obj.x && mouse_y > 100 && mouse_y < candy_obj.y) {
                    rope_cut = 1;
                    candy_obj.falling = 1;
                    candy_obj.vx = HORIZONTAL_SPEED * sin(candy_obj.angle);
                    candy_obj.vy = HORIZONTAL_SPEED * cos(candy_obj.angle);
                }
            }

            clear_to_color(buffer, makecol(0, 0, 0));
            blit(background, buffer, 0, 0, 0, 0, background->w, background->h);
            masked_blit(beka, buffer, 0, 0, 340, 480, beka->w, beka->h);

            if (!rope_cut) {
                draw_rope(buffer, SCREEN_WIDTH -150, 50, candy_obj.x + candy->w / 2, candy_obj.y);
            }

            masked_blit(candy, buffer, 0, 0, candy_obj.x, candy_obj.y, candy->w, candy->h);

            if (game_over) {
                textout_ex(buffer, font, message, 10, SCREEN_HEIGHT - 20, makecol(255, 255, 255), -1);
            }
        }

        draw_sprite(screen, buffer, 0, 0);
        //std::cout << candy_obj.x << " " << candy_obj.y << " ";

        rest(20);
    }

    destroy_bitmap(buffer);
    destroy_bitmap(background);
    destroy_bitmap(beka);
    destroy_bitmap(candy);

    allegro_exit();
    return 0;
}
END_OF_MAIN();
