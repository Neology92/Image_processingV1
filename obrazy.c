
/*
/     Autor: Oskar Legner
/     Temat: Przetwarzanie obrazow 1
/     Data ukonczenia (wykonania): 2 grudnia 2018
/
/
/    Poczatek kodu zrodlowego    */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<limits.h>

#define MAX 512           /* Maksymalny rozmiar wczytywanego obrazu */ 
                          /*dla 1024 i powyzej moga pojawic sie problemy z dzialaniem programu */
#define DL_LINII 1024         /* maksymalna dlugosc linii komentarza */
#define ILE_LINII 100         /* Ile maksymalnie linii komentarzy (w tym listy zmian) */
#define W_MASKI  3            /* Szerokosc i wysokosc macierzy przechowujacej maske */



/************************************************************************************
 * Funkcja wyswietla menu glowne programu                                           *
 *                                                                                  *
 * \param[in] komunikat tablica, przekazuje komnuikat do wyswietlenia               *
 * \return wybrana opcja z menu                                                     * 
 ************************************************************************************/
int main_menu(char komunikat[])
{
  int opcja=0;

  system("clear"); //wyczyszczenie okna konsoli dla menu
   
   printf("%s",komunikat); //Wyswietlenie komunikatu, jesli zostal wyslany do uzytkownika
   sprintf(komunikat," ");

    printf("\n\n============================================\n"); /* Wyswietlenie menu glownego */
    printf("\t\tMENU\t\t\n");
    printf("--------------------------------------------\n");       
    printf("1. Wczytaj\n2. Zapisz\n3. Wyswietl\n4. Negatyw\n5. Konturowanie\n6. Progowanie\n7. Rozmycie\n8. Maski\n9. Koniec\n");
    printf("--------------------------------------------\n");       
    printf("Twoj wybor [1-9]: ");
    scanf("%d",&opcja);
    printf("============================================\n");
return opcja;
}


/************************************************************************************
 * Funkcja wyswietla menu wczytywania i pobiera nazwe plliku ze strumieniu wejscia  *
 *                                                                                  *
 * \param[out] nazwa tablica, do ktorej zostanie zpaisana nazwa pliku do wczytywania*
 ************************************************************************************/
void czytaj_menu(char nazwa[])
{
  system("clear"); //wyczyszczenie okna konsoli dla menu

  printf("\n\n============================================\n");
  printf("\t\tWCZYTYWANIE\t\t\n");
  printf("--------------------------------------------\n");  
  printf("Podaj nazwe pliku (nie zawierajaca spacji): ");
  scanf("%s",nazwa);                /* Pobiera ze stdin nazwe pliku do wczytania */
}


/************************************************************************************
 * Funkcja wyswietla menu wczytania kolejnego pliku (zastapienie poprzedniego)      *
 *                                                                                  *
 * \return wybrana opcja z menu                                                     * 
 ************************************************************************************/
int czytaj_ponownie_menu()
{
  int wybor;
  char komunikat[100]=" ";

  do
  {
    system("clear"); //wyczyszczenie okna konsoli dla menu

    printf("%s",komunikat); //Wyswietlenie komunikatu, jesli zostal wyslany do uzytkownika
    sprintf(komunikat," ");

    printf("\n\n============================================\n");
    printf("\t\tWCZYTYWANIE\t\t\n");
    printf("--------------------------------------------\n");  
    printf("Wczytujac kolejny obraz, utracisz zmiany wprowadzone w pliku, czy na pewno chcesz to zrobic?\n");
    printf("1. Tak \n2. Nie \n");
    printf("--------------------------------------------\n");  
    printf("Twoj wybor: ");
    scanf("%d",&wybor);           /* Pobiera ze stdin wybor uzytkownika */

    if(wybor!=1 && wybor!=2)                                          /* Sprawdza czy wybor uzytkownika */
    sprintf(komunikat,"Nie na takiej opcji, wybierz jeszcze raz!");   /* lezy w zadanym przedziale */

  }while(wybor!=1 && wybor!=2);                                       /* Jesli nie, to wyswietla komunikat */
                                                                      /* i powtarza pobranie wyboru */
  return wybor;                   /*Wybor zakonczony sukcesem, zwraca nr wybranej opcji */
}


/************************************************************************************
 * Funkcja wczytuje obraz PGM z pliku do tablicy                                    *
 *                                                                                  *
 * \param[in] plik_we uchwyt do pliku z obrazem w formacie PGM                      *
 * \param[out] obraz_pgm tablica, do ktorej zostanie zapisany obraz                 *
 * \param[out] komentarze tablica, do ktorej zostana zapisane komentarze z pliku    *
 * \param[out] wymx szerokosc obrazka                                               *
 * \param[out] wymy wysokosc obrazka                                                *
 * \param[out] szarosci liczba odcieni szarosci                                     *
 * \return liczba wczytanych pikseli                                                * 
 ************************************************************************************/
int czytaj(FILE *plik_we,int obraz_pgm[][MAX], char komentarze[][DL_LINII],int *wymx,int *wymy, int *szarosci) 
{
  char buf[DL_LINII];      /* bufor pomocniczy do czytania naglowka i komentarzy */
  int znak;                /* zmienna pomocnicza do czytania komentarzy */
  int koniec=0;            /* czy napotkano koniec danych w pliku */
  int i,j;

  /*Sprawdzenie czy podano prawid³owy uchwyt pliku */
  if (plik_we==NULL) {
    fprintf(stderr,"Blad: Nie podano uchwytu do pliku\n");
    return(0);
  }

  /* Sprawdzenie "numeru magicznego" - powinien byæ P2 */
  if (fgets(buf,DL_LINII,plik_we)==NULL)   /* Wczytanie pierwszej linii pliku do bufora */
    koniec=1;                              /* Nie udalo sie? Koniec danych! */

  if ( (buf[0]!='P') || (buf[1]!='2') || koniec) {  /* Czy jest magiczne "P2"? */
    fprintf(stderr,"Blad: To nie jest plik PGM\n");
    return(0);
  }

  /* Zapamietanie komentarzy */
  for(i=0; i<DL_LINII; i++)
  {
    sprintf(komentarze[i]," "); /* Zainicjowanie tablicy pustymi liniami */
  }                             /* Pozbycie sie ewentualnych "smieci", np po poprzednim obrazie */ 

  i = 0;        /* Zainicjowanie iteratora */
  do {
    if ((znak=fgetc(plik_we))=='#') 
    {                                         /* Czy linia rozpoczyna sie od znaku '#'? */
      if (fgets(buf,DL_LINII,plik_we)==NULL)  /* Przeczytaj ja do bufora                */
      {
        koniec=1;                      /* Zapamietaj ewentualny koniec danych */
      }
      else if(i<ILE_LINII)                 /* Sprawdzenie, czy nie przekraczamy limitu linii w tablicy */
      {
        strcpy(komentarze[i],"#");    /* Dopisujemy na poczatek przepisywanego komentarza "#" */
        strcat(komentarze[i],buf);    /* Przepiosujemy linie komentarza z bufora */
        i++;                          /* inkrementacja iteratora */
      }
      else
      {
        i = 0;                        /* Gdy dojdziemy do konca tablicy */
        strcpy(komentarze[i],buf);    /* zaczynamy nadpisywac komentarze od pierwszego */
      }
    }  
    else 
    {
      ungetc(znak,plik_we);                   /* Gdy przeczytany znak z poczatku linii */
    }                                         /* nie jest '#' zwroc go                 */
  } while (znak=='#' && !koniec);   /* Powtarzaj dopoki sa linie komentarza */
                                    /* i nie nastapil koniec danych         */

  /* Pobranie wymiarow obrazu i liczby odcieni szarosci */
  if (fscanf(plik_we,"%d %d %d",wymx,wymy,szarosci)!=3) {
    fprintf(stderr,"Blad: Brak wymiarow obrazu lub liczby stopni szarosci\n");
    return(0);
  }
  /* Pobranie obrazu i zapisanie w tablicy obraz_pgm*/
  for (i=0;i<*wymy;i++) {
    for (j=0;j<*wymx;j++) {
      if (fscanf(plik_we,"%d",&(obraz_pgm[i][j]))!=1) {
  fprintf(stderr,"Blad: Niewlasciwe wymiary obrazu\n");
  return(0);
      }
    }
  }
  return *wymx**wymy;   /* Czytanie zakonczone sukcesem    */
}                       /* Zwroc liczbe wczytanych pikseli */


/************************************************************************************
 * Funkcja wyswietla menu zapisywania i pobiera nazwe plliku ze strumieniu wejscia  *
 *                                                                                  *
 * \param[out] nazwa tablica, do ktorej zostanie zpaisana nazwa pliku do zapisania  *
 ************************************************************************************/
void zapisz_menu(char nazwa_out[])
{
  system("clear"); //wyczyszczenie okna konsoli dla menu

  printf("\n\n============================================\n");
  printf("\t\tZAPISYWANIE\t\t\n");
  printf("--------------------------------------------\n");  
  printf("Podaj nazwe do zapisu (nie zawierajaca spacji): ");          /* Pobranie ze standardowego stumienia wejscia */
  scanf("%s",nazwa_out);                                              /* nazwy pliku do ktorej ma byc dokonany zapis */
}


/************************************************************************************
 * Funkcja zapisuje obraz PGM z tablicy do pliku                                    *
 *                                                                                  *
 * \param[out] plik_we uchwyt do pliku do zapisania obrazu                          *
 * \param[in] obraz_pgm tablica, z ktorej zostanie pobrany obraz do zapisu          *
 * \param[in] komentarze tablica, z ktorej zostana pobrane komentarze do pliku      *
 * \param[in] wymx szerokosc obrazka                                                *  
 * \param[in] wymy wysokosc obrazka                                                 *
 * \param[in] szarosci liczba odcieni szarosci                                      *
 * \return 0, gdy pojawi sie blad i 1 gdy zapisano poprawnie                        * 
 ************************************************************************************/
int zapisz(FILE *plik_we,int obraz_pgm[][MAX], char komentarze[][DL_LINII],int wymx,int wymy, int szarosci) 
{
  int i,j;
  char znak;

  /*Sprawdzenie czy podano prawidlowy uchwyt pliku */
  if (plik_we==NULL) {
    fprintf(stderr,"Blad: Nie podano uchwytu do pliku\n");
    return(0);
  }

  fprintf(plik_we, "P2\n");         /* Wpisanie na poczatku numeru magicznego P2 */

  i=0;
  do
  {
    znak = komentarze[i][0];          /* Przepisanie pierwszego znaku kolejnej linii */
   if(znak == '#')                   /* Sprawdzenie, czy linia zaczyna sie od '#' */
    { 
      fprintf(plik_we, "%s",komentarze[i]); /* Przepisanie kolejnego komentarza do pliku */
    }
    i++;                              /* inkrementacja iteratora */
  }while(i<DL_LINII && znak == '#');
  
  /* Dopisanie wymiarow obrazu i liczby odcieni szarosci */
  fprintf(plik_we,"%d %d\n%d\n",wymx,wymy,szarosci);

  /* Wczytanie tablicy i zapisanie do pliku*/
  for (i=0;i<wymy;i++) {
    for (j=0;j<wymx;j++) {
      fprintf(plik_we,"%d ",(obraz_pgm[i][j])); 
    }
  }
  return 1;     /* Zapisywanie zakonczone sukcesem    */
}                      


/************************************************************************************
 * Funkcja wyswietla obraz o zadanej nazwie za pomoca programu "display"            *
 *                                                                                  *
 * \param[in] tablica nazwirajaca nazwe pliku do wyswietlenia                       *
 ************************************************************************************/
void wyswietl(char *n_pliku) {
  char polecenie[DL_LINII];      /* bufor pomocniczy do zestawienia polecenia */

  strcpy(polecenie,"display ");  /* konstrukcja polecenia postaci */
  strcat(polecenie,n_pliku);     /* display "nazwa_pliku" &       */
  strcat(polecenie," &");
  printf("%s\n",polecenie);      /* wydruk kontrolny polecenia */
  system(polecenie);             /* wykonanie polecenia        */
}


/************************************************************************************
 * Funkcja wyswietla menu progowania                                                *
 *                                                                                  *
 * \param[in]  szarosci liczba odcieni szarosci                                     *
 * \param[out] procent progowania                                                   *
 * \return     prog, czyli procent ,podany przez usera, z liczby odcieni szarosci   *
 ************************************************************************************/
int progowanie_menu(int szarosci, float *proc)
{
  char komunikat[100]=" ";

  do     
  {                                 
  system("clear"); //wyczyszczenie okna konsoli dla menu
   
  printf("%s",komunikat); //Wyswietlenie komunikatu, jesli zostal wyslany do uzytkownika
  sprintf(komunikat," ");

  printf("\n\n============================================\n");
  printf("\t\tPROGOWANIE\t\t\n");
  printf("--------------------------------------------\n");  
  printf("Podaj wartosc progu w procentach [0-100]: ");
  scanf("%f",&*proc);                 /* Zapytanie i odebranie od uzytkownika */
                                        /* wartosci progu w procentach */
  if (*proc>100||*proc<0)
  sprintf(komunikat,"Niepoprawna wartosc! Sprobuj ponownie!"); /* Wyslanie komunikatu do uzytkownika */
                                                               /* gdy wartosc nie miesci sie w przedziale */
  }while(*proc>100||*proc<0);
    
  return ((int)(szarosci * (*proc / 100.0)));  /* Obliczenie wartosci progu */;
}


/************************************************************************************
 * Funkcja wyswietla menu rozmycia                                                  *
 *                                                                                  *
 * \return  wybor, opcja wybrana przez uzytkownika                                  *
 ************************************************************************************/
int rozmycie_menu(void)
{
  int wybor;
  char komunikat[100]=" ";

  do
  {
    system("clear"); //wyczyszczenie okna konsoli dla menu
  
    printf("%s",komunikat); //Wyswietlenie komunikatu, jesli zostal wyslany do uzytkownika
    sprintf(komunikat," ");

    printf("\n\n============================================\n");
    printf("\t\tROZMYCIE\t\t\n");
    printf("--------------------------------------------\n");  
    printf("Wybierz typ rozmycia:\n");
    printf(" 1. Pionowe\n 2. Poziome\n 0. Powrot\n");
    printf("--------------------------------------------\n");  
    printf(" Twoj wybor: ");                          /* Pobranie od uzytkownika */
    scanf("%d",&wybor);                               /* Wyboru z menu */    

    if(wybor<0 || wybor>2)
    sprintf(komunikat,"Nie ma takiej opcji, wybierz ponownie!"); /* Sprawdzenie, czy wybor  */
  }while(wybor<0 || wybor>2);                                    /* Jest poprawny i podanie komuniaktu */

  return wybor; /* Zwrocenie wybranej opcji z menu */
}


/************************************************************************************
 * Funkcja nakladana negatyw na obraz                                               *
 *                                                                                  *
 * \param[out] obraz_pgm tablica w ktorej wartosci zostana odwrocone wzg. szarosci  *
 * \param[in] wymx szerokosc obrazka                                                *
 * \param[in] wymy wysokosc obrazka                                                 *
 * \param[in] szarosci liczba odcieni szarosci                                      *
 * \return 1 gdy pomyslnie, 0 gdy cos nie tak                                       *
 ************************************************************************************/
int negatyw(int obraz_pgm[][MAX],int wymx,int wymy, int szarosci) 
{
  int i,j;
 	for (i=0;i<wymy;i++)
 	{
    	for (j=0;j<wymx;j++) 
    	{
    		obraz_pgm[i][j] = szarosci - obraz_pgm[i][j]; //odwrocenie wartosci pixela wzgledem skali szarosci
    	   
         if(obraz_pgm[i][j]>szarosci || obraz_pgm[i][j]<0)  
          return (0);                             /* Zwroc zero, gdy cos nie tak */
      }
  }

return 1; //Zwroc 1, gdy pomyslnie
}                       


/************************************************************************************
 * Funkcja nakladana konturowanie na obraz                                          *
 *                                                                                  *
 * \param[out] obraz_pgm tablica w ktorej wartosci zostana zmienione                *
 * \param[in] wymx szerokosc obrazka                                                *
 * \param[in] wymy wysokosc obrazka                                                 *
 * \param[in] szarosci liczba odcieni szarosci                                      *
 * \return 1 gdy pomyslnie, 0 gdy cos nie tak                                       *
 ************************************************************************************/
int konturowanie(int obraz_pgm[][MAX],int wymx,int wymy, int szarosci) 
{
  int i,j;
  for (i=0;i<wymy;i++)
  {
      for (j=0;j<wymx;j++) 
      {                         
        if(i==wymy-1||j==wymx-1)
        {
          obraz_pgm[i][j] = 0; /*Dla ostaniego wiersza i kolumny przypisanie wartosci 0*/
        }
        else
        {               /*obliczanie wartosci pixela wzgledem dwoch innych z otoczenia (obok i ponizej)*/
          obraz_pgm[i][j] =  (abs(obraz_pgm[i+1][j]-obraz_pgm[i][j]) + abs(obraz_pgm[i][j+1]-obraz_pgm[i][j]));
          if(obraz_pgm[i][j]>szarosci) //Sprawdzenie, czy wartosc pixela nie wychodzi poza skale szarosci
          obraz_pgm[i][j]=szarosci;
        }
         if(obraz_pgm[i][j]>szarosci || obraz_pgm[i][j]<0)  
          return (0);                             /* Zwroc zero, gdy cos nie tak */
      }
    }

return 1; //Zwroc 1, gdy pomyslnie
}         


/************************************************************************************
 * Funkcja nakladana progowanie na obraz                                            *
 *                                                                                  *
 * \param[out] obraz_pgm tablica w ktorej wartosci zostana zmienione                *
 * \param[in] wymx szerokosc obrazka                                                *
 * \param[in] wymy wysokosc obrazka                                                 *
 * \param[in] szarosci liczba odcieni szarosci                                      *
 * \param[in] prog , podany wczesniej przez uzytkownika i funkcji progowanie_menu() *
 * \return 1 gdy pomyslnie, 0 gdy cos nie tak                                       *
 ************************************************************************************/
int progowanie(int obraz_pgm[][MAX],int wymx,int wymy, int szarosci, int prog) 
{
  int i,j;
  for (i=0;i<wymy;i++)
  {
      for (j=0;j<wymx;j++) 
      {                    
        if(obraz_pgm[i][j]>prog)    /*dla wartosci wiekszych niz "prog" wpisywanie */ 
          obraz_pgm[i][j]=szarosci; /* maksymalnej wartosci ze skali szarosci */
        else
          obraz_pgm[i][j]=0; //dla wartosci mniejszych, lub rownych wpisywanie wartosci "0"

          if(obraz_pgm[i][j]>szarosci || obraz_pgm[i][j]<0)  
          return (0);                             /* Zwroc zero, gdy cos nie tak */
      } 
    }

return 1;     //Zwroc 1, gdy pomyslnie
}


/************************************************************************************
 * Funkcja nakladana rozmycie pionowe na obraz                                      *
 *                                                                                  *
 * \param[out] obraz_pgm tablica w ktorej wartosci zostana zmienione                *
 * \param[in] wymx szerokosc obrazka                                                *
 * \param[in] wymy wysokosc obrazka                                                 *
* \param[in] szarosci liczba odcieni szarosci                                      *
 * \return 1 gdy pomyslnie, 0 gdy cos nie tak                                       *
 ************************************************************************************/
int rozmycie_pion(int obraz_pgm[][MAX],int wymx,int wymy, int szarosci) 
{
  int bufor1=0; /* Bufory pomocnicze */
  int bufor2=0; /* do przechowywania wartosci pixeli */
  int i,j;
  for (j=0;j<wymx;j++) 
  {
      for (i=0;i<wymy;i++)  
      {                
          if(i>0 && i<wymy-1)    /* Nie liczymy rozmycia dla gornej i dolnej krawedzi */
            bufor1 = (obraz_pgm[i-1][j] + obraz_pgm[i][j] + obraz_pgm[i+1][j])/3;
         
          if(i>1)         /* Przypisanie wartosci bufora dopiero drugiemu pixelowi w kazdej kolumnie, */
            obraz_pgm[i-1][j] = bufor2;   /* czyli dopiero, gdy iterator "i" bedzie na trzecim rzedzie*/
         
          bufor2 = bufor1; // Przekazanie wartosci miedzy buforami

          if(obraz_pgm[i][j]>szarosci || obraz_pgm[i][j]<0)  
          return (0);                             /* Zwroc zero, gdy cos nie tak */      
        } 
    }

return 1; //zwrocenie 1, gdy wszystko pomyslnie
}


/************************************************************************************
 * Funkcja nakladana rozmycie poziome na obraz                                      *
 *                                                                                  *
 * \param[out] obraz_pgm tablica w ktorej wartosci zostana zmienione                *
 * \param[in] wymx szerokosc obrazka                                                *
 * \param[in] wymy wysokosc obrazka                                                 *
 * \param[in] szarosci liczba odcieni szarosci                                      *
 * \return 1 gdy pomyslnie, 0 gdy cos nie tak                                       *
 ************************************************************************************/
int rozmycie_poz(int obraz_pgm[][MAX],int wymx,int wymy, int szarosci) 
{
  int bufor1=0;        /* Bufory pomocnicze */
  int bufor2=0;        /* do przechowywania wartosci pixeli */
  int i,j;
  for (i=0;i<wymy;i++)  
  {
      for (j=0;j<wymx;j++) 
      {                
          if(j>0 && j<wymx-1)    /* Nie liczymy rozmycia dla bocznych krawedzi */
            bufor1 = (obraz_pgm[i][j-1] + obraz_pgm[i][j] + obraz_pgm[i][j+1])/3;
         
          if(j>1)            /* Przypisanie wartosci bufora dopiero drugiemu pixelowi w kazdym rzedzie, */
            obraz_pgm[i][j-1] = bufor2;   /* czyli dopiero, gdy iterator "i" bedzie na trzeciej kolumnie*/
         
          bufor2 = bufor1; // Przekazanie wartosci miedzy buforami
          if(obraz_pgm[i][j]>szarosci || obraz_pgm[i][j]<0)  
          return (0);                             /* Zwroc zero, gdy cos nie tak */  
      } 
  }

return 1; //zwroc 1, gdy pomyslnie
}


/************************************************************************************
 * Funkcja wyswietla menu masek                                                     *
 *                                                                                  *
 * \return  wybor, opcja wybrana przez uzytkownika                                  *
 ************************************************************************************/
int maski_menu()
{
  char komunikat[100]=" ";
  int wybor;

  do
  {
    system("clear"); //wyczyszczenie okna konsoli dla menu

    printf("%s",komunikat); //Wyswietlnie ewentualnego komunikatu
    sprintf(komunikat," ");
                                                                    /* Wypisanie menu */
    printf("\n\n============================================\n");
    printf("\t\tMASKI\t\t\n");
    printf("--------------------------------------------\n");
    printf(" 1. Wlasna maska\n 2. Menu masek\n 0. Powrot\n");
    printf("--------------------------------------------\n");  
    printf(" Twoj wybor: ");
    scanf("%d",&wybor);

    if (wybor<0 || wybor>2)
    {
      sprintf(komunikat, "Nie ma takiej opcji do wyboru, wybierz ponownie!"); /* Sprawdzenie czy wybor poprawny */
    }                                                             /* i podanie odpowiedniego komunikatu */
  }while(wybor<0 || wybor>2);

return wybor; //Zwrocenie opcji wybranej w menu
}


/************************************************************************************
 * Funkcja wyswietla spis masek                                                     *
 *                                                                                  *
 * \param[in] komunikat tablica, przekazuje komnuikat do wyswietlenia               *
 * \return  wybor, opcja wybrana przez uzytkownika                                  *
 ************************************************************************************/
int maski_spis_menu(char komunikat[])
{
  int wybrano = 0;
  
  system("clear"); //wyczyszczenie okna konsoli dla menu
  
   printf("%s",komunikat); //Wyswietlenie komunikatu, jesli zostal wyslany do uzytkownika
   sprintf(komunikat," ");
  
  printf("\n\n============================================\n"); /* Wyswietlenie menu */
  printf("\t\tMASKI\t\t\n");
  printf("--------------------------------------------\n");
  printf(" 1. Filtr Usredniajacy\n 2. Filtr usredniajacy ze wzmocnieniem\n 3. Gradient Robertsa\n 4. Maska Prewitta\n 5. Maska Sobela\n 6. Maska wykrywająca narożniki\n 7. Laplasjany\n 8. Opisy masek\n 9. Powrot\n");
  printf("--------------------------------------------\n");  
  printf("Twoj wybor: ");
  scanf("%d",&wybrano);                                 /* Pobranie od uzytkownika wyboru */
  printf("============================================\n");

return wybrano; /* Zwrocenie wybranej opcji */
}


/************************************************************************************
 * Funkcja wyswietla informacje o maskach                                           *
 *                                                                                  *
] ************************************************************************************/
void maski_info()
{

  system("clear"); //wyczyszczenie okna konsoli dla menu

  printf("\n\n============================================\n");
  printf("\t\tMASKI\t\t\n");
  printf("--------------------------------------------\n");
  printf("> Filtr usredniajacy: \n\tCechy filtru: usuwanie drobnych zaklocen z obrazu przy jednoczesnym rozmyciu konturow obiektow i pogorszeniu rozpoznawalności ich kształtów.\n\n");
  printf("> Filtr usredniajacy ze wzmocnieniem: \n\tCechy filtru: usuwanie drobnych zaklocen z obrazu, efekt rozmycia konturow jest zniwelowany poprzez wzmocnienie punktu centralnego.\n\n");
  printf("> Gradient Robertsa: \n\tCechy filtru: eksponowanie krawędzi obiektów. \n\n");
  printf("> Maska Prewitta\n\tCechy filtru: eksponowanie poziomych linii. Maska „obrócona” o 90 stopni eksponuje linie pionowe. \n\n");
  printf("> Maska Sobela \n\tCechy filtru: wzmocnienie wpływu najbliższego otoczenia piksela, możliwość obrotu maski w różnych kierunkach (o 90 i 45 stopni) pozwala na eksponowanie linii o różnych orientacjach. \n\n");
  printf("> Maska wykrywajaca narozniki \n\tCechy filtru: możliwość obrotu maski, wykrywanie narożników. \n\n");
  printf("> Laplasjany \n\tCechy: podkreślanie krawędzi i konturów obiektów niezależnie od tego, pod jakim kątem one przebiegają.\n\n");
  
  printf("\nNacisnij enter, aby powrocic (wprowadzenie innych znakow moze powodowac blad programu...).");

  getchar(); //Wychodzi z funkcji po przycisnieciu przycisku enter
  getchar();
}


/************************************************************************************
 * Funkcja tworzy maske z liczb podanych przez uzytkownika                          *
 *                                                                                  *
 * \param[out] maska tablica w ktorej zostanie zapisana stworzona maska             *
 * \param[in] szarosci skala szarosci obrazu                                        *
 * \return 1 gdy maska ma zostac nalozone, 0 gdy nie                                *
 ************************************************************************************/
int maski_stworz(int maska[][W_MASKI],int szarosci)
{
  int i,j;
  int max_w_maski = 0;
  int min_w_maski = 0;
  int wybor = 2;
  char komunikat[100]=" ";

  min_w_maski = (INT_MIN/(W_MASKI*W_MASKI))/szarosci; /* obliczenie maks i min wartosci w masce */
  max_w_maski = (INT_MAX/(W_MASKI*W_MASKI))/szarosci; /* Dla wiekszych/mniejszych moze zostac przekroczona  */
                                                      /* maks wartosc w tablicy bufor w funkcji maski_naloz() */

  while(wybor == 2)
  {

    for(i=0; i<W_MASKI; i++)
    {
      for(j=0; j<W_MASKI; j++)
      {
        do
        {
          system("clear"); //wyczyszczenie okna konsoli dla menu
          
            printf("%s",komunikat); //Wyswietlenie komunikatu, jesli zostal wyslany do uzytkownika
            sprintf(komunikat," ");
          
          printf("\n\n============================================\n");
          printf("\t   TWORZENIE MASKI\t\n");
          printf("--------------------------------------------\n");
          printf("Podaj 9 (3 rzedy po 3) kolejnych liczb calkowitych z zakresu [%d - %d]:\n",min_w_maski,max_w_maski);
          printf("[%d][%d]: ",i+1,j+1);

          scanf("%d", &maska[i][j]);                                        /* Pobranie wartosci o uzytkownika */
          if(maska[i][j]>max_w_maski || maska[i][j]<min_w_maski)            /* i sprawdzenie, czy miesci sie */
            sprintf(komunikat,"Niepoprawna wartosc, sprobuj jeszcze raz!"); /* w przedziale */ 
        }while(maska[i][j]>max_w_maski || maska[i][j]<min_w_maski);         
        
      }
      printf("\n");
    }

    do
    {
      system("clear"); //wyczyszczenie okna konsoli dla menu

        printf("%s",komunikat); //Wyswietlenie komunikatu, jesli zostal wyslany do uzytkownika
        sprintf(komunikat," ");

      printf("\n\n============================================\n");
      printf("\t   TWORZENIE MASKI\t\n");
      printf("--------------------------------------------\n");
      printf("Twoja maska, to:\n\n");

      for(i=0; i<W_MASKI; i++)
      {
        for(j=0; j<W_MASKI; j++)
        {
          printf("\t[%d]", maska[i][j]);        /* Wypisanie stworzonej maski */
        }
        printf("\n");
      }

      printf("\nCzy chcesz ja nalozyc na obraz?\n 1.Tak \n 2.Nie (stworzenie nowej maski) \n 3.Nie (anuluj) \n\nTwoj wybor: ");
      scanf("%d", &wybor);  /* Wyswietlenie zapytania i pobranie odpowiedzi od uzytkownika */
        
      if(wybor!=1 && wybor!=2 && wybor!=3)    /* Sprawdzenie poprawnosci wyboru uzytkownika */
        sprintf(komunikat,"Niepoprawny wybor, sprobuj jeszcze raz!");                  
    }while(wybor!=1 && wybor!=2 && wybor!=3);
  }

  if(wybor==1)
    return (1); //Zwroc 1, jesli uzytkownik chce nalozyc maske
  else if(wybor==3)
    return (0); //Zwroc 0, jesli uzytkownik nie chce nalozyc stworzonej maski
}


/************************************************************************************
 * Funkcja naklada maske na obraz                                                   *
 *                                                                                  *
 * \param[out] obraz_pgm tablica, na ktora zostanie nalozona maska                  *
 * \param[in] wymx szerokosc obrazka                                                *  
 * \param[in] wymy wysokosc obrazka                                                 *
 * \param[in] szarosci liczba odcieni szarosci                                      *
 * \param[out] maska tablica w ktorej przekazana do funkcji jest maska do nalozenia *
 * \return 1 gdy pomyslnie, 0 gdy cos nie tak                                       *
 ************************************************************************************/
int maski_naloz(int obraz_pgm[][MAX], int wymx, int wymy, int szarosci, const int maska[][W_MASKI]) 
{
  int i,j;
  int bufor[MAX][MAX]={0}; /* bufor do nakladania maski*/
  int w_max=0,w_min=0;


  for (i=1;i<wymy-1;i++)        /* Petla pomijajaca brzegi obrazka */
  {
      for (j=1;j<wymx-1;j++) 
      {                
          bufor[i][j] = (maska[0][0]*obraz_pgm[i-1][j-1] + maska[0][1]*obraz_pgm[i-1][j] + maska[0][2]*obraz_pgm[i-1][j+1]
                      +  maska[1][0]*obraz_pgm[i][j-1]   + maska[1][1]*obraz_pgm[i][j]   + maska[1][2]*obraz_pgm[i][j+1]
                      +  maska[2][0]*obraz_pgm[i+1][j-1] + maska[2][1]*obraz_pgm[i+1][j] + maska[2][2]*obraz_pgm[i+1][j+1]);

          if(bufor[i][j]<w_min)           /* Znalezienie wartosci maksymalnej */
            w_min = bufor[i][j];          /* i wartosci minimalnej po nalozeniu */
          else if(bufor[i][j]>w_max)      /* maski - w celu normalizacji */
            w_max = bufor[i][j];
      } 
  }


  if(w_max-w_min==0)  //Upewnienie sie, ze nie podzielimy przez zero
    w_max++;

  for (i=1;i<wymy-1;i++)        /* Nadpisanie zmian z buforu */
  {
      for (j=1;j<wymx-1;j++) 
      {    
        obraz_pgm[i][j] = ((float)(bufor[i][j]-w_min)/(w_max-w_min))*szarosci;

          if(obraz_pgm[i][j]>szarosci || obraz_pgm[i][j]<0)  
          return (0);                             /* Zwroc zero, gdy cos nie tak */  
      } 
  }

return 1; //zwroc 1, gdy wszystko pomyslnie
}


/************************************************************************************
 * Funkcja wyswietla menu wyboru maski                                              *
 *                                                                                  *
 * \return wybrana opcja z menu                                                     * 
 ************************************************************************************/
int maski_prewitta_menu()
{
  int wybor;
  char komunikat[100]=" ";

  do
  {
    system("clear"); //wyczyszczenie okna konsoli dla menu

    printf("%s",komunikat); //Wyswietlenie komunikatu, jesli zostal wyslany do uzytkownika
    sprintf(komunikat," ");

    printf("\n\n============================================\n");
    printf("\t\tNAKLADANIE MASKI\t\t\n");
    printf("--------------------------------------------\n");  
    printf("Wybierz rodzaj maski Prewitta:\n");
    printf("1. Eksponujaca linie poziome \n2. Eksponujaca linie pionowe \n");
    printf("--------------------------------------------\n");  
    printf("Twoj wybor: ");
    scanf("%d",&wybor);           /* Pobiera ze stdin wybor uzytkownika */

    if(wybor!=1 && wybor!=2)                                          /* Sprawdza czy wybor uzytkownika */
    sprintf(komunikat,"Nie na takiej opcji, wybierz jeszcze raz!");   /* lezy w zadanym przedziale */

  }while(wybor!=1 && wybor!=2);                                       /* Jesli nie, to wyswietla komunikat */
                                                                      /* i powtarza pobranie wyboru */
  return wybor;                   /*Wybor zakonczony sukcesem, zwraca nr wybranej opcji */
}


/************************************************************************************
 * Funkcja wyswietla menu wyboru maski                                              *
 *                                                                                  *
 * \return wybrana opcja z menu                                                     * 
 ************************************************************************************/
int maski_sobela_menu()
{
  int wybor;
  char komunikat[100]=" ";

  do
  {
    system("clear"); //wyczyszczenie okna konsoli dla menu

    printf("%s",komunikat); //Wyswietlenie komunikatu, jesli zostal wyslany do uzytkownika
    sprintf(komunikat," ");

    printf("\n\n============================================\n");
    printf("\t\tNAKLADANIE MASKI\t\t\n");
    printf("--------------------------------------------\n");  
    printf("Wybierz o jaki kat ma byc obrocona maska Sobela:\n");
    printf("1. 0 stopni \n2. 45 stopni \n3. 90 stopni \n4. 135 stopni \n");
    printf("--------------------------------------------\n");  
    printf("Twoj wybor: ");
    scanf("%d",&wybor);           /* Pobiera ze stdin wybor uzytkownika */

    if(wybor<1 || wybor>4)                                          /* Sprawdza czy wybor uzytkownika */
    sprintf(komunikat,"Nie na takiej opcji, wybierz jeszcze raz!");   /* lezy w zadanym przedziale */

  }while(wybor<1 || wybor>4);                                       /* Jesli nie, to wyswietla komunikat */
                                                                      /* i powtarza pobranie wyboru */
  return wybor;                   /*Wybor zakonczony sukcesem, zwraca nr wybranej opcji */
}


/************************************************************************************
 * Funkcja wyswietla menu wyboru maski                                              *
 *                                                                                  *
 * \return wybrana opcja z menu                                                     * 
 ************************************************************************************/
int maski_narozniki_menu()
{
  int wybor;
  char komunikat[100]=" ";

  do
  {
    system("clear"); //wyczyszczenie okna konsoli dla menu

    printf("%s",komunikat); //Wyswietlenie komunikatu, jesli zostal wyslany do uzytkownika
    sprintf(komunikat," ");

    printf("\n\n============================================\n");
    printf("\t\tNAKLADANIE MASKI\t\t\n");
    printf("--------------------------------------------\n");  
    printf("Wybierz ktore narozniki ma wykrywac maska:\n");
    printf("1. Lewy-gorny \n2. Prawy-gorny \n3. Prawy-dolny \n4. Lewy-dolny \n");
    printf("--------------------------------------------\n");  
    printf("Twoj wybor: ");
    scanf("%d",&wybor);           /* Pobiera ze stdin wybor uzytkownika */

    if(wybor<1 || wybor>4)                                          /* Sprawdza czy wybor uzytkownika */
    sprintf(komunikat,"Nie na takiej opcji, wybierz jeszcze raz!");   /* lezy w zadanym przedziale */

  }while(wybor<1 || wybor>4);                                       /* Jesli nie, to wyswietla komunikat */
                                                                      /* i powtarza pobranie wyboru */
  return wybor;                   /*Wybor zakonczony sukcesem, zwraca nr wybranej opcji */
}


/************************************************************************************
 * Funkcja wyswietla menu wyboru maski                                              *
 *                                                                                  *
 * \return wybrana opcja z menu                                                     * 
 ************************************************************************************/
int maski_laplasjany_menu()
{
  int wybor;
  char komunikat[100]=" ";

  do
  {
    system("clear"); //wyczyszczenie okna konsoli dla menu

    printf("%s",komunikat); //Wyswietlenie komunikatu, jesli zostal wyslany do uzytkownika
    sprintf(komunikat," ");

    printf("\n\n============================================\n");
    printf("\t\tNAKLADANIE MASKI\t\t\n");
    printf("--------------------------------------------\n");  
    printf("Wybierz laplasjan: \n");
    printf("1. \n2. \n3. \n");
    printf("--------------------------------------------\n");  
    printf("Twoj wybor: ");
    scanf("%d",&wybor);           /* Pobiera ze stdin wybor uzytkownika */

    if(wybor<1 || wybor>3)                                          /* Sprawdza czy wybor uzytkownika */
    sprintf(komunikat,"Nie na takiej opcji, wybierz jeszcze raz!");   /* lezy w zadanym przedziale */

  }while(wybor<1 || wybor>3);                                       /* Jesli nie, to wyswietla komunikat */
                                                                      /* i powtarza pobranie wyboru */
  return wybor;                   /*Wybor zakonczony sukcesem, zwraca nr wybranej opcji */
}


int main() {
  int obraz[MAX][MAX] ;
  int wymx,wymy,odcieni;
  char komentarz[ILE_LINII][DL_LINII];
  int prog=0;

  int maska_wlasna[W_MASKI][W_MASKI] = {0};
  
      /* Deklaracja stalych tablic masek */
  const int maska_usr[W_MASKI][W_MASKI]           = {{1,1,1},{1,1,1},{1,1,1}};
  const int maska_usr_wzm[W_MASKI][W_MASKI]       = {{1,1,1},{1,2,1},{1,1,1}};
  const int maska_robertsa[W_MASKI][W_MASKI]      = {{0,0,0},{-1,0,0},{0,1,0}};
  const int maska_prewitta_poz[W_MASKI][W_MASKI]  = {{-1,-1,-1},{0,0,0},{1,1,1}};
  const int maska_prewitta_pion[W_MASKI][W_MASKI] = {{1,0,-1},{1,0,-1},{1,0,-1}};
  const int maska_sobela_0[W_MASKI][W_MASKI]      = {{-1,-2,-1},{0,0,0},{1,2,1}};
  const int maska_sobela_45[W_MASKI][W_MASKI]     = {{0,-1,-2},{1,0,-1},{2,1,0}};
  const int maska_sobela_90[W_MASKI][W_MASKI]     = {{1,0,-1},{2,0,-2},{1,0,-1}};
  const int maska_sobela_135[W_MASKI][W_MASKI]    = {{2,1,0},{1,0,-1},{0,-1,-2}};
  const int maska_naroznik_ld[W_MASKI][W_MASKI]   = {{1,1,1},{-1,-2,1},{-1,-1,1}};
  const int maska_naroznik_pg[W_MASKI][W_MASKI]   = {{1,-1,-1},{1,-2,-1},{1,1,1}};
  const int maska_naroznik_lg[W_MASKI][W_MASKI]   = {{-1,-1,1},{-1,-2,1},{1,1,1}};
  const int maska_naroznik_pd[W_MASKI][W_MASKI]   = {{1,1,1},{1,-2,-1},{1,-1,-1}};
  const int maska_laplasjany_1[W_MASKI][W_MASKI]  = {{0,-1,0},{-1,4,-1},{0,-1,0}};
  const int maska_laplasjany_2[W_MASKI][W_MASKI]  = {{-1,-1,-1},{-1,8,-1},{-1,-1,-1}};
  const int maska_laplasjany_3[W_MASKI][W_MASKI]  = {{1,-2,1},{-2,4,-2},{1,-2,1}};

  int opcja = 0;

  int odczytano = 0, zapisano = 0;
  FILE *plik;
  FILE *plik_out;
  char nazwa[100], nazwa_out[100];
  char nazwa_tmp[] = "tmp.pgm";
  char komunikat[100]=" ";
  int wybor;
  int nalozono_maske=0;


	while(opcja!=9) //zapetlone menu (az do wybrania opcji 9 - wyjdz)
	{
    wybor = 0; //wyczyszczenie ostatniego wyboru uzytkownika


		switch (opcja=main_menu(komunikat))
		{
			case 1: //wczytaj
			  			/*Ostrzezenie, jesli utykownik juz wczytal obraz*/
            if(odczytano)
            {
              wybor = czytaj_ponownie_menu(); //Wywolanie menu przypisanie zwroconej wartosci do wybor
              
              if (wybor==1)
                odczytano=0;
            }
            if(!odczytano)
            {
                czytaj_menu(nazwa);
                plik=fopen(nazwa,"r"); //otworzenie pliku o nazwie pobranej ze stdin w funkcji czytaj_menu()

  					  if (plik != NULL) 
  					  {       	
                                    /* Wczytanie zawartosci wskazanego pliku do pamieci */
  					    odczytano = czytaj(plik,obraz,komentarz,&wymx,&wymy,&odcieni);
  					    fclose(plik);  //Zamkniecie pliku
  					  }

              if(odczytano) //Wyslanie uzytkownikowi odpowienidego komunikatu
                sprintf(komunikat,"Poprawnie wczytano plik \"%s\"",nazwa);
              else
              {
                if(wybor == 1)
                {
                  sprintf(komunikat,"Nie udalo sie wczytac zawartosci pliku \"%s\"! Przywrocono poprzedni obraz!",nazwa);                  
                  odczytano=1;
                }
                else
                  sprintf(komunikat,"Nie udalo sie wczytac zawartosci pliku \"%s\"!",nazwa);
              }
						}

            break;
			
			case 2: //zapisz
            if(odczytano)
            {
                zapisz_menu(nazwa_out);
  					  
              plik_out=fopen(nazwa_out,"w"); //Otworzenie pliku o nazwie pobranej w funkcji zapisz_menu()
              
              zapisano=0;

  					  if (plik_out != NULL) { 
                                /* Zapisanie zawartosci tablicy do  pliku */
  					    zapisano = zapisz(plik_out,obraz,komentarz,wymx,wymy,odcieni);
  					    fclose(plik_out); //zamkniecie pliku
  					  }		

              if(zapisano) //Wyslanie uzytkownikowi odpowienidego komunikatu
                sprintf(komunikat,"Poprawnie zapisano do pliku \"%s\"",nazwa_out);
              else
                sprintf(komunikat,"Nie udalo sie zapisac pliku!");
            }
            else
              sprintf(komunikat,"Najpierw nalezy wczytac jakis plik!");

					  break;

			case 3: //wyswietl
            if(odczytano)
            {  
  							/* Zapisanie zawartosci tablicy do  pliku */
  					  plik_out=fopen(nazwa_tmp,"w");

  					  if (plik_out != NULL) 
  					  { 
  					    zapisano = zapisz(plik_out,obraz,komentarz,wymx,wymy,odcieni);
  					    fclose(plik_out);
  					  }	
  						  /* Wyswietlenie poprawnie wczytanego obrazu zewnetrznym programem */
                if(zapisano)
  					    {
                  wyswietl(nazwa_tmp);
                  sprintf(komunikat,"Wyswietlono obraz"); 
                }
                else
                  sprintf(komunikat,"Nie udalo sie wyswietlic obrazu!"); 
            }
            else
              sprintf(komunikat,"Najpierw nalezy wczytac jakis plik!");

						break;

			case 4: //negatyw
            if(odczytano)
            {
                      /* Odwracanie wartosci tablicy wzgledem zakresu */
              if(negatyw(obraz,wymx,wymy,odcieni))
                sprintf(komunikat,"Pomyslnie nalozono negatyw"); 
            }
            else
              sprintf(komunikat,"Najpierw nalezy wczytac jakis plik!");

  				break;
	
			case 5: //konturowanie
          if(odczytano)
            { 
              if(konturowanie(obraz,wymx,wymy,odcieni))       /* Nalozenie konturowania */
              sprintf(komunikat,"Konturowanie zakonczone");   /* i podanie odpowienidego komunikatu*/
            }
            else
              sprintf(komunikat,"Najpierw nalezy wczytac jakis plik!");
            
            break;

			case 6: //progowanie
            if(odczytano)
            {
                  /*Pobranie wartosci progowania*/
              float procent;
              prog = progowanie_menu(odcieni,&procent);

              if(progowanie(obraz,wymx,wymy,odcieni,prog))  /* Nalozenie progowania i podanie komunikatu */
                sprintf(komunikat,"Zastosowano progowanie (Prog: %.1f%%)",procent); 
              else
                sprintf(komunikat,"Progowanie sie nie udalo!"); 
            }
            else
              sprintf(komunikat,"Najpierw nalezy wczytac jakis plik!");
            
          break;

      case 7: //rozmycie
          if(odczytano)
            {
              wybor = rozmycie_menu();

              if(wybor)
              {
                if(wybor == 1)
                {
                  rozmycie_pion(obraz,wymx,wymy,odcieni);             /* Wywolanie fnkcji rozmywajacej */
                  sprintf(komunikat,"Rozmyto w pionie!");     /* w pionie i wyslanie uzytkownikowi komunikatu */
                }
                else if(wybor == 2)
                {
                  rozmycie_poz(obraz,wymx,wymy,odcieni);              /* Wywolanie funkcji rozmywajacej */
                  sprintf(komunikat,"Rozmyto w poziomie!");   /* w poziomie i wyslanie uzytkownikowi komunikatu */
                }                                           
              }
              
            }
            else
              sprintf(komunikat,"Najpierw nalezy wczytac jakis plik!");
            
          break;

			case 8: //maski 
            if(odczytano)
            {
              wybor = maski_menu(); //Przekazanie z funkcji maski_menu() wyboru uzytkownika

              if(wybor)
              {
                if(wybor == 1)  /* Tworzenie nowej maski, jesli uzytkownika wybral taka opcje z menu */
                {
                    maski_stworz(maska_wlasna,odcieni);//Wlasna maska
                    nalozono_maske = maski_naloz(obraz,wymx,wymy,odcieni,maska_wlasna);
                }
                else
                {  
                  int nr_maski = 0;

                  while(nr_maski>9 || nr_maski<1)
                  { 

                    switch (nr_maski = maski_spis_menu(komunikat))  /* Nalozenie jednej z masek, wybranej */
                    {                                               /* Przez uzytkownika */
                      
                      case 1: // filtr usredniajacy
                        nalozono_maske = maski_naloz(obraz,wymx,wymy,odcieni,maska_usr);
                        break;

                      case 2: // filtr usredniajacy ze wzmocnieniem
                        nalozono_maske = maski_naloz(obraz,wymx,wymy,odcieni,maska_usr_wzm);
                        break;

                      case 3: // maska Robertsa
                        nalozono_maske = maski_naloz(obraz,wymx,wymy,odcieni,maska_robertsa);
                        break;

                      case 4: // maska Prewitta
                          wybor = maski_prewitta_menu();  /* Wybor rodzaju maski Prewitta */
                          if (wybor==1)
                            nalozono_maske = maski_naloz(obraz,wymx,wymy,odcieni,maska_prewitta_poz); //pozioma
                          else if(wybor == 2)
                            nalozono_maske = maski_naloz(obraz,wymx,wymy,odcieni,maska_prewitta_pion); //pionowa
                        break;

                      case 5: // maska Sobela obrocona o rozne katy
                          switch (maski_sobela_menu())
                          {
                            case 1:
                              nalozono_maske = maski_naloz(obraz,wymx,wymy,odcieni,maska_sobela_0); // 0 stopni
                              break;
                            case 2:
                              nalozono_maske = maski_naloz(obraz,wymx,wymy,odcieni,maska_sobela_45); //45 stopni
                              break;
                            case 3:
                              nalozono_maske = maski_naloz(obraz,wymx,wymy,odcieni,maska_sobela_90); // 90 stopni
                              break;
                            case 4:
                              nalozono_maske = maski_naloz(obraz,wymx,wymy,odcieni,maska_sobela_135); //135 stopni
                              break;
                          }
                        break;

                      case 6: // maska wykrywajaca narozniki
                        switch (maski_narozniki_menu())
                        {
                          case 1:
                            nalozono_maske = maski_naloz(obraz,wymx,wymy,odcieni,maska_naroznik_lg); //Lewy gorny
                            break;
                          case 2:
                            nalozono_maske = maski_naloz(obraz,wymx,wymy,odcieni,maska_naroznik_pg); // Prawy gorny
                            break;
                          case 3:
                            nalozono_maske = maski_naloz(obraz,wymx,wymy,odcieni,maska_naroznik_pd); // Prawy dolny
                            break;
                          case 4:
                            nalozono_maske = maski_naloz(obraz,wymx,wymy,odcieni,maska_naroznik_ld); // Lewy dolny
                          break;
                        }
                        break;

                      case 7: // laplasjany
                        switch (maski_laplasjany_menu())
                        {
                        case 1:
                          nalozono_maske = maski_naloz(obraz,wymx,wymy,odcieni,maska_laplasjany_1); // pierwszy
                          break;
                        case 2:
                          nalozono_maske = maski_naloz(obraz,wymx,wymy,odcieni,maska_laplasjany_2); // drugi
                          break;
                        case 3:
                          nalozono_maske = maski_naloz(obraz,wymx,wymy,odcieni,maska_laplasjany_3); // trzeci
                          break;
                        }
                        break;

                      case 8: //informacje o maskach
                        maski_info();
                        break;

                      case 9: //wyjscie
                        break;

                      default:
                        sprintf(komunikat,"Nie ma takiej opcji, wybierz ponownie!");
                        break;

                    }//switch
                  }//while
                }
              }
                    if(nalozono_maske)        /* Podanie odpowiedniego komunikatu */
                    {
                      sprintf(komunikat,"Pomyslnie nalozono maske!");
                      nalozono_maske = 0;
                    }
                    else
                    {
                      sprintf(komunikat,"Nie nalozono maski");
                    }
            }//if
            else
              sprintf(komunikat,"Najpierw nalezy wczytac jakis plik!");
        
        		break;
			
      case 9: 
					  if( access("tmp.pgm", F_OK ) != -1 ) /*Usuniecie pliku tmp, jesli istnieje */
            {                                    /*Sprawdzenie czy istnieje za pomoca funkcji access() */
					  	system("rm tmp.pgm");              /*z biblioteki "unistd.h" */
					  }
					  break;

			default: printf("Nie ma takiej opcji - wybierz ponownie\n");
		}
	}

  return 0;
}
/*    Koniec kodu zrodlowego    */

/*    Testy oprogramowania:

      ( Przy sprawdzaniu, kazdego z ponizszych:
        negatywu, konturowania, progowania, rozmycia, oraz mask -
        - sprawdzenie takze wyswietlaniem, czy obraz sie odpowiedniu wizualnie zmienia! )

     1. Sprawdzenie wczytywania plikow - 1
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      | Dane wyjsciowe                    |  Komentarz
      * 1                                |  * 'Nie udalo sie wczytac         |    * Przy niepoprawnych danych
      * abc.pgm                          |   zawartosci pliku "abc.pgm"!'    |      program dziala poprawnie
      (nazwa nieistniejacego pliku)      |                                   |
      * 9                                |                                   |
    ============================================================================================================
     
     2. Sprawdzenie wczytywania plikow - 2
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      | Dane wyjsciowe                    |  Komentarz
      * 1                                |  * 'Poprawnie wczytano            |    * Przy poprawnych danych
      * kubus.pgm                        |    plik "kubus.pgm"'              |      program dziala poprawnie
        (poprawna nazwa pliku)           |                                   |
      * 9                                |                                   |
    ============================================================================================================
    
     3. Sprawdzenie wczytywania plikow - 3
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      | Dane wyjsciowe                    |  Komentarz
      * 1                                | * 'Poprawnie wczytano             |   * Poprawne dzialanie
      * kubus.pgm                        |    plik "kubus.pgm"'              |
      (poprawna nazwa pliku)             |                                   |
      * 1                                |                                   |
      * 2                                |                                   |
      * 9                                |                                   |
    ============================================================================================================

      4. Sprawdzenie wczytywania plikow - 4
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 1                                | * 'Poprawnie wczytano             |    * Poprawne dzialanie
      * kubus.pgm                        |    plik "kubus.pgm"'              |
      (poprawna nazwa pliku)             |                                   |
      * 1                                |                                   |
      * 1                                |  * 'Nie udalo sie wczytac         |
      * abc.pgm                          |   zawartosci pliku "abc.pgm"!     |
      (nazwa nieistniejacego pliku)      |   Przywrocono poprzedni obraz'    |
      * 9                                |                                   |
    ============================================================================================================

      5. Sprawdzenie wczytywania plikow - 5
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 1                                | * 'Poprawnie wczytano             |    * Poprawne dzialanie
      * kubus.pgm                        |    plik "kubus.pgm"'              |
      (poprawna nazwa pliku)             |                                   |
      * 1                                |                                   |
      * 1                                | * 'Poprawnie wczytano             |
      * kubus2.pgm                       |    plik "kubus2.pgm"'             |
      (nazwa istniejacego pliku)         |                                   |
      * 9                                |                                   |
    ============================================================================================================

      6. Sprawdzenie wczytywania plikow - 6
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 1                                | *                                 |    * Program sie
      * (jakakolwiek nazwa               |                                   |      "wysypuje"
          zawierajaca " " - spacje)      |                                   |
    ============================================================================================================

      7. Sprawdzenie zapisywania plikow - 1
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 1                                | * 'Poprawnie wczytano             |    * Poprawne dzialanie
      * kubus.pgm                        |    plik "kubus.pgm"'              |
      (poprawna nazwa pliku)             |                                   |
      * 2                                |                                   |
      * 'loremipsum.pgm'                 | * 'Poprawnie zapisano do          |
      * 9                                |   pliku "loremipsum.pgm"'         |
    ============================================================================================================


      8. Sprawdzenie zapisywania plikow - 2
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 1                                | * 'Poprawnie wczytano             |    * Program sie
      * kubus.pgm                        |    plik "kubus.pgm"'              |      "wysypuje"
      (poprawna nazwa pliku)             |                                   |
      * 2                                |                                   |
      * (jakakolwiek nazwa               |                                   |
          zawierajaca " " - spacje)      |                                   |
                                         |                                   |
    ============================================================================================================
    
      9. Sprawdzenie wyswietlania
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 1                                | * 'Poprawnie wczytano             |    * Poprawne dzialanie
      * kubus.pgm                        |    plik "kubus.pgm"'              |     
      (poprawna nazwa pliku)             | * 'Wyswietlono obraz'             |
      * 3                                |                                   |
      * 9                                |                                   |
    ============================================================================================================

      10. Sprawdzenie negatywu
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 1                                | * 'Poprawnie wczytano             |    * Poprawne dzialanie
      * kubus.pgm                        |    plik "kubus.pgm"'              |     
      (poprawna nazwa pliku)             | * 'Pomyslnie nalozono             |
      * 4                                |    negatyw'                       |
      * 9                                |                                   |
    ============================================================================================================

      11. Sprawdzenie konturowania
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 1                                | * 'Poprawnie wczytano             |    * Poprawne dzialanie
      * kubus.pgm                        |    plik "kubus.pgm"'              |     
      (poprawna nazwa pliku)             | * 'Konturowanie                   |
      * 5                                |    zakonczone'                    |
      * 9                                |                                   |
    ============================================================================================================

      12. Sprawdzenie progowania
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 1                                | * 'Poprawnie wczytano             |    * Poprawne dzialanie
      * kubus.pgm                        |    plik "kubus.pgm"'              |     
      (poprawna nazwa pliku)             |                                   |
      * 6                                |                                   |
      * -1                               | * 'Niepoprawna wartosc!'          |
      * 101                              |    Sprobuj ponownie' x2           |
      * 50                               | * 'Zastosowano progowanie         |
      * 9                                |    (Prog: 50.0%)'                 |
    ============================================================================================================
      
      13. Sprawdzenie rozmycia - 1
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 1                                | * 'Poprawnie wczytano             |    * Poprawne dzialanie
      * kubus.pgm                        |    plik "kubus.pgm"'              |     
      (poprawna nazwa pliku)             |                                   |
      * 7                                |                                   |
      * 1 / 2                            | * 'Rozmyto w pionie/poziomie      |
      * 7                                |                                   |
      * 0                                |                                   |
      * 9                                |                                   |
    ============================================================================================================

      14. Sprawdzenie rozmycia - 2
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 1                                | * 'Poprawnie wczytano             |    * Poprawne dzialanie
      * kubus.pgm                        |    plik "kubus.pgm"'              |     
      (poprawna nazwa pliku)             |                                   |
      * 7                                |                                   |
      * -1 / 3 (wartosci spoza przedz)   | * 'Nie ma takiej opcji,           |
      * 0                                |    wybierz ponownie!'             |
      * 9                                |                                   |
    ============================================================================================================

      15. Sprawdzenie masek - 2
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 1                                | * 'Poprawnie wczytano             |    * Poprawne dzialanie
      * kubus.pgm                        |    plik "kubus.pgm"'              |     
      (poprawna nazwa pliku)             |                                   |
      * 8                                |                                   |
      * 1                                |                                   |
      * 9 kolejnych liczb z przedzialu   |                                   |
      * 1                                | * 'Pomyslnie nalozono maske'      |
      * 9                                |                                   |
    ============================================================================================================

      16. Sprawdzenie masek - 2
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 1                                | * 'Poprawnie wczytano             |    * Poprawne dzialanie
      * kubus.pgm                        |    plik "kubus.pgm"'              |     
      (poprawna nazwa pliku)             |                                   |
      * 8                                |                                   |
      * 1                                |                                   |
      * Wartosc spoza przedzialu         | * 'Nieprawidlowa wartosc,         |
      * 9 kolejnych liczb z przedzialu   |   sprobuj jeszcze raz!'           |
      * 1                                | * 'Pomyslnie nalozono maske'      |
      * 9                                |                                   |
    ============================================================================================================

      17. Sprawdzenie masek - 3
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 1                                | * 'Poprawnie wczytano             |    * Poprawne dzialanie
      * kubus.pgm                        |    plik "kubus.pgm"'              |     
      (poprawna nazwa pliku)             |                                   |    * Program wyswietla zawsze 
      * 8                                |                                   |      ostrzezenie, gdy uzytkwnik
      * 2                                |                                   |      chce wybrac wartosc spoza
      * 1-7                              |                                   |      podanego przedzialu i kaze
      * sprawdzenie wszystkich podmenu   |   * Stosowne komunikaty           |      wybrac jeszcze raz
      *                                  |                                   |                        
      *                                  |                                   |    * Po wybraniu poprawnej maski
      * 9                                |                                   |      jest zawsze nakladana
    ============================================================================================================

      18. Sprawdzenie masek - 4
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 1                                | * 'Poprawnie wczytano             |    *                    
      * kubus.pgm                        |    plik "kubus.pgm"'              |      
      (poprawna nazwa pliku)             |                                   |    *  "Wysypanie" sie      
      * 8                                |                                   |       programu               
      * 2                                |                                   |                              
      * 8                                |                                   |                             
      * klawisz + enter                  |                                   |                        
      *                                  |                                   |                        
      *                                  |                                   |                                 
      * 9                                |                                   |                              
    ============================================================================================================

      19. Sprawdzenie masek - 5
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 1                                | * 'Poprawnie wczytano             |    *   Poprawne dzialanie
      * kubus.pgm                        |    plik "kubus.pgm"'              |      programu
      (poprawna nazwa pliku)             |                                   |                            
      * 8                                |                                   |                              
      * 2                                |                                   |                              
      * 8                                |                                   |                             
      * enter                            |                                   |                        
      *                                  |                                   |                        
      *                                  |                                   |                                 
      * 9                                |                                   |                              
    ============================================================================================================

      19. Sprawdzenie masek - 6
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 1                                | * 'Poprawnie wczytano             |    *   Poprawne dzialanie
      * kubus.pgm                        |    plik "kubus.pgm"'              |        programu
      (poprawna nazwa pliku)             |                                   |    
      * 8                                |                                   |    
      * 2                                |                                   |                              
      * 9                                |                                   |                             
      *                                  |  *'Nie nalozono maski'            |                        
      *                                  |                                   |                        
      *                                  |                                   |                                 
      * 9                                |                                   |                              
    ============================================================================================================

      20. Sprawdzenie wyjscia z programu - 1
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 9                                |                                   |    *   Poprawne dzialanie
                                         |                                   |        programu                               
    ============================================================================================================

      21. Sprawdzenie wyjscia z programu - 2
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 1                                | * 'Poprawnie wczytano             |    *   Poprawne dzialanie
      * kubus.pgm                        |    plik "kubus.pgm"'              |        programu
      (poprawna nazwa pliku)             |                                   | 
      * 9                                |                                   |   
                                         |                                   |                                  
    ============================================================================================================

    22. Sprawdzenie zabezpieczenia przed niewczytaniem pliku
    ------------------------------------------------------------------------------------------------------------
     Dane wejsciowe                      |  Dane wyjsciowe                   |  Komentarz
      * 2-8                              |  * 'Najpierw wczytaj jakis plik'  |    *   Poprawne dzialanie
                                         |                                   |        programu
                                         |                                   | 
      * 9                                |                                   |   
                                         |                                   |                                  
    ============================================================================================================

Wnioski:
  Program powinien dzialac poprawnie, gdy

  - Podczas wczytywania/zapisywania nie zostanie podana nazwa ze spacja
  - Przy wyswietlaniu informacji o maskach, uzytkownik nie wcisnie innego przycisku niz enter

*/