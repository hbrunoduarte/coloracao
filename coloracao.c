#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "coloracao.h"
#include "arquivo.h"

void resetaCores(Grafo* grafo) {
    for(int i = 0; i < grafo->numVertices; i++) {
        grafo->listaAdj[i].cor = 0;
    }
    grafo->verticesColoridos = 0;
}

int temVerticeSemCor(Grafo *g) {
    return g->verticesColoridos != g->numVertices;
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

int coloreBacktrackingAux(Grafo* grafo, int verticeAtual, int qtdCores) {
    if(!temVerticeSemCor(grafo)) {
        return 1;
    }

    for(int c = 1; c <= qtdCores; c++) {
        if(ehSeguro(grafo, verticeAtual, c)) {
            grafo->listaAdj[verticeAtual].cor = c;
            grafo->verticesColoridos++;

            if(coloreBacktrackingAux(grafo, verticeAtual+1, qtdCores)) {
                return 1;
            } else {
                grafo->listaAdj[verticeAtual].cor = 0;
                grafo->verticesColoridos--;
            }
        }
    }

    return 0;
}

int coloreBacktracking(Grafo* grafo) {
    for(int m = 1; m <= grafo->numVertices; m++) {
        resetaCores(grafo);

        if(coloreBacktrackingAux(grafo, 0, m)) {
            return m;
        }
    }
    return grafo->numVertices;
}

void coloreGulosoAuxiliar(Grafo *grafo, int verticeAtual, int *paletaCores) {

    Vertice *vAdj = grafo->listaAdj[verticeAtual].head;
    while (vAdj) {
        int corVizinho = grafo->listaAdj[vAdj->indice].cor;
        if (corVizinho != 0) paletaCores[corVizinho-1] = 0;
        vAdj = vAdj->proximo;
    }

    int corEscolhida = 1;
    for (; corEscolhida < grafo->numVertices; corEscolhida++)
        if (paletaCores[corEscolhida-1])
            break;

    grafo->listaAdj[verticeAtual].cor = corEscolhida;

    vAdj = grafo->listaAdj[verticeAtual].head;
    while (vAdj) {
        int corVizinho = grafo->listaAdj[vAdj->indice].cor;
        if (corVizinho != 0) paletaCores[corVizinho-1] = 1;
        vAdj = vAdj->proximo;
    }

    grafo->verticesColoridos++;

    Vertice *proxVertice = grafo->listaAdj[verticeAtual].head;
    while (temVerticeSemCor(grafo) && proxVertice != NULL) {
        if (grafo->listaAdj[proxVertice->indice].cor == 0)
            coloreGulosoAuxiliar(grafo, proxVertice->indice, paletaCores);
        proxVertice = proxVertice->proximo;
    }
}

int coloreGuloso(Grafo *grafo) {

    int *paletaCores = malloc(sizeof(int) * grafo->numVertices);
    for (int i = 0; i < grafo->numVertices; i++)
        paletaCores[i] = 1;

    for (int i = 0; i < grafo->numVertices && temVerticeSemCor(grafo); i++)
        if (grafo->listaAdj[i].cor == 0)
            coloreGulosoAuxiliar(grafo, i, paletaCores);
    
    int numCores = 0;

    for (int i = 0; i < grafo->numVertices && numCores < grafo->numVertices; i++) {
        if (paletaCores[grafo->listaAdj[i].cor-1]) {
            numCores++;
            paletaCores[grafo->listaAdj[i].cor-1] = 0;
        }
    }

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

// Fisher-Yates (auxiliar de criaArestasRandom)
// Não há auto-relacionamento nem vértices sem adjacencias
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

    // Garantir a simetria do grafo
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
            if (g->listaAdj[vAdj->indice].cor == g->listaAdj[i].cor) {
                printf("\n[%i -> %i]\n", i, vAdj->indice);
                return 0;
            }
            vAdj = vAdj->proximo;
        }
    }
    return 1;
}

void freeGrafo(Grafo *g) {

    if (!g || g->numVertices == 0 || !g->listaAdj) return;

    for (int i = 0; i < g->numVertices; i++) {
        Vertice *vAdj = g->listaAdj[i].head;
        while (vAdj) {
            Vertice *aux = vAdj;
            vAdj = vAdj->proximo;
            free(aux);
        }
    }
    free(g->listaAdj);
    g->numVertices = 0;
    g->verticesColoridos = 0;
}

int main(int argc, char *argv[]) {
    
    srand(time(NULL));

    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    Grafo *grafo;

    if (argc == 2 && argv[1][0] == '1') {
        grafo = lerArquivo();
    } else {
        grafo = criaGrafo(MIN_VERTICES + (rand() % (RANGE_ADD_VERTICES + 1)));
        criaArestasRandom(grafo);
    }

    printaGrafo(grafo, 0);
    int numCores;

    //////////////////////////////////

    /*numCores = coloreBacktracking(grafo);

    printf("\n-----------------------------------\n");
    printf("Backtracking: Grafo colorido com %d cores\n", numCores);
    printf("-----------------------------------\n\n");

    printaGrafo(grafo, 1);
    printf("\nEstá corretamente colorido? %s\n", estaBemColorido(grafo) ? "sim" : "não");
    
    //////////////////////////////////

    resetaCores(grafo);*/

    //////////////////////////////////

    numCores = coloreGuloso(grafo);

    printf("\n-----------------------------------\n");
    printf("Guloso: Grafo colorido com %d cores\n", numCores);
    printf("-----------------------------------\n\n");

    printaGrafo(grafo, 1);
    printf("\nEstá corretamente colorido? %s\n", estaBemColorido(grafo) ? "sim" : "não");

    //////////////////////////////////

    freeGrafo(grafo);

    return 0;
}