#ifndef EQUALIZATION_H
#define EQUALIZATION_H

#include "image_loader.h"

// Cria uma versao equalizada sem alterar a imagem original em cinza.
// output deve estar zerado ou conter recursos validos, e ser distinto de original.
// Em caso de erro, output permanece intacto.
bool equalization_create(SDL_Renderer *renderer, const MyImage *original, MyImage *output);

#endif
