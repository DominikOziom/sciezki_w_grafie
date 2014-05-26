#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <limits.h>
#include <math.h>
#include <sys/time.h>
#include <fstream>
#include <list>
#include <iomanip>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

//WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
//WWWWW  DEFINICJA KLASY IMPLEMENTUJACEJ WIERZCHOLEK   WWW
//WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
template <typename Typ>
class wierzcholek{
public:
  int key;  // oryginalny klucz wierzcholka
  Typ val;   // przechowywana wartosc
};
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
//KKKKK  DEFINICJA KLASY IMPLEMENTUJACEJ KRAWEDZ  KKKKKKKK
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
template <typename Typ>
class krawedz{
public:
  int waga;  // wartosc krawedzi
  wierzcholek<Typ> *first;  // wskaznik na pierwszy wierzcholek
  wierzcholek<Typ> *second;     // wskaznik na drugi wierzcholek

  krawedz();  // konstruktor
  ~krawedz(); // destruktor
};
// KONSTRUKTOR
template <typename Typ>
krawedz<Typ>::krawedz(){
  waga=0;
  first=NULL;
  second=NULL;
}
// DESTRUKTOR
template <typename Typ>
krawedz<Typ>::~krawedz(){
  first=NULL;
  second=NULL;
}
// KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
// KKKK DEFINICJA KLASY COMPARE (REGULA SORTOWANIA HEAP) KKKK
// KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
template <typename Typ>
class Compare{
public:
  bool operator()(const krawedz<Typ> x, const krawedz<Typ> y) const
  {
    return x.waga > y.waga;
  } 
};
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//MM DEFINICJA KLASY IMPLEMETUJACEJ MACIERZ SASIEDZTWA MM
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
template <typename Typ>
class MacierzS{
public:

  krawedz<Typ>** tab;    // tablica dynamiczna dwuwymiarowa (struktura grafu)
  krawedz<Typ>** tab2;    // tablica dynamiczna dwuwymiarowa (struktura grafu)
  wierzcholek<Typ> *Twierzch;   // tablica dynamiczna jednowym. z wartosciami wierzcholkow
  bool *visited;  // tablica z informacja czy dany wierzcholek zostal juz odwiedzony
  int l_wierzch;   // zmienna przechowujaca liczbe wierzcholkow
  int l_kraw;   // zmienna przechowujaca liczbe krawedzi
  vector<int> wagi_dojscia;
  vector<int> sciezki_dojscia;
  vector<int> Poprzednicy; // wektor poprzednikow w drogach (dijkstra)
  int w_start;  // klucz wierzcholka startowego


  MacierzS();  // konstruktor
  ~MacierzS(); // destruktor
  void ZmienWielkosc(int l_w, int l_k);  // zmienia wielkosc tablicy wg zadanych param
  int operator () (int W, int K) const {return tab[W][K];} // przeciaza operator () zwraca wartosc
  int & operator () (int W, int K) {return tab[W][K];} // przeciaza operator () - zwraca referencje
  bool WczytajZPliku(string NazwaPliku); // wczytuje graf z pliku
  bool ZapiszDoPliku(string NazwaPliku); // zapisuje graf do pliku
  void Print();  // drukuje macierz na standardowym wyjsciu
  void GenerujGraf(int l_w, int gestosc, int CzyUjemne, int PrawdUjemnej);  // generuje losowy graf
  void DFS(int wierzch_start);   // przechodzenie grafu w glab
  bool CzySpojny();   // sprawdza czy graf jest spojny

  void Przygotowanie_tab_wag_i_sciezek(int ind_pocz); // wypelnia wektory inf i -1
  void Bellman_Ford(int ind_pocz);  // alg. Bellmana-Forda dla macierzy
  void PrintSciezki(); // drukuje wynik dzialania algorytmu B-F
  void Dijkstra(int w_pocz); // alg. Dijkstry
};

// konstruktor
template <typename Typ>
MacierzS<Typ>::MacierzS(){
  int i,j;
  l_wierzch=0;
  l_kraw=0;
  Twierzch=new wierzcholek<Typ> [l_wierzch]; // alokacja pamieci dla tablicy wierzcholkow
  visited = new bool [l_wierzch];        // alokacja pamieci tablicy odwiedzin

  tab=new krawedz<Typ>* [l_wierzch]; // alokacja pamieci dla pierwszych elementow wierszy
  for (i=0; i<l_wierzch; i++)
    tab[i]=new krawedz<Typ> [l_wierzch];  // alokacja pamieci dla wierszy

  tab2=new krawedz<Typ>* [l_wierzch]; // alokacja pamieci dla pierwszych elementow wierszy
  for (i=0; i<l_wierzch; i++)
    tab2[i]=new krawedz<Typ> [l_wierzch];  // alokacja pamieci dla wierszy

  for (i=0; i<l_wierzch; i++){
    visited[i]=false;         // wyzerowanie tablicy odwiedzin
    for (j=0; j<l_wierzch; j++)
      (tab[i][j]).waga=0;       // wyzerowanie macierzy
  }
}

// destruktor
template <typename Typ>
MacierzS<Typ>::~MacierzS(){
  int i;
  delete Twierzch;
  delete visited;
  for (i=0; i<l_wierzch; i++)
    delete [] tab[i];
  delete tab;

  for (i=0; i<l_wierzch; i++)
    delete [] tab2[i];
  delete tab2;

  l_wierzch=0;
  l_kraw=0;
}
// Zwalnia zajeta pamiec i rezerwuje na nowo o zadanych parametrach
template <typename Typ>
void MacierzS<Typ>::ZmienWielkosc(int l_w, int l_k){
  int i,j;
  delete Twierzch;   // zwolnienie aktualnie zajetej pamieci
  delete visited;
  for (i=0; i<l_wierzch; i++)
    delete [] tab[i];
  delete tab;
  l_wierzch=l_w;
  l_kraw=l_k;
  Twierzch=new wierzcholek<Typ> [l_wierzch]; // alokacja pamieci dla tablicy wierzcholkow
  visited = new bool [l_wierzch];      // alokacja pamieci dla tablicy odwiedzin

  tab=new krawedz<Typ>* [l_wierzch]; // alokacja pamieci dla pierwszych elementow wierszy
  for (i=0; i<l_wierzch; i++)
    tab[i]=new krawedz<Typ> [l_wierzch];  // alokacja pamieci dla wierszy

  tab2=new krawedz<Typ>* [l_wierzch]; // alokacja pamieci dla pierwszych elementow wierszy
  for (i=0; i<l_wierzch; i++)
    tab2[i]=new krawedz<Typ> [l_wierzch];  // alokacja pamieci dla wierszy

  for (i=0; i<l_wierzch; i++){
    visited[i]=false;         // wyzerowanie tablicy odwiedzin
    for (j=0; j<l_wierzch; j++)
      (tab[i][j]).waga=0;       // wyzerowanie macierzy
  }
}
/* Funkcja wczytuje graf z pliku.
 * Najpierw nastepuje proba otworzenia pliku.
 * W przypadku powodzenia pobrane z niego sa: liczba wierzcholkow,
 * liczba krawedzi, a nastepnie cala macierz.
 */
template <typename Typ>
bool MacierzS<Typ>::WczytajZPliku(string NazwaPliku){
  fstream plik(NazwaPliku.c_str(), ios::in);  // otworzenie pliku
  if(!plik.is_open()){
    cerr << "Blad otwarcia pliku" << endl;
    return false;              // niepowodzenie otwarcia pliku o zadanej nazwie
  }
  int i,wag,V1,V2,l_w,l_k,ind_w_pocz;
  plik >> l_k;
  plik >> l_w;
  plik >> ind_w_pocz;
  w_start=ind_w_pocz;

  ZmienWielkosc(l_w,l_k);

  for (i=0; i<l_wierzch; i++){   // wczytanie wartosci wierzcholkow
    plik >> (Twierzch[i]).val;
    (Twierzch[i]).key=i;
  }

  for (i=0; i<l_kraw; i++){     // wczytanie krawedzi z ich wagami
    plik >> V1 >> V2 >> wag;
    (tab[V1][V2]).waga=(tab[V2][V1]).waga=wag;
    (tab[V1][V2]).first=(&Twierzch[V1]);   // przypisanie wskaznikow na wierzcholki
    (tab[V2][V1]).first=(&Twierzch[V2]);
    (tab[V1][V2]).second=(&Twierzch[V2]);
    (tab[V2][V1]).second=(&Twierzch[V1]);
  }
  return true;
}

/* Funkcja zapisuje graf do pliku.
 * Najpierw nastepuje proba otworzenia pliku.
 * W przypadku powodzenia zapisane do niego sa: liczba wierzcholkow,
 * liczba krawedzi, a nastepnie cala macierz.
 */
template <typename Typ>
bool MacierzS<Typ>::ZapiszDoPliku(string NazwaPliku){
  fstream plik(NazwaPliku.c_str(), ios::out);
  if(!plik.is_open()){
    cerr << "Blad otwarcia pliku" << endl;
    return false;              // niepowodzenie otwarcia pliku o zadanej nazwie
  }
  int w, k;

  plik << l_kraw << " " << l_wierzch << " " << w_start << endl;

  for (w=0; w<l_wierzch;w++)      // zapisanie kolejnych wartosci wierzcholkow
    plik << Twierzch[w].val << endl;

  for (w=0; w<l_wierzch; w++){  // 'wybiera' kolejne wiersze
    for (k=0; k<l_wierzch; k++){   // 'wybiera' kolejne komorki wiersza
      plik << w << " " << k << " " << tab[w][k].waga << endl;  // zapisuje nr wierzch start, nr wierzch. konc, wage,
    }
  }
  return true;
}

// drukuje macierz na standardowym wyjsciu
template <typename Typ>
void MacierzS<Typ>::Print(){
  int w,k;
  int z=0;
  cout << endl;
  for (w=0; w<l_wierzch; w++){
    for (k=0; k<l_wierzch; k++){
      if(tab[w][k].waga!=0){
	cout << setw(12) << tab[w][k].waga << " ";  // druk wag
      }
      else
	cout << setw(12) << z << " ";   // druk zer
    }
    cout << endl;
  }
  
}
/* Funkcja generuje losowy graf o zadanej liczbie wierzcholkow
 * i zadanej gestosci.
 * Jesli CzyUjemne = 1 będą występować ujemne wagi dla 0 nie.
 * PrawdUjemnej - definiuje prawdopodobienstwo wystapienia wagi ujemnej [%].
 */
template <typename Typ>
void MacierzS<Typ>::GenerujGraf(int l_w, int gestosc, int CzyUjemne, int PrawdUjemnej){
  int i,j;
  srand (time(NULL));
  ZmienWielkosc(l_w,0);
  for (i=0; i<l_wierzch; i++){    // wygenerowanie tablicy wierzcholkow
    Twierzch[i].val=rand()%INT_MAX; // generowanie wartosci wierzcholka
    Twierzch[i].key=i;   // przypisanie klucza
  }
  for (i=0; i<l_wierzch; i++){    // wygenerowanie macierzy
    for (j=0; j<l_wierzch; j++){
      if((rand()%100)<=gestosc && (i!=j)){   // warunek zachowania gestosci
	(tab[i][j]).waga=rand()%(INT_MAX/1000)+1;   // losowa waga 1-INT_MAX/1000
	(tab[i][j]).first=&Twierzch[i];   // przypisanie wskaznika na 1 wierzcholek
	(tab[i][j]).second=&Twierzch[j];   // przypisanie wskaznika na 2 wierzcholek
	l_kraw++;
      }
      else{
	(tab[i][j]).waga=(tab[j][i]).waga=0;      // zero dla zachowania gestosci
      }
    }
  }
  if (CzyUjemne){
    for (i=0; i<l_wierzch; i++){
      for (j=0; j<l_wierzch; j++){
	if(rand()%100<PrawdUjemnej){    // szansa na wystapienie ujemnej wagi
	  tab[i][j].waga*=-1;}   // zmiana znaku 
      }
    }
  }
}
// ALGORYTM REKURENCYJNY DFS
// przechodzi przez cala macierz zapisujac wierzcholki
// w ktorych juz byl ( tablica bool visited )
template <typename Typ>
void MacierzS<Typ>::DFS(int wierzch_start){
  int i;
  visited[wierzch_start]=true;
  for (i=0; i<l_wierzch; i++)
    if((tab[wierzch_start][i].waga!=0) && !visited[i])
      DFS(i);
}
// CZY SPOJNY
template <typename Typ>
bool MacierzS<Typ>::CzySpojny(){
  int i,odwiedzone=0;
  DFS(0);
  for (i=0; i<l_wierzch; i++)
    if (visited[i]==true)
      odwiedzone++;
  if (odwiedzone==l_wierzch) return true;
  else return false;
}
// FUNKCJA WYPELNIAJACA WEKTOR WAG BARDZO DUZYMI WARTOSCIAMI
// WEKTOR SCIEZEK WARTOSCIAMI -1
template <typename Typ>
void MacierzS<Typ>::Przygotowanie_tab_wag_i_sciezek(int ind_pocz){
  int INF=INT_MAX/2;
  int brak=-1;

  wagi_dojscia.resize(l_wierzch);
  sciezki_dojscia.resize(l_wierzch);

  for (int i=0; i<l_wierzch; i++){    
    wagi_dojscia[i]=INF; // wypelnienie bardzo duzymi wartosciami wektora wag
    sciezki_dojscia[i]=brak; // wypelnienie -1 wektora sciezek
    }
  wagi_dojscia[ind_pocz]=0; // darmowe dostanie sie do wierzcholka poczatkowego
}
// BBBBBBBBBBBBBBBBBBBBBBBBBFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
//                    BELLMAN-FORD
template <typename Typ>
void MacierzS<Typ>::Bellman_Ford(int ind_pocz){
  Przygotowanie_tab_wag_i_sciezek(ind_pocz);

  for(int a=1; a<=l_wierzch; a++){ // warunek relaksacji (n-1 przejsc) 

    for (int j=0; j<l_wierzch; j++){
      for(int k=0; k<l_wierzch; k++){
	if(tab[j][k].waga!=0){
	  if(wagi_dojscia[k]>(wagi_dojscia[j]+tab[j][k].waga)){ // jesli wieksza waga albo inf
	    wagi_dojscia[k]=(wagi_dojscia[j]+tab[j][k].waga);  // zmiana najmnieszjej wagi dojscia
	    sciezki_dojscia[k]=j;  //zmiana wierzch poprzedzajacego
	  }
	}
      }}
  }
}
// DRUKUJE WEKTOR NAJMNIEJSZYCH WAG SCIEZEK DLA KAZDEGO WIERZCHOLKA
// I NAJKROTSZA DROGE DOJSCIA Z ZADANEGO WIERZCH DO KAZDEGO INNEGO
template <typename Typ>
void MacierzS<Typ>::PrintSciezki(){


 for (int p=0; p<l_wierzch; p++){
    cout << setw(12) << p << " ";  // druk nr wierzcholkow
  }
  cout << endl;

  for (int w=0; w<l_wierzch; w++){
    cout << setw(12) << wagi_dojscia[w] << " ";  // druk wag
  }
  cout << endl;

for (int t=0; t<l_wierzch; t++){
    cout << setw(12) << t << " ";  // druk nr wierzcholkow
  }
  cout << endl;

  for (int k=0; k<l_wierzch; k++){
    cout << setw(12) << sciezki_dojscia[k] << " ";  // druk poprzednikow
  }
  cout << endl;
}


vector<int> Odleglosci; // wektor wynikowy algorytmu dijkstry

// F POROWNUJACA DO DRZEWA
struct compare{
  bool operator()(const int x, const int y) const
  {
    if(Odleglosci[x]<Odleglosci[y]) return true;
    if(Odleglosci[x]>Odleglosci[y]) return false;
    return x < y;
  } 
};

//DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD
//                                   DIJKSTRA
template <typename Typ>
void MacierzS<Typ>::Dijkstra(int w_pocz)
{
 
  vector<int> Kolor;
  
  for(int z=0; z<l_wierzch; z++) Kolor.push_back(0);
 
  int u, wag;
  set<int, compare> drzewo; // drzewo do wyszukiwania wierzcholka o najmniejszej odl
 
  Odleglosci.clear(); //czyszczenie wektora
  Odleglosci.resize(l_wierzch, INT_MAX/2);

  Poprzednicy.clear(); //czyszczenie wektora
  Poprzednicy.resize(l_wierzch, 0);
 
  Odleglosci[w_pocz]=0; 
  drzewo.clear(); 
  drzewo.insert(w_pocz);
 
  while(!drzewo.empty())
    {
      u= *(drzewo.begin());      
      drzewo.erase(drzewo.begin());
      Kolor[u]=1;
      for(int i=0; i< l_wierzch; i++) //petla szukajaca sasiadow w macierzy
	{	 
	  if(tab[u][i].waga!=0){
	    if(Kolor[i]==0){
	      wag=tab[u][i].waga;
	      drzewo.insert(i);
	      if(Odleglosci[u]+wag<Odleglosci[i])
		{	
		  drzewo.erase(drzewo.find(i));
		  Odleglosci[i]=Odleglosci[u]+wag;
		  Poprzednicy[i]=u;
		  drzewo.insert(i);
		}
	    
	    }
	  }
	}
    }
}


// LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
// LLLLLLLLLLL    LISTA SASIEDZTWA    LLLLLLLLLLLLLLLLLLLLLLLLLLL
// LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
template <typename Typ>
class ListaS: public vector<list<wierzcholek<Typ> > >{
public:
  krawedz<Typ> *Tkraw;   // tablica dynamiczna jednowym. z krawedziami
  int l_wierzch;   // zmienna przechowujaca liczbe wierzcholkow
  int l_kraw;   // zmienna przechowujaca liczbe krawedzi
  vector<int> wagi_dojscia;
  vector<int> sciezki_dojscia;
  vector<int> Poprzednicy; // wektor poprzednikow w sciezkach (dijsktra)
  int w_start;  // indeks wierzcholka startowego

  ListaS();  // konstruktor
  ~ListaS(); // destruktor
  void ZmienWielkosc(int l_w, int l_k); // zmiana wielkosci struktur danych
  void DodajWierzcholek(wierzcholek<Typ> W, int ind);  // dodaje wierzcholek do odpowiedniej listy
  void DodajKrawedz(krawedz<Typ> K, int ind);  // dodaje krawedz do tabeli i drugi wierzcholek do listy
  bool WczytajZPliku(string NazwaPliku); // wczytuje graf z pliku
  bool ZapiszDoPliku(string NazwaPliku); // zapisuje graf do pliku
  void Print();  // drukuje liste na standardowym wyjsciu
  void GenerujGraf(int l_w, int gestosc, int CzyUjemne, int PrawdUjemnej);  // generuje graf losowy

  void Przygotowanie_tab_wag_i_sciezek(int ind_pocz); // wypelnia wektory inf i -1
  void Bellman_Ford(int ind_pocz);  // alg. Bellmana-Forda dla listy
  void PrintSciezki();  // drukuje wynik dzialania alg. B-F
  void Dijkstra(int w_pocz); // dijkstra dla listy,  w_pocz-indeks wierzcholka startowego
};
// KONSTRUKTOR
template <typename Typ>
ListaS<Typ>::ListaS(){
  l_wierzch=0;
  l_kraw=0;
  Tkraw=new krawedz<Typ> [l_kraw];  // alokacja pamieci dla tablicy krawedzi
}
// DESTRUKTOR
template <typename Typ>
ListaS<Typ>::~ListaS(){
  int i;
  for (i=0; i<l_wierzch; i++)
    delete (*this)[i];
  delete (*this);
  delete Tkraw;    // usuniecie tablicy krawedzi
}
// ZMIENIA MAX WIELKOSC TABLICY I LISTY
template <typename Typ>
void ListaS<Typ>::ZmienWielkosc(int l_w, int l_k){
  int i;
  list<wierzcholek<Typ> > T;
  l_wierzch=l_w;
  l_kraw=l_k;
  Tkraw=new krawedz<Typ> [l_k];
  for (i=0; i<l_w; i++){
    (*this).push_back(T);
  }
}
// DODAJE WIERZCHOLEK
template <typename Typ>
void ListaS<Typ>::DodajWierzcholek(wierzcholek<Typ> W, int ind){
  (*this)[ind].push_back(W);
}
// DODAJE KRAWEDZ
template <typename Typ>
void ListaS<Typ>::DodajKrawedz(krawedz<Typ> K, int ind){
  Tkraw[ind]=K;                            // dodaje krawedz do tabeli krawedzi
  DodajWierzcholek((*this)[K.second->key].front(),K.first->key); // dodaje drugi wierzcholek do listy
}
// WCZYTUJE GRAF Z PLIKU
template <typename Typ>
bool ListaS<Typ>::WczytajZPliku(string NazwaPliku){
  fstream plik(NazwaPliku.c_str(), ios::in);  // otworzenie pliku
  if(!plik.is_open()){
    cerr << "Blad otwarcia pliku" << endl;
    return false;              // niepowodzenie otwarcia pliku o zadanej nazwie
  }
  wierzcholek<Typ> W;
  krawedz<Typ> K;
  int i,wag,wart,V1,V2,l_w,l_k,w_pocz;
  plik >> l_k;
  plik >> l_w;
  plik >> w_pocz;
  w_start=w_pocz;
  ZmienWielkosc(l_w,l_k);

  for (i=0; i<l_w; i++){   // wczytanie wartosci wierzcholkow
    plik >> wart;
    W.val=wart;
    W.key=i;
    DodajWierzcholek(W,i);
  }

  for (i=0; i<l_k; i++){     // wczytanie krawedzi z ich wagami
    plik >> V1 >> V2 >> wag;
    K.waga=wag;
    K.first=&(*this)[V1].front();
    K.second=&(*this)[V2].front();
    DodajKrawedz(K,i);
  }
  return true;
}
// ZAPISUJE GRAF DO PLIKU
template <typename Typ>
bool ListaS<Typ>::ZapiszDoPliku(string NazwaPliku){
  fstream plik(NazwaPliku.c_str(), ios::out);
  if(!plik.is_open()){
    cerr << "Blad otwarcia pliku" << endl;
    return false;              // niepowodzenie otwarcia pliku o zadanej nazwie
  }
  int w;
  plik << l_kraw << " " << l_wierzch << " " << w_start << endl;

  for (w=0; w<l_wierzch;w++)      // zapisanie kolejnych wartosci wierzcholkow
    plik << (*this)[w].begin()->val << endl;

  for (w=0; w<l_kraw; w++){  // 'wybiera' kolejne krawedzie
    plik << Tkraw[w].first->key<< " " << Tkraw[w].second->key << " " << Tkraw[w].waga << endl;
  }
  return true;
}
// DRUKUJE GRAF
template <typename Typ>
void ListaS<Typ>::Print(){
  int i;
  cout << l_wierzch << " " << l_kraw << endl;
  for (i=0; i<l_wierzch; i++){   // drukowanie wierzcholkow
    cout << (*this)[i].begin()->val << endl;
  }
  for (i=0; i<l_kraw; i++){   // drukowanie wierzcholka - krawedzi - wierzcholka
    cout << Tkraw[i].first->key << " " << Tkraw[i].waga << " " << Tkraw[i].second->key << endl;
  }
}
//GENERUJE GRAF O ZADANEJ L WIERZCHOLKOW I GESTOSCI
template <typename Typ>
void ListaS<Typ>::GenerujGraf(int l_w, int gestosc, int CzyUjemne, int PrawdUjemnej){
  int i,j,y=0;
  int **tab;

  tab=new int* [l_w]; // alokacja pamieci dla pierwszych elementow wierszy
  for (i=0; i<l_w; i++)
    tab[i]=new int [l_w];  // alokacja pamieci dla wierszy

  wierzcholek<Typ> W;
  krawedz<Typ> K;
  srand (time(NULL));

  for (i=0; i<l_w; i++){    // wygenerowanie macierzy wag
    for (j=0; j<l_w; j++){
      if((rand()%100)<=gestosc && (i!=j)){
	tab[i][j]=rand()%(INT_MAX/1000)+1;   // losowa waga 1-INT_MAX
	if (CzyUjemne){
	  if(rand()%100<PrawdUjemnej){
	    tab[i][j]*=-1;}   // zmiana znaku 
	}
	y++;
      }
      else{
	tab[i][j]=0;          // zero dla zachowania gestosci
      }
    }
  }
 
  ZmienWielkosc(l_w,y);

  for (i=0; i<l_w; i++){   // wygenerowanie wierzcholkow
    W.val=rand()%INT_MAX;
    W.key=i;
    DodajWierzcholek(W,i);
  }
  y=0;
  for (i=0; i<l_w; i++){   //przeksztalcenie macierzy 2W na tablice 1W
    for (j=0; j<l_w; j++){
      if(tab[i][j]!=0){
	K.waga=tab[i][j];
	K.first=&(*this)[i].front();
	K.second=&(*this)[j].front();
	DodajKrawedz(K,y);
	y++;
      }
    }
  }

  for (i=0; i<l_wierzch; i++)  // usuniecie pomocniczej tablicy
    delete [] tab[i];
  delete tab;
}
// FUNKCJA WYPELNIAJACA WEKTOR WAG BARDZO DUZYMI WARTOSCIAMI
// WEKTOR SCIEZEK WARTOSCIAMI -1
template <typename Typ>
void ListaS<Typ>::Przygotowanie_tab_wag_i_sciezek(int ind_pocz){
  int INF=INT_MAX/2;
  int brak=-1;

  wagi_dojscia.resize(l_wierzch);
  sciezki_dojscia.resize(l_wierzch);

  for (int i=0; i<l_wierzch; i++){    
    wagi_dojscia[i]=INF; // wypelnienie bardzo duzymi wartosciami wektora wag
    sciezki_dojscia[i]=brak; // wypelnienie -1 wektora sciezek
    }
  wagi_dojscia[ind_pocz]=0; // darmowe dostanie sie do wierzcholka poczatkowego
}
// BBBBBBBBBBBBBBBBBBBBBBBBBFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
//                    BELLMAN-FORD
template <typename Typ>
void ListaS<Typ>::Bellman_Ford(int ind_pocz){
  int prev_key, next_key, wag;
  Przygotowanie_tab_wag_i_sciezek(ind_pocz);

  for(int a=1; a<=l_wierzch; a++){ // warunek relaksacji (n-1 przejsc) 
    for (int j=0; j<l_kraw; j++){  // przejscie po wszystkich krawedziach
      prev_key=Tkraw[j].first->key;
      next_key=Tkraw[j].second->key;
      wag=Tkraw[j].waga;
      if(wagi_dojscia[next_key]>(wagi_dojscia[prev_key]+wag)){ //jesli ma wage wieksza lub (inf)
	wagi_dojscia[next_key]=(wagi_dojscia[prev_key]+wag); //zmiana wagi dojscia na mniejsza
	sciezki_dojscia[next_key]=prev_key; //zmiana wierzch poprzedzajacego
	  }
	}
      }
  }
// DRUKUJE WEKTOR NAJMNIEJSZYCH WAG SCIEZEK DLA KAZDEGO WIERZCHOLKA
// I NAJKROTSZA DROGE DOJSCIA Z ZADANEGO WIERZCH DO KAZDEGO INNEGO
template <typename Typ>
void ListaS<Typ>::PrintSciezki(){


 for (int p=0; p<l_wierzch; p++){
    cout << setw(12) << p << " ";  // druk sciezek
  }
  cout << endl;

  for (int w=0; w<l_wierzch; w++){
    cout << setw(12) << wagi_dojscia[w] << " ";  // druk wag
  }
  cout << endl;

for (int t=0; t<l_wierzch; t++){
    cout << setw(12) << t << " ";  // druk sciezek
  }
  cout << endl;

  for (int k=0; k<l_wierzch; k++){
    cout << setw(12) << sciezki_dojscia[k] << " ";  // druk sciezek
  }
  cout << endl;
}

vector<int> Odleglosci2; // wektor wynikowy algorytmu dijkstry

// F POROWNUJACA DLA DRZEWA
struct compare2{
  bool operator()(const int x, const int y) const
  {
    if(Odleglosci2[x]<Odleglosci2[y]) return true;
    if(Odleglosci2[x]>Odleglosci2[y]) return false;
    return x < y;
  } 
};

//DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD
//                                    DIJKSTRA
template <typename Typ>
void ListaS<Typ>::Dijkstra(int w_pocz)
{ 
  vector<int> Kolor;
  
  for(int z=0; z<l_wierzch; z++) Kolor.push_back(0);
 
  int klucz2;
  int u, wag;
  set<int, compare2> drzewo; // drzewo do wyszukiwania wierzcholka o najmniejszej odl

  //<typename> list<wierzcholek<Typ> >::iterator it;

  Odleglosci2.clear(); //czyszczenie wektora
  Odleglosci2.resize(l_wierzch, INT_MAX/2);

  Poprzednicy.clear(); //czyszczenie wektora
  Poprzednicy.resize(l_wierzch, 0);
 
  Odleglosci2[w_pocz]=0;
  drzewo.clear(); 
  drzewo.insert(w_pocz);
 
  while(!drzewo.empty())
    {      
      u= *(drzewo.begin());      
      drzewo.erase(drzewo.begin()); //zdjecie wierzcholka z najmniejsza odlegloscia
      Kolor[u]=1;    
      for(typename list<wierzcholek<Typ> >::iterator it=(*this)[u].begin(); it != (*this)[u].end(); it++ ){

	klucz2=(it)->key;
	for(int q=0;q<l_kraw;q++){ // szuka krawedz kiedy dane 2 wierzcholki
	  if(Tkraw[q].first->key==u && Tkraw[q].second->key==klucz2 && Kolor[klucz2]==0){
	    wag=Tkraw[q].waga;
	    drzewo.insert(klucz2); // wrzuca na drzewo znaleziony wierzcholek
	  }
	}
      
	if(Odleglosci2[u]+wag<Odleglosci2[klucz2]) 
	  {
	    drzewo.erase(drzewo.find(klucz2));
	    Odleglosci2[klucz2]=Odleglosci2[u]+wag;
	    Poprzednicy[klucz2]=u;
	    drzewo.insert(klucz2);
	  }

      }
    }
}

// WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
// WWWWWWWWWWWWW GLOWNY PROGRAM WWWWWWWWWWWWWWWWWWWWWWW
// WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW

int main()
{
  cout << "Program grafy" << endl << endl;

  double difference, difference1; 
  timeval start, stop, start1,stop1;

  ListaS<int> *L=new ListaS<int>;
  MacierzS<int> *M=new MacierzS<int>;
  //M->WczytajZPliku("plik.txt");
  
  
  do{
    M->GenerujGraf(100,25,0,20);   // graf dla macierzy S
  }while(!(M->CzySpojny()));
  gettimeofday(&start,NULL);
  //M->Bellman_Ford(5);
  M->Dijkstra(5);
  gettimeofday(&stop,NULL);
  cout << endl << endl ;
  //M->PrintSciezki();
  
  L->GenerujGraf(100,25,0,20);    // graf dla listy S
  gettimeofday(&start1,NULL);
  //L->Bellman_Ford(5);
  L->Dijkstra(5);
  gettimeofday(&stop1,NULL);
  cout << endl << endl ;
  //L->PrintSciezki();

  //M->ZapiszDoPliku("plik.txt");
    cout << endl << "ALGORYTM BELLMANA-FORDA DLA MACIERZY" << endl;
    difference = (stop.tv_sec - start.tv_sec)*1000.0;
    difference += (stop.tv_usec - start.tv_usec) / 1000.0;
    cout << "czas pracy programu: " << difference << "ms" << endl;
    cout << "##################"<< endl;

    cout << endl << "ALGORYTM BELLMANA-FORDA DLA LISTY" << endl;
    difference1 = (stop1.tv_sec - start1.tv_sec)*1000.0;
    difference1 += (stop1.tv_usec - start1.tv_usec) / 1000.0;
    cout << "czas pracy programu: " << difference1 << "ms" << endl;
    cout << "##################"<< endl; 

  return 0;

}
