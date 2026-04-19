#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define N_VERTICES 5
#define N_CORES 3

typedef struct Vertice {
    int indice;
    struct Vertice* proximo;
} Vertice;

typedef struct Adjacencias {
    int cor;
    Vertice* head;
} Adjacencias;

typedef struct Grafo {
    int numVertices;
    int verticesColoridos;
    Adjacencias* listaAdj;
} Grafo;

void printaGrafo(Grafo* grafo, int printaCor);

void resetaCores(Grafo* grafo) {
    for(int i = 0; i < grafo->numVertices; i++) {
        grafo->listaAdj[i].cor = 0;
    }
}

int temVerticeSemCor(Grafo *g) {
    return g->verticesColoridos != g->numVertices;
}

int todosVizinhosColoridos(Grafo *g, Vertice *v) {
    Vertice *aux = v;
    while (aux != NULL) {
        if (g->listaAdj[v->indice].cor == 0)
            return 0;
        aux = aux->proximo;
    }
    return 1;
}

int ehSeguro(Grafo* grafo, int v, int corTestada) {
    Vertice* vizinho = grafo->listaAdj[v].head;

    while(vizinho != NULL) {
        if(grafo->listaAdj[vizinho->indice].cor == corTestada)
            return 0;

        vizinho = vizinho->proximo;
    }
    return 1;
}

int coloreBacktracking(Grafo* grafo, int qtdCores, int verticeAtual) {
    if(verticeAtual == grafo->numVertices) {
        printf("------------------------------------------------------\n");
        printf("Backtracking: Grafo colorido com %d cores com sucesso!\n", qtdCores);
        printf("------------------------------------------------------\n");
        return 1;
    }


    for(int c = 1; c <= qtdCores; c++) {

        if(ehSeguro(grafo, verticeAtual, c)) {
            grafo->listaAdj[verticeAtual].cor = c;

            if(coloreBacktracking(grafo, qtdCores, verticeAtual+1) == 1) {
                return 1;
            }

            grafo->listaAdj[verticeAtual].cor = 0;
        }
    }

    printf("------------------------------------------------------\n");
    printf("Backtracking: Não foi possível colorir o grafo com %d cores.\n", qtdCores);
    printf("------------------------------------------------------\n");
    return 0;
}

void coloreGulosoAuxiliar(Grafo *grafo, int verticeAtual, int *numCores) {

    int qtdCoresInseguras = 0;
    while (qtdCoresInseguras != *numCores && !ehSeguro(grafo, verticeAtual, *numCores-qtdCoresInseguras))
        qtdCoresInseguras++;
    
    //printf("verticeAtual = %i, numCores = %i, qtdCoresInseguras = %i\n", verticeAtual, *numCores, qtdCoresInseguras);

    if (qtdCoresInseguras == *numCores) {
        (*numCores)++;
        grafo->listaAdj[verticeAtual].cor = *numCores;
    } else {
        grafo->listaAdj[verticeAtual].cor = *numCores-qtdCoresInseguras;
    }

    grafo->verticesColoridos++;

    Vertice *proxVertice = grafo->listaAdj[verticeAtual].head;
    while (temVerticeSemCor(grafo) && proxVertice != NULL) {
        if (grafo->listaAdj[proxVertice->indice].cor == 0)
            coloreGulosoAuxiliar(grafo, proxVertice->indice, numCores);
        proxVertice = proxVertice->proximo;
    }
}

int coloreGuloso(Grafo *grafo) {

    int numCores = 1;

    for (int i = 0; i < grafo->numVertices && temVerticeSemCor(grafo); i++)
        if (grafo->listaAdj[i].cor == 0)
            coloreGulosoAuxiliar(grafo, i, &numCores);
    
    grafo->verticesColoridos = 0;
    return numCores;
}

void printaGrafo(Grafo* grafo, int printaCor) {
    printf("Lista de Adjacências: \n");

    for(int i = 0; i < grafo->numVertices; i++) {

        if(printaCor) {
            printf("[ (COR %d) %d:", grafo->listaAdj[i].cor, i);
        } else {
            printf("[ %d:", i);    
        }
        
        Vertice* aux = grafo->listaAdj[i].head;

        while(aux != NULL) {
            printf(" -> %d", aux->indice);
            aux = aux->proximo;
        }

        printf(" ]\n");
    }
}

// fisher-yates (auxiliar de criaArestasRandom)
// não há auto-relacionamento nem vértices sem adjacencias
Vertice* escolheVizinhos(Grafo* grafo, int i, int qtdAleat) {
    if (qtdAleat == 0) return NULL; 

    int* baralho = malloc((grafo->numVertices - 1) * sizeof(int));
    int pos = 0;

    for(int k = 0; k < grafo->numVertices; k++) {
        if(k != i) {
            baralho[pos] = k;
            pos++;
        }
    }

    for (int k = pos - 1; k > 0; k--) {
        int r = rand() % (k + 1); 
        int temp = baralho[k];
        baralho[k] = baralho[r];
        baralho[r] = temp;
    }

    Vertice* headListaNova = NULL;

    for(int k = 0; k < qtdAleat; k++) { 
        int vizinhoEscolhido = baralho[k];

        Vertice* novoVertice = malloc(sizeof(Vertice));
        novoVertice->indice = vizinhoEscolhido;
        novoVertice->proximo = headListaNova;
        headListaNova = novoVertice;
    }

    free(baralho);
    return headListaNova; 
}

void criaArestasRandom(Grafo* grafo) {
    int qtdAleat;

    for(int i = 0; i < grafo->numVertices; i++) {
        qtdAleat = (rand() % (grafo->numVertices - 1)) + 1;
        grafo->listaAdj[i].head = escolheVizinhos(grafo, i, qtdAleat);
    }

    // garantir a simetria do grafo
    for (int i = 0; i < grafo->numVertices; i++) {

        Vertice *vAdj_i = grafo->listaAdj[i].head;
        while (vAdj_i != NULL) {

            Vertice *vAdj_j = grafo->listaAdj[vAdj_i->indice].head;
            while (vAdj_j != NULL) {
                if (vAdj_j->indice == i)
                    break;
                vAdj_j = vAdj_j->proximo;
            }

            if (vAdj_j == NULL) {
                Vertice *novoVertice = malloc(sizeof(Vertice));
                novoVertice->indice = i;
                novoVertice->proximo = grafo->listaAdj[vAdj_i->indice].head;
                grafo->listaAdj[vAdj_i->indice].head = novoVertice;
            }
    
            vAdj_i = vAdj_i->proximo;
        }
    }
}

Grafo* criaGrafo(int n) {
    Grafo* grafo = malloc(sizeof(Grafo));
    grafo->numVertices = n;
    grafo->verticesColoridos = 0;
    grafo->listaAdj = malloc(sizeof(Adjacencias)*n);

    for(int i = 0; i < n; i++) {
        grafo->listaAdj[i].head = NULL;
        grafo->listaAdj[i].cor = 0;
    }
        
    return grafo;
}

int estaBemColorido(Grafo *g) {
    for (int i = 0; i < g->numVertices; i++) {
        Vertice *vAdj = g->listaAdj[i].head;
        while (vAdj) {
            if (g->listaAdj[vAdj->indice].cor == g->listaAdj[i].cor)
                return 0;
            vAdj = vAdj->proximo;
        }
    }
    return 1;
}

int main() {

    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    Grafo* grafo = criaGrafo(N_VERTICES);
    srand(time(NULL));
    criaArestasRandom(grafo);

    printaGrafo(grafo, 0);
    
    /*coloreBacktracking(grafo, N_CORES, 0);
    printaGrafo(grafo, 1);
    printf("\nEstá corretamente colorido? %s\n", estaBemColorido(grafo) ? "sim" : "não");
    
    resetaCores(grafo);*/

    int numCores = coloreGuloso(grafo);

    printf("-----------------------------------------\n");
    printf("Backtracking: Grafo colorido com %d cores\n", numCores);
    printf("-----------------------------------------\n");

    printaGrafo(grafo, 1);
    printf("\nEstá corretamente colorido? %s\n", estaBemColorido(grafo) ? "sim" : "não");

    return 0;
}