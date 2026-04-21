#include<stdio.h>
#include<stdlib.h>
#include"gamelib.h"

//modifica

int main() {
    unsigned short int choice;

    // Display the initial game logo
    display_logo();

    // Clear the screen after the logo
    system("clear");

    do {
        printf("**-----------------------------------**");
        set_color(PURPLE);
        printf("\nWELCOME TO THE MAIN MENU\n");
        printf("\nChoose one of the options below:\n");
        
        set_color(GREEN);
        printf("1. Set up game\n");
        
        set_color(YELLOW);
        printf("2. Play\n");
        
        set_color(RED);
        printf("3. End game\n");
        
        set_color(CYAN);
        printf("4. Credits\n");
        
        reset_color();
        printf("\n**-----------------------------------**\n");
        printf("Choice: ");

        // Input validation for the menu choice
        if (scanf("%hu", &choice) != 1) {
            while (getchar() != '\n'); // Clear the input buffer
            set_color(RED);
            printf("You did not enter a valid number. TRY AGAIN\n");
            reset_color();
            continue;
        }
        while (getchar() != '\n'); // Consume the newline character

        // Check if the choice is within the valid range
        if (choice < 1 || choice > 4) {
            set_color(RED);
            printf("Error. The number must be between 1 and 4\n");
            reset_color();
        }

        // Logic to trigger the translated functions
        switch (choice) {
            case 1: 
                setup_game();
                break;

            case 2:
                start_game();
                break;

            case 3:
                end_game();
                break;

            case 4:
                credits();
                break;

            default:
                break;
        }

    } while (1); // The loop continues until the exit(0) is triggered inside end_game()

    return 0;
}
