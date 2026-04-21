#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "gamelib.h"

typedef struct Player Player;
typedef struct Room Room;
typedef struct Enemy Enemy;

Player* players[3];

static Room* first_room = NULL;
static Room* last_room = NULL;

int control_var = 0; // Control variable
static unsigned short current_turn = 0;
unsigned short player_count;
static bool game_over;

// Game Setup
bool is_setup = 0;
unsigned short prince_present = 0;
bool is_jaffar = false;
static void create_player(unsigned short player_count);
static unsigned short get_player_count();

// Gameplay functions
static void player_menu(Player* player);
static void move_forward(Player* player);
static void fight(Player* player, Enemy* enemy);
static void attack(int attack_dice, int defense_dice, unsigned char* health, const char* attacker_name, const char* defender_name);
static void escape(Player* player);
static void print_player_info(Player* player);
static void print_room_info(Player* player);
static void take_treasure(Player* player);
static void search_secret_door(Player* player);
static void pass_turn();

// Map Setup
static int room_count;
static void map_setup_menu();
static void add_room();
static void delete_room();
static void print_rooms();
static void generate_random_map();
static void close_map();
static const char* get_room_name(enum Room_type type);
static bool is_valid_position(Room* reference, unsigned short direction);
static const char* get_class_name(enum player_class p_class);
static char* get_treasure_name(enum Treasure_type p_treasure);
static char* get_trap_name(enum Trap_type p_trap);

// Enemy functions
static Enemy* init_enemy(enum Enemy_type type);
static char* get_enemy_name(enum Enemy_type type);
static void get_enemy_intel(Player* player);

void set_color(enum colors color);
void reset_color();

static void free_memory();
static void clear_buffer();
static bool play_turn();
static void set_winners(Player* player);

Player* first_winner;
Player* second_winner;
Player* third_winner;

void display_logo() {
    set_color(PURPLE);
    printf("**---------------------------------------------------------------------------------**\n");
    printf("** _____                                 __   _____                 _   _       **\n");
    printf("** |  __ |   (_)                       / _| |_   _|               | | (_)      **\n");
    printf("** | |__) | __ _ _ __   ___ ___   ___ | |_    | |  _ __   ___ _ __| |_ _  __ _ **\n");
    printf("** |  ___/ '__| | '_   / __/ _   / _   |  _|   | | | '_   / _   '__| __| |/ _` |**\n");
    printf("** | |   | |  | | | | | (_|  __/ | (_) | |    _| |_| | | |  __/ |  | |_| | (_| |**\n");
    printf("** |_|   |_|  |_|_| |_| ___ ___|  |___/|_|   |_____|_| |_| __  |_| | __|_| __,_|**\n");
    printf("**---------------------------------------------------------------------------------**\n");

    printf("\nShadows lengthen in the realm... evil is near.\n");
    reset_color();
    printf("**---------------------------------------------------------------------------------**\n");
    set_color(YELLOW);
    printf("Prince of Inertia: a game of courage and sacrifice.\n");
    printf("Traps are lethal, but glory is eternal.\n");
    printf("Jaffar laughs in the shadows, but your steel shall not tremble.\n");
    reset_color();
    set_color(CYAN);
    printf("Grip your sword. The legend begins now.");
    reset_color();
    printf("(ENTER)\n");
    printf("**---------------------------------------------------------------------------------**\n");
    clear_buffer();
}

void setup_game() {
    srand(time(NULL));
    
    if (is_setup == true) {
        set_color(BLUE);
        printf("The game has already been set up. The previous configuration will be deleted!\n");
        reset_color();
        free_memory();
        is_setup = false;
        printf("Press Enter to continue\n");
        clear_buffer();
        system("clear");
    }

    first_room = NULL;
    last_room = NULL;
    room_count = 0;

    // Request number of players
    player_count = get_player_count();
    system("clear");

    // Create and set up players
    create_player(player_count);

    // Proceed to map configuration
    map_setup_menu();
    is_setup = true;
    printf("Setup status: %d\n", is_setup);
}

void start_game() {
    // Verify game setup
    if (is_setup == false) {
        set_color(RED);
        printf("\nYou must set up the game before playing. Try again!\n");
        reset_color();
        printf("Press Enter to continue\n");
        clear_buffer();
        return;
    }

    if (game_over == true) {
        printf("You must reset the game again!\n");
        printf("Press Enter to continue!\n");
        clear_buffer();
        return;
    }

    if (player_count <= 0) {
        set_color(RED);
        printf("\nError: No players available.\n");
        reset_color();
        return;
    }

    if (first_room == NULL) {
        set_color(RED);
        printf("\nError: The map was not initialized correctly.\n");
        reset_color();
        return;
    }
    
    printf("**---------------------------------------------------------------------------------**\n");
    set_color(CYAN);
    puts("CORE PRINCIPLES:\n");
    puts("The journey is turn-based, and the turn order changes randomly each round.\n");
    puts("During your turn, you can choose options that will influence your destiny.\n");
    puts("The goal is to reach the end of the map without losing all your health points.\n");
    puts("If your health points reach zero, your journey ends.\n");
    puts("Press ENTER to start your quest!\n");
    reset_color();
    printf("**---------------------------------------------------------------------------------**\n");

    clear_buffer();
    system("clear");

    // Position all players in the first room
    for (int i = 0; i < player_count; i++) {
        players[i]->position = first_room;
        printf("**-----------------------------------**\n");
        set_color(GREEN);
        printf("Player %s is located in the first room: %s\n", 
               players[i]->name, get_room_name(first_room->type_stanza));   
        reset_color();      
    }

    while (true) {
        if (play_turn()) {
            is_setup = false;
            game_over = false;
            break;
        }
    }
}

static bool play_turn() {
    static bool first_turn = true;
    if (game_over) {
        return true;
    }

    if (first_turn || current_turn >= player_count) {
        // Shuffle players
        for (int i = player_count - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            Player* temp = players[i];
            players[i] = players[j];
            players[j] = temp;
        }
        current_turn = 0;
        first_turn = false;
    }

    // Look for a living player
    while (current_turn < player_count && players[current_turn]->hp == 0) {
        current_turn++;
    }

    // If at the end of the list, check for survivors
    if (current_turn >= player_count) {
        bool anyone_alive = false;
        for (int i = 0; i < player_count; i++) {
            if (players[i]->hp > 0) {
                anyone_alive = true;
                current_turn = i; 
                break;
            }
        }

        if (!anyone_alive) {
            set_color(RED);
            printf("All players have perished!\n");
            game_over = true;

            set_color(CYAN);
            printf("Returning to the main menu!\n");
            printf("Press Enter to continue...\n");
            reset_color();
            clear_buffer();
            return true;
        }
    }

    Player* current_player = players[current_turn];
    current_player->has_acted = 0;

    if (game_over) {
        return true;
    }

    printf("\n=== %s's Turn ===\n", current_player->name);
    if (current_player->hp > 0 && !current_player->is_winner) {
        player_menu(current_player);
    } else {
        if (current_player->hp == 0) {
            printf(" %s, you are dead!", current_player->name);
        } else if (current_player->is_winner) {
            printf("%s has already won and is no longer playing.\n", current_player->name);
        }
    }
    current_turn++;
    return false;
}

static unsigned short get_player_count() {
    unsigned short count;
    do {
        printf("**-----------------------------------**\n");
        set_color(PURPLE);
        printf("How many people want to play? Enter a number between 1 and 3\n ");
        reset_color();
        printf("Choice: ");
        scanf("%hu", &count);
        getchar();
        printf("\n**-----------------------------------**\n");
        if (count < 1 || count > 3) {
            set_color(RED);
            puts("Please enter a number between 1 and 3\n");
            reset_color();
            scanf("%*[^\n]");
        }
    } while (count < 1 || count > 3);

    return count;
}

static void create_player(unsigned short count) {
    prince_present = 0;

    for (int i = 0; i < count; i++) {
        printf("\nCreating player %d\n", i + 1);

        players[i] = malloc(sizeof(Player));
        if (players[i] == NULL) {
            printf("Memory allocation error\n");
            return;
        }

        printf("**-----------------------------------**\n");
        printf("Enter name for player %d: ", i + 1);
        char player_name[30];
        scanf("%30s", player_name);
        getchar(); 
        strcpy(players[i]->name, player_name);
        set_color(GREEN);
        printf("\nName entered: %s\n", players[i]->name);
        reset_color();
        printf("**-----------------------------------**\n");

        unsigned short choice;
        bool valid_choice = false;

        do {
            printf("Player %d, choose your class:\n", i + 1);
            set_color(CYAN);
            printf("1. Prince | HP: 3 | Attack Dice: 2 | Defense Dice: 2 | Perk: Light Feet\n");
            reset_color();
            set_color(YELLOW);
            printf("2. Doppelganger | HP: 3 | Attack Dice: 2 | Defense Dice: 2\n");
            reset_color();
            printf("Choice: ");
            if (scanf("%hu", &choice) != 1) {
                while (getchar() != '\n');
                set_color(RED);
                printf("You must enter a number!\n");
                reset_color();
                continue;
            }
            while (getchar() != '\n');

            if (choice != 1 && choice != 2) {
                set_color(RED);
                printf("Error: You must enter 1 or 2.\n");
                reset_color();
                continue;
            }

            if (count == 1 && choice != 1) {
                set_color(RED);
                printf("\nWARNING! You are the only player, you must be the Prince.\n");
                reset_color();
                continue;
            }

            if (choice == 1 && prince_present) {
                set_color(RED);
                printf("WARNING! A Prince already exists. Choose another class.\n");
                reset_color();
                continue;
            }

            valid_choice = true;
        } while (!valid_choice);

        if (choice == 1) {
            players[i]->p_class = prince;
            players[i]->attack_dice = 2;
            players[i]->defense_dice = 2;
            players[i]->hp = 3;
            players[i]->max_hp = 3;
            players[i]->has_acted = 1;
            players[i]->is_winner = 0;
            prince_present = 1;
            set_color(GREEN);
            printf("Class chosen: Prince\n");
            reset_color();
        } else {
            players[i]->p_class = doppelganger;
            players[i]->attack_dice = 2;
            players[i]->defense_dice = 2;
            players[i]->hp = 3;
            players[i]->max_hp = 3;
            players[i]->has_acted = 0;
            players[i]->is_winner = 0;
            set_color(GREEN);
            printf("Class chosen: Doppelganger\n");
            reset_color();
        }

        printf("**-----------------------------------**\n");
        set_color(GREEN);
        printf("\nPlayer %d successfully configured\n", i + 1);
        reset_color();
    }

    if (!prince_present) {
        set_color(RED);
        printf("\nWARNING! No one chose Prince. The last player is now assigned as Prince.\n");
        players[count - 1]->p_class = prince;
        players[count - 1]->attack_dice = 2;
        players[count - 1]->defense_dice = 2;
        players[count - 1]->hp = 3;
        players[count - 1]->max_hp = 3;
        players[count - 1]->has_acted = 0;
        players[count - 1]->is_winner = 0;
        prince_present = 1;
        printf("Player %d has been forced to be the Prince.\n", count);
        reset_color();
        printf("\nPress Enter to continue setup!\n");
        while ((control_var = getchar()) != '\n' && control_var != EOF);
    }
}

static void map_setup_menu() {
    unsigned short int choice;
    do {
        printf("**-----------------------------------**\n");
        printf("\nWelcome to the Map Setup Menu\n");
        set_color(GREEN);
        printf("1. Add a new room\n");
        reset_color();
        set_color(RED);
        printf("2. Delete the last room\n");
        reset_color();
        set_color(BLUE);
        printf("3. Print all rooms\n");
        set_color(PURPLE);
        printf("4. Generate random map\n");
        reset_color();
        set_color(CYAN);
        printf("5. Close map setup\n");
        reset_color();
        printf("Choice: ");
        if (scanf("%hu", &choice) != 1) {
            while (getchar() != '\n');
            set_color(RED);
            printf("You must enter a number\n");
            reset_color();
            continue;
        }

        while (getchar() != '\n');
        
        printf("\n**-----------------------------------**\n");
        printf("You chose: %d\n", choice);

        if (choice > 5 || choice < 1) {
            set_color(RED);
            printf("Please enter a value between 1 and 5\n");
            reset_color();
            continue;
        }

        switch (choice) {
            case 1: add_room(); break;
            case 2: delete_room(); break;
            case 3: print_rooms(); break;
            case 4: generate_random_map(); break;
            case 5: 
                close_map();
                if (room_count < 15) continue;
                break;
        }
    } while (choice != 5 || room_count < 15);
}

static void player_menu(Player* player) {
    if (game_over) return;
    unsigned short choice = 0;
    bool turn_finished = 0;
  
    do {
        if (game_over) return;
        printf("**-----------------------------------**\n");
        printf("Choose one of the following options: \n");
        set_color(GREEN);
        printf("1. Move Forward\n");
        set_color(YELLOW);
        printf("2. Fight\n");
        set_color(RED);
        printf("3. Escape\n");
        set_color(CYAN);
        printf("4. View Stats\n");
        set_color(PURPLE);
        printf("5. View Room Info\n");
        set_color(BLUE);
        printf("6. Take Treasure\n");
        set_color(WHITE);
        printf("7. Search for Secret Door\n");
        set_color(RED);
        printf("8. Pass Turn\n");
        reset_color();
        printf("9. Quit Game\n");
        printf("10. Enemy Intel\n");
        
        printf("Choice: ");
        if (scanf("%hu", &choice) != 1) {
            while (getchar() != '\n');
            set_color(RED);
            printf("You must enter a number!\n");
            reset_color();
            continue;
        }
        while (getchar() != '\n');
         
        printf("**-----------------------------------**\n");
        if (choice < 1 || choice > 10) {
            set_color(RED);
            puts("Invalid choice! Try again.\n");
            reset_color();
            continue;
        }
    
        switch (choice) {
            case 1: move_forward(player); break;
            case 2:
                if (player->position->enemy_alive && player->position->enemy != NULL) {
                    fight(player, player->position->enemy);
                } else {
                    set_color(YELLOW);
                    printf("Nothing to fight here but boredom!\n");
                    reset_color();
                    if (player->hp == 0) puts("Health reached 0!\n");
                }
                break;
            case 3: escape(player); break;
            case 4: print_player_info(player); break;
            case 5: print_room_info(player); break;
            case 6: take_treasure(player); break;
            case 7:
                if (player->has_acted) {
                    set_color(RED);
                    printf("Action limit reached! You cannot search for a secret door.\n");
                    reset_color();
                } else {
                    search_secret_door(player);
                }
                break;
            case 8: pass_turn(); turn_finished = true; break;
            case 9: end_game(); break;
            case 10: get_enemy_intel(player); break;
        }

        if (player->hp == 0) turn_finished = true;
        
    } while (!turn_finished);
}

static void move_forward(Player* player) {
    if (game_over) {
        printf("The game is over\n");
        return;
    }
    if (player->has_acted) {
        set_color(RED);
        printf("Cannot move. Action limit reached!\n");
        reset_color();
        return;
    }

    Room* current_room = player->position;

    if (current_room == last_room) {
        if (current_room->enemy_alive) {
            set_color(RED);
            printf("No escape! Face JAFFAR in a duel to the death!\n");
            reset_color();
            return;
        }
        set_color(RED);
        printf("You cannot advance! You are in the final room.\n");
        reset_color();
        return;
    }

    if (current_room->enemy_alive) {
        set_color(RED);
        printf("You must defeat the enemy in this area before moving forward.\n");
        reset_color();
        return;
    }

    Room* next_room = current_room->next;
    if (next_room == NULL) {
        set_color(RED);
        printf("Error: No next room found.\n");
        reset_color();
        return;
    }

    player->previous_room = player->position;

    // Trap management
    if (!next_room->trap_activated) {
        if (next_room->trap_type == none) {
            set_color(CYAN);
            printf("No traps in this room!\n");
            reset_color();
        } else if (player->p_class == prince && !player->first_trap_ignored) {
            printf("The first trap is ignored because you are a Prince.\n");
            player->first_trap_ignored = true;
        } else {
            unsigned char damage = 0;
            switch (next_room->trap_type) {
                case tile: damage = 1; break;
                case blades: damage = 2; break;
                case fall:
                case pitfall: damage = (rand() % 2) + 1; break;
                default: break;
            }

            if (player->hp <= damage) player->hp = 0;
            else player->hp -= damage;

            next_room->trap_activated = true;

            set_color(RED);
            switch (next_room->trap_type) {
                case tile: printf("You lost 1 HP due to a trap!\n"); break;
                case blades: printf("You lost 2 HP due to a trap!\n"); break;
                case fall: printf("You fell into a pit! Lost %d HP!\n", damage); break;
                case pitfall: printf("You nearly fell into a ravine! Lost %d HP and skipped your turn!\n", damage); break;
                default: break;
            }
            reset_color();

            if (player->hp == 0) {
                printf("%s died from a trap!\n", player->name);
                printf("Press ENTER to continue...\n");
                clear_buffer();
                pass_turn();
                return;
            }

            if (next_room->trap_type == pitfall) {
                printf("Press ENTER to continue...\n");
                clear_buffer();
                pass_turn();
                return;
            }
        }
    } else {
        set_color(CYAN);
        printf("The trap in this room has already been triggered.\n");
        reset_color();
    }

    player->position = next_room;

    if (next_room == last_room) {
        set_color(YELLOW);
        printf("FINAL ROOM!\n");

        if (next_room->enemy_alive) {
            printf("Prepare to fight the final boss: JAFFAR!\n");
            printf("Boss Info: Attack: 3 | Defense: 2 | HP: 3\n");
        } else {
            printf("CONGRATULATIONS! JAFFAR DEFEATED!\n");
            printf("Game Over.\n");
            reset_color();
            return;
        }
        reset_color();
    }

    set_color(GREEN);
    printf("%s moved into room: ", player->name);
    set_color(YELLOW);
    printf("%s\n", get_room_name(next_room->type_stanza));
    reset_color();

    player->has_acted = true;
}

static void fight(Player* player, Enemy* enemy) {
    if (!player) { printf("Player is NULL!\n"); return; }
    if (!player->position) { printf("Position is NULL!\n"); return; }
    if (!player->position->enemy_alive || !enemy) { 
        printf("No living enemies here!\n"); 
        return; 
    }

    printf("Now fighting against %s\n", get_enemy_name(player->position->enemy_type));

    do {
        printf("**-----------------------------------**\n");
        printf("\n**--------");
        set_color(PURPLE);
        printf(" Combat Round ");
        reset_color();
        printf("-------**\n");

        unsigned short p_die = (rand() % 6) + 1;
        unsigned short e_die = (rand() % 6) + 1;
        set_color(CYAN);
        printf("Player %s die: %hu VS Enemy die: %hu\n", player->name, p_die, e_die);
        reset_color();

        if (p_die >= e_die) {
            set_color(GREEN);
            printf("Player %s strikes first!\n", player->name);
            reset_color();
            attack(player->attack_dice, enemy->defense_dice, &enemy->hp, player->name, get_enemy_name(enemy->type));

            if (enemy->hp > 0) {
                attack(enemy->attack_dice, player->defense_dice, &player->hp, get_enemy_name(enemy->type), player->name);
            }
        } else {
            set_color(RED);
            printf("Enemy %s strikes first!\n", get_enemy_name(enemy->type));
            reset_color();
            attack(enemy->attack_dice, player->defense_dice, &player->hp, get_enemy_name(enemy->type), player->name);

            if (player->hp > 0) {
                attack(player->attack_dice, enemy->defense_dice, &enemy->hp, player->name, get_enemy_name(enemy->type));
            }
        }

    } while (player->hp > 0 && enemy->hp > 0);

    if (enemy->hp <= 0) {
        player->position->enemy_alive = false;
        printf("**-----------------------------------**\n");
        set_color(GREEN);
        printf("You defeated the enemy!\n");
        set_color(YELLOW);
        printf("%s gained +1 HP!\n", player->name);
        reset_color();
        player->hp += 1;
    }

    if (player->hp > player->max_hp) player->max_hp = player->hp;

    if (player->position == last_room && enemy->type == Jaffar && enemy->hp == 0) {
        set_winners(player);
        player->is_winner = true;
        player->position->enemy_alive = false;
        printf("Congratulations! %s, you defeated Jaffar and won the game.\n", player->name); 
        game_over = true;

        printf("**-----------------------------------**\n");
        printf("Returning to the main menu. Reset the game to play again. (ENTER)\n");
        clear_buffer();
    } else if (player->hp <= 0) {
        set_color(RED);
        printf("\nFor %s, the battle is over.\n", player->name);
        reset_color();      
        puts("Press ENTER to skip turn");
        clear_buffer();   
        pass_turn();     
    }
}

static void attack(int attack_dice, int defense_dice, unsigned char* health, const char* attacker_name, const char* defender_name) {
    int total_atk = 0;
    int total_def = 0;

    for (int i = 0; i < attack_dice; i++) total_atk += (rand() % 6) + 1;
    for (int i = 0; i < defense_dice; i++) total_def += (rand() % 6) + 1;

    printf("**-----------------------------------**\n");
    printf("%s attacks %s!\n", attacker_name, defender_name);
    set_color(CYAN);
    printf("Attack: %d vs Defense: %d\n", total_atk, total_def);
    reset_color();

    if (total_atk > total_def) {
        (*health)--;
        set_color(GREEN);
        printf("Hit successful!");
        reset_color();
        printf(" %s Health: %d\n", defender_name, *health);
    } else {
        set_color(GREEN);
        printf("Defense successful! No damage to %s.\n", defender_name);
        reset_color();
    }
}

static void escape(Player* player) {
    if (player->position == first_room || player->previous_room == NULL) {
        set_color(RED);
        printf("Cannot escape from the starting area.\n");
        reset_color();
        return;
    }
    if (player->position->enemy == NULL || player->position->enemy_alive == false) {
        set_color(RED);
        printf("Escape? From what? Your imagination?\n");
        reset_color();
        return;
    } 

    int max_escapes = (player->p_class == prince) ? 1 : 2;

    if (player->escapes >= max_escapes) {
        printf("Cannot escape anymore! Limit reached.\n");
        return;
    }

    player->position = player->previous_room;
    player->previous_room = NULL;
    player->escapes++;

    set_color(YELLOW);
    printf("%s fled back!\n", player->name);
    reset_color();
}

static void print_player_info(Player* player) {
    printf("Name: %s | ", player->name);
    set_color(YELLOW);
    printf("Class: %s\n", get_class_name(player->p_class));
  
    set_color(RED);
    printf("Attack Dice: %d |", player->attack_dice);
    printf("Defense Dice: %d\n", player->defense_dice);
    set_color(CYAN);
    printf("HP: %d | ", player->hp);
    printf("Max HP: %d\n", player->max_hp);
    reset_color();
}

static void print_room_info(Player* player) {
    Room* current = player->position;
    printf("Room No: %d\n", player->position->room_id);
    set_color(YELLOW);
    printf("Room Type: %s\n", get_room_name(current->type_stanza));
    
    set_color(GREEN);
    if (current->treasure_type != no_treasure && !current->treasure_taken) {
        printf("Something glimmers! It's treasure!\n");
    } else {
        printf("No treasure here.\n");
    }

    set_color(RED);
    if (current->enemy_alive) {
        const char* e_name;
        switch (current->enemy_type) {
            case skeleton: e_name = "a skeleton"; break;
            case guard: e_name = "a guard"; break;
            case Jaffar: e_name = "Jaffar"; break;
            default: e_name = "unknown"; break;
        }
        printf("There is %s in this area!\n", e_name);
        reset_color();
    } else {
        set_color(CYAN);
        printf("No enemies in this area!\n");
        reset_color();
    }
}

static void take_treasure(Player* player) {
    if (player->position->treasure_taken) {
        set_color(RED);
        puts("No treasure left.\n");
        reset_color();
        return;
    }

    enum Treasure_type t_type = player->position->treasure_type;
    switch (t_type) {
        case no_treasure:
            set_color(RED);
            printf("No treasure in this room.\n");
            reset_color();
            break;
        case green_poison:
            set_color(RED);
            printf("Poisoned! Lose 1 HP!\n");
            reset_color();
            player->hp -= 1;
            if (player->hp <= 0) {
                player->hp = 0;
                set_color(RED);
                printf("%s died from poison.\n", player->name);
                reset_color();
                puts("Press ENTER to skip turn!\n");
                clear_buffer();
                pass_turn();
                return;
            }
            break;
        case blue_healing:
            set_color(CYAN);
            printf("Healing! %s gained a heart.\n", player->name);
            reset_color();
            player->hp += 1;
            if (player->hp > player->max_hp) player->max_hp = player->hp;
            break;
        case red_max_health:
            set_color(CYAN);
            printf("Red Potion! %s is fully restored!\n", player->name);
            reset_color();
            player->hp = player->max_hp;
            break;
        case sharp_sword:
            set_color(CYAN);
            printf("Sharp Sword! +1 Attack Die.\n");
            reset_color();
            player->attack_dice += 1;
            break;
        case shield:
            set_color(CYAN);
            printf("Shield! +1 Defense Die.\n");
            reset_color();
            player->defense_dice += 1;
            break;
    }
    player->position->treasure_taken = true;
}

static void search_secret_door(Player* player) {
    if (player->has_acted) {
        set_color(RED);
        printf("Action limit reached!\n");
        reset_color();
        return;
    }

    Room* room = player->position;
    int prob;
    int attempts_left = 0;

    if (!room->first_attempt) {
        prob = 33; room->first_attempt = true;
        printf("First attempt!\n"); attempts_left = 2;
    } else if (!room->second_attempt) {
        prob = 20; room->second_attempt = true;
        printf("Second attempt!\n"); attempts_left = 1;
    } else if (!room->third_attempt) {
        prob = 15; room->third_attempt = true;
        printf("Third attempt!\n"); attempts_left = 0;
    } else {
        printf("All attempts exhausted!\n");
        return;
    }

    int roll = rand() % 100;

    if (roll < prob) {
        printf("**-----------------------------------**\n");
        printf("You found a secret door!\n");
   
        Room* secret = (Room*) malloc(sizeof(Room));
        if (secret == NULL) { puts("Memory not allocated\n"); return; }

        secret->up = NULL; secret->left = NULL; secret->right = NULL; secret->down = NULL;
        secret->type_stanza = rand() % 10;
        secret->first_attempt = false; secret->second_attempt = false; secret->third_attempt = false;

        bool valid_dir = false;  
        unsigned short direction;
    
        while (!valid_dir) {
            direction = rand() % 4 + 1;
            if (is_valid_position(room, direction)) valid_dir = true;
        }
    
        switch (direction) {
            case 1: room->down = secret; last_room->up = room; break;
            case 2: room->left = secret; last_room->right = room; break;
            case 3: room->right = secret; last_room->left = room; break;
            case 4: room->up = secret; last_room->down = room; break;
        }

        int t_roll = rand() % 100;
        if (t_roll < 20) secret->treasure_type = no_treasure;
        else if (t_roll < 40) secret->treasure_type = green_poison;
        else if (t_roll < 60) secret->treasure_type = blue_healing;
        else if (t_roll < 75) secret->treasure_type = red_max_health;
        else if (t_roll < 90) secret->treasure_type = sharp_sword;
        else secret->treasure_type = shield;
          
        player->position = secret;
           
        unsigned short choice = 0;
        do {
            printf("Want to check for treasure in this room? (1. Yes | 2. No)\n");
            printf("Choice: ");
            if (scanf("%hu", &choice) != 1) {
                while (getchar() != '\n');
                set_color(RED);
                printf("Enter a number!\n");
                reset_color();
                continue;
            }
            printf("**-----------------------------------**\n");

            if (choice == 1) {
                take_treasure(player);
                player->position = room;
                printf("Returned to previous room.\n");
                free(secret);
                break;
            } else if (choice == 2) {
                player->position = room;
                printf("Returned to previous room!\n");
                free(secret);
                break;
            } else {
                printf("Enter a value between 1 and 2\n");
            }
        } while (1);

    } else {
        printf("You found nothing!\n");
        if (attempts_left >= 2) printf("%d attempts remaining.\n", attempts_left);
        else if (attempts_left == 1) printf("Only 1 attempt left!\n");
        else printf("Attempts finished!\n");
    }
    player->has_acted = true;
}

static void pass_turn() {
    current_turn++;
    system("clear");
    play_turn();
}

static void add_room() {
    unsigned short choice = 0;

    Room* new_room = (Room*)malloc(sizeof(Room));
    if (new_room == NULL) { printf("Memory not allocated\n"); return; }

    new_room->up = NULL; new_room->down = NULL; new_room->right = NULL; new_room->left = NULL;
    new_room->next = NULL; new_room->type_stanza = rand() % 10;
    new_room->room_id = room_count;
    new_room->enemy_alive = false; new_room->enemy = NULL; new_room->enemy_type = none;

    if (first_room == NULL) {
        first_room = new_room; last_room = new_room; room_count = 1;
        set_color(GREEN); printf("First room created!\n"); reset_color();
        return;
    }

    bool valid_dir = false;
    do {
        printf("\nChoose direction to add the new room\n");
        set_color(GREEN); printf("1. Up\n"); reset_color();
        set_color(YELLOW); printf("2. Right\n"); reset_color();
        set_color(CYAN); printf("3. Left\n"); reset_color();
        set_color(RED); printf("4. Down\n"); reset_color();
        printf("Choice: ");
        scanf("%hu", &choice); getchar();
        printf("\n**-----------------------------------**\n");

        if (is_valid_position(last_room, choice)) valid_dir = true;
        else {
            set_color(RED); printf("Direction occupied or invalid choice! Try again.\n"); reset_color();
        }
    } while (!valid_dir);

    switch (choice) {
        case 1: new_room->down = last_room; last_room->up = new_room; break;
        case 2: new_room->left = last_room; last_room->right = new_room; break;
        case 3: new_room->right = last_room; last_room->left = new_room; break;
        case 4: new_room->up = last_room; last_room->down = new_room; break;
    }

    last_room->next = new_room; last_room = new_room; room_count++;

    int trap_roll = rand() % 100;
    if (trap_roll < 65) new_room->trap_type = none;
    else if (trap_roll < 75) new_room->trap_type = tile;
    else if (trap_roll < 84) new_room->trap_type = blades;
    else if (trap_roll < 92) new_room->trap_type = fall;
    else new_room->trap_type = pitfall;

    int t_roll = rand() % 100;
    if (t_roll < 20) new_room->treasure_type = none;
    else if (t_roll < 40) new_room->treasure_type = green_poison;
    else if (t_roll < 60) new_room->treasure_type = blue_healing;
    else if (t_roll < 75) new_room->treasure_type = red_max_health;
    else if (t_roll < 90) new_room->treasure_type = sharp_sword;
    else new_room->treasure_type = shield;

    if (room_count >= 15) {
        Room* tmp = first_room;
        while (tmp != NULL) {
            if (tmp->enemy_type == Jaffar) {
                tmp->enemy_type = none; tmp->enemy_alive = false;
                if (tmp->enemy) free(tmp->enemy);
                tmp->enemy = NULL;
            }
            tmp = tmp->next;
        }
        new_room->enemy_alive = true; new_room->enemy_type = Jaffar;
        new_room->enemy = init_enemy(Jaffar); is_jaffar = true;
    } else {
        if (rand() % 100 < 25) {
            new_room->enemy_alive = true;
            new_room->enemy_type = (rand() % 100 < 60) ? skeleton : guard;
            new_room->enemy = init_enemy(new_room->enemy_type);
        }
    }

    set_color(GREEN); printf("New room successfully connected!\n"); reset_color();
}

static bool is_valid_position(Room* reference, unsigned short direction) {
    if (reference == NULL) return false;
    switch (direction) {
        case 1: return (reference->up == NULL);
        case 2: return (reference->right == NULL);
        case 3: return (reference->left == NULL);
        case 4: return (reference->down == NULL);
    }
    return false;
}

static void delete_room() {
    if (last_room == NULL) {
        set_color(RED); printf("No rooms to delete.\n"); reset_color();
        return;
    }

    bool deleting_jaffar = (last_room->enemy_alive && last_room->enemy_type == Jaffar);

    if (last_room->enemy) { free(last_room->enemy); last_room->enemy = NULL; }

    Room* second_to_last = first_room;
    while (second_to_last != NULL && second_to_last->next != last_room) {
        second_to_last = second_to_last->next;
    }

    if (last_room->up) last_room->up->down = NULL;
    if (last_room->down) last_room->down->up = NULL;
    if (last_room->left) last_room->left->right = NULL;
    if (last_room->right) last_room->right->left = NULL;

    free(last_room);
    last_room = second_to_last;

    if (last_room == NULL) { first_room = NULL; is_jaffar = false; }
    else { last_room->next = NULL; }

    if (deleting_jaffar && last_room != NULL) {
        if (last_room->enemy) free(last_room->enemy);
        last_room->enemy_alive = true; last_room->enemy_type = Jaffar;
        last_room->enemy = init_enemy(Jaffar); is_jaffar = true;
    } else if (deleting_jaffar) { is_jaffar = false; }

    room_count--;
    set_color(GREEN); printf("Last room deleted successfully.\n"); reset_color();
}

static void close_map() {
    if (first_room == NULL) {
        set_color(RED); printf("Error: Map not created.\n"); reset_color();
        return;
    }
    if (room_count < 15) {
        set_color(RED); printf("Not enough rooms. Create at least 15!\n"); reset_color();
        printf("Press ENTER to continue\n"); clear_buffer();
        return;
    } else {
        set_color(BLUE); printf("Rooms created: %d\n", room_count); reset_color();
        set_color(GREEN); printf("Map closed successfully\n"); reset_color();
    }
    printf("Press ENTER to continue\n");
    while ((control_var = getchar()) != '\n' && control_var != EOF);
}

static void generate_random_map() {
    Room* temp = first_room;
    while (temp != NULL) {
        Room* next = temp->next;
        free(temp);
        temp = next;
    }

    first_room = NULL; last_room = NULL; room_count = 0;

    for (int i = 0; i < 15; i++) {
        Room* new_room = (Room*)malloc(sizeof(Room));
        new_room->up = NULL; new_room->right = NULL; new_room->left = NULL; new_room->down = NULL;
        new_room->next = NULL; new_room->room_id = room_count;
        new_room->enemy_alive = false; new_room->enemy_type = none; new_room->enemy = NULL;
        new_room->type_stanza = rand() % 10;

        if (first_room == NULL) { first_room = new_room; last_room = new_room; room_count = 1; continue; }

        bool valid_dir = 0;
        unsigned short direction;
        while (!valid_dir) {
            direction = rand() % 4 + 1;
            if (is_valid_position(last_room, direction)) valid_dir = true;
        }

        switch (direction) {
            case 1: new_room->down = last_room; last_room->up = new_room; break;
            case 2: new_room->left = last_room; last_room->right = new_room; break;
            case 3: new_room->right = last_room; last_room->left = new_room; break;
            case 4: new_room->up = last_room; last_room->down = new_room; break;
        }
        last_room->next = new_room; last_room = new_room; room_count++;

        int trap_roll = rand() % 100;
        if (trap_roll < 65) new_room->trap_type = none;
        else if (trap_roll < 75) new_room->trap_type = tile;
        else if (trap_roll < 84) new_room->trap_type = blades;
        else if (trap_roll < 92) new_room->trap_type = fall;
        else new_room->trap_type = pitfall;

        int t_roll = rand() % 100;
        if (t_roll < 20) new_room->treasure_type = no_treasure;
        else if (t_roll < 40) new_room->treasure_type = green_poison;
        else if (t_roll < 60) new_room->treasure_type = blue_healing;
        else if (t_roll < 75) new_room->treasure_type = red_max_health;
        else if (t_roll < 90) new_room->treasure_type = sharp_sword;
        else new_room->treasure_type = shield;

        new_room->enemy_alive = (rand() % 100 < 25);
        if (new_room->enemy_alive) {
            new_room->enemy_type = (rand() % 100 < 60) ? skeleton : guard;
            new_room->enemy = init_enemy(new_room->enemy_type);
        }
    }

    if (room_count >= 15) {
        if (is_jaffar) {
            Room* tmp = first_room;
            while (tmp != NULL) {
                if (tmp->enemy_type == Jaffar && tmp->enemy_alive) { tmp->enemy_alive = false; break; }
                tmp = tmp->next;
            }
        }
        last_room->enemy_alive = true; last_room->enemy_type = Jaffar;
        last_room->enemy = init_enemy(Jaffar); is_jaffar = true;
    }
    set_color(GREEN); printf("Map generated with %d areas!\n", room_count); reset_color();
}

static const char* get_class_name(enum player_class p_class) {
    switch (p_class) {
        case prince: return "Prince";
        case doppelganger: return "Doppelganger";
        default: return "Unknown";
    }
}

static const char* get_room_name(enum Room_type type) {
    switch (type) {
        case corridor: return "Corridor";
        case stairs: return "Stairs";
        case banquet_hall: return "Banquet Hall";
        case warehouse: return "Warehouse";
        case guard_post: return "Guard Post";
        case prison: return "Prison";
        case armory: return "Armory";
        case flies: return "Room of Flies";
        case tower: return "Tower";
        case bathroom: return "Bathroom";
        default: return "Unknown";
    }
}

static void print_rooms() {
    if (first_room == NULL) {
        set_color(RED); printf("No rooms created.\n"); reset_color();
        return;
    }

    Room* current = first_room;
    int id = 0;
    while (current != NULL) {
        printf("\n");
        set_color(PURPLE); printf("Room %d: %p - ", id++, (void*)current);
        set_color(YELLOW); printf("%s", get_room_name(current->type_stanza));
        set_color(GREEN); printf(" | Treasure: %s", get_treasure_name(current->treasure_type));
        set_color(RED); printf(" | Trap: %s\n", get_trap_name(current->trap_type));
        reset_color();

        if (current->left) { set_color(BLUE); printf("  Room to the Left: %p\n", (void*)current->left); reset_color(); }
        if (current->right) { set_color(BLUE); printf("  Room to the Right: %p\n", (void*)current->right); reset_color(); }
        if (current->up) { set_color(BLUE); printf("  Room Above: %p\n", (void*)current->up); reset_color(); }
        if (current->down) { set_color(BLUE); printf("  Room Below: %p\n", (void*)current->down); reset_color(); }
        current = current->next;
    }
}

static char* get_trap_name(enum Trap_type p_trap) {
    switch (p_trap) {
        case none: return "None";
        case tile: return "Falling Tile";
        case blades: return "Blades";
        case fall: return "Pit Fall";
        case pitfall: return "Ravine";
        default: return "???";
    }
}

static char* get_treasure_name(enum Treasure_type p_treasure) {
    switch (p_treasure) {
        case no_treasure: return "None";
        case green_poison: return "Green Poison";
        case blue_healing: return "Blue Healing";
        case red_max_health: return "Red Max Health";
        case sharp_sword: return "Sharp Sword";
        case shield: return "Shield";
        default: return "||";
    }
}

static Enemy* init_enemy(enum Enemy_type type) {
    Enemy* enemy = malloc(sizeof(Enemy));
    enemy->type = type;
    switch (type) {
        case skeleton: enemy->hp = 1; enemy->attack_dice = 1; enemy->defense_dice = 1; break;
        case guard: enemy->hp = 2; enemy->attack_dice = 2; enemy->defense_dice = 2; break;
        case Jaffar: enemy->hp = 3; enemy->attack_dice = 3; enemy->defense_dice = 2; break;
    }
    return enemy;
}

static char* get_enemy_name(enum Enemy_type type) {
    switch (type) {
        case skeleton: return "Skeleton";
        case guard: return "Guard";
        case Jaffar: return "Jaffar";
        default: return "Unknown";
    }
}

void set_color(enum colors color) {
    switch (color) {
        case 0: printf("\033[;30m"); break; // Black
        case 1: printf("\033[;31m"); break; // Red
        case 2: printf("\033[;32m"); break; // Green
        case 3: printf("\033[;33m"); break; // Yellow
        case 4: printf("\033[;34m"); break; // Blue
        case 5: printf("\033[;35m"); break; // Purple
        case 6: printf("\033[;36m"); break; // Cyan
        case 7: printf("\033[;37m"); break; // White
        default: break;
    }
}

void reset_color() { printf("\033[0m"); }

static void free_memory() {
    Room* curr = first_room;
    while (curr != NULL) {
        Room* next = curr->next;
        free(curr);
        curr = next;
    }
    first_room = NULL; last_room = NULL; room_count = 0;

    for (int i = 0; i < 3; i++) {
        if (players[i] != NULL) { free(players[i]); players[i] = NULL; }
    }
}

void end_game() {
    printf("Are you sure you want to end the game?\n");
    printf("1. Yes | 2. No\n");
    unsigned short choice = 0;

    do {
        printf("Choice: ");
        if (scanf("%hu", &choice) != 1) {
            while (getchar() != '\n');
            set_color(RED); printf("You must enter a number!\n"); reset_color();
            continue;
        }

        if (choice < 1 || choice > 2) puts("Enter a value between 1 and 2\n");
        if (choice == 1) {
            puts("Thanks for playing! Game Over.");
            puts("Until the next adventure!");
            puts("Press ENTER to close the game.\n");
            clear_buffer();
            exit(0);
        }
        if (choice == 2) return;
    } while (choice < 1 || choice > 2);
}

void credits() {
    printf("\n**-----------------------------------**\n");
    if (!first_winner && !second_winner && !third_winner) {
        set_color(RED); printf("No winners yet!\n"); reset_color();
    } else {
        set_color(GREEN); printf("Leaderboard:\n");
        if (first_winner) printf("First Place: %s | ", first_winner->name);
        if (second_winner) printf("Second Place: %s | ", second_winner->name);
        if (third_winner) printf("Third Place: %s", third_winner->name);
        reset_color();
    }
    set_color(CYAN); printf("\n* CREDITS *\n");
    set_color(PURPLE); printf("* Developer: Ruan \n");
    printf("* Thank you for playing Prince of Inertia!\n");
    reset_color();
    printf("\n**-----------------------------------**\n");
    printf("\nPress Enter to return to the main menu\n");
    clear_buffer();
}

static void set_winners(Player* player) {
    if (!first_winner) {
        first_winner = player;
        printf("**-----------------------------------**\n");
        printf("First Winner: %s\n", player->name);
    } else if (!second_winner && player != first_winner) {
        second_winner = player;
        printf("**-----------------------------------**\n");
        printf("Second Winner: %s\n", player->name);
    } else if (!third_winner && first_winner && player != second_winner) {
        third_winner = player;
        printf("**-----------------------------------**\n");
        printf("Third Winner: %s\n", player->name);
    }
}

static void clear_buffer() {
    while ((control_var = getchar()) != '\n' && control_var != EOF);   
}

static void get_enemy_intel(Player* player) {
    if (player == NULL || player->position == NULL) return;
    Room* first_intel = player->position->next;

    if (first_intel != NULL) {
        if (first_intel->enemy_alive) {
            printf("There is a %s in the next area!\n", get_enemy_name(first_intel->enemy_type));
        } else {
            set_color(CYAN); printf("No enemies in the next area!\n"); reset_color();
        }
    } else {
        printf("There is no next room.\n");
        return;  
    }

    Room* second_intel = first_intel->next;
    if (second_intel != NULL) {
        if (second_intel->enemy_alive) {
            printf("There is a %s two rooms away!\n", get_enemy_name(second_intel->enemy_type));
        } else {
            set_color(CYAN); printf("No enemies two rooms away!\n"); reset_color();
        }
    } else {
        printf("There is no second room ahead.\n");
    }
}

 

   
   
