#ifndef structure
#define structure
typedef struct uzytkownik{
    char nick[20];
    int id;
    int zalogowany;
    int liczbawiadomosci;
} uzytkownik;

typedef struct grupa {
    char nazwa[20];
    int nridzapisanych[20];
} grupa;


typedef struct wiadomosc{
    long typ;
    int iduzytkownika;
    char nick[20];
    char tresc[255];
    char data[15];
    int operacja;
    int wynik;
    char odbiorca[20];
} wiadomosc;
#endif
