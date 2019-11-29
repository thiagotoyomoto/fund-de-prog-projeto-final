#include <math.h>

#include "trabalho3.h"

void rgbParaEscalaDeCinza(Imagem *img, Imagem *out)
{
    int i, j;
    for(i = 0; i < img->altura; i++)
        for(j = 0; j < img->largura; j++)
            out->dados[0][i][j] = img->dados[0][i][j] * 0.3 + img->dados[1][i][j] * 0.59 + img->dados[2][i][j] * 0.11;
}

void binariza (Imagem* img, Imagem* out, unsigned char threshold)
{
    int i, j;
    for(i = 0; i < img->altura; i++)
        for(j = 0; j < img->largura; j++)
            out->dados[0][i][j] = img->dados[0][i][j] >= threshold ? 255 : 0;
}

void filtroMedia(Imagem *img, Imagem *out, unsigned int winSize)
{
    int i, j, k, l, m, n, ui, uj;
    unsigned int s;

    n = winSize / 2;
    ui = img->altura-n;
    uj = img->largura-n;

    for(i = n; i < ui; ++i) {
        for(j = n; j < uj; ++j) {
            s = 0;
            for(k = i - n; k <= i + n; ++k)
                for(l = j - n; l <= j + n; ++l)
                    s += img->dados[0][k][l];
            out->dados[0][i][j] = s / 9;
        }
    }
}

void retiraFundoEBinariza(Imagem* bg, Imagem* img, Imagem* out) {
    int i, j;

    Imagem *bgCinza = criaImagem(bg->largura, bg->altura, 1);
    Imagem *imgCinza = criaImagem(img->largura, img->altura, 1);

    rgbParaEscalaDeCinza(bg, bgCinza);
    rgbParaEscalaDeCinza(img, imgCinza);

    Imagem *aux = criaImagem(img->largura, img->altura, 1);
    for(i = 0; i < img->altura; ++i) {
        for(j = 0; j < img->largura; ++j)
            if(abs(imgCinza->dados[0][i][j] - bgCinza->dados[0][i][j]) < 28)
                out->dados[0][i][j] = 0;
            else
                out->dados[0][i][j] = 255;
    }

    filtroMedia(out, aux, 3);
    filtroMedia(aux, out, 3);
    binariza(out, out, 80);

    destroiImagem(aux);
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

Coordenada acharCoordenadaDaPontaSuperiorEsquerda(Imagem* img) {
    int i, qtdBrancos = 0;
    Coordenada coord;
    coord.linha = -1;
    coord.coluna = -1;
    for(i = 0; i < img->altura; ++i) {
        qtdBrancos = quantidadeDePixelBrancoNaLinha(img, i);
        if(qtdBrancos >= 3) {
            coord.linha = i;
            break;
        }
    }

    for(i = 0; i < img->largura; ++i) {
        qtdBrancos = quantidadeDePixelBrancoNaColuna(img, i);
        if(qtdBrancos >= 3) {
            coord.coluna = i;
            break;
        }
    }

    return coord;
}

Coordenada acharCoordenadaDaPontaInferiorDireita(Imagem* img) {
    int i, qtdBrancos = 0;
    Coordenada coord;

    for(i = img->altura-1; i >= 0; --i) {
        qtdBrancos = quantidadeDePixelBrancoNaLinha(img, i);
        if(qtdBrancos >= 3) {
            coord.linha = i;
            break;
        }
    }

    for(i = img->largura-1; i >= 0; --i) {
        qtdBrancos = quantidadeDePixelBrancoNaColuna(img, i);
        if(qtdBrancos >= 3) {
            coord.coluna = i;
            break;
        }
    }

    return coord;
}

Posicoes pegarPosicoesDoCarro(Imagem* img) {
    Posicoes posicoes;
    posicoes.pontaSuperiorEsquerda = acharCoordenadaDaPontaSuperiorEsquerda(img);
    posicoes.pontaInferiorDireita = acharCoordenadaDaPontaInferiorDireita(img);
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

    posicoes1 = pegarPosicoesDoCarro(img1Bin);
    posicoes2 = pegarPosicoesDoCarro(img2Bin);

    distancia1 = calculaDistanciaEntreCoordenadas(posicoes1.pontaSuperiorEsquerda, posicoes2.pontaSuperiorEsquerda);
    distancia2 = calculaDistanciaEntreCoordenadas(posicoes1.pontaInferiorDireita, posicoes2.pontaInferiorDireita);
    distanciaMedia = (distancia1 + distancia2) / 2.0;

    destroiImagem(img1Bin);
    destroiImagem(img2Bin);

    return distanciaMedia;
}
