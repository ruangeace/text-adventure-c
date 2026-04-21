
#include <stdbool.h>

// Function prototypes to be called in main.c
void display_logo();
void setup_game();
void start_game();
void end_game();
void credits();

// Library type definitions
enum player_class {
    prince = 1,
    doppelganger = 2,
};

enum Room_type {
    corridor,
    stairs,
    banquet_hall,
    warehouse,
    guard_post,
    prison,
    armory,
    flies,
    tower,
    bathroom,
};

enum Trap_type {
    none,
    tile,
    blades,
    fall,
    pitfall,
};

enum Treasure_type {
    no_treasure,
    green_poison,
    blue_healing,
    red_max_health,
    sharp_sword,
    shield,
};

enum Enemy_type {
    skeleton, 
    guard, 
    Jaffar,
};

struct Player {
    char name[30];
    enum player_class p_class;
    struct Room *position;
    struct Room *previous_room;
    int escapes;              // Used in escape function
    bool has_acted; 
    bool first_trap_ignored;  // If prince, ignores the first trap
    bool is_winner;
    unsigned char max_hp;
    unsigned char hp;
    unsigned char attack_dice;
    unsigned char defense_dice;
};

struct Room {
    struct Room* right;
    struct Room* left;
    struct Room* up;
    struct Room* down;
    struct Room* next;
    int room_id;              // Room number used when printing info
    bool treasure_taken;
    bool trap_activated;
    enum Room_type type_stanza;
    enum Trap_type trap_type;
    enum Treasure_type treasure_type;
    enum Enemy_type enemy_type;
    struct Enemy* enemy;
    bool enemy_alive;
    bool first_attempt;
    bool second_attempt;
    bool third_attempt;
    bool is_player;
};

struct Enemy {
    enum Enemy_type type;
    unsigned char hp;
    unsigned char attack_dice;
    unsigned char defense_dice;
};

// Console colors and styling
enum colors { BLACK, RED, GREEN, YELLOW, BLUE, PURPLE, CYAN, WHITE };
void set_color(enum colors color);
void reset_color();


