#include<stdio.h>
#include<stdlib.h>
#include"gamelib.h"

//modifica








int main(){

unsigned short int scelta;

logo();

system("clear");



do{
    //system("clear");
    
    printf("**-----------------------------------**");
    setColor(PURPLE);
    printf("\nBENVENUTO AL MENU\n");
    printf("\nScegliere una delle opzioni sotto:\n");
    setColor(GREEN);
    printf("1.Imposta gioco\n");
    setColor(YELLOW);
    printf("2.Giocare\n");
    setColor(RED);
    printf("3.Termina gioco\n");
    setColor(CYAN);
    printf("4.Crediti\n");
    resetColor();
    printf("\n**-----------------------------------**\n");
    printf("Scelta : ");
    if(scanf("%hu", &scelta)!= 1){
            while(getchar() != '\n');
            setColor(RED);
            printf("Non vai inserito un numero valido. RIPROVA\n");
            resetColor();
            continue;
         }
        while(getchar() != '\n');
        

    if( scelta <  1 || scelta > 4){
        setColor(RED);
        printf("Errore. il numero deve essere compresso tra 1 e 4\n");
        resetColor();
    }


    switch(scelta){
        
        case 1: 
        imposta_gioco();
        break;

        case 2:
        gioca();
        break;

        case 3:
        termina_gioco();
        break;

        case 4:
        crediti();
        break;

    }
    







}while(1); // il gioco non finisce finche' il giocatore e' sicuro di finire il programma nella funzione termina gioco

}
