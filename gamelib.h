/// // Dichiarazione delle funzioni da chiamare in main.c
// Definizione dei tipi necessari alla libreria
#include<stdbool.h>

void logo();
void imposta_gioco();
void gioca();
void termina_gioco();
void crediti();





enum classe_giocatore{
    principe = 1,
    doppleganger = 2,
};


enum Tipo_stanza{
    corridoio,
    scala, 
    sala_banchetto,        
    magazzino,
    posto_guardia,
    prigione,
    armeria,
    mosche,
    torre, 
    bagno,
};

enum Tipo_trabocchetto{
    nessuno, 
    tegola,
    lame,
    caduta,
    burrone,
};

enum Tipo_tesoro{
    nessun_tesoro,
    verde_veleno,
    blu_guarigione, 
    rosso_aumenta_vita,
    spada_tagliente,
    scudo,


};

enum Tipo_nemico{
    schelettro, guardia, Jaffar,
};

struct Giocatore
{
    char nome[30];
    enum classe_giocatore classe;
    struct Stanza *posizione;
    struct Stanza *stanzaPrecedente;
    int fughe;// nella funzione scappa
    bool azione; 
    bool primoTrabocchetto;// ignora il primo trabocchetto se e' un principe
    bool vincitore;
    unsigned char p_vita_max;
    unsigned char p_vita;
    unsigned char dadi_attaco;
    unsigned char dadi_difesa;

};

struct Stanza{
    struct Stanza* stanza_destra;
    struct Stanza* stanza_sinistra;
    struct Stanza* stanza_sopra;
    struct Stanza* stanza_sotto;
    struct Stanza* next;
    int riferimento_stanza; // il numero della stanza quando si stampa le sue informazioni
    bool tesoroPreso;
    bool trabocchettoAttivato;
    enum Tipo_stanza tipo_stanza;
    enum Tipo_trabocchetto tipo_trabocchetto;
    enum Tipo_tesoro tipo_tesoro;
    enum Tipo_nemico tipoNemico;
     struct Nemico* nemico;
    bool nemicoVivo;
    bool primo_tentativo;
    bool secondo_tentativo;
    bool terzo_tentativo;
    bool isGiocatore;

};

struct Nemico{
    enum Tipo_nemico tipo_nemico;
    unsigned char vitaNemico;
    unsigned char dadiAtacco;
    unsigned char dadiDifesa;

};

enum colors{BLACK, RED, GREEN, YELLOW, BLUE, PURPLE, CYAN, WHITE};
void setColor(enum colors color);
void resetColor(); 
// Definizione dei tipi necessari alla libreria

