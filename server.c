//--------server---------
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include "structures.ch"



int main(){
    printf("Uruchamiam serwer chatu..\n");
    FILE *plik=fopen("plikkonfiguracyjny.txt","r");
    size_t dlugosc = 0;
    char *linia = NULL;
    
    uzytkownik tablicauzytkownikow[90];
    grupa tablicagrup[30];
    int nrgrupy=0;
    int nruzytkownika=1;
    int przeczytane = 0;
    
    if(plik==NULL)
        printf("Blad: nie udalo sie wczytac pliku konfiguracyjnego!\n");
    else
        printf("Wczytuje plik konfiguracyjny..\n");
    
    while((przeczytane = getline(&linia,&dlugosc,plik)) != -1)
    {   
        if (linia[przeczytane - 1] == '\n') {
            linia[--przeczytane] = '\0';
        }
        
        if(linia[0]=='g'){
            strcpy(tablicagrup[nrgrupy].nazwa,linia);
            nrgrupy++;
        }
        else if(linia[0]=='u'){
            strcpy(tablicauzytkownikow[nruzytkownika].nick,linia);
            tablicauzytkownikow[nruzytkownika].id=nruzytkownika;
            tablicauzytkownikow[nruzytkownika].zalogowany=0;
            nruzytkownika++;
        }
        
    }
    printf("Wczytano %d grupy i %d uzytkownikow do bazy..\n",nrgrupy,nruzytkownika-1);
    fclose(plik);
    
    int id;
    id=msgget(2017013,0644|IPC_CREAT);
    if(id==-1)
        printf("Blad: nie udalo sie utworzyc kolejki komunikatow!\n");
    else
        printf("Kolejka komunikatow pracuje..\n");
    wiadomosc odbierz,wyslij;
    int kolejka;
    while(1){
        printf("Czekam na operacje................\n");
        kolejka=msgrcv(id,&odbierz,sizeof(odbierz)-sizeof(long),1,0);    
        if(kolejka==-1)
            printf("Blad: niepoprawny odbior komunikatu!\n");
        switch(odbierz.operacja){
            case 0:{
                printf("Sprawdzam czy istnieje uzytkownik: %s\n",odbierz.tresc);
                int i=0;
                int znaleziony=0;
                for(i=1;i<(sizeof tablicauzytkownikow / sizeof *tablicauzytkownikow);i++){                    
                    if(strcmp(tablicauzytkownikow[i].nick,odbierz.tresc)==0)
                    {
                        printf("->uzytkownik: %s istnieje\n",odbierz.tresc);
                        znaleziony=1;
                        wyslij.wynik=1;
                        wyslij.operacja=0;
                        wyslij.typ=(long)odbierz.iduzytkownika;
                        msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                        break;
                    }
                }
                if(!znaleziony) {
                    printf("->uzytkownik: %s nie istnieje\n",odbierz.tresc);
                    wyslij.wynik=0;
                    wyslij.operacja=1;
                    wyslij.typ=(long)odbierz.iduzytkownika;
                    msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                }
                break;
            }
            case 1:{
                
                int i=0;
                for(i=1;i<(sizeof tablicauzytkownikow / sizeof *tablicauzytkownikow);i++){
                    
                    if(strcmp(tablicauzytkownikow[i].nick,odbierz.tresc)==0)
                    {
                        if(tablicauzytkownikow[i].zalogowany!=1){
                            wyslij.wynik=tablicauzytkownikow[i].id;
                            wyslij.typ=(long)odbierz.iduzytkownika;
                            tablicauzytkownikow[i].zalogowany=1;
                            printf("Zalogowal sie uzytkownik: %s\n",odbierz.tresc);
                        }
                        else{
                            printf("Odrzucenie zadania: ponowna proba logowania uzytkownika: %s\n",odbierz.tresc);  
                            wyslij.wynik=-1;
                            wyslij.typ=(long)odbierz.iduzytkownika;
                        }
                        msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                        break;
                    }
                }
                
                break;}
            case 2:{
                printf("Sprawdzam zalogowanych..\n");
                char zalogowani[200]={0};
                int i=0;
                
                for(i=1;i<nruzytkownika;i++){
                    if(tablicauzytkownikow[i].zalogowany==1){     
                        strcat(zalogowani,tablicauzytkownikow[i].nick);
                        strcat(zalogowani," ");
                    }
                }
                wyslij.typ=odbierz.iduzytkownika;
                strcpy(wyslij.tresc,zalogowani);
                msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                break;
            }
            case 3:{
                printf("Sprawdzam aktywne grupy..\n");
                char grupy[200]={0};
                int i=0;
                for(i=0;i<nrgrupy;i++){
                    strcat(grupy,tablicagrup[i].nazwa);
                    strcat(grupy," ");
                }
                wyslij.typ=odbierz.iduzytkownika;
                strcpy(wyslij.tresc,grupy);
                printf("%s",grupy);
                msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                break;
            }
            case 4:{
                printf("Sprawdzam uzytkownikow grupy %s..\n",odbierz.tresc);
                char uzytkownicy[200]={0};
                strcat(uzytkownicy,"Uzytkownicy grupy: ");
                int i=0;
                for(i=0;i<nrgrupy;i++){
                    if(strcmp(odbierz.tresc,tablicagrup[i].nazwa)==0){
                        int j,k;
                        for(j=0;j<20;j++){
                            if(tablicagrup[i].nridzapisanych[j]==0) break;
                            for(k=1;k<(sizeof tablicauzytkownikow / sizeof *tablicauzytkownikow);k++){
                                
                                if(tablicauzytkownikow[k].id==tablicagrup[i].nridzapisanych[j])
                                {        
                                    strcat(uzytkownicy,tablicauzytkownikow[k].nick);
                                    strcat(uzytkownicy," ");
                                    break;
                                    
                                }
                            }
                        }
                        break;
                    }
                }
                strcpy(wyslij.tresc,uzytkownicy);
                if(i==nrgrupy)
                {
                    strcpy(wyslij.tresc,"Grupa nie istnieje");
                    printf("->Odrzucenie zadania: grupa nie istnieje!\n");
                }
                wyslij.typ=odbierz.iduzytkownika;
                
                msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                break;
            }
            case 5:{
                int i=0;
                int bylzapisany=0;
                printf("Odebrano zadanie zapisania uzytkownika %s do grupy %s..\n",odbierz.nick,odbierz.tresc);
                for(i=0;i<nrgrupy;i++){
                    if(strcmp(odbierz.tresc,tablicagrup[i].nazwa)==0){
                        int j;
                        for(j=0;j<20;j++){
                            if(tablicagrup[i].nridzapisanych[j]==odbierz.iduzytkownika){
                                strcpy(wyslij.tresc,"Byles juz zapisany");
                                printf("->Odrzucenie zadania: uzytkownik byl juz zapisany!\n");
                                bylzapisany=1;
                                break;
                            }
                            else if(tablicagrup[i].nridzapisanych[j]==0) break;
                        }
                        if(!bylzapisany){
                            tablicagrup[i].nridzapisanych[j]=odbierz.iduzytkownika;
                            strcpy(wyslij.tresc,"Zapisano do grupy");
                            printf("->Zapisano do grupy\n");
                        }
                        break;
                    }
                }
                if(i==nrgrupy)
                {
                    strcpy(wyslij.tresc,"Grupa nie istnieje");
                    printf("->Odrzucenie zadania: grupa nie istnieje!\n");
                }
                wyslij.typ=odbierz.iduzytkownika;
                msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                break;
            }
            case 6:{
                int i=0;
                int bylzapisany=0;
                
                printf("Odebrano zadanie wypisania uzytkownika %s z grupy %s..\n",odbierz.nick,odbierz.tresc);
                for(i=0;i<nrgrupy;i++){
                    if(strcmp(odbierz.tresc,tablicagrup[i].nazwa)==0){
                        int j;
                        for(j=0;j<20;j++){
                            if(tablicagrup[i].nridzapisanych[j]==odbierz.iduzytkownika){
                                tablicagrup[i].nridzapisanych[j]=0;
                                strcpy(wyslij.tresc,"Wypisano z grupy");
                                printf("->Wypisano z grupy\n");
                                bylzapisany=1;
                                break;
                            }
                            else if(tablicagrup[i].nridzapisanych[j]==0) break;
                        }
                        if(!bylzapisany){ 
                            strcpy(wyslij.tresc,"Nie byles zapisany w tej grupie");
                            printf("->Odrzucenie zadania: uzytkownik nie byl zapisany!\n");
                        }
                        break;
                    }
                }
                if(i==nrgrupy)
                {
                    strcpy(wyslij.tresc,"Grupa nie istnieje");
                    printf("->Odrzucenie zadania: grupa nie istnieje!\n");
                }
                wyslij.typ=odbierz.iduzytkownika;
                msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                break;
            }
            case 7:{
                printf("Odbieram wiadomosc..\n");
                int i;
                int wyslana;
                wyslana=-1;
                time_t     now;
                struct tm *ts;
                char       buf[80];
                now = time(NULL);
                ts = localtime(&now);
                strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ts);
                
                for(i=0;i<nrgrupy;i++){
                    if(strcmp(odbierz.odbiorca,tablicagrup[i].nazwa)==0){
                        wyslana=0;
                        int j,k;
                        for(j=0;j<20;j++){
                            if(tablicagrup[i].nridzapisanych[j]==0) break;
                            for(k=1;k<(sizeof tablicauzytkownikow / sizeof *tablicauzytkownikow);k++){
                                
                                if(tablicauzytkownikow[k].id==tablicagrup[i].nridzapisanych[j])
                                {        
                                    wyslij.typ=(tablicauzytkownikow[k].id+1)*8;
                                    strcpy(wyslij.tresc,odbierz.tresc);
                                    wyslana=1;
                                    strcpy(wyslij.data,buf);
                                    strcpy(wyslij.nick,odbierz.nick);
                                    tablicauzytkownikow[k].liczbawiadomosci++;
                                    msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                                    printf("->Wyslano wiadomosc do %s\n",tablicauzytkownikow[k].nick);
                                    
                                    break;
                                    
                                }
                            }
                        }
                        
                        break;
                    }
                }
                if(wyslana==0){
                    wyslij.wynik=0;        
                    printf("Odrzucenie zadania wyslania wiadomosci: grupa %s jest pusta!\n",odbierz.odbiorca);
                }
                else if(wyslana==-1){
                    wyslij.wynik=-1;
                    printf("Odrzucenie zadania wyslania wiadomosci: grupa %s nie istnieje\n",odbierz.odbiorca);
                }
                else{ 
                    wyslij.wynik=1;
                    printf("Wyslano wiadomosc od %s do %s\n",odbierz.nick,odbierz.odbiorca);
                    
                }
                wyslij.typ=odbierz.iduzytkownika;
                msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                
                
                break;
            }
            case 8:{
                printf("Odbieram wiadomosc..\n");
                int i;
                int wyslana;
                wyslana=0;
                time_t     now;
                struct tm *ts;
                char       buf[80];
                now = time(NULL);
                ts = localtime(&now);
                strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ts);
                for(i=1;i<(sizeof tablicauzytkownikow / sizeof *tablicauzytkownikow);i++){
                    
                    if(strcmp(tablicauzytkownikow[i].nick,odbierz.odbiorca)==0)
                    {        
                        wyslij.typ=(tablicauzytkownikow[i].id+1)*8;
                        strcpy(wyslij.tresc,odbierz.tresc);
                        strcpy(wyslij.data,buf);
                        strcpy(wyslij.nick,odbierz.nick);
                        tablicauzytkownikow[i].liczbawiadomosci++;
                        msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                        wyslana=1;
                        wyslij.typ=odbierz.iduzytkownika;
                        wyslij.wynik=1;
                        msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                        printf("Wyslano wiadomosc od %s do %s\n",odbierz.nick,odbierz.odbiorca);
                        break;
                    }
                }
                if(!wyslana){
                    wyslij.typ=odbierz.iduzytkownika;
                    wyslij.wynik=-1;
                    msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                    printf("Odrzucenie zadania wyslania wiadomosci: uzytkownik %s nie istnieje!\n",odbierz.odbiorca);
                }
                
                break;
            }
            case 9:{
                int i;
                for(i=1;i<(sizeof tablicauzytkownikow / sizeof *tablicauzytkownikow);i++){
                    
                    if(tablicauzytkownikow[i].id==odbierz.iduzytkownika)
                    {        
                        wyslij.typ=(long)odbierz.iduzytkownika;
                        wyslij.wynik=tablicauzytkownikow[i].liczbawiadomosci;
                        tablicauzytkownikow[i].liczbawiadomosci=0;
                        break;
                    }
                }
                msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                printf("Pobrano wiadomosci dla konta uzytkownika %s..\n",tablicauzytkownikow[i].nick);
                
                break;
            }
            case 10:{
                printf("Wylogowal sie uzytkownik: %s\n",odbierz.tresc);
                int i=0;
                for(i=1;i<(sizeof tablicauzytkownikow / sizeof *tablicauzytkownikow);i++){
                    if(strcmp(tablicauzytkownikow[i].nick,odbierz.tresc)==0)
                    {
                        wyslij.wynik=1;
                        wyslij.operacja=10;
                        wyslij.typ=(long)odbierz.iduzytkownika;
                        tablicauzytkownikow[i].zalogowany=0;
                        tablicauzytkownikow[i].id=odbierz.iduzytkownika;                        
                        msgsnd(id,&wyslij,sizeof(wyslij)-sizeof(long),0);
                        break;
                    }
                }
                break;}
            case 11:{
                printf("Odebrano zlecenie wylaczenia serwera..\nWylaczam serwer..\n");
                sleep(3);
                return 0;
                break;
            }
            default:{break;}
        }       
    }      
    return 0;
}
