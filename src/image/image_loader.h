// Adaptado do exemplo "04-invert_image" fornecido pelo Prof. André Kishimoto
// (https://kishimoto.com.br/), licenciado sob Apache-2.0.

#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <stdbool.h>
#include <SDL3/SDL.h>

//------------------------------------------------------------------------------
// Representa uma imagem carregada em memória, já convertida para o formato
// RGBA32 (uniformiza o processamento independentemente do formato original
// do arquivo: PNG, JPG, BMP etc.).
//------------------------------------------------------------------------------
typedef struct MyImage MyImage;
struct MyImage
{
  SDL_Surface *surface; // Dados de pixel (usados para leitura/escrita direta).
  SDL_Texture *texture; // Gerada a partir da surface, usada para renderizar.
  SDL_FRect rect;        // Posição/tamanho para renderização.
};

//------------------------------------------------------------------------------
// Carrega a imagem indicada em `filename` e converte para RGBA32.
//
// Requisito 1 do escopo: trata arquivo inexistente ou formato inválido,
// exibindo mensagem de erro pertinente no terminal (via SDL_Log) e
// retornando false nesses casos.
//------------------------------------------------------------------------------
bool image_load_rgba32(const char *filename, SDL_Renderer *renderer, MyImage *output_image);

//------------------------------------------------------------------------------
// Libera surface e texture associadas a `image` e zera os campos da struct.
//------------------------------------------------------------------------------
void image_destroy(MyImage *image);

//------------------------------------------------------------------------------
// Recria `image->texture` a partir do conteúdo atual de `image->surface`.
// Deve ser chamada sempre que os pixels da surface forem manipulados
// diretamente (ex.: conversão para escala de cinza, equalização de
// histograma), já que a texture não reflete mudanças na surface automaticamente.
//------------------------------------------------------------------------------
void image_refresh_texture(SDL_Renderer *renderer, MyImage *image);

#endif // IMAGE_LOADER_H
