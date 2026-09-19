## Projeto 1 - Processamento de imagens em C e SDL3
Computação Visual - Prof. André Kishimoto
## Integrantes e contribuições
| Integrante | RA |
| --- | --- |
| Marina Cantarelli Barroca | 10740412 |
| Matheus Mendonça Lopes | 10443495 |
| Miguel Piñeiro Coratolo Simões | 10427085 |
| Thais Ferreira Canguçu | 10403283 |

## O que o programa faz
O programa recebe uma imagem pela linha de comando e abre uma interface para processamento em escala de cinza: analisa o histograma, equaliza as intensidades, alterna a resolução de exibição e salva o resultado em PNG. Foi desenvolvido em C99, com SDL3, a partir do exemplo `04-invert_image` da disciplina (créditos nos arquivos-fonte), e testado em **macOS, Windows 11 e Linux (Ubuntu)**. A validação nativa no Windows 11 (GCC 15.1.0) passou em 16/09/2026; detalhes em [docs/VALIDACAO_WINDOWS.](docs/VALIDACAO_WINDOWS.[](docs/VALIDACAO_MULTIPLATAFORMA.
Ao abrir uma imagem, o programa informa no terminal se ela é colorida ou já está em escala de cinza (convertendo-a quando necessário) e exibe duas janelas: a principal, com a imagem, e uma secundária, com o histograma, as estatísticas (total de pixels, média, desvio padrão e classificações de brilho/contraste) e dois botões.

- **Equalizar / Ver original**: aplica a equalização por distribuição acumulada (CDF) e permite voltar à imagem original em cinza a qualquer momento, sem precisar recarregar o arquivo.
- **Resolução original / 1024x768**: alterna o tamanho de exibição da janela principal, que é recentralizada no monitor; a janela é redimensionada usando o tamanho e a posição informados pelo sistema operacional.
- **Tecla S** (ou Shift+S): salva a versão atualmente exibida (original ou equalizada) em `output_image.png`, na pasta em que o comando foi executado, mantendo as dimensões e o canal alfa do arquivo original. Sobrescreve o arquivo existente e informa o resultado no terminal.

Fechar qualquer uma das janelas encerra o programa. Rodar sem argumento mostra a sintaxe de uso; arquivos inexistentes ou inválidos geram erro e encerramento.

## Como compilar e executar
Entre na pasta do projeto (onde estão este README e o Makefile) antes de rodar os comandos.

**macOS** (com [Homebrew](https://brew.sh/)):

```sh
xcode-select --install
brew install gcc pkgconf sdl3 sdl3_image sdl3_ttf
make
./build/programa kodim23.png
```

**Windows 10/11** (via [MSYS2](https://www.msys2.org/), no terminal **MSYS2 UCRT64** — não use PowerShell/CMD):

```sh
pacman -Syu
pacman -S --needed make mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-pkgconf mingw-w64-ucrt-x86_64-sdl3 mingw-w64-ucrt-x86_64-sdl3-image mingw-w64-ucrt-x86_64-sdl3-ttf
make
./build/programa.exe kodim23.png
```

**Linux (Ubuntu 26.04)**:

```sh
sudo apt update
sudo apt install build-essential pkg-config libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev
make
./build/programa kodim23.png
```

Substitua `kodim23.png` pelo caminho da imagem desejada (use aspas se houver espaços no caminho). PNG e JPEG são suportados.

Comandos úteis do Makefile:

| Comando | Finalidade |
| --- | --- |
| `make` | Compila e gera o executável em `build/`. |
| `make run ARGS=kodim23.png` | Compila (se necessário) e executa com a imagem de exemplo. |
| `make test` | Roda os testes automatizados (cálculos, geometria e renderização). |
| `make test-window` | Roda os testes que abrem janelas reais (requer ambiente gráfico). |
| `make info` | Mostra sistema, compilador e versões das bibliotecas encontradas. |
| `make clean` | Remove os arquivos gerados em `build/`. |

## Status dos requisitos do enunciado
| Item | Situação |
| --- | --- |
| 1. Carregamento da imagem | Concluído — validado em macOS, Linux e Windows 11 |
| 2. Conversão para escala de cinza | Concluído — validado em macOS, Linux e Windows 11 |
| 3. Duas janelas (principal + secundária) | Concluído — validado em macOS, Linux e Windows 11 |
| 4. Histograma e estatísticas | Concluído — validado em macOS, Linux e Windows 11 |
| 5. Equalização | Concluído — validado em macOS, Linux e Windows 11 |
| 6. Alternância de resolução | Concluído — validado em macOS, Linux e Windows 11 |
| 7. Salvamento em PNG | Concluído — validado em macOS, Linux e Windows 11 |
| 8. Textos na interface (fonte Pixelify Sans) | Concluído — validado em macOS, Linux e Windows 11 |
