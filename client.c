//--------klient---------
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <termios.h> 
#include "structures.ch"

//Zamiennik funkcji getch z systemu windows
char getch(){
    char buf=0;
    struct termios old={0};
    fflush(stdout);
    if(tcgetattr(0, &old)<0)
        perror("tcsetattr()");
    old.c_lflag&=~ICANON;
    old.c_lflag&=~ECHO;
    old.c_cc[VMIN]=1;
    old.c_cc[VTIME]=0;
    if(tcsetattr(0, TCSANOW, &old)<0)
        perror("tcsetattr ICANON");
    if(read(0,&buf,1)<0)
        perror("read()");
    old.c_lflag|=ICANON;
    old.c_lflag|=ECHO;
    if(tcsetattr(0, TCSADRAIN, &old)<0)
        perror ("tcsetattr ~ICANON");
    printf("%c\n",buf);
    return buf;
}

void poczekaj() {
    printf("\n------------Aby przejsc dalej wcisnij ESC!------------\n");
    char a = getch();
    while (a != '\x1B') {
        if (a == '\x1B') return;
        a = getch();
    }}
    
    
    
    int main(){
        int id;
        id=msgget(2017013,0644|IPC_CREAT);
        if(id==-1)
            printf("Blad: nie udalo sie utworzyc kolejki komunikatow!\n");
        else
            printf("Polaczono z serwerem..\n");
        
        wiadomosc wyslij,odbierz;
        char nick[20];
        
        
        
        int wybor=1;
        int idklienta=0;
        int zalogowany=0;
        while(wybor){
            system("clear");
            printf(
                "- - - - - - - - - - - - - - - - - - - - -\n"
                "------------ P o l i C H A T ------------\n"
                "- - - - - - - - - - - - - - - - - - - - -\n"
                "Co chcesz dalej zrobic?\n"
                "1.Zaloguj sie\n"
                "2.Pokaz zalogowanych\n"
                "3.Pokaz liste grup dyskusyjnych\n"
                "4.Pokaz liste uzytkownikow wybranej grupy\n"
                "5.Zapisz sie do wybranej grupy\n"
                "6.Wypisz sie z grupy\n"
                "7.Wyslij wiadomosc do grupy\n"
                "8.Wyslij wiadomosc do uzytkownika\n"
                "9.Odczytaj swoje wiadomosci\n"
                "10.Wyloguj sie\n"
                "11.Wylacz chat\n");
            while(!zalogowany){
                scanf("%d",&wybor);
                if(wybor!=1) printf("Brak dostepu! Zaloguj sie..\n");
                else break;
            }
            while(zalogowany){
                scanf("%d",&wybor);
                if(wybor==1) printf("Jestes juz zalogowany..\n");
                else break;
            }
            switch(wybor){
                case 1:{
                    printf("Wpisz swoj nick: ");
                    while(1){
                        scanf("%s",nick);
                        wyslij.typ=1;
                        strcpy(wyslij.tresc,nick);
                        wyslij.operacja=0;
                        wyslij.iduzytkownika=getpid();
                        
                        msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                        
                        msgrcv(id,&odbierz,sizeof(odbierz)-sizeof(long),getpid(),0);
                        
                        
                        if(odbierz.wynik==1) break;
                        else if (odbierz.wynik==0)printf("Blad: brak uzytkownika o podanym nicku!\nWpisz swoj nick: ");
                    }
                    wyslij.typ=1;
                    wyslij.operacja=1;
                    strcpy(wyslij.tresc,nick);
                    wyslij.iduzytkownika=getpid();
                    msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                    msgrcv(id,&odbierz,sizeof(odbierz)-sizeof(long),getpid(),0);
                    if(odbierz.wynik!=-1){
                        idklienta=odbierz.wynik;
                        printf("Witaj %s, zalogowano poprawnie, twoje id: %d\n",nick,idklienta);
                        zalogowany=1;
                    }
                    else{
                        printf("Uzytkownik %s jest juz zalogowany!\n",nick);
                    }
                    
                    break;
                }
                case 2:{
                    wyslij.typ=1;
                    wyslij.operacja=2;
                    wyslij.iduzytkownika=idklienta;
                    msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                    msgrcv(id,&odbierz,sizeof(odbierz)-sizeof(long),idklienta,0);
                    printf("Zalogowani: %s\n",odbierz.tresc);
                    break;}
                case 3:{
                    wyslij.typ=1;
                    wyslij.operacja=3;
                    wyslij.iduzytkownika=idklienta;
                    msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                    msgrcv(id,&odbierz,sizeof(odbierz)-sizeof(long),idklienta,0);
                    printf("Grupy: %s\n",odbierz.tresc);
                    break;}
                case 4:{
                    char nazwagrupy[20];
                    printf("Wprowadz nazwe grupy, ktorej uzytkownikow chcesz poznac: ");
                    scanf("%s",nazwagrupy);
                    wyslij.typ=1;
                    wyslij.operacja=4;
                    wyslij.iduzytkownika=idklienta;
                    strcpy(wyslij.tresc,nazwagrupy);
                    msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                    msgrcv(id,&odbierz,sizeof(odbierz)-sizeof(long),idklienta,0);
                    printf("%s\n",odbierz.tresc);
                    break;}	
                case 5:{
                    char nazwagrupy[20];
                    printf("Wprowadz nazwe grupy, do ktorej chcesz sie zapisac: ");
                    scanf("%s",nazwagrupy);
                    wyslij.typ=1;
                    wyslij.operacja=5;
                    wyslij.iduzytkownika=idklienta;
                    strcpy(wyslij.nick,nick);
                    strcpy(wyslij.tresc,nazwagrupy);
                    msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                    msgrcv(id,&odbierz,sizeof(odbierz)-sizeof(long),idklienta,0);
                    printf("Info: %s\n",odbierz.tresc);
                    break;}
                case 6:{
                    char nazwagrupy[20];
                    printf("Wprowadz nazwe grupy, z ktorej chcesz sie wypisac: ");
                    scanf("%s",nazwagrupy);
                    wyslij.typ=1;
                    wyslij.operacja=6;
                    wyslij.iduzytkownika=idklienta;
                    strcpy(wyslij.nick,nick);
                    strcpy(wyslij.tresc,nazwagrupy);
                    msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                    msgrcv(id,&odbierz,sizeof(odbierz)-sizeof(long),idklienta,0);
                    printf("Info: %s\n",odbierz.tresc);
                    break;}
                case 7:{
                    char odbiorca[20];
                    char tresc[255];
                    printf("Podaj nazwe grupy odbiorczej: ");
                    scanf("%s",odbiorca);
                    printf("Wpisz tresc wiadomosci [enter konczy wiadomosc]: ");
                    scanf("%s",tresc);
                    wyslij.typ=1;
                    strcpy(wyslij.tresc,tresc);
                    strcpy(wyslij.nick,nick);
                    wyslij.operacja=7;
                    wyslij.iduzytkownika=idklienta;
                    strcpy(wyslij.odbiorca,odbiorca);
                    msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                    msgrcv(id,&odbierz,sizeof(odbierz)-sizeof(long),idklienta,0);
                    if(odbierz.wynik==1)
                        printf("Wiadomosc wyslano\n");
                    else if(odbierz.wynik==0)
                        printf("Blad wyslania: grupa jest pusta!\n");
                    else
                        printf("Blad wyslania: grupa nie istnieje!\n");
                    break;}
                case 8:{
                    char odbiorca[20];
                    char tresc[255];
                    printf("Podaj nick odbiorcy: ");
                    scanf("%s",odbiorca);
                    printf("Wpisz tresc wiadomosci [enter konczy wiadomosc]: ");
                    scanf("%s",tresc);
                    wyslij.typ=1;
                    strcpy(wyslij.tresc,tresc);
                    strcpy(wyslij.nick,nick);
                    wyslij.operacja=8;
                    wyslij.iduzytkownika=idklienta;
                    strcpy(wyslij.odbiorca,odbiorca);
                    msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                    msgrcv(id,&odbierz,sizeof(odbierz)-sizeof(long),idklienta,0);
                    if(odbierz.wynik!=-1)
                        printf("Wiadomosc wyslano\n");
                    else
                        printf("Blad wyslania: uzytkownik nie istnieje!\n");
                    break;}
                case 9:{
                    wyslij.typ=1;
                    wyslij.operacja=9;
                    wyslij.iduzytkownika=idklienta;
                    msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                    msgrcv(id,&odbierz,sizeof(odbierz)-sizeof(long),idklienta,0);
                    int liczbawiadomosci;
                    liczbawiadomosci=odbierz.wynik;
                    printf(
                        "Twoje wiadomosci\n"
                        "Liczba wiadomosci: %d\n"
                        "* * * * * * * * * * * * * * * *\n",liczbawiadomosci);
                    while(liczbawiadomosci){
                        msgrcv(id,&odbierz,sizeof(odbierz)-sizeof(long),(idklienta+1)*8,0);
                        printf("Nadawca: %s\n",odbierz.nick);
                        printf("Tresc: %s\n",odbierz.tresc);
                        printf("Data: %s\n",odbierz.data);
                        printf("----------------------------\n");
                        liczbawiadomosci--;}
                        break;}
                case 10:{
                    wyslij.typ=1;
                    wyslij.operacja=10;
                    strcpy(wyslij.tresc,nick);
                    wyslij.iduzytkownika=idklienta;
                    msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                    msgrcv(id,&odbierz,sizeof(odbierz)-sizeof(long),idklienta,0);
                    printf("Wylogowano poprawnie..\n");
                    zalogowany=0;
                    break;
                }
                case 11:{
                    printf("Wylaczam serwer\n");
                    wyslij.operacja=11;
                    wyslij.typ=1;
                    msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                    printf("Wylaczam program klienta..\nDo widzenia %s\n\n",nick);
                    sleep(3);
                    return 0;
                    break;}
                default:{break;}
                
            }
            
            poczekaj();  
        }
        return 0;
    }
    
    
