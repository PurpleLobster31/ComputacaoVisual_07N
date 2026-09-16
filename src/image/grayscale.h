#ifndef GRAYSCALE_H
#define GRAYSCALE_H

#include <stdbool.h>
#include <SDL3/SDL.h>
#include "image_loader.h"

//------------------------------------------------------------------------------
// Requisito 2 do escopo: verifica se a imagem carregada ja esta em escala de
// cinza (todos os pixels com R == G == B) ou se e colorida.
//
// Assume que `image->surface` esta no formato RGBA32.
//------------------------------------------------------------------------------
bool grayscale_is_image_grayscale(const MyImage *image);

//------------------------------------------------------------------------------
// Converte `image` (assumida colorida) para escala de cinza, usando
// Y = 0.2125*R + 0.7154*G + 0.0721*B em cada pixel.
//
// Modifica image->surface diretamente e chama image_refresh_texture() ao
// final, ja que a texture nao reflete mudancas na surface automaticamente.
// Retorna false em falha; a inicializacao deve ser interrompida nesse caso.
//------------------------------------------------------------------------------
bool grayscale_convert(SDL_Renderer *renderer, MyImage *image);

#endif // GRAYSCALE_H
