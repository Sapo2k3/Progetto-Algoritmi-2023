/*Matia Saponaro 0001071328 gruppo B mattia.saponaro2@studio.unibo.it */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
/* Dichiaro tutte le strutture di supporto al mio programma, in particolare una per
rappresentare la matrice con tutte le caratteristiche utili al funzionamento dell'algoritmo
e una per usare la coda di priorita` sfruttata nella BFS*/
typedef enum { WHITE, BLACK, GREY } colore;
typedef enum { wet, dry } status;

struct posizione {
	int x;
	int y;
};
typedef struct posizione posizione;

/* La struttura mt contiene: un valore che indica la posizione della singola casella all'interno della matrice, una
che contiene il valore da 0 a 9 per identificarla come "percorso" o "edificio", il colore (serve in BFS), uno
che indica se la casella è bagnata o meno un puntatore alla casella precedente nel percorso minimo,
la distanza dalla sorgente e quante caselle bagnate sono state colpite nel percorso fino alla casella corrente*/
struct mt {
	posizione pos;
	char val;
	colore colour;
	status stat;
	struct mt* pred;
	int dist;
	int pioggia;
};
typedef struct mt mat;

struct lista {
	posizione pos;
	struct lista* next;
};
typedef struct lista* queue;

static int righe, colonne;

/* queue_init() inizializza e mi restituisce una coda vuota una coda vuota */
queue* queue_init(void) {
	queue* Q = (queue*)malloc(sizeof(queue));
	if (Q != NULL) {
		*Q = NULL;
	}
	return Q;
}
/* is_empty() serve a controllare se la coda sia vuota o meno:
restituisce 1 in caso affermativo altrimenti 0*/
int is_empty(queue* Q) {
	return Q == NULL || (*Q == NULL);
}
/* node_alloc() alloca lo spazio necessario per aggiungere un nuovo nodo nella lista e restituisce un
puntatore a quel nodo*/
struct lista* node_alloc(int i, int j) {
	struct lista* tmp = (struct lista*)malloc(sizeof(struct lista));
	tmp->pos.x = i;
	tmp->pos.y = j;
	tmp->next = NULL;
	return tmp;
}
/* node_insert() inserisce l'elemento nella lista dopo aver eseguito node_alloc*/
void node_insert(struct lista* Q, int i, int j) {
	struct lista* tmp = node_alloc(i, j);
	if (tmp != NULL) {
		tmp->next = Q->next;
		Q->next = tmp;
	}
}

/* enqueue() serve a mettere in coda alla lista la posizione del nodo grigio appena
scoperto dalla BFS, se la lista e` vuota mi basta fare un inserimento in testa alla lista
altrimenti la devo scorrere (usando un puntatore alla testa) per inserire il nuovo nodo
in coda*/
void enqueue(queue* Q, int i, int j) {
	if (is_empty(Q)) {
		struct lista* tmp = node_alloc(i, j);
		tmp->next = *Q;
		*Q = tmp;
	}
	else {
		struct lista* tmp = *Q;
		while (tmp->next != NULL)
			tmp = tmp->next;
		node_insert(tmp, i, j);
		return;
	}
}

/* head_select() seleziona l'elemento in testa alla lista */
posizione head_select(queue* Q) {
	posizione p;
	p.x = 0;
	p.y = 0;
	if (is_empty(Q))
		return p;
	else {
		p.x = (*Q)->pos.x;
		p.y = (*Q)->pos.y;
		return p;
	}
}
/* head_delete() cancella l'elemento in testa alla lista, se la lista e`
vuota non fa niente altrimenti cancella la testa e fa una free del puntatore
temporaneo usato*/
void head_delete(queue* Q) {
	if (is_empty(Q))
		return;
	else {
		struct lista* tmp = *Q;
		*Q = (*Q)->next;
		free(tmp);
	}
}

/* dequeue() elimina il primo elemento della coda, visto che questa
struttura di dati segue la politica FIFO, se la coda e` vuota non fa niente
altrimenti elimina la testa*/
void dequeue(queue* Q, int i, int j) {
	if (is_empty(Q))
		return;
	else {
		head_delete(Q);
		return;
	}
}

/* init_matrice() inizializza la struttura utile al corretto funzionamento
del programma, leggendo la matrice da file, viene creata una matrice come
puntatori a puntatori. Ogni casella della nostra matrice ha come
caratteristiche: colore (bianco inizialmente), distanza(inizialmente -1), un puntatore (NULL)
all'elemento precedente nel path minimo, la sua posizione all'interno della matrice
e lo status "bagnata" o "asciutta"*/
void init_matrice(mat** matrice, FILE* finput) {
	int i = 0, j = 0;
	for (i = 0; i < righe; i++)
		for (j = 0; j < colonne; j++) {
			fscanf(finput, " %c", &matrice[i][j].val);
			matrice[i][j].colour = WHITE;
			matrice[i][j].dist = -1;
			matrice[i][j].pred = NULL;
			matrice[i][j].pos.x = i;
			matrice[i][j].pos.y = j;
			if (matrice[i][j].val != '0') {
				int a = (int)matrice[i][j].val - 48, c = j;
				while (c <= j + a && c <= colonne - 1) {
					matrice[i][c].stat = dry;
					c++;
				}
			}
			else if (matrice[i][j].stat != dry)
				matrice[i][j].stat = wet;
			matrice[i][j].pioggia = 0;
		}
}

/* controllo_bfs() esegue il controllo dei nodi adiacenti al nodo corrente. Se questi sono bianchi: vengono scoperti,
il loro colore passa a grigio e gli viene assegnata la distanza dalla sorgente. Per quanto riguarda le caselle bagnate,
la funziona controlla se intorno alla casella ci sono caselle con distanza uguale a quella del nodo che bfs sta scoprendo ma con
meno caselle bagnate colpite. La funzione sceglie, in base a ciò, a quale casella far puntare il nodo corrente per
ricostruire il giusto cammino*/
void controllo_bfs(queue* Q, mat** matrice, int a, int b, int c, int d) {
	if (matrice[a][b].colour == WHITE) {
		matrice[a][b].colour = GREY;
		matrice[a][b].dist = matrice[c][d].dist + 1;
		if (matrice[a][b].stat == wet)
			matrice[a][b].pioggia = matrice[c][d].pioggia + 1;
		else
			matrice[a][b].pioggia = matrice[c][d].pioggia;
		matrice[a][b].pred = &matrice[c][d];
		enqueue(Q, a, b);
	}
	else {
		if (matrice[a][b].dist == matrice[c][d].dist - 1)
			if (matrice[a][b].pioggia <= matrice[c][d].pioggia - 1) {
				matrice[c][d].pred = &matrice[a][b];
				if (matrice[c][d].stat == wet) {
					matrice[c][d].pioggia = matrice[a][b].pioggia + 1;
				}
				else
					matrice[c][d].pioggia = matrice[a][b].pioggia;
			}

	}
}

/* L'algoritmo bfs() prima inizializza la sorgente del grafo, crea una coda vuota in cui verranno inseriti tuti i nodi
che vanno scoperti. Inserisce la sorgente in coda e inizia ad esplorare il grafo, controllando tra tutti i nodi adiacenti
a quello corrente e per ognuno di essi chiama controllo_bfs() che si occuperà anche di mettere in coda i nodi da
finire di scoprire. La funzione in fine ritorna 1 se è stato trovato un percorso dalla sorgente alla destinazione, 0
altrimenti*/
int bfs(mat** matrice) {
	queue* Q;
	matrice[0][0].colour = GREY;
	matrice[0][0].dist = 0;
	matrice[0][0].pred = NULL;
	matrice[0][0].pos.x = 0;
	matrice[0][0].pos.y = 0;
	if (matrice[0][0].stat == wet)
		matrice[0][0].pioggia = 1;
	else
		matrice[0][0].pioggia = 0;
	Q = queue_init();
	enqueue(Q, 0, 0);
	while (!is_empty(Q)) {
		posizione p = head_select(Q);
		int i = p.x, j = p.y;
		if (j != colonne - 1 && matrice[i][j + 1].val == '0') {
			controllo_bfs(Q, matrice, i, j + 1, i, j);
		}
		if (j != 0 && matrice[i][j - 1].val == '0') {
			controllo_bfs(Q, matrice, i, j - 1, i, j);
		}
		if (i != righe - 1 && matrice[i + 1][j].val == '0') {
			controllo_bfs(Q, matrice, i + 1, j, i, j);
		}
		if (i != 0 && matrice[i - 1][j].val == '0') {
			controllo_bfs(Q, matrice, i - 1, j, i, j);
		}
		dequeue(Q, i, j);
		matrice[i][j].colour = BLACK;
	}
	free(Q);
	if (matrice[righe - 1][colonne - 1].pred == NULL)
		return 0;
	else
		return 1;
}

void spostamento(mat** matrice, int* i, int* j) {
	int c = *i;
	if (*i != 0 || *j != 0)
		*i = matrice[*i][*j].pred->pos.x;
	if (*i != 0 || *j != 0)
		*j = matrice[c][*j].pred->pos.y;
}

/*print_soluzione() stampa i passi da eseguire per arrivare dalla sorgente alla destinazione
implementata in modo ricorsivo. Scorre il cammino grazie ai puntatori ai nodi precedenti e, in base
alla posizione di ognuno, stampa la coordinata polare esatta*/
void print_soluzione(mat** matrice, int r, int col) {
	int i = r - 1, j = col - 1;
	if (i != 0 || j != 0)
	{
		int a, b;
		a = (matrice[i][j].pos.x) - (matrice[i][j].pred->pos.x);
		b = (matrice[i][j].pos.y) - (matrice[i][j].pred->pos.y);
		if (a == 1) {
			spostamento(matrice, &i, &j);
			print_soluzione(matrice, i + 1, j + 1);
			printf("S");
		}
		if (a == -1) {
			spostamento(matrice, &i, &j);
			print_soluzione(matrice, i + 1, j + 1);
			printf("N");
		}
		if (b == 1) {
			spostamento(matrice, &i, &j);
			print_soluzione(matrice, i + 1, j + 1);
			printf("E");
		}
		if (b == -1) {
			spostamento(matrice, &i, &j);
			print_soluzione(matrice, i + 1, j + 1);
			printf("O");
		}
	}
	else {
		if (i == 0 && j == 0)
			printf("%d %d\n", matrice[righe - 1][colonne - 1].dist + 1, matrice[righe - 1][colonne - 1].pioggia);
	}
}

/* dealloca_matrice() cancella tutta la memoria usata per memorizzare la matrice */
void dealloca_matrice(mat** matrice) {
	int i;
	for (i = 0; i < righe; i++) {
		free(matrice[i]);
	}
	free(matrice);
}

int main(int argc, char* argv[]) {
	mat** matrice;
	int i;
	/* apertura del file in modalità solo lettura*/
	FILE* finput = fopen(argv[1], "r");		
	/* lettura del numero di righe e colonne da file*/
	fscanf(finput, "%d", &righe);
	fscanf(finput, "%d", &colonne);	
	/* allocazione dello spazio necessario per contenere la matrice
	righexcolonne dove ogni casella ha dimensione sizeof(mat)*/
	matrice = (mat**)malloc(righe * sizeof(mat*));
	for (i = 0; i < righe; i++)								
		matrice[i] = (mat*)malloc(colonne * sizeof(mat));	
	init_matrice(matrice, finput);
	/* se la funzione bfs ritorna 1, è stato trovato il percorso minimo
	altrimenti non esiste un percorso da(0, 0) a(righe - 1, colonne - 1)*/
	if (bfs(matrice)) {
		print_soluzione(matrice, righe, colonne);			
	}
	else
		printf("-1 -1");
	/* chiusura del file di input*/
	fclose(finput);	
	/* liberazione dello spazio di memoria usato per la matrice*/
	dealloca_matrice(matrice);								
	return 0;
}