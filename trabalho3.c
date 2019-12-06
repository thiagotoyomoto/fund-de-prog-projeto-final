#include <math.h>

#include "trabalho3.h"

#define PIXEL_PRETO 0
#define PIXEL_BRANCO 255

typedef struct coordenada {
    unsigned long linha;
    unsigned long coluna;
} Coordenada;

typedef struct posicoes {
    Coordenada pontaSuperiorEsquerda;
    Coordenada pontaInferiorDireita;
} Posicoes;

/**
* Esta função copia a matrizA para a matrizB, de acordo com o tamanho das duas matrizes.
*
* Parâmetros:
*     unsigned char** matrizA: Matriz a ser copiada;
*     unsigned int linhasA: quantidade de linhas da matriz A;
*     unsigned int colunasA: quantidade de linhas da matriz A;
*     unsigned char** matrizB: Matriz com a copia;
*     unsigned int linhasB: quantidade de linhas da matriz B;
*     unsigned int colunasB: quantidade de linhas da matriz B.
*/
void copiaMatriz(unsigned char** matrizA, unsigned int linhasA, unsigned int colunasA,
                 unsigned char** matrizB, unsigned int linhasB, unsigned int colunasB) {
    int i, j;
    for(i = 0; i < linhasA && i < linhasB; ++i) {
        for(j = 0; j < colunasA && j < colunasB; ++j) {
            matrizB[i][j] = matrizA[i][j];
        }
    }
}

/**
* Esta função converte uma imagen em RGB para escala de cinza.
*
* Parâmetros:
*     Imagem* img: Imagem em RGB;
*     Imagem* out: Imagem resultante em escala de cinza.
*/
void rgbParaEscalaDeCinza(Imagem* img, Imagem* out) {
    int i, j;
    for(i = 0; i < img->altura; i++)
        for(j = 0; j < img->largura; j++)
            out->dados[CANAL_R][i][j] = (img->dados[CANAL_R][i][j] + img->dados[CANAL_G][i][j] + img->dados[CANAL_B][i][j]) / 3;
}

/**
* Esta função faz com que a imagem passada fique com a borda
* totalmente preta.
*
* Parâmetros:
*     Imagem* img:
*     Imagem* out:
*/
void deixaBordaPreta(Imagem* img, Imagem* out) {
    int i;

    // Caso sejam imagens diferentes, faz uma cópia do conteúdo
    // da imagem de entrada para a imagem de saída
    if(img != out)
        copiaMatriz(img->dados[CANAL_R], img->altura, img->altura,
                    out->dados[CANAL_R], out->altura, out->largura);

    // Preenche de preto a primera e última coluna
    for(i = 0; i < img->altura; ++i) {
        out->dados[CANAL_R][i][0] = 0;
        out->dados[CANAL_R][i][img->largura-1] = 0;
    }

    // Preenche de preto a primeira e última linha
    for(i = 1; i < img->largura-1; ++i) {
        out->dados[CANAL_R][0][i] = 0;
        out->dados[CANAL_R][img->altura-1][i] = 0;
    }
}

void limparPixelsIsolados(Imagem *img, Imagem *out) {
    int i, j, k, l, ui, uj;
    char vizinhosBrancos;

    copiaMatriz(img->dados[CANAL_R], img->altura, img->largura,
                out->dados[CANAL_R], out->altura, out->largura);

    ui = img->altura-1;
    uj = img->largura-1;

    for(i = 1; i < ui; ++i) {
        for(j = 1; j < uj; ++j) {
            if(img->dados[0][i][j] == 255) {
                vizinhosBrancos = 0;
                for(k = i - 1; k <= i + 1 && !vizinhosBrancos; ++k)
                    for(l = j - 1; l <= j + 1; ++l)
                        if(k != i && l != j && img->dados[CANAL_R][k][l] == 255) {
                            vizinhosBrancos = 1;
                            break;
                        }
                if(!vizinhosBrancos)
                    out->dados[CANAL_R][i][j] = 0;
            }
        }
    }
}

void binariza (Imagem* img, Imagem* out, unsigned char limiar) {
    int i, j;

    for(i = 0; i < img->altura && i < out->altura; i++)
        for(j = 0; j < img->largura && j < out->largura; j++)
            out->dados[CANAL_R][i][j] = img->dados[CANAL_R][i][j] >= limiar ? 255 : 0;
}

void binarizaPorDiferencaEntreImagens(Imagem* img1, Imagem* img2, Imagem* out, unsigned char limiar) {
    int i, j;

    for(i = 0; i < img1->altura && i < img2->altura && i < out->altura; ++i)
        for(j = 0; j < img1->largura && j < img2->largura && j < out->largura; ++j)
            out->dados[0][i][j] = abs(img1->dados[0][i][j] - img2->dados[0][i][j]) >= limiar ? 255 : 0;
}

void filtroDeMedia(Imagem *img, Imagem *out, unsigned int largura) {
    int i, j, k, l, limite, ui, uj;
    unsigned int s, larguraAoQuadrado;

    larguraAoQuadrado = largura * largura;
    limite = largura / 2;        // Fator de soma e subtração dos índices de acordo com a largura do filtro
    ui = img->altura - limite;   // Último i
    uj = img->largura - limite;  // Último j

    for(i = limite; i < ui; ++i) {
        for(j = limite; j < uj; ++j) {
            s = 0;
            for(k = i - limite; k <= i + limite; ++k)
                for(l = j - limite; l <= j + limite; ++l)
                    s += img->dados[CANAL_R][k][l];
            out->dados[CANAL_R][i][j] = s / larguraAoQuadrado;
        }
    }
}

void retiraFundoEBinariza(Imagem* bg, Imagem* img, Imagem* out) {
    Imagem *aux = criaImagem(img->largura, img->altura, 1),
            *bgCinza = criaImagem(bg->largura, bg->altura, 1),
             *imgCinza = criaImagem(img->largura, img->altura, 1);

    rgbParaEscalaDeCinza(bg, bgCinza);
    rgbParaEscalaDeCinza(img, imgCinza);

    binarizaPorDiferencaEntreImagens(bgCinza, imgCinza, out, 19);

    // Pode-se passar a mesma imagem porque modifica-se o elemento
    // de acordo com ele mesmo, sem depender de elementos ao redor.
    deixaBordaPreta(out, out);

    // Como neste caso, no se pode passar a mesma imagem pelo fato
    // de que, cada elemento, para ser modificado, depende dos
    // elementos ao redor.
    filtroDeMedia(out, aux, 3);
    filtroDeMedia(aux, out, 3);

    deixaBordaPreta(out, aux);

    binariza(aux, out, 116);

    // Esta função se encaixa no mesmo caso da função "filtroDeMedia"
    limparPixelsIsolados(out, aux);
    limparPixelsIsolados(aux, out);

    // Desalocando as imagens não necessárias posteriormente
    destroiImagem(aux);
    destroiImagem(bgCinza);
    destroiImagem(imgCinza);
}

int quantidadeDePixelBrancoNaLinha(Imagem* img, unsigned int linha) {
    int i, qtd = 0;

    for(i = 0; i < img->largura; ++i)
        if(img->dados[CANAL_R][linha][i] == 255)
            ++qtd;

    return qtd;
}

int quantidadeDePixelBrancoNaColuna(Imagem* img, unsigned int coluna) {
    int i, qtd = 0;
    for(i = 0; i < img->altura; ++i)
        if(img->dados[CANAL_R][i][coluna] == 255)
            ++qtd;

    return qtd;
}

Coordenada achaCoordenadaDaPontaSuperiorEsquerda(Imagem* img, int qtdPixelsBrancos) {
    int i, qtdPixelsBrancosEncontrada = 0;
    Coordenada coord;

    for(i = 0; i < img->altura; ++i) {
        qtdPixelsBrancosEncontrada = quantidadeDePixelBrancoNaLinha(img, i);
        if(qtdPixelsBrancosEncontrada >= qtdPixelsBrancos) {
            coord.linha = i;
            break;
        }
    }

    for(i = 0; i < img->largura; ++i) {
        qtdPixelsBrancosEncontrada = quantidadeDePixelBrancoNaColuna(img, i);
        if(qtdPixelsBrancosEncontrada >= qtdPixelsBrancos) {
            coord.coluna = i;
            break;
        }
    }

    return coord;
}

Coordenada achaCoordenadaDaPontaInferiorDireita(Imagem* img, int qtdPixelsBrancos) {
    int i, qtdPixelsBrancosEncontrada = 0;
    Coordenada coord;

    for(i = img->altura-1; i >= 0; --i) {
        qtdPixelsBrancosEncontrada = quantidadeDePixelBrancoNaLinha(img, i);
        if(qtdPixelsBrancosEncontrada >= qtdPixelsBrancos) {
            coord.linha = i;
            break;
        }
    }

    for(i = img->largura-1; i >= 0; --i) {
        qtdPixelsBrancosEncontrada = quantidadeDePixelBrancoNaColuna(img, i);
        if(qtdPixelsBrancosEncontrada >= qtdPixelsBrancos) {
            coord.coluna = i;
            break;
        }
    }

    return coord;
}

Posicoes pegaPosicoesDoCarro(Imagem* img) {
    Posicoes posicoes;

    posicoes.pontaSuperiorEsquerda = achaCoordenadaDaPontaSuperiorEsquerda(img, 3);
    posicoes.pontaInferiorDireita = achaCoordenadaDaPontaInferiorDireita(img, 3);

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

    salvaImagem(img1Bin, "img-definitiva.bmp");
    salvaImagem(img2Bin, "img-definitiva.bmp");

    posicoes1 = pegaPosicoesDoCarro(img1Bin);
    posicoes2 = pegaPosicoesDoCarro(img2Bin);

    distancia1 = calculaDistanciaEntreCoordenadas(posicoes1.pontaSuperiorEsquerda, posicoes2.pontaSuperiorEsquerda);
    distancia2 = calculaDistanciaEntreCoordenadas(posicoes1.pontaInferiorDireita, posicoes2.pontaInferiorDireita);
    distanciaMedia = (distancia1 + distancia2) / 2.0;

    destroiImagem(img1Bin);
    destroiImagem(img2Bin);

    return distanciaMedia;
}
