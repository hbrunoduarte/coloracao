#include <stdio.h>
#include <stdlib.h>
#include "arquivo.h"

#define NOME_ARQUIVO "grafo_25nos.csv"
#define QTD_VERTICES 25
#define BUFFER_LEITURA 2*QTD_VERTICES

Grafo* lerArquivo() {

    Grafo *g = criaGrafo(QTD_VERTICES);

    FILE *file = fopen(NOME_ARQUIVO, "r");
    if (!file) {
        #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        #endif
        perror("\nNão foi possível ler o arquivo.\n");
        exit(1);
    }

    char linha[BUFFER_LEITURA+1];
    linha[BUFFER_LEITURA] = '\0';

    int v = 0;
    while (!feof(file)) {

        fgets(linha, BUFFER_LEITURA+1, file);

        int vAdj = 0;
        while (vAdj < QTD_VERTICES) {

            if (linha[2*vAdj] == '1') {
                Vertice *novaAdj = malloc(sizeof(Vertice));
                novaAdj->indice = vAdj;
                novaAdj->proximo = g->listaAdj[v].head;
                g->listaAdj[v].head = novaAdj;
            }
            vAdj++;
        }

        v++;
    }

    fclose(file);

    return g;
}