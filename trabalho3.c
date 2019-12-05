#include <math.h>

#include "trabalho3.h"

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

/*
    Esta função recebe uma imagem em RGB e a converte para escala de cinza.

    Parâmetros:
        Imagem *img: Imagem de entrada, em RGB;
        Imagem *out: Imagem de saída, em escala de cinza.
*/
void rgbParaEscalaDeCinza(Imagem *img, Imagem *out) {
    int i, j;
    for(i = 0; i < img->altura; i++)
        for(j = 0; j < img->largura; j++)
            out->dados[0][i][j] = (img->dados[0][i][j] + img->dados[1][i][j] + img->dados[2][i][j]) / 3;
}

/*
    Esta função faz com que todos os pixels da borda da imagem de entrada
    recebam 0.

    Parâmetros:
        Imagem *img: Imagem de entrada.
        Imagem *out: Imagem de saída.
*/
void deixaBordaPreta(Imagem *img, Imagem *out) {
    int i;
    for(i = 0; i < img->altura; ++i) {
        out->dados[0][i][0] = 0;
        out->dados[0][i][img->largura-1] = 0;
        if(out->n_canais == 3) {
            out->dados[1][i][0] = 0;
            out->dados[2][i][0] = 0;
            out->dados[1][i][img->largura-1] = 0;
            out->dados[2][i][img->largura-1] = 0;
        }
    }
    for(i = 1; i < img->largura-1; ++i) {
        out->dados[0][0][i] = 0;
        out->dados[0][img->altura-1][i] = 0;
        if(out->n_canais == 3) {
            out->dados[1][0][i] = 0;
            out->dados[2][0][i] = 0;
            out->dados[1][img->altura-1][i] = 0;
            out->dados[2][img->altura-1][i] = 0;
        }
    }
}

void limparPixelsIsolados(Imagem *img, Imagem *out) {
    int i, j, k, l, ui, uj;
    char vizinhoBranco;

    ui = img->altura-1;
    uj = img->largura-1;

    for(i = 1; i < ui; ++i) {
        for(j = 1; j < uj; ++j) {
            if(img->dados[0][i][j] == 255) {
                vizinhoBranco = 0;
                for(k = i - 1; k <= i + 1 && !vizinhoBranco; ++k)
                    for(l = j - 1; l <= j + 1; ++l)
                        if(img->dados[0][k][l] == 255) {
                            vizinhoBranco = 1;
                            break;
                        }
                out->dados[0][i][j] = (!vizinhoBranco ? 0 : 255);
            } else
                out->dados[0][i][j] = 0;
        }
    }
}

void binariza (Imagem* img, Imagem* out, unsigned char threshold) {
    int i, j;
    for(i = 0; i < img->altura; i++)
        for(j = 0; j < img->largura; j++)
            out->dados[0][i][j] = img->dados[0][i][j] >= threshold ? 255 : 0;
}

void filtroDaMedia(Imagem *img, Imagem *out, unsigned int winSize) {
    int i, j, k, l, n, ui, uj;
    unsigned int s, winSizeAoQuadrado;

    winSizeAoQuadrado = winSize * winSize;
    n = winSize / 2;
    ui = img->altura-n;
    uj = img->largura-n;

    for(i = n; i < ui; ++i) {
        for(j = n; j < uj; ++j) {
            s = 0;
            for(k = i - n; k <= i + n; ++k)
                for(l = j - n; l <= j + n; ++l)
                    s += img->dados[0][k][l];
            out->dados[0][i][j] = s / winSizeAoQuadrado;
        }
    }
}

void retiraFundoEBinariza(Imagem* bg, Imagem* img, Imagem* out) {
    int i, j;

    Imagem *aux1 = criaImagem(img->largura, img->altura, 1);
    Imagem *aux2 = criaImagem(img->largura, img->altura, 1);
    Imagem *bgCinza = criaImagem(bg->largura, bg->altura, 1);
    Imagem *imgCinza = criaImagem(img->largura, img->altura, 1);

    rgbParaEscalaDeCinza(bg, bgCinza);
    rgbParaEscalaDeCinza(img, imgCinza);

    for(i = 0; i < img->altura; ++i) {
        for(j = 0; j < img->largura; ++j)
            if(abs(imgCinza->dados[0][i][j] - bgCinza->dados[0][i][j]) < 19)
                out->dados[0][i][j] = 0;
            else
                out->dados[0][i][j] = 255;
    }

    deixaBordaPreta(out, out);
    filtroDaMedia(out, aux1, 3);
    filtroDaMedia(aux1, aux2, 3);
    filtroDaMedia(aux2, aux1, 3);
    binariza(aux1, aux2, 116);
    limparPixelsIsolados(aux2, out);

    destroiImagem(aux1);
    destroiImagem(aux2);
    destroiImagem(bgCinza);
    destroiImagem(imgCinza);
}

int quantidadeDePixelBrancoNaLinha(Imagem* img, int linha) {
    int i, s = 0;
    for(i = 0; i < img->largura; ++i)
        if(img->dados[0][linha][i] == 255)
            ++s;
    return s;
}

int quantidadeDePixelBrancoNaColuna(Imagem* img, int coluna) {
    int i, s = 0;
    for(i = 0; i < img->altura; ++i)
        if(img->dados[0][i][coluna] == 255)
            ++s;
    return s;
}

Coordenada achaCoordenadaDaPontaSuperiorEsquerda(Imagem* img) {
    int i, qtdBrancos = 0;
    Coordenada coord;

    for(i = 0; i < img->altura; ++i) {
        qtdBrancos = quantidadeDePixelBrancoNaLinha(img, i);
        if(qtdBrancos >= 2) {
            coord.linha = i;
            break;
        }
    }

    for(i = 0; i < img->largura; ++i) {
        qtdBrancos = quantidadeDePixelBrancoNaColuna(img, i);
        if(qtdBrancos >= 2) {
            coord.coluna = i;
            break;
        }
    }

    return coord;
}

Coordenada achaCoordenadaDaPontaInferiorDireita(Imagem* img) {
    int i, qtdBrancos = 0;
    Coordenada coord;

    for(i = img->altura-1; i >= 0; --i) {
        qtdBrancos = quantidadeDePixelBrancoNaLinha(img, i);
        if(qtdBrancos >= 2) {
            coord.linha = i;
            break;
        }
    }

    for(i = img->largura-1; i >= 0; --i) {
        qtdBrancos = quantidadeDePixelBrancoNaColuna(img, i);
        if(qtdBrancos >= 2) {
            coord.coluna = i;
            break;
        }
    }

    return coord;
}

Posicoes pegaPosicoesDoCarro(Imagem* img) {
    Posicoes posicoes;

    posicoes.pontaSuperiorEsquerda = achaCoordenadaDaPontaSuperiorEsquerda(img);
    posicoes.pontaInferiorDireita = achaCoordenadaDaPontaInferiorDireita(img);

    return posicoes;
}

double calculaDistanciaEntreCoordenadas(Coordenada a, Coordenada b) {
    Coordenada distancia;

    distancia.linha = a.linha - b.linha;
    distancia.coluna = a.coluna - b.coluna;

    return sqrt(distancia.linha * distancia.linha + distancia.coluna * distancia.coluna);
}

double calculaDistancia (Imagem* bg, Imagem* img1, Imagem* img2) {
    double distancia1, distancia2, distanciaMedia;
    Posicoes posicoes1, posicoes2;

    Imagem* img1Bin = criaImagem(img1->largura, img1->altura, 1);
    Imagem* img2Bin = criaImagem(img2->largura, img2->altura, 1);

    retiraFundoEBinariza(bg, img1, img1Bin);
    retiraFundoEBinariza(bg, img2, img2Bin);

    posicoes1 = pegaPosicoesDoCarro(img1Bin);
    posicoes2 = pegaPosicoesDoCarro(img2Bin);

    distancia1 = calculaDistanciaEntreCoordenadas(posicoes1.pontaSuperiorEsquerda, posicoes2.pontaSuperiorEsquerda);
    distancia2 = calculaDistanciaEntreCoordenadas(posicoes1.pontaInferiorDireita, posicoes2.pontaInferiorDireita);
    distanciaMedia = (distancia1 + distancia2) / 2.0;

    destroiImagem(img1Bin);
    destroiImagem(img2Bin);

    return distanciaMedia;
}
