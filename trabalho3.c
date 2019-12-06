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
* Esta função converte uma imagem em RGB para escala de cinza.
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
*     Imagem* img: Imagem original
*     Imagem* out: Imagem de saída com a borda preta
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

/**
* Esta função faz com que cada pixel que não tenha nenhum vizinho branco, vire preto.
*
* Parâmetros:
*     Imagem* img: Imagem de entrada;
*     Imagem* out: Imagem de saída.
*/
void limparPixelsIsolados(Imagem *img, Imagem *out) {
    int i, j, k, l, ui, uj;
    char vizinhosBrancos;

    copiaMatriz(img->dados[CANAL_R], img->altura, img->largura,
                out->dados[CANAL_R], out->altura, out->largura);

    ui = img->altura-1;
    uj = img->largura-1;

    for(i = 1; i < ui; ++i) {
        for(j = 1; j < uj; ++j) {
            if(img->dados[CANAL_R][i][j] == 255) {
                vizinhosBrancos = 0;
                for(k = i - 1; k <= i + 1 && !vizinhosBrancos; ++k)
                    for(l = j - 1; l <= j + 1; ++l)
                        if(k != i && l != j && img->dados[CANAL_R][k][l] == PIXEL_BRANCO) {
                            vizinhosBrancos = 1;
                            break;
                        }
                if(!vizinhosBrancos)
                    out->dados[CANAL_R][i][j] = PIXEL_PRETO;
            }
        }
    }
}

/**
* Esta função binariza a imagem passada de acordo com o limiar passado.
* Todo pixel que está com o valor maior ou igual ao limiar, recebe 255,
* caso contrário recebe 0.
*
* Parâmetros:
*     Imagem* img: Imagem de entrada;
*     Imagem* out: Imagem binarizada;
*     unsigned char limiar: Limiar de binarização.
*/
void binariza (Imagem* img, Imagem* out, unsigned char limiar) {
    int i, j;

    for(i = 0; i < img->altura && i < out->altura; i++)
        for(j = 0; j < img->largura && j < out->largura; j++)
            out->dados[CANAL_R][i][j] = img->dados[CANAL_R][i][j] >= limiar ? PIXEL_BRANCO : PIXEL_PRETO;
}

/**
* Esta função binariza uma diferença entre duas imagens de acordo com o limiar passado.
* Se a diferença de cada pixel for maior ou igual ao limiar, o pixel resultante é igual
* a 255, caso contrário, ele receberá 0.
*
* Parâmetros:
*     Imagem* img1: Primeira imagem de entrada;
*     Imagem* img2: Segunda imagem de entrada;
*     Imagem* out: Imagem resultante binarizada;
*     unsigned char limiar: limiar de binarização.
*/
void binarizaPorDiferencaEntreImagens(Imagem* img1, Imagem* img2, Imagem* out, unsigned char limiar) {
    int i, j;

    for(i = 0; i < img1->altura && i < img2->altura && i < out->altura; ++i)
        for(j = 0; j < img1->largura && j < img2->largura && j < out->largura; ++j)
            out->dados[CANAL_R][i][j] = abs(img1->dados[CANAL_R][i][j] - img2->dados[CANAL_R][i][j]) >= limiar ? PIXEL_BRANCO : PIXEL_PRETO;
}

/**
* Esta função aplica na imagem passada um filtro de média de largura
* variável.
*
* Parâmetros:
*      Imagem* img: Imagem a ser processada;
*      Imagem* out: Imagem processada;
*      unsigned int largura: Largura do filtro.
*/
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

/**
* Esta função chama todas as funções precisas para retirar o fundo
* e binarizar a imagem, sendo mais usada para uma melhor organização
* do codigo.
*
* Parâmetros:
*      Imagem* bg: Imagem de fundo;
*      Imagem* img: Imagem de entrada;
*      Imagem* out: Imagem sem o fundo e binarizada.
*/
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

    deixaBordaPreta(out, out);

    binariza(out, aux, 116);

    // Esta função se encaixa no mesmo caso da função "filtroDeMedia"
    //limparPixelsIsolados(out, aux);
    limparPixelsIsolados(aux, out);

    // Desalocando as imagens não necessárias posteriormente
    destroiImagem(aux);
    destroiImagem(bgCinza);
    destroiImagem(imgCinza);
}

/**
* Esta função calcula quantos pixels iguais a 255 há na linha desejada.
*
* Parâmetros:
*     Imagem* img: Imagem a ser analisada;
*     unsigned int linha: Índice da linha da imagem.
*
* Retorna: Quantidade de pixels brancos na linha.
*/
int quantidadeDePixeisBrancosNaLinha(Imagem* img, unsigned int linha) {
    int i, qtd = 0;

    for(i = 0; i < img->largura; ++i)
        if(img->dados[CANAL_R][linha][i] == PIXEL_BRANCO)
            ++qtd;

    return qtd;
}

/**
* Esta função calcula quantos pixels iguais a 255 há na coluna desejada.
*
* Parâmetros:
*     Imagem* img: Imagem a ser analisada;
*     unsigned int coluna: Índice da coluna da imagem.
*
* Retorna: Quantidade de pixels brancos na coluna.
*/
int quantidadeDePixeisBrancosNaColuna(Imagem* img, unsigned int coluna) {
    int i, qtd = 0;
    for(i = 0; i < img->altura; ++i)
        if(img->dados[CANAL_R][i][coluna] == PIXEL_BRANCO)
            ++qtd;

    return qtd;
}

/**
* Esta função calcula a coordenada da ponta superior esquerda da imagem.
*
* Parâmetros:
*     Imagem* img: Imagem a ser analisada;
*     unsigned int qtdPixeisBrancos: Quantidade de pixels mínimos na linha
*                                    e coluna para descobrir a coordenada.
*
* Retorna: A coordenada da ponta superior esquerda do objeto.
*/
Coordenada achaCoordenadaDaPontaSuperiorEsquerda(Imagem* img, unsigned int qtdPixeisBrancos) {
    int i, qtdPixeisBrancosEncontrada = 0;
    Coordenada coord;

    for(i = 0; i < img->altura; ++i) {
        qtdPixeisBrancosEncontrada = quantidadeDePixeisBrancosNaLinha(img, i);
        if(qtdPixeisBrancosEncontrada >= qtdPixeisBrancos) {
            coord.linha = i;
            break;
        }
    }

    for(i = 0; i < img->largura; ++i) {
        qtdPixeisBrancosEncontrada = quantidadeDePixeisBrancosNaColuna(img, i);
        if(qtdPixeisBrancosEncontrada >= qtdPixeisBrancos) {
            coord.coluna = i;
            break;
        }
    }

    return coord;
}

/**
* Esta função calcula a coordenada da ponta inferior direita da imagem.
*
* Parâmetros:
*     Imagem* img: Imagem a ser analisada;
*     unsigned int qtdPixeisBrancos: Quantidade de pixels mínimos na linha
*                                    e coluna para descobrir a coordenada.
*
* Retorna: A coordenada da ponta inferior direita do objeto.
*/
Coordenada achaCoordenadaDaPontaInferiorDireita(Imagem* img, int qtdPixeisBrancos) {
    int i, qtdPixeisBrancosEncontrada = 0;
    Coordenada coord;

    for(i = img->altura-1; i >= 0; --i) {
        qtdPixeisBrancosEncontrada = quantidadeDePixeisBrancosNaLinha(img, i);
        if(qtdPixeisBrancosEncontrada >= qtdPixeisBrancos) {
            coord.linha = i;
            break;
        }
    }

    for(i = img->largura-1; i >= 0; --i) {
        qtdPixeisBrancosEncontrada = quantidadeDePixeisBrancosNaColuna(img, i);
        if(qtdPixeisBrancosEncontrada >= qtdPixeisBrancos) {
            coord.coluna = i;
            break;
        }
    }

    return coord;
}

/**
* Esta função calcula as posições(conjunto de coordenadas) do objeto.
*
* Parâmetros:
*     Imagem* img: Imagem a ser analisada para a coleta das coordenadas.
*/
Posicoes pegaPosicoesDoCarro(Imagem* img) {
    Posicoes posicoes;

    posicoes.pontaSuperiorEsquerda = achaCoordenadaDaPontaSuperiorEsquerda(img, 3);
    posicoes.pontaInferiorDireita = achaCoordenadaDaPontaInferiorDireita(img, 3);

    return posicoes;
}

/**
* Esta função calcula a distância entre duas coordenadas, utilizando-se
* do teorema de Pitágoras.
*
* Parâmetros:
*     Coordenada a: Primeira coordenada;
*     Coordenada b: Segunda coordenada.
*
* Retorna: Distância entre as duas distâncias.
*/
double calculaDistanciaEntreCoordenadas(Coordenada a, Coordenada b) {
    Coordenada distancia;

    distancia.linha = a.linha - b.linha;
    distancia.coluna = a.coluna - b.coluna;

    return sqrt(distancia.linha * distancia.linha + distancia.coluna * distancia.coluna);
}

/**
* Esta função calcula a distância entre os dois objetos encontrados
* nas duas imagens passadas, juntamente com uma imagem separada do
* fundo unico das imagens.
*
* Parâmetros:
*     Imagem* bg: Imagem do fundo;
*     Imagem* img1: Imagem com o primeiro objeto;
*     Imagem* img2: Imagem com o segundo objeto.
*
* Retorna: A distância entre os dois objetos.
*/
double calculaDistancia (Imagem* bg, Imagem* img1, Imagem* img2) {
    Posicoes posicoes1, posicoes2;

    Imagem* img1Bin = criaImagem(img1->largura, img1->altura, 1);
    Imagem* img2Bin = criaImagem(img2->largura, img2->altura, 1);

    retiraFundoEBinariza(bg, img1, img1Bin);
    retiraFundoEBinariza(bg, img2, img2Bin);

    posicoes1 = pegaPosicoesDoCarro(img1Bin);
    posicoes2 = pegaPosicoesDoCarro(img2Bin);

    destroiImagem(img1Bin);
    destroiImagem(img2Bin);

    return (calculaDistanciaEntreCoordenadas(posicoes1.pontaSuperiorEsquerda, posicoes2.pontaSuperiorEsquerda) +
            calculaDistanciaEntreCoordenadas(posicoes1.pontaInferiorDireita, posicoes2.pontaInferiorDireita)) / 2.0;
}
