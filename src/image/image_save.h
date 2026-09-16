#ifndef IMAGE_SAVE_H
#define IMAGE_SAVE_H

#include "image_loader.h"

// Exporta os pixels da versao ativa, sem redimensionamento de exibicao.
// overwritten so e atualizado em caso de sucesso.
bool image_save_png(const MyImage *image, const char *path, bool *overwritten);
bool image_save_requested(const SDL_Event *event);

#endif
