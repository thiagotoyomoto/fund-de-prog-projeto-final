/*============================================================================*/
/* MÓDULO QUE REALIZA O CÁLCULO DA DISTANCIA                                  */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/

#ifndef __TRABALHO3_H
#define __TRABALHO3_H

/*============================================================================*/

#include "imagem.h"

typedef struct coordenada
{
    unsigned long linha;
    unsigned long coluna;
} Coordenada;

typedef struct posicoes
{
    Coordenada pontaSuperiorEsquerda;
    Coordenada pontaInferiorDireita;
} Posicoes;

/*============================================================================*/
/* Função central do trabalho. */

void rgbParaEscalaDeCinza(Imagem *img, Imagem *out);
void binariza (Imagem* img, Imagem* out, unsigned char threshold);
void filtroMedia(Imagem *img, Imagem *out, unsigned int winSize);
void retiraFundoEBinariza(Imagem* bg, Imagem* img, Imagem* out);
int quantidadeDePixelBrancoNaLinha(Imagem* img, int linha);
int quantidadeDePixelBrancoNaColuna(Imagem* img, int coluna);
Coordenada acharCoordenadaDaPontaSuperiorEsquerda(Imagem* img);
Coordenada acharCoordenadaDaPontaInferiorDireita(Imagem* img);
Posicoes pegarPosicoesDoCarro(Imagem* img);
double calculaDistanciaEntreCoordenadas(Coordenada a, Coordenada b);
double calculaDistancia (Imagem* bg, Imagem* img1, Imagem* img2);

/*============================================================================*/
#endif /* __TRABALHO3_H */
