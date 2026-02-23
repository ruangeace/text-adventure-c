#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "gamelib.h"

typedef struct Giocatore Giocatore;
typedef struct Stanza Stanza;
typedef struct Nemico Nemico;

Giocatore* giocatori[3];

static Stanza* pFirst = NULL;
static Stanza* pLast = NULL;


int c = 0; // Variabile di controllo
static unsigned short turno_attuale = 0;
unsigned short numeroGiocatori;
static bool giocoFinito;


// Impostazione Gioco
bool impostato = 0;
unsigned short principe_presente = 0;
bool isJaffar = false;
static void creazione_giocatore(unsigned short numeroGiocatori);
static unsigned short getNumeroGiocatori();

// Impostazioni quando si gioca
static void menu_giocatore(Giocatore* giocatore);
static void avanza(Giocatore* giocatore);
static void combatti();
static void attaco(int dadi_attaco, int dadi_difesa, unsigned char* vita, const char* nome_attaccante, const char* nome_difensore);
static void scappa(Giocatore* giocatore);
static void stampa_giocatore();
static void stampa_zona(Giocatore* giocatore);
static void prendi_tesoro(Giocatore* giocatore);
static void cerca_porta_segreta();
static void passa();

// Impostazione della mappa
static int numero_stanze;
static void menu_impostazione_mappa();
static void ins_stanza();
static void canc_stanza();
static void stampa_stanze();
static void genera_random();
static void chiudi_mappa();
static const char* getNome_stanza(enum Tipo_stanza tipo);
static bool posizione_valida(Stanza* riferimento, unsigned short direzione);
static const char* getNomeClasse(enum classe_giocatore classe);
static char* getNomeTesoro(enum Tipo_tesoro pTesoro);
static char* getNomeTrabocchetto(enum Tipo_trabocchetto pTrabocchetto);

//nemico
static Nemico* inizializza_nemico(enum Tipo_nemico tipo);
static char* getNomeNemico(enum Tipo_nemico tipo);


static void informazioni_nemici(Giocatore* giocatore);




void setColor(enum colors color);
void resetColor();



static void deallocazione_memoria();
static void svuota_buffer();
static bool turno();
static void Vincitori(Giocatore* giocatore);
Giocatore* primo_vincitore;
Giocatore* secondo_vincitore;
Giocatore* terzo_vincitore;



void logo(){
        setColor(PURPLE);
        printf("**---------------------------------------------------------------------------------**\n");
        printf("**     _____      _                         __   _____                 _   _       **\n");
        printf("**    |  __  |   (_)                       / _| |_   _|               | | (_)      **\n");
        printf("**    | |__) | __ _ _ __   ___ ___    ___ | |_    | |  _ __   ___ _ __| |_ _  __ _ **\n");
        printf("**    |  ___/ '__| | '_   / __/ _   / _   |  _|   | | | '_   / _   '__| __| |/ _` |**\n");
        printf("**    | |   | |  | | | | | (_|  __/ | (_) | |    _| |_| | | |  __/ |  | |_| | (_| |**\n");
        printf("**    |_|   |_|  |_|_| |_| ___ ___|  |___/|_|   |_____|_| |_| __  |_| | __|_| __,_|**\n");
        printf("**---------------------------------------------------------------------------------**\n");
    
    
            
        printf("\nLe ombre si allungano nel regno... il male è vicino.\n");
        resetColor();
        printf("**---------------------------------------------------------------------------------**\n");
        setColor(YELLOW);
        printf("Prince of Inertia: un gioco di coraggio e sacrificio.\n");
        printf("Le trappole sono letali, ma la gloria è eterna\n");
        printf("Jaffar ride nell'ombra, ma il tuo ferro non tremerà\n");
        resetColor();
        setColor(CYAN);
        printf("Afferra la spada. La leggenda inizia ora.");
        resetColor();
        printf("(INVIO)\n");
        printf("**---------------------------------------------------------------------------------**\n");
        svuota_buffer();

}

void imposta_gioco() {
     srand(time(NULL));
     
     if (impostato == true) {
        setColor(BLUE);
        printf("Il gioco e' gia' stato impostato. La configurazione precedente sara' eliminata!\n");
        resetColor();
        deallocazione_memoria();
        impostato = false;
        printf("Premere Invio per continuare\n");
        svuota_buffer();
        system("clear");
    }

    pFirst = NULL;
    pLast = NULL;
    numero_stanze = 0;

    // Richiedi il numero di giocatori
    numeroGiocatori = getNumeroGiocatori();
    system("clear");

    // Crea e imposta i giocatori
    creazione_giocatore(numeroGiocatori);

    // Passa alla configurazione della mappa
    menu_impostazione_mappa();
    impostato = true;
    printf("Valore di impostato: %d\n", impostato);
    
    
}

void gioca() {
    

    // Verifica che il gioco sia stato impostato correttamente
    if (impostato == false) {
        setColor(RED);
        printf("\nPrima di giocare devi impostare il gioco. Riprova!\n");
        resetColor();
        printf("Premere Invio per continuare\n");
        svuota_buffer();
        return;
    }

    if(giocoFinito == true){
        printf("Devi reimpostare il gioco nuovamente!\n");
        printf("Premere Invio per continuare!\n");
        svuota_buffer();

        
        return;
    }

    // Verifica che ci sia almeno un giocatore
    if (numeroGiocatori <= 0) {
        setColor(RED);
        printf("\nErrore: nessun giocatore disponibile.\n");
        resetColor();
        return;
    }

    // Verifica che la mappa sia stata creata correttamente
    if (pFirst == NULL) {
        setColor(RED);
        printf("\nErrore: la mappa non è stata inizializzata correttamente.\n");
        resetColor();
        return;
    }
    
    printf("**---------------------------------------------------------------------------------**\n");
    setColor(CYAN);
    puts("PRINCIPI FONDAMENTALI:\n");
    puts("Il viaggio si svolge a turni, e l'ordine di gioco cambia in modo casuale ad ogni turno.\n");
    puts("Durante il tuo turno, potrai scegliere tra diverse opzioni che influenzeranno il tuo destino.\n");
    puts("L'obiettivo è raggiungere la fine della mappa senza perdere tutti i punti vita.\n");
    puts("Se i tuoi punti vita scendono a zero, il tuo viaggio termina e non potrai più continuare.\n");
    puts("Premi INVIO e inizia la tua impresa!\n");
    resetColor();
    printf("**---------------------------------------------------------------------------------**\n");

    svuota_buffer();

    system("clear");

    // Posiziona tutti i giocatori nella prima posizione della mappa
    for (int i = 0; i < numeroGiocatori; i++) {
        giocatori[i]->posizione = pFirst;
        printf("**-----------------------------------**\n");
        setColor(GREEN);
        printf("Il giocatore %s è posizionato nella prima stanza: %s\n", 
               giocatori[i]->nome, getNome_stanza(pFirst->tipo_stanza));   
         resetColor();      

    }
   while(true){
   

    if(turno()){

        impostato = false;
        giocoFinito = false;
        break;
    }
   
}

}


    
static bool turno() {
    static bool primo_turno = true;
    if(giocoFinito){
        return true;
      }

    if (primo_turno || turno_attuale >= numeroGiocatori) {
        // Mischia i giocatori
        for (int i = numeroGiocatori - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            Giocatore* temp = giocatori[i];
            giocatori[i] = giocatori[j];
            giocatori[j] = temp;
        }
        turno_attuale = 0;
        primo_turno = false;
    }

    // Cerca un giocatore vivo
    while (turno_attuale < numeroGiocatori &&
           giocatori[turno_attuale]->p_vita == 0) {
        turno_attuale++;
    }

    // Se siamo alla fine della lista, controlla se ci sono vivi in generale
    if (turno_attuale >= numeroGiocatori) {
        bool qualcuno_vivo = false;
        for (int i = 0; i < numeroGiocatori; i++) {
            if (giocatori[i]->p_vita > 0) {
                qualcuno_vivo = true;
                turno_attuale = i; // fai partire da qui il prossimo giro
                break;
            }
        }

        if (!qualcuno_vivo) {
            setColor(RED);
            printf("Tutti i giocatori sono morti!\n");
            giocoFinito = true;

                setColor(CYAN);
                printf("Ora ritornerai al menu principale!\n");
                
                printf("Premere Invio per continuare...\n");
                resetColor();
                svuota_buffer();

                return true;
               
        }
    }

    // Gestione del turno
    Giocatore* giocatore_corrente = giocatori[turno_attuale];

    giocatore_corrente->azione = 0;

    

    if(giocoFinito){
        return true;
      }
 
    printf("\n=== Turno di %s ===\n", giocatore_corrente->nome);
    if(giocatore_corrente->p_vita > 0 && !giocatore_corrente->vincitore){
 
        menu_giocatore(giocatore_corrente);
    
     }else{
        if(giocatore_corrente->p_vita == 0){
        printf(" %s sei morto!", giocatore_corrente->nome);
     }
       else if(giocatore_corrente->vincitore){
        printf("%s ha già vinto e non gioca più\n", giocatore_corrente->nome);
       
}

  }
           turno_attuale++;
        
            return false;

   }

                 

static unsigned short getNumeroGiocatori() {
    unsigned short numeroGiocatori;

    do {
        printf("**-----------------------------------**\n");
        setColor(PURPLE);
        printf("Quanti persone vogliono giocare la partita? Inserisci un numero compreso tra 1 e 3\n ");
        resetColor();
        printf("scelta: ");
        scanf("%hu", &numeroGiocatori);
        getchar();
        printf("\n**-----------------------------------**\n");
        if (numeroGiocatori < 1 || numeroGiocatori > 3) {
            setColor(RED);
            puts("Inserisci un numero compreso tra 1 e 3\n");
            resetColor();
            scanf("%*[^\n]");
            
        }
    } while (numeroGiocatori < 1 || numeroGiocatori > 3);

    return numeroGiocatori;
}

static void creazione_giocatore(unsigned short numeroGiocatori) {
    // Inizializza la variabile per il principe
    principe_presente = 0;

    // Crea e imposta i giocatori
    for (int i = 0; i < numeroGiocatori; i++) {
        printf("\nCreazione giocatore %d\n", i + 1);

        // Alloca memoria per il giocatore
        giocatori[i] = malloc(sizeof(Giocatore));
        if (giocatori[i] == NULL) {
            printf("Errore nella allocazione di memoria\n");
            return;
        }

        // Imposta il nome del giocatore
        printf("**-----------------------------------**\n");
        printf("Inserisci il nome del giocatore %d: ", i + 1);
        char nomeGiocatore[30];
        scanf("%30s", nomeGiocatore);
        getchar(); // Consuma il newline
        strcpy(giocatori[i]->nome, nomeGiocatore);
        setColor(GREEN);
        printf("\nNome inserito: %s\n", giocatori[i]->nome);
        resetColor();
        printf("**-----------------------------------**\n");

        // Imposta la classe del giocatore
        unsigned short scelta;
        bool sceltaValida = false;

        do {
            printf("Giocatore %d, scegli la tua classe:\n", i + 1);
            setColor(CYAN);
            printf("1. Principe | Vita: 3 | dadi attacco: 2 | dadi difesa: 2 | vantaggio: piede leggero\n");
            resetColor();
            setColor(YELLOW);
            printf("2. Doppleganger | Vita: 3 | dadi attacco: 2 | dadi difesa: 2\n");
            resetColor();
            printf("scelta: ");
            if(scanf("%hu", &scelta)!= 1){
                while (getchar() != '\n');
                setColor(RED);
                printf("Dovresti inserire un numero!\n");
                resetColor();
                continue;
            }
            while(getchar() != '\n');
            

            if (scelta != 1 && scelta != 2) {
                setColor(RED);
                printf("Errore: devi inserire 1 oppure 2.\n");
                resetColor();
                continue;
                
            }
                
        

            if (numeroGiocatori == 1 && scelta != 1) {
                setColor(RED);
                printf("\nATTENZIONE! Sei l'unico giocatore, devi essere il Principe.\n");
                resetColor();
                continue;
            }

            if (scelta == 1 && principe_presente) {
                setColor(RED);
                printf("ATTENZIONE! C'è già un Principe. Scegli un'altra classe.\n");
                resetColor();
                continue;
            }

            sceltaValida = true;
        } while (!sceltaValida);

        // Assegna la classe e gli attributi del giocatore
        if (scelta == 1) {
            giocatori[i]->classe = principe;
            giocatori[i]->dadi_attaco = 2;
            giocatori[i]->dadi_difesa = 2;
            giocatori[i]->p_vita = 3;
            giocatori[i]->p_vita_max = 3;
            giocatori[i]->azione = 1;
            giocatori[i]->vincitore = 0;
            principe_presente = 1;
            
            setColor(GREEN);
            printf("Hai scelto: Principe\n");
            resetColor();
        } else {
            giocatori[i]->classe = doppleganger;
            giocatori[i]->dadi_attaco = 2;
            giocatori[i]->dadi_difesa = 2;
            giocatori[i]->p_vita = 3;
            giocatori[i]->p_vita_max = 3;
            giocatori[i]->azione = 0;
            giocatori[i]->vincitore = 0;
            setColor(GREEN);
            printf("Hai scelto: Doppleganger\n");
            resetColor();
        }

        printf("**-----------------------------------**\n");
        setColor(GREEN);
        printf("\nGiocatore %d impostato con successo\n", i + 1);
        resetColor();
    }

    // Se nessuno ha scelto il Principe, assegna forzatamente l'ultimo giocatore come Principe
    if (!principe_presente) {
        setColor(RED);
        printf("\nATTENZIONE! Nessuno ha scelto Principe. L'ultimo giocatore sarà assegnato forzatamente come Principe.\n");
        giocatori[numeroGiocatori - 1]->classe = principe;
        giocatori[numeroGiocatori - 1]->dadi_attaco = 2;
        giocatori[numeroGiocatori - 1]->dadi_difesa = 2;
        giocatori[numeroGiocatori - 1]->p_vita = 3;
        giocatori[numeroGiocatori - 1]->p_vita_max = 3;
        giocatori[numeroGiocatori - 1]->azione = 0;
        giocatori[numeroGiocatori - 1]->vincitore = 0;
        principe_presente = 1;
        printf("Giocatore %d è stato forzato a essere Principe.\n", numeroGiocatori);
        resetColor();
        printf("\nPremere Invio per continuare l'impostazione!\n");
        while ((c = getchar()) != '\n' && c != EOF);
    }
}


// Menu per impostare la mappa del gioco
static void menu_impostazione_mappa() {
    unsigned short int scelta;
    do {
        printf("**-----------------------------------**\n");
        printf("\nBenvenuto al menu di impostazione della mappa\n");
        setColor(GREEN);
        printf("1 . Inserisci una nuova stanza\n");
        resetColor();
        setColor(RED);
        printf("2 . Cancella la stanza\n");
        resetColor();
        setColor(BLUE);
        printf("3 . Stampa tutte le stanze\n");
        setColor(PURPLE);
        printf("4 . Genera le stanze di modo casuale\n");
        resetColor();
        setColor(CYAN);
        printf("5 . Chiudi la mappa\n");
        resetColor();
        printf("scelta : ");
        if(scanf("%hu", &scelta) != 1){
            while(getchar() != '\n');
            setColor(RED);
            printf("Dovresti inserire un numero\n");
            resetColor();
            continue;
        }

        while(getchar()!= '\n');
        
        printf("\n**-----------------------------------**\n");
        
        printf("Hai scelto: %d\n", scelta);

        if (scelta > 5 || scelta < 1) {
            setColor(RED);
            printf("Dovresti inserire un valore compresso tra 1 e 5\n");
            resetColor();
            continue;
            
        }

        switch (scelta) {
            case 1:
                ins_stanza();
                break;
            case 2:
                canc_stanza();
                break;
            case 3:
                
                stampa_stanze();
                break;
            case 4:
                genera_random();
                break;
            case 5:            
                chiudi_mappa();
                if(numero_stanze < 15){ // controllo se ci sono stanze abbastanza per chiudere la mappa
                    continue;
                }
                break;
        }
    } while (scelta != 5 || numero_stanze < 15);
}

static void menu_giocatore(Giocatore* giocatore){
    if(giocoFinito){ // non apre se e' finito
        return;
    }
unsigned short scelta = 0;
bool turnoFinito = 0;
  
    do{
        if(giocoFinito){
            return;
        }
         printf("**-----------------------------------**\n");
         printf("Scegli una delle seguenti opzioni: \n");
         setColor(GREEN);
         printf("1 . Avanza\n");
         setColor(YELLOW);
         printf("2 . Combatti\n");
         setColor(RED);
         printf("3 . Scappa\n");
         setColor(CYAN);
         printf("4 . Stampa le tue informazioni\n");
         setColor(PURPLE);
         printf("5 . Stampa le informazioni della stanza\n");
         setColor(BLUE);
         printf("6 . Prendi un tesoro\n");
         setColor(WHITE);
         printf("7 . Cerca una porta segreta\n");
         setColor(RED);
         printf("8 . Passa il turno\n");
         resetColor();
         printf("9 . Termina gioco\n");
         printf("10 . Informazioni nemici\n");
         
         printf("scelta: ");
         if(scanf("%hu", &scelta)!= 1){
            while(getchar() != '\n');
            setColor(RED);
            printf("Dovresti inserire un numero!\n");
            resetColor();
            continue;
         }
         while(getchar()!= '\n');
         
         printf("**-----------------------------------**\n");
         if(scelta < 1 || scelta > 10){
         setColor(RED);
            puts("Non hai inserito un numero valido! Riprova.\n");
            resetColor();


            continue;
            
         }
    
    switch(scelta){
        case 1:
        avanza(giocatore);
        break;
        case 2:
        if(giocatore->posizione->nemicoVivo && giocatore->posizione->nemico != NULL){
        combatti(giocatore, giocatore->posizione->nemico);

        } else{
            setColor(YELLOW);
            printf("Niente da combattere qui, a parte la noia!\n");
            resetColor();
            if(giocatore->p_vita == 0){
                puts("Hai raggiunto 0 di vita!\n");
            }
        }
        break;
        case 3:
        scappa(giocatore);
        break;
        case 4:
        stampa_giocatore(giocatore);
        break;
        case 5:
        stampa_zona(giocatore);
        break;
        case 6:
        prendi_tesoro(giocatore);
        break;
        case 7:
        if(giocatore->azione){
            setColor(RED);
            printf("Hai già fatto una azione in questo turno! Non puoi cercare una porta segreta.\n");
            resetColor();
        } else{
        cerca_porta_segreta(giocatore);
        }
        break;
        case 8:
        passa();
        turnoFinito = true;
        break;
        case 9:
        termina_gioco();
        break;
        case 10:
        informazioni_nemici(giocatore);

        
    }

        if(giocatore->p_vita == 0){
            turnoFinito = true;
     }
        
    
} while(!turnoFinito);

}

static void avanza(Giocatore* giocatore) {
    if(giocoFinito){
        printf("Il gioco è finito\n");
        return;
    }
    if (giocatore->azione) {
        setColor(RED);
        printf("Impossibile avanzare. Limite di azione raggiunto!\n");
        resetColor();
        return;
    }

    Stanza* stanzaCorrente = giocatore->posizione;

    // Se siamo già nell'ultima stanza, blocca avanzamento
    if (stanzaCorrente == pLast) {
        if (stanzaCorrente->nemicoVivo) {
            setColor(RED);
            printf("Non c'è più via di fuga. Affronta JAFFAR in un duello all'ultimo sangue!\n");
            resetColor();
            return;
        }
        setColor(RED);
        printf("Non puoi avanzare! Ti trovi già nell'ultima stanza.\n");
        resetColor();
        return;
    }

    // Se c'è un nemico vivo nella stanza corrente, blocca avanzamento
    if (stanzaCorrente->nemicoVivo) {
        setColor(RED);
        printf("Devi sconfiggere il nemico in questa zona prima di avanzare.\n");
        resetColor();
        return;
    }

    Stanza* prossimaStanza = stanzaCorrente->next;
    if (prossimaStanza == NULL) {
        setColor(RED);
        printf("Errore: nessuna stanza successiva trovata.\n");
        resetColor();
        return;
    }

    giocatore->stanzaPrecedente = giocatore->posizione;

    // Gestione trabocchetti (come prima)
    if (!prossimaStanza->trabocchettoAttivato) {
        if (prossimaStanza->tipo_trabocchetto == nessuno) {
            setColor(CYAN);
            printf("Non ci sono trabocchetti in questa stanza!\n");
            resetColor();
        } else if (giocatore->classe == principe && !giocatore->primoTrabocchetto) {
            printf("Il primo trabocchetto viene ignorato perché sei un principe.\n");
            giocatore->primoTrabocchetto = true;
        } else {
            unsigned char danno = 0;
            switch (prossimaStanza->tipo_trabocchetto) {
                case tegola: danno = 1; break;
                case lame: danno = 2; break;
                case caduta:
                case burrone: danno = (rand() % 2) + 1; break;
                default: break;
            }

            if (giocatore->p_vita <= danno)
                giocatore->p_vita = 0;
            else
                giocatore->p_vita -= danno;

            prossimaStanza->trabocchettoAttivato = true;

            setColor(RED);
            switch (prossimaStanza->tipo_trabocchetto) {
                case tegola:
                    printf("Hai perso 1 punto di vita a causa del trabocchetto!\n");
                    break;
                case lame:
                    printf("Hai perso 2 punti di vita a causa del trabocchetto!\n");
                    break;
                case caduta:
                    printf("Sei caduto in una buca! Hai perso %d punti di vita!\n", danno);
                    break;
                case burrone:
                    printf("Sei finito quasi in un burrone! Hai perso %d punti di vita e salti il turno!\n", danno);
                    break;
                default:
                    break;
            }
            resetColor();

            if (giocatore->p_vita == 0) {
                printf("%s è morto a causa del trabocchetto!\n", giocatore->nome);
                printf("Premi INVIO per continuare...\n");
                svuota_buffer();
                passa();
                return;
            }

            if (prossimaStanza->tipo_trabocchetto == burrone) {
                printf("Premi INVIO per continuare...\n");
                svuota_buffer();
                passa();
                return;
            }
        }
    } else {
        setColor(CYAN);
        printf("il trabocchetto è già stato attivato in questa stanza.\n");
        resetColor();
    }

    // Sposto il giocatore nella nuova stanza
    giocatore->posizione = prossimaStanza;

    if (prossimaStanza == pLast) {
        setColor(YELLOW);
        printf("ULTIMA STANZA!\n");

        if (prossimaStanza->nemicoVivo) {
            
            printf("Preparati a combattere il boss finale: JAFFAR!\n");
            printf("Info boss: Attacco: 3 | Difesa: 2 | Vita: 3\n");

        } else {
            // Boss sconfitto, vittoria!
            printf("CONGRATULAZIONI! JAFFAR SCONFITTO!\n");
            printf("Fine del gioco.\n");

            resetColor();
            return;
        }
        resetColor();
    }

    setColor(GREEN);
    printf("%s si è mosso nella stanza: ", giocatore->nome);
    setColor(YELLOW);
    printf("%s\n", getNome_stanza(prossimaStanza->tipo_stanza));
    resetColor();

    giocatore->azione = true;
}



   


static void combatti(Giocatore* giocatore, Nemico* nemico) {
    if (!giocatore) {
         printf("Giocatore è NULL!\n"); return; }
    if (!giocatore->posizione) {
         printf("Posizione NULL!\n"); return; 
        }
        //controllo se esiste un nemico oppure se e' gia stato combattuto
    if (!giocatore->posizione->nemicoVivo || !nemico) { 
        printf("Nessun nemico vivo!\n"); 
        return; 
    }
    

    printf("Ora stai combattendo contro %s\n", getNomeNemico(giocatore->posizione->tipoNemico));

    do {
        printf("**-----------------------------------**\n");
        printf("\n**--------");
        setColor(PURPLE);
        printf(" Turno di combattimento ");
        resetColor();
        printf("-------**\n");


        unsigned short dado_giocatore = (rand() % 6) + 1;
        unsigned short dado_nemico = (rand() % 6) + 1;
        setColor(CYAN);
        printf("Dado del giocatore %s : %hu X Dado nemico : %hu\n", giocatore->nome, dado_giocatore, dado_nemico);
        resetColor();

        if (dado_giocatore >= dado_nemico) {
            setColor(GREEN);
            printf("Il giocatore %s inizia ad attaccare per primo!\n", giocatore->nome);
            resetColor();
            attaco(giocatore->dadi_attaco, nemico->dadiDifesa, &nemico->vitaNemico, giocatore->nome, getNomeNemico(nemico->tipo_nemico));

            if (nemico->vitaNemico > 0) {
                attaco(nemico->dadiAtacco, giocatore->dadi_difesa, &giocatore->p_vita, getNomeNemico(nemico->tipo_nemico), giocatore->nome);
            }
        } else {
            setColor(RED);
            printf("Il nemico %s attacca per primo!\n", getNomeNemico(nemico->tipo_nemico));
            resetColor();
            attaco(nemico->dadiAtacco, giocatore->dadi_difesa, &giocatore->p_vita, getNomeNemico(nemico->tipo_nemico), giocatore->nome);

            if (giocatore->p_vita > 0) {
                attaco(giocatore->dadi_attaco, nemico->dadiDifesa, &nemico->vitaNemico, giocatore->nome, getNomeNemico(nemico->tipo_nemico));
            }
        }

    } while (giocatore->p_vita > 0 && nemico->vitaNemico > 0);

    if (nemico->vitaNemico <= 0) {
        giocatore->posizione->nemicoVivo = false;
        printf("**-----------------------------------**\n");
        setColor(GREEN);
        printf("Hai sconfitto il nemico!\n");
        setColor(YELLOW);
        printf("%s ha guadagnato +1 di vita!\n", giocatore->nome);
        resetColor();
        giocatore->p_vita += 1;
    }

        if(giocatore->p_vita > giocatore->p_vita_max){
            giocatore ->p_vita_max = giocatore->p_vita;
        }

        if(giocatore->posizione == pLast && nemico->tipo_nemico == Jaffar && nemico->vitaNemico == 0){
                Vincitori(giocatore);
                giocatore->vincitore = true;
                giocatore->posizione->nemicoVivo = false;
            printf("Complimenti! %s hai sconffinto Jaffar e hai vinto la partita.\n", giocatore->nome); 
            giocoFinito = true;

        printf("**-----------------------------------**\n");
        printf("Torni al menu principale e imposti il gioco nuovamente. (INVIO)\n");
        
        svuota_buffer();
        
         



    } else if (giocatore->p_vita <= 0) {
        setColor(RED);
        printf("\nPer %s la battaglia è finita.\n", giocatore->nome);
        resetColor();      
        puts("Premere Invio per saltare il turno");
        svuota_buffer();   
        passa();     
       
    }

    
}

static void attaco(int dadi_attaco, int dadi_difesa, unsigned char* vita, const char* nome_attaccante, const char* nome_difensore) {
    int attacco = 0;
    int difesa = 0;

    for (int i = 0; i < dadi_attaco; i++)
        attacco += (rand() % 6) + 1;

    for (int i = 0; i < dadi_difesa; i++)
        difesa += (rand() % 6) + 1;
    printf("**-----------------------------------**\n");
    printf("%s attacca %s!\n", nome_attaccante, nome_difensore);
    setColor(CYAN);
    printf("Attacco: %d x Difesa: %d\n", attacco, difesa);
    resetColor();

    if (attacco > difesa) {
        (*vita)--;
        setColor(GREEN);
        printf("Colpo riuscito!");
        resetColor();
        printf(" Vita di %s: %d\n", nome_difensore, *vita);
    } else {
        setColor(GREEN);
        printf("Difesa riuscita! Nessun danno a %s.\n", nome_difensore);
        resetColor();
    }
}


static void scappa(Giocatore* giocatore) {

    if(giocatore->posizione == pFirst || giocatore->stanzaPrecedente == NULL){
        setColor(RED);
        printf("Impossibile fuggire dall'area di partenza.\n");
        resetColor();
        return;

     
  }
    if(giocatore->posizione->nemico == NULL || giocatore->posizione->nemicoVivo == false){
        setColor(RED);
        printf("Fuggire? Ma da cosa? Dalla tua immaginazione?\n");
        resetColor();
        return;
    } 
    

    int maxFughe = 0;
    if(giocatore->classe == principe){
        maxFughe = 1;
    } else if(giocatore->classe == doppleganger){
        maxFughe = 2;
    }

    if(giocatore->fughe >= maxFughe){
        printf("Non puoi scappare più! Hai raggiunto il limite\n");
        return;
    }


        giocatore->posizione = giocatore->stanzaPrecedente;
        giocatore->stanzaPrecedente = NULL;
        giocatore->fughe++;

        setColor(YELLOW);
        printf("%s è tornato indietro!\n", giocatore->nome);
        resetColor();
}

//stampa tutte le informazioni del giocatore
static void stampa_giocatore(Giocatore* giocatore) {
    

    printf("Nome: %s | ", giocatore->nome);
    setColor(YELLOW);
    printf("Classe: %s\n", getNomeClasse(giocatore->classe));
  
    setColor(RED);
    printf("Dadi di attacco: %d |", giocatore->dadi_attaco);
    printf("Dadi di difesa: %d\n", giocatore->dadi_difesa);
    setColor(CYAN);
    printf("Vita: %d | ", giocatore->p_vita);
    printf("Vita massima: %d\n", giocatore->p_vita_max);
    resetColor();


}

// funzione per stampare le informazioni della zona corrente
static void stampa_zona(Giocatore* giocatore) {
    
    Stanza* stanzaCorrente = giocatore->posizione;
    printf("N. Stanza: %d\n", giocatore->posizione->riferimento_stanza);
    setColor(YELLOW);
    printf("Tipo Stanza: %s\n", getNome_stanza(stanzaCorrente->tipo_stanza));
    
    setColor(GREEN);
    if(stanzaCorrente->tipo_tesoro != nessun_tesoro && !stanzaCorrente->tesoroPreso){
        printf("Brilla qualcosa! È un tesoro!\n");

    } else{
        printf("Nessun tesoro!\n");
    }

    setColor(RED);
    if(stanzaCorrente->nemicoVivo){
        const char* nome_nemico;
        switch(stanzaCorrente->tipoNemico){

            case schelettro: nome_nemico = "uno scheletro"; break;
            case guardia: nome_nemico = "una guardia"; break;
            case Jaffar: nome_nemico = "Jaffar"; break;
            
        }
        printf("C'è %s in questa zona!\n", nome_nemico);
        resetColor();
    } else{
        setColor(CYAN);
        printf("Nessun nemico in questa zona!\n");
        resetColor();
        
    }
    
}



static void prendi_tesoro(Giocatore* giocatore) {
    if(giocatore->posizione->tesoroPreso){
        setColor(RED);
        puts("Nessun tesoro\n");
        resetColor();
        return;
    }

    enum Tipo_tesoro tipoTesoro = giocatore->posizione->tipo_tesoro;
     switch(tipoTesoro){

        case nessun_tesoro:
        setColor(RED);
        printf("Nessun tesoro in questa stanza\n");
        resetColor();
        break;
        
        case verde_veleno:
        setColor(RED);
        printf("Sei avvelenato! Perdi 1 punto vita!\n");
        resetColor();
        giocatore->p_vita -= 1;
        if(giocatore->p_vita <= 0){
            giocatore->p_vita = 0;
            //se il giocatore muore allora salta il turno
        
                setColor(RED);
                printf("%s morto a causa di un veleno\n", giocatore->nome);
                resetColor();
                puts("Premere Invio per saltare il turno!\n");
                svuota_buffer();
                passa();
                
                
          return;
            
        }
        break;

        case blu_guarigione:
        setColor(CYAN);
        printf("Guarigione! %s ha un cuoricino in più\n", giocatore->nome);
        resetColor();
        giocatore->p_vita +=1;
        if(giocatore->p_vita > giocatore->p_vita_max){
            giocatore->p_vita_max = giocatore->p_vita;
        }
        break;
        case rosso_aumenta_vita:
        setColor(CYAN);
        printf("Pozione rossa! %s si è ricaricato! Come nuovo!\n", giocatore->nome);
        resetColor();
        giocatore->p_vita = giocatore->p_vita_max;
        break;

        case spada_tagliente:
        setColor(CYAN);
        printf("Spada tagliente! +1 dadi di attacco\n");
        resetColor();
        giocatore->dadi_attaco +=1;
        break;

        case scudo:
        setColor(CYAN);
        printf("Scudo! +1 dadi di difesa\n");
        resetColor();
        giocatore->dadi_difesa +=1;
        break;
    }

    giocatore->posizione->tesoroPreso = true;

}

static void cerca_porta_segreta(Giocatore* giocatore) {
    if (giocatore->azione) {
        setColor(RED);
        printf("Limite di azione raggiunto!\n");
        resetColor();
        return;
    }

    Stanza* stanza = giocatore->posizione;
    int prob;
    int tentativi_rima = 0;

    if(!stanza->primo_tentativo){
        prob = 33;
        stanza->primo_tentativo = true;
        printf("Primo tentativo!\n");
        tentativi_rima = 2;

    } else if(!stanza->secondo_tentativo){
        prob = 20;
        stanza->secondo_tentativo = true;
        printf("Secondo tentativo!\n");
        tentativi_rima = 1;
    } else if(!stanza->terzo_tentativo){
        prob = 15;
        stanza->terzo_tentativo = true;
        printf("Terzo tentativo!\n");
        tentativi_rima = 0;
    }
    else{
        printf("Tutti i tentativi sono finiti!\n");
        return;
    }

    int probabilita = rand () % 100;

    if(probabilita < prob){
        printf("**-----------------------------------**\n");
        printf("Hai trovato una porta segreta!\n");
   
        Stanza* segreta = (Stanza*) malloc(sizeof(Stanza));
        if(segreta == NULL){
            puts("Memoria non allocata\n");
            return;
        }

        segreta->stanza_sopra = NULL;
        segreta->stanza_sinistra = NULL;
        segreta->stanza_destra = NULL;
        segreta->stanza_sotto = NULL;
        segreta->tipo_stanza = rand() % 10;
        segreta->primo_tentativo = false;
        segreta->secondo_tentativo = false;
        segreta->terzo_tentativo = false;

        bool direzione_valida = false;  
        unsigned short direzione;
    
        while(!direzione_valida){
            direzione = rand() % 4 + 1;

            if(posizione_valida(stanza, direzione)){
                direzione_valida = true;
            }
        }
    
        switch(direzione){
            case 1:
                stanza->stanza_sotto = segreta;
                pLast->stanza_sopra = stanza;
                break;
            case 2:
                stanza->stanza_sinistra = segreta;
                pLast->stanza_destra = stanza;
                break;
            case 3:
                stanza->stanza_destra = segreta;
                pLast->stanza_sinistra = stanza;
                break;
            case 4: 
                stanza->stanza_sopra = segreta;
                pLast->stanza_sotto = stanza;
                break;
        }

        int probTesoro = rand () % 100;
    if(probTesoro < 20){
        segreta->tipo_tesoro = nessun_tesoro;
    } else if(probTesoro < 40){
        segreta->tipo_tesoro = verde_veleno;
    } else if(probTesoro < 60 ){
        segreta->tipo_tesoro = blu_guarigione;
    } else if(probTesoro < 75){
        segreta->tipo_tesoro = rosso_aumenta_vita;
    } else if(probTesoro < 90){
        segreta->tipo_tesoro = spada_tagliente;
    } else{
        segreta->tipo_tesoro = scudo;
    }
          
        if(!direzione_valida){
            printf("Nessuna direzione disponibile!\n");
            free(segreta);
            giocatore->azione = true;
            return;
        }
        
        giocatore->posizione = segreta;
        
           
        unsigned short scelta = 0;
        do{
            printf("Vuoi vedere se esiste un tesoro in questa stanza?(1. Sì | 2. No)\n");
            printf("scelta: ");
            if(scanf("%hu", &scelta)!= 1){
            while(getchar() != '\n');
            setColor(RED);
            printf("Dovresti inserire un numero!\n");
            resetColor();
            continue;
         }
         
            printf("**-----------------------------------**\n");


            if(scelta == 1){
                prendi_tesoro(giocatore);
                giocatore->posizione = stanza;
                printf("Sei tornato nella stanza precedente\n");
                free(segreta);
                break;
            } else if(scelta == 2){
                giocatore->posizione = stanza;
                printf("Sei tornato nella stanza precedente!\n");
                free(segreta);
                break;
            } else {
                printf("Inserire un valore compreso tra 1 e 2\n");
            }
        } while(1);

    } else {
        printf("Non hai trovato nulla!\n");
        
        if(tentativi_rima >=2){
            printf("Restano %d tentativi.\n", tentativi_rima);

         } else if(tentativi_rima == 1){
            printf("Resta solo 1 tentativo!\n");

        }    
         else{
            printf("Sono finiti i tentativi!\n");
        }
    }

    giocatore->azione = true;
}




static void passa() {
    turno_attuale++;
    system("clear");
    turno();
}



static void ins_stanza() {
    unsigned short scelta = 0;

    Stanza* nuova_stanza = (Stanza*)malloc(sizeof(Stanza));
    if (nuova_stanza == NULL) {
        printf("Memoria non allocata\n");
        return;
    }

    // Inizializza i puntatori a NULL
    nuova_stanza->stanza_sopra = NULL;
    nuova_stanza->stanza_sotto = NULL;
    nuova_stanza->stanza_destra = NULL;
    nuova_stanza->stanza_sinistra = NULL;
    nuova_stanza->next = NULL;
    nuova_stanza->tipo_stanza = rand() % 10;
    nuova_stanza->riferimento_stanza = numero_stanze;

    nuova_stanza->nemicoVivo = false;
    nuova_stanza->nemico = NULL;
    nuova_stanza->tipoNemico = nessuno;


    // Se è la prima stanza, la creiamo direttamente
    if (pFirst == NULL) {
        pFirst = nuova_stanza;
        pLast = nuova_stanza;
        numero_stanze = 1;
        setColor(GREEN);
        printf("Prima stanza creata!\n");
        resetColor();
        return;
    }

    bool direzione_valida = false;

    do {
        printf("\nScegli la direzione per inserire la nuova stanza\n");
        setColor(GREEN);
        printf("1 . Sopra\n");
        resetColor();
        setColor(YELLOW);
        printf("2 . Destra\n");
        resetColor();
        setColor(CYAN);
        printf("3 . Sinistra\n");
        resetColor();
        setColor(RED);
        printf("4 . Sotto\n");
        resetColor();
        printf("Scelta: ");
        scanf("%hu", &scelta);
        getchar();
        printf("\n**-----------------------------------**\n");

        // Controlla se la posizione è valida
        if (posizione_valida(pLast, scelta)) {
            direzione_valida = true;
        } else {
            setColor(RED);
            printf("Direzione già occupata o numero non valido! Riprova.\n");
            resetColor();
        }
    } while (!direzione_valida);

    // Collegamento della nuova stanza
    switch (scelta) {
        case 1: // sopra
            nuova_stanza->stanza_sotto = pLast;
            pLast->stanza_sopra = nuova_stanza;
            break;

        case 2: // destra
            nuova_stanza->stanza_sinistra = pLast;
            pLast->stanza_destra = nuova_stanza;
            break;

        case 3: // sinistra
            nuova_stanza->stanza_destra = pLast;
            pLast->stanza_sinistra = nuova_stanza;
            break;

        case 4: // sotto
            nuova_stanza->stanza_sopra = pLast;
            pLast->stanza_sotto = nuova_stanza;
            break;
    }

    // Aggiorna l'ultima stanza
    pLast->next = nuova_stanza;
    pLast = nuova_stanza;
    numero_stanze++;

    int probabilita = rand() % 100;
    if(probabilita < 65){
        nuova_stanza->tipo_trabocchetto = nessuno;
    } else if(probabilita < 75){
        nuova_stanza->tipo_trabocchetto = tegola;
    } else if(probabilita < 84){
        nuova_stanza->tipo_trabocchetto = lame;
    } else if(probabilita < 92){
        nuova_stanza->tipo_trabocchetto = caduta;
    } else{
        nuova_stanza->tipo_trabocchetto = burrone;
    }

    int probTesoro = rand () % 100;
    if(probTesoro < 20){
        nuova_stanza->tipo_tesoro = nessuno;
    } else if(probTesoro < 40){
        nuova_stanza->tipo_tesoro = verde_veleno;
    } else if(probTesoro < 60 ){
        nuova_stanza->tipo_tesoro = blu_guarigione;
    } else if(probTesoro < 75){
        nuova_stanza->tipo_tesoro = rosso_aumenta_vita;
    } else if(probTesoro < 90){
        nuova_stanza->tipo_tesoro = spada_tagliente;
    } else{
        nuova_stanza->tipo_tesoro = scudo;
    }


    nuova_stanza->nemicoVivo = false;
    nuova_stanza->nemico = NULL;
    nuova_stanza->tipoNemico = nessuno;

    if(numero_stanze >= 15){
        Stanza* tmp = pFirst;
        while(tmp != NULL){
            if(tmp->tipoNemico == Jaffar){
                tmp->tipoNemico = nessuno;
                tmp->nemicoVivo = false;
                if(tmp->nemico != NULL){
                free(tmp->nemico);
                }
                tmp->nemico = NULL;
            }
            tmp = tmp->next;
        }

        nuova_stanza->nemicoVivo = true;
        nuova_stanza->tipoNemico = Jaffar;
        nuova_stanza->nemico = inizializza_nemico(Jaffar);
        isJaffar = true;
    } else{
        int probNemico = rand() % 100;
    if(probNemico < 25){
        nuova_stanza->nemicoVivo = true;
        nuova_stanza->tipoNemico = (rand() % 100 < 60) ? schelettro : guardia;
        nuova_stanza->nemico = inizializza_nemico(nuova_stanza->tipoNemico);
    }

    }

 

    setColor(GREEN);
    printf("Nuova stanza connessa con successo!\n");
    resetColor();
}




static bool posizione_valida(Stanza* riferimento, unsigned short direzione) {
    if (riferimento == NULL) return false;
    

    switch (direzione) {
        case 1: return (riferimento->stanza_sopra == NULL);
        case 2: return (riferimento->stanza_destra == NULL);
        case 3: return (riferimento->stanza_sinistra == NULL);
        case 4: return (riferimento->stanza_sotto == NULL);
    }
    return false;
}

// cancella l'ultima zona inserita nella mappa
static void canc_stanza() {
    if (pLast == NULL) {
        setColor(RED);
        printf("Nessuna stanza da cancellare.\n");
        resetColor();
        return;
    }

   //controlla se sto cancellando Jaffar
    bool cancella_jaffar = (pLast->nemicoVivo && pLast->tipoNemico == Jaffar);

    if(pLast->nemicoVivo && pLast->nemico != NULL){
        free(pLast->nemico);
        pLast->nemico = NULL;
    }


    // Trova la penultima stanza
    Stanza* penultima = pFirst;
    while (penultima != NULL && penultima->next != pLast) {
        penultima = penultima->next;
    }

    // Aggiorna i puntatori delle stanze adiacenti
    if (pLast->stanza_sopra != NULL) {
        pLast->stanza_sopra->stanza_sotto = NULL;
    }
    if (pLast->stanza_sotto != NULL) {
        pLast->stanza_sotto->stanza_sopra = NULL;
    }
    if (pLast->stanza_sinistra != NULL) {
        pLast->stanza_sinistra->stanza_destra = NULL;
    }
    if (pLast->stanza_destra != NULL) {
        pLast->stanza_destra->stanza_sinistra = NULL;
    }

    // Libera l'ultima stanza
    free(pLast);
    pLast = penultima; // Aggiorna pLast alla penultima stanza

    // Se pLast è NULL, significa che non ci sono più stanze
    if (pLast == NULL) {
        pFirst = NULL; // Resetta anche pFirst
        isJaffar = false;
    } else {
        pLast->next = NULL; // Rimuovi il collegamento all'ultima stanza
    }

    if(cancella_jaffar){
        if(pLast != NULL){

            if(pLast->nemico != NULL){
            free(pLast->nemico);
            }
          pLast->nemicoVivo = true;
          pLast->tipoNemico = Jaffar;
          pLast->nemico = inizializza_nemico(Jaffar);
          isJaffar = true;
            
        } else{

            isJaffar = false;
        }
    }
    

    numero_stanze--; // Decrementa il numero di stanze
    setColor(GREEN);
    printf("L'ultima stanza cancellata con successo\n");
    resetColor();
}
 
    
static void chiudi_mappa() {
        if (pFirst == NULL) {
            setColor(RED);
            printf("Errore: la mappa non è stata creata.\n");
            resetColor();
            return;
        }
        if (numero_stanze < 15) {
            setColor(RED);
            printf("Non ci sono stanze abbastanza. Dovresti creare almeno 15 stanze!\n");
            resetColor();
            printf("Premere invio per continuare\n");
            svuota_buffer();
            return;
        } else {
            setColor(BLUE);
            printf("Numero di stanze create: %d\n", numero_stanze);
            resetColor();
            setColor(GREEN);
            printf("Mappa chiusa con successo\n");
            resetColor();
        }
        printf("Premere invio per continuare\n");
        while ((c = getchar()) != '\n' && c != EOF);
    }

static void genera_random() {

     Stanza* Temp = pFirst;
     while(Temp != NULL){
         Stanza* next = Temp->next;
         if(Temp->stanza_sopra != NULL){
            Temp->stanza_sopra->stanza_sotto = NULL;
         } if(Temp->stanza_sinistra != NULL){
            Temp->stanza_sinistra->stanza_destra = NULL;
         } if(Temp->stanza_destra != NULL){
            Temp->stanza_destra->stanza_sinistra = NULL;
         } if(Temp->stanza_sotto != NULL){
            Temp->stanza_sotto->stanza_sopra = NULL;
         }

         free(Temp);
         Temp = next;
         
     }

     pFirst = NULL;
     pLast = NULL;
     numero_stanze = 0;

    
    
        for (int i = 0; i < 15; i++) {
            Stanza* nuova_stanza = (Stanza*)malloc(sizeof(Stanza));
            if (nuova_stanza == NULL) {
                printf("Memoria non allocata\n");
                return;
            }
    
            nuova_stanza->stanza_sopra = NULL;
            nuova_stanza->stanza_destra = NULL;
            nuova_stanza->stanza_sinistra = NULL;
            nuova_stanza->stanza_sotto = NULL;
            nuova_stanza->next = NULL;
            nuova_stanza->riferimento_stanza = numero_stanze;

            nuova_stanza->nemicoVivo = false;
            nuova_stanza->tipoNemico = nessuno;
            nuova_stanza->nemico = NULL;
    
            nuova_stanza->tipo_stanza = rand() % 10;
            
            if (pFirst == NULL) {
                // Prima stanza creata
                pFirst = nuova_stanza;
                pLast = nuova_stanza;
                numero_stanze = 1;
                continue;
            }
    
            bool direzione_valida = 0;
            unsigned short direzione;
    
            while(!direzione_valida){
    
                direzione = rand() % 4 + 1;
    
            if(posizione_valida(pLast, direzione)){
                direzione_valida = true;
            
            }
        }
    
            switch(direzione){
                
                case 1:
                nuova_stanza->stanza_sotto = pLast;
                pLast->stanza_sopra = nuova_stanza;
                
                break;
    
                case 2:
                nuova_stanza->stanza_sinistra = pLast;
                pLast->stanza_destra = nuova_stanza;
                
                break;
    
                case 3:
                nuova_stanza->stanza_destra = pLast;
                pLast->stanza_sinistra = nuova_stanza;
                
                break;
    
                case 4: 
                nuova_stanza->stanza_sopra = pLast;
                pLast->stanza_sotto = nuova_stanza;
                
                break;
    
            }
            pLast->next = nuova_stanza; // collega l'ultima stanza alla nuova
            pLast = nuova_stanza;
            numero_stanze++;
           
            // genera di forma casuale il tipo di traboccheto con il percentuale
            int probabilita = rand() % 100;

           if (probabilita < 65) {
              nuova_stanza->tipo_trabocchetto = nessuno;
            } else if (probabilita < 75) { // da 65 a 74 (10 numeri) → tegola
             nuova_stanza->tipo_trabocchetto = tegola;
         } else if (probabilita < 84) { // da 75 a 83 (9 numeri) → lame
           nuova_stanza->tipo_trabocchetto = lame;
          } else if (probabilita < 92) { // da 84 a 91 (8 numeri) → caduta
             nuova_stanza->tipo_trabocchetto = caduta;
            } else { // da 92 a 99 (8 numeri) → burrone
              nuova_stanza->tipo_trabocchetto = burrone;
}

          int probabilitaTesoro = rand() % 100;
          if(probabilitaTesoro < 20){
            nuova_stanza->tipo_tesoro = nessun_tesoro;
          } else if(probabilitaTesoro < 40){
            nuova_stanza->tipo_tesoro = verde_veleno;
          } else if(probabilitaTesoro < 60){
            nuova_stanza->tipo_tesoro = blu_guarigione;
          } else if(probabilitaTesoro < 75){
            nuova_stanza->tipo_tesoro = rosso_aumenta_vita;
          } else if(probabilitaTesoro < 90){
            nuova_stanza->tipo_tesoro = spada_tagliente;
          } else{
              nuova_stanza->tipo_tesoro =scudo;
          }
    
            nuova_stanza->nemicoVivo = (rand() % 100 < 25);
                if(nuova_stanza->nemicoVivo) {
                    // 60% scheletro, 40% guardia
                    nuova_stanza->tipoNemico = (rand() % 100 < 60) ? schelettro : guardia;
                    nuova_stanza->nemico = (inizializza_nemico(nuova_stanza->tipoNemico));
                }
    
    
     } // se ci sono almeno 15 stanze jaffar starà nell'ultima
        if(numero_stanze >= 15){
            if(isJaffar){
             Stanza* tmp = pFirst;
             while(tmp != NULL){
                if(tmp->tipoNemico == Jaffar && tmp->nemicoVivo){
                    tmp->nemicoVivo = false;
                    break;
                }
                tmp = tmp->next;
             }

            }
            pLast->nemicoVivo = true;
            pLast->tipoNemico = Jaffar;
            pLast->nemico = inizializza_nemico(Jaffar);
            isJaffar = true;
        }
     
            setColor(GREEN);
            printf("Mappa con %d zone create!\n", numero_stanze); 
            resetColor();
            
            
        }

        


static const char* getNomeClasse(enum classe_giocatore classe){
   switch(classe){
     case principe: return "principe";
     case doppleganger: return "doppleganger";
     default: return "sconosciuto";
   }

}

static const char* getNome_stanza(enum Tipo_stanza tipo) {
    switch (tipo) {
        case corridoio: return "corridoio";
        case scala: return "scala";
        case sala_banchetto: return "sala banchetto";
        case magazzino: return "magazzino";
        case posto_guardia: return "posto guardia";
        case prigione: return "prigione";
        case armeria: return "armeria";
        case mosche: return "mosche";
        case torre: return "torre";
        case bagno: return "bagno";
        default: return "sconosciuto";
    }
}

static void stampa_stanze() {
    if (pFirst == NULL) {
        setColor(RED);
        printf("Nessuna stanza creata.\n");
        resetColor();
        return;
    }

    Stanza* corrente = pFirst;
    int numero_stanza = 0;

    while (corrente != NULL) {
        printf("\n");

        
        setColor(PURPLE); 
        printf("Stanza %d: %p - ", numero_stanza++, (void*)corrente);

        
        setColor(YELLOW); 
        printf("%s", getNome_stanza(corrente->tipo_stanza));

        
        setColor(GREEN); 
        printf(" | tesoro: %s", getNomeTesoro(corrente->tipo_tesoro));

        
        setColor(RED); 
        printf(" | trabocchetto: %s\n", getNomeTrabocchetto(corrente->tipo_trabocchetto));

        resetColor();

        //Controlla e stampa solo le stanze collegate
        if (corrente->stanza_sinistra) {
            setColor(BLUE); 
            printf("  Stanza a sinistra: %p\n", (void*)corrente->stanza_sinistra);
            resetColor();
        }
        if (corrente->stanza_destra) {
            setColor(BLUE); 
            printf("  Stanza a destra: %p\n", (void*)corrente->stanza_destra);
            resetColor();
        }
        if (corrente->stanza_sopra) {
            setColor(BLUE); 
            printf("  Stanza sopra: %p\n", (void*)corrente->stanza_sopra);
            resetColor();
        }
        if (corrente->stanza_sotto) {
            setColor(BLUE); 
            printf("  Stanza sotto: %p\n", (void*)corrente->stanza_sotto);
            resetColor();
        }

        corrente = corrente->next;
    }
}




static char* getNomeTrabocchetto(enum Tipo_trabocchetto pTrabocchetto){
    switch(pTrabocchetto){
        case nessuno: return "//";
        case tegola: return "tegola";    
        case lame: return "lame";
        case caduta: return "caduta";
        case burrone: return "burrone";
        default: return "???";
    
    }
}

static char* getNomeTesoro(enum Tipo_tesoro pTesoro){
    switch(pTesoro){
        case nessun_tesoro: return "nessun tesoro";
        case verde_veleno: return "verde veleno";
        case blu_guarigione: return "blu guarigione";
        case rosso_aumenta_vita: return "rosso aumenta vita";
        case spada_tagliente: return "spada tagliente";
        case scudo: return "scudo";

        default: return "||";
    }
}


static Nemico* inizializza_nemico(enum Tipo_nemico tipo){

    Nemico* nemico = malloc(sizeof(Nemico));

    nemico->tipo_nemico = tipo;

     
    switch(tipo){

       case schelettro:
       nemico->vitaNemico = 1;
       nemico->dadiAtacco = 1;
       nemico->dadiDifesa = 1;
       break;

       case guardia:
       nemico->vitaNemico = 2;
       nemico->dadiAtacco = 2;
       nemico->dadiDifesa = 2;
       break;

       case Jaffar:
       nemico->vitaNemico = 3;
       nemico->dadiAtacco = 3;
       nemico->dadiDifesa = 2;

    }

    return nemico;


}



static char* getNomeNemico(enum Tipo_nemico tipo){
    switch(tipo){
        case schelettro : return "schelettro";
        break;
        case guardia: return "guardia";
        break;
        case Jaffar: return "Jaffar";
        break;
        default: return "Sconociuto";
        break;
    }
}


void setColor(enum colors color) {
    switch (color) {
    case 0:
        printf("\033[;30m");
        break;
    case 1:
        printf("\033[;31m");
        break;
    case 2:
        printf("\033[;32m");
        break;
    case 3:
        printf("\033[;33m");
        break;
    case 4:
        printf("\033[;34m");
        break;
    case 5:
        printf("\033[;35m");
        break;
    case 6:
        printf("\033[;36m");
        break;
    case 7:
        printf("\033[;37m");
        break;
    default:
        break;
    }
}
   
void resetColor(){
    printf("\033[0m");
}

static void deallocazione_memoria() {

    //dealloca in modo ricorsivo
    Stanza* curr = pFirst;
    while(curr != NULL){
       Stanza* next = curr->next; // salva il riferimento alla prossima stanza
       free(curr);
       curr = next;

    }

    pFirst = NULL;
    pLast = NULL;
    numero_stanze = 0;

    //Dealloca i giocatori
    for (int i = 0; i < 3; i++) {
        if (giocatori[i] != NULL) {
            
            free(giocatori[i]);
            giocatori[i] = NULL;
        }
    }

   
}


void termina_gioco() {
    printf("Sei sicuro che vuoi terminare il gioco?\n");
    printf("1. Sì | 2. No\n");
    unsigned short scelta = 0;

    do{
        printf("Scelta:");
    


        if(scanf("%hu", &scelta)!= 1){
            while(getchar() != '\n');
            setColor(RED);
            printf("Dovresti inserire un numero!\n");
            resetColor();
            continue;
         }

        if(scelta < 1 || scelta > 2){
            puts("Inserire un valore compresso tra 1 e 2\n");
        }

        if(scelta == 1){
            puts("Grazie per aver giocato! Il gioco è finito. ");
            puts("Alla prossima avventura!");
            puts("Premi INVIO per chiudere il gioco.\n");
            svuota_buffer();
            exit(0);
        } if(scelta == 2){
            return;
           
        }

        
    } while( scelta < 1 || scelta > 2);

     

}

void crediti() {
    printf("\n**-----------------------------------**\n");

    if(!primo_vincitore && !secondo_vincitore && !terzo_vincitore){
        setColor(RED);
        printf("Ancora non ci sono vincitori!\n");
        resetColor();
    } else {
        setColor(GREEN);
        printf("Classifica:\n");
        if(primo_vincitore){
            printf("Primo posto: %s | ", primo_vincitore->nome);
        }
        if(secondo_vincitore){
            printf("Secondo posto: %s | ", secondo_vincitore->nome);

        }
        if(terzo_vincitore){
            printf("Terzo posto: %s", terzo_vincitore->nome);
        }
        resetColor();

    }
    setColor(CYAN);
    printf("\n* CREDITI *\n");
    setColor(PURPLE);
    printf("* Sviluppatore: Ruan \n");
    printf("* Grazie per aver giocato a Prince of Inertia!\n");
    resetColor();
    printf("\n**-----------------------------------**\n");
   
   printf("\nPremi Invio per tornare al menu principale\n");
   svuota_buffer();

}

static void Vincitori(Giocatore* giocatore){
    if(!primo_vincitore){
        primo_vincitore = giocatore;
        printf("**-----------------------------------**\n");
        printf("Primo vincitore: %s\n", giocatore->nome);
    } else if(!secondo_vincitore && giocatore != primo_vincitore){
        secondo_vincitore = giocatore;
        printf("**-----------------------------------**\n");
        printf("Secondo vincitore: %s\n", giocatore->nome);
    } else if(!terzo_vincitore && primo_vincitore && giocatore != secondo_vincitore){
        terzo_vincitore = giocatore;
        printf("**-----------------------------------**\n");
        printf("Terzo vincitore : %s\n", giocatore->nome);
    }

}


static void svuota_buffer(){
    while((c = getchar() != '\n' && getchar() != EOF));   
}


// stampa le informazione se nelle prossime due stanze ci sono nemici
static void informazioni_nemici(Giocatore* giocatore) {

    
    if (giocatore == NULL || giocatore->posizione == NULL) {
        return;
    }

   
    Stanza* prima_info = giocatore->posizione->next;

    if (prima_info != NULL) {

        if (prima_info->nemicoVivo) {
            printf("C'è %s nella zona successiva!\n",
                   getNomeNemico(prima_info->tipoNemico));
        } else {
            setColor(CYAN);
            printf("Nessun nemico nella zona successiva!\n");
            resetColor();
        }

    } else {
        printf("Non esiste una stanza successiva.\n");
        return;  
    }

    
    Stanza* seconda_info = prima_info->next;

    if (seconda_info != NULL) {

        if (seconda_info->nemicoVivo) {
            printf("C'è %s fra due stanze!\n",
                   getNomeNemico(seconda_info->tipoNemico));
        } else {
            setColor(CYAN);
            printf("Nessun nemico fra due stanze!\n");
            resetColor();
        }

    } else {
        printf("Non esiste una seconda stanza.\n");
    }
}


 

   
   