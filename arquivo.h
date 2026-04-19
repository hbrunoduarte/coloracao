#ifndef _ARQUIVO_H
#define _ARQUIVO_H

#include "coloracao.h"

#define NOME_ARQUIVO "grafo_25nos.csv"
#define QTD_VERTICES 25
#define BUFFER_LEITURA 1 + 2*QTD_VERTICES

Grafo* lerArquivo();

#endif