# Processamento de imagens em C e SDL3

Projeto 1 da disciplina Computação Visual, da Universidade Presbiteriana Mackenzie, ministrada pelo Prof. André Kishimoto. O programa recebe uma imagem pela linha de comando e oferece uma interface para processamento em escala de cinza. A versão atual analisa seu histograma, equaliza as intensidades, alterna a resolução de exibição e salva o resultado em PNG.

O desenvolvimento considera **macOS, Windows e Linux**. Cada sistema precisa de seu próprio executável, compilado com as bibliotecas correspondentes. O código está em C99 e parte do exemplo `04-invert_image` da disciplina, conforme os créditos presentes nos arquivos-fonte.

A validação nativa de **Windows 11 com GCC 15.1.0 passou em 16/09/2026**, incluindo os cinco testes e janelas reais. Consulte [resultados, reprodução em PowerShell e pendências](docs/VALIDACAO_WINDOWS.md).

## Guia de uso

### O que funciona nesta versão

A versão atual carrega a imagem, informa no terminal se ela é colorida ou está em escala de cinza e converte imagens coloridas. Abre uma janela principal com a imagem e uma janela secundária com o histograma, as estatísticas e dois botões com texto que respondem ao mouse.

A equalização já funciona: clique em **Equalizar** para exibir o resultado e em **Ver original** para voltar à imagem original em cinza. O histograma e as estatísticas acompanham a imagem exibida. O segundo botão alterna entre a resolução original da imagem e 1024×768. A janela principal inicia em 1024×768 e a imagem preenche toda sua área. Pressione **S** para salvar a versão atual em `output_image.png` na pasta atual do terminal. O arquivo existente é sobrescrito, e o terminal informa o resultado.

Clique em **Resolução original** para usar as dimensões do arquivo; depois, clique em **1024x768** para retornar ao tamanho inicial. A janela é recentralizada no monitor principal; se o tamanho pedido ultrapassar a largura ou a altura desse monitor, a posição solicitada será (0,0). Resolução e equalização são independentes: trocar o tamanho não desfaz a equalização.

O modo 1024×768 pode deformar imagens com proporção diferente de 4:3, pois preenche a área inteira, sem recortes ou barras de compensação. Também é possível redimensionar a janela manualmente; o próximo clique aplica o outro modo em relação ao último selecionado pelo botão. O sistema operacional pode restringir tamanho e posição, conforme detalhado na implementação.

O histograma mostra quantos pixels possuem cada intensidade, de 0 (preto) a 255 (branco). A barra mais alta ocupa toda a altura do gráfico, e sua contagem aparece em “Pico”. Abaixo estão o total de pixels, a média, o desvio padrão e as classificações de brilho e contraste. Os valores descrevem a imagem em escala de cinza.

### Salvar a imagem

Com qualquer uma das duas janelas em foco, pressione **S**. Se estiver vendo a versão equalizada, ela será salva; após “Ver original”, será salva a original em cinza. Shift+S também funciona. Manter a tecla pressionada não dispara gravações repetidas.

O PNG conserva as dimensões do arquivo de entrada e seu canal alfa; o tamanho da janela não altera o arquivo exportado. Por exemplo, `kodim23.png` continua gerando um PNG de 768×512 mesmo quando é exibido em 1024×768. Esta é a interpretação adotada para “imagem atualmente exibida” no item 7: exportar os pixels da versão ativa, e não uma captura da janela. O enunciado não fixa a resolução do PNG; se o professor exigir o tamanho de exibição, essa decisão deverá ser ajustada.

Exemplos de mensagens: `Arquivo 'output_image.png' criado (768x512).` e `Arquivo 'output_image.png' sobrescrito (768x512).` Se o diretório não permitir escrita ou houver outro erro de arquivo, o terminal informa a falha e o programa continua aberto para uma nova tentativa. A pasta de saída é a pasta de onde o comando foi executado, não necessariamente a pasta da imagem ou do executável.

### Abrir uma imagem

Após seguir a instalação e compilação do seu sistema, entre na pasta que contém este README e o Makefile. No macOS ou Linux:

```sh
./build/programa kodim23.png
```

No terminal MSYS2 UCRT64 do Windows:

```sh
./build/programa.exe kodim23.png
```

Substitua `kodim23.png` pelo caminho da imagem desejada. Use aspas quando houver espaços:

```sh
./build/programa "/caminho/Minhas imagens/foto.png"
```

No Windows, dentro do MSYS2, um caminho pode ser escrito como `"/c/Users/SeuNome/Pictures/foto.png"`. O caminho relativo é interpretado a partir da pasta atual do terminal. PNG e JPEG são exemplos de formatos suportados pelas instalações indicadas abaixo.

Fechar qualquer uma das duas janelas encerra o programa. Sem argumento, o terminal mostra a sintaxe de uso. Arquivos inexistentes ou inválidos geram uma mensagem de erro e encerramento com código 1.

## Instalação e compilação

### Dependências e ambientes

São necessários GNU GCC, GNU Make, `pkg-config` (também fornecido por `pkgconf`), SDL3, SDL3_image e SDL3_ttf. O Makefile verifica bibliotecas de versão 3.2.0 ou superior; esse limite de API não substitui a exigência da disciplina de usar versões estáveis atualizadas.

| Ambiente | Situação | Compilador e bibliotecas |
| --- | --- | --- |
| macOS 26.6.2, Apple Silicon arm64 | Compilação validada localmente em 15/09/2026 | GNU GCC 16.2.0; SDL3 3.4.16; SDL3_image 3.4.6; SDL3_ttf 3.2.2; pkgconf 3.0.7 |
| Windows 11 x86_64, build 26200 | Compilação nativa e testes com janelas passaram em 16/09/2026 | WinLibs MinGW-w64 UCRT GCC 15.1.0; SDKs SDL 3.4.16 / image 3.4.6 / ttf 3.2.2 |
| Linux Ubuntu 26.04 aarch64 em VM/container, X11/Openbox/Xvfb | Compilação e testes, incluindo janelas, passaram | GCC 15.2.0; SDL 3.4.16 / image 3.4.6 / ttf 3.2.2; pkgconf 2.5.1 |
| WSL Ubuntu 26.04 / WSLg | Execução específica ainda pendente | O teste Linux acima não reproduz WSLg nem Windows |

O enunciado indica GCC 15.1.0 no Windows e GCC 15.2.0 no WSL Ubuntu 26.04 para correção. O teste Linux usou GCC 15.2.0, porém em aarch64/X11 virtualizado, não no WSL. Windows nativo com GCC 15.1.0 passou nesta rodada; WSLg continua pendente.

### Validação realizada nesta etapa

No Mac, a compilação com `make -j4` terminou sem avisos. Nesta revisão, o salvamento foi testado por eventos e pela comparação exata dos pixels do PNG reaberto, incluindo criação, sobrescrita e falha recuperável. Foram verificados os retornos de erro para ausência de argumento, arquivo inexistente e formato inválido. A imagem `kodim23.png` foi carregada e convertida para cinza; o processo permaneceu ativo durante o teste breve de inicialização e foi encerrado pelo teste. O histograma também foi verificado com imagens sintéticas de resultado conhecido, e o painel foi renderizado em memória para inspeção visual de barras, textos e acentos. A execução a partir de outra pasta foi testada para verificar a localização da fonte. A equalização e a reversão também foram verificadas com resultados conhecidos, incluindo preservação da original, atualização do painel e reutilização das versões em memória. No Mac, `make test-window` também verificou cliques, dimensões e posições reais, manutenção da equalização, redimensionamento manual e pedido de tamanho maior que o monitor. A mesma bateria passou no Ubuntu 26.04 com GCC 15.2.0, incluindo janelas X11/Openbox numa tela virtual de 1920×1080. A filha ficou em (0,0), e o tamanho maior 2020×1180 foi aceito. Windows foi recompilado nativamente com GCC 15.1.0 e passou na mesma bateria em 16/09/2026. Consulte o [registro detalhado e roteiro Windows](docs/VALIDACAO_MULTIPLATAFORMA.md).

### macOS

Instale as Command Line Tools da Apple, caso ainda não existam:

```sh
xcode-select --install
```

Instale o [Homebrew](https://brew.sh/) seguindo suas instruções e a configuração de PATH apresentada pelo instalador. Em seguida:

```sh
brew install gcc pkgconf sdl3 sdl3_image sdl3_ttf
make
make info
./build/programa kodim23.png
```

O `gcc` fornecido pela Apple executa Clang. Por isso, o Makefile procura o GNU GCC com sufixo de versão dentro da instalação da fórmula `gcc` do Homebrew. Ele também consulta `pkg-config` para encontrar os diretórios das bibliotecas, sem fixar `/opt/homebrew` ou `/usr/local` no projeto. Isso permite preparar tanto Macs Apple Silicon quanto Intel; Intel ainda não foi testado.

### Windows 10 e 11

Instale o [MSYS2](https://www.msys2.org/) e abra especificamente o terminal **MSYS2 UCRT64**. Os comandos desta seção são executados nele, inclusive o `make`; não no PowerShell ou no Prompt de Comando.

Atualize os pacotes:

```sh
pacman -Syu
```

Se a atualização solicitar o fechamento do terminal, reabra o UCRT64 e execute o comando novamente. Instale as ferramentas e bibliotecas:

```sh
pacman -S --needed make mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-pkgconf mingw-w64-ucrt-x86_64-sdl3 mingw-w64-ucrt-x86_64-sdl3-image mingw-w64-ucrt-x86_64-sdl3-ttf
```

Entre na pasta do projeto, adaptando o caminho, e compile:

```sh
cd "/c/Users/SeuNome/Documents/ComputacaoVisual_07N-main"
make
make info
./build/programa.exe kodim23.png
```

O executável usa as DLLs disponíveis no ambiente UCRT64. Foi preparado `build/validacao-windows-x64.zip` para teste posterior, com executáveis compilados no Mac, DLLs oficiais, fonte, licenças e `VALIDAR_WINDOWS.cmd`. Extraia todo o ZIP; copiar apenas `programa.exe` não basta. Esse pacote passou no Windows 11; a recompilação nativa separada com GCC 15.1.0 também passou. O ZIP é um artefato local e não está no GitHub. Para construir a partir de um clone limpo usando PowerShell e os SDKs oficiais, siga o [roteiro Windows](docs/VALIDACAO_WINDOWS.md#reproduzir-no-powershell). O [roteiro detalhado](docs/VALIDACAO_MULTIPLATAFORMA.md#roteiro-para-a-máquina-windows) explica como registrar os resultados. Não misture bibliotecas UCRT64 com as de MINGW64 ou MSYS.

Referências dos pacotes: [SDL3](https://packages.msys2.org/packages/mingw-w64-ucrt-x86_64-sdl3), [SDL3_image](https://packages.msys2.org/packages/mingw-w64-ucrt-x86_64-sdl3-image) e [SDL3_ttf](https://packages.msys2.org/packages/mingw-w64-ucrt-x86_64-sdl3-ttf).

### Linux e WSL com Ubuntu 26.04

Em um Ubuntu com os pacotes SDL3 disponíveis nos repositórios habilitados:

```sh
sudo apt update
sudo apt install build-essential pkg-config libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev
make
make info
./build/programa kodim23.png
```

Os pacotes SDL3 do Ubuntu 26.04 estão no componente `universe`. Se necessário, habilite-o com `sudo add-apt-repository universe` e atualize a lista de pacotes. Consulte o [catálogo do Ubuntu](https://packages.ubuntu.com/resolute/libsdl3-dev).

A versão de uma distribuição pode ser anterior à versão estável mais recente do projeto SDL. Antes da entrega, compare `make info` com as versões estáveis oficiais. Se a distribuição não oferecer os pacotes necessários ou oferecer versões inadequadas, será preciso instalar versões estáveis pelo código-fonte conforme o [guia oficial de compilação da SDL](https://wiki.libsdl.org/SDL3/README-cmake), além das instruções dos projetos SDL_image e SDL_ttf. A compilação das três bibliotecas estáveis pelo código-fonte foi validada no Ubuntu 26.04 aarch64; a receita reproduzível está em `validation/ubuntu.Dockerfile`. Ela usa pacotes de desenvolvimento do Ubuntu e instala as versões testadas em `/usr/local`.

Outras distribuições Linux usam nomes de pacotes e gerenciadores próprios. A receita `apt` acima é específica do Ubuntu; não deve ser executada como instrução universal de Linux.

No WSL, é necessário suporte a aplicações gráficas, como WSLg em uma instalação compatível. Sem um ambiente gráfico disponível, a criação das janelas pode falhar mesmo que a compilação tenha sucesso. O posicionamento de janelas também precisa ser validado no ambiente gráfico usado pelo grupo.

### O que muda entre os sistemas

As fórmulas, pixels salvos e estrutura dos módulos são iguais. Mudam instalação, executável e bibliotecas: Homebrew e bibliotecas dinâmicas no Mac, `.exe` e DLLs no Windows, pacotes/`.so` no Linux. O Makefile preserva o console no Windows com `-mconsole`, para exibir diagnósticos.

A fonte deve acompanhar o executável em `assets/fonts`, com a grafia exata dos nomes (especialmente em Linux). Ela é encontrada mesmo ao executar de outra pasta. A entrada e `output_image.png`, por sua vez, são relativos à pasta atual do terminal. Use aspas nos caminhos com espaços; no Windows, a sintaxe do caminho depende do terminal.

O posicionamento depende do sistema gráfico: Cocoa ajustou (0,0) para (0,65) no Mac testado; X11/Openbox respeitou (0,0) no Linux testado. Windows 11 respeitou (0,0) nesta rodada. Wayland/WSLg, outras escalas DPI e múltiplos monitores ainda requerem conferência específica. O tamanho da janela é expresso em coordenadas SDL e não determina as dimensões do PNG. A [matriz completa](docs/VALIDACAO_MULTIPLATAFORMA.md#diferenças-que-afetam-o-uso) reúne diferenças e limites da validação.

### Comandos comuns e diagnóstico

| Comando | Finalidade |
| --- | --- |
| `make` | Compila e gera o executável em `build/`. |
| `make -j4` | Compila até quatro arquivos simultaneamente. |
| `make run ARGS=kodim23.png` | Compila se necessário e executa com a imagem de exemplo. |
| `make test-build` | Compila os testes sem executar, inclusive para compilação cruzada. |
| `sh scripts/validate.sh --window` | Registra versões, compilação e testes em `build/validation/`; exige sessão gráfica. |
| `make test` | Verifica cálculos, geometria e renderização em memória; gera as prévias dos painéis em `build/`. |
| `make test-window` | Abre janelas para testar a alternância real de resolução; requer ambiente gráfico. |
| `make info` | Mostra sistema/arquitetura, compilador, pkg-config e versões das três bibliotecas, nessa ordem. |
| `make clean` | Remove os arquivos gerados em `build/`. |

É possível selecionar outro compilador com `make CC=gcc-15`, desde que esteja instalado. Ao trocar de compilador ou de opções de compilação, execute `make clean` antes de recompilar. Registre também a versão completa do sistema: `sw_vers` no Mac, `winver` no Windows e `cat /etc/os-release` no Linux.

Se aparecer erro de `pkg-config`, confira a instalação e o terminal utilizado. Instalações de bibliotecas em prefixos personalizados podem exigir `PKG_CONFIG_PATH` apontando para o diretório que contém os arquivos `.pc`. Não substitua pacotes SDL3 por SDL2: as APIs são diferentes.

## Implementação do programa

Esta seção descreve o código existente. Será ampliada conforme cada funcionalidade for concluída, incluindo as decisões e os algoritmos adotados.

### Organização e fluxo principal

`src/main.c` coordena o programa: verifica o argumento, inicializa a SDL, cria a janela principal, carrega a imagem, detecta/converte a escala de cinza e cria a janela secundária, inicializa SDL_ttf e carrega a fonte. Calcula o histograma e prepara as texturas das estatísticas e os botões. Depois entra no laço de eventos, que trata fechamento e cliques e redesenha as duas janelas, com uma pausa de 16 ms por ciclo para evitar um laço sem limite de velocidade. A função `shutdown`, registrada com `atexit`, libera os recursos no encerramento.

### Representação e carregamento da imagem

`src/image/image_loader.h` define `MyImage`. Sua `surface` contém os pixels em memória para processamento; a `texture` é o recurso utilizado pelo renderizador para desenhar; o `rect` registra o retângulo inicial da textura. A renderização principal agora preenche a saída inteira e não usa esse retângulo como limite de desenho.

Em `image_loader.c`, `image_load_rgba32` usa `IMG_Load` para ler o arquivo, converte a superfície para RGBA32 e cria a textura. Um formato comum simplifica as operações sobre os canais de cor. `image_refresh_texture` recria a textura após alterar os pixels, pois modificar a superfície não atualiza automaticamente o que é exibido. `image_destroy` libera superfície e textura.

### Detecção e conversão para cinza

`src/image/grayscale.c` verifica se R, G e B são iguais em todos os pixels. Caso encontre um pixel com canais diferentes, a imagem é considerada colorida. A conversão calcula, para cada pixel:

```text
Y = 0.2125 × R + 0.7154 × G + 0.0721 × B
```

O valor arredondado substitui os três canais de cor, preservando o alfa. Ao final, a textura é recriada. Essa imagem em cinza é a entrada do histograma e da equalização. A detecção e a conversão percorrem cada linha pelo `pitch`, sem interpretar bytes de preenchimento como pixels. A conversão verifica o bloqueio da superfície e retorna sucesso/falha; se a recriação da textura falhar, a inicialização informa o erro e é interrompida.

R, G e B são os valores dos canais vermelho, verde e azul, cada um entre 0 e 255. Y é a intensidade calculada; os coeficientes somam 1 e dão maior peso ao verde. O código aplica a fórmula exigida diretamente aos valores dos canais, sem uma etapa adicional de linearização de gama. `SDL_round` arredonda para o inteiro mais próximo; para valores não negativos terminados em 0,5, arredonda para cima. O resultado é armazenado em 8 bits.

Por exemplo, um pixel vermelho `(255, 0, 0)` produz `Y = 0.2125 × 255 = 54.1875`, que vira `(54, 54, 54)`. O alfa mantém seu valor original. Pixels já cinzentos satisfazem `R = G = B` e não precisam de conversão.

**A conversão de cor para cinza não tem inversa única.** Diferentes combinações RGB podem gerar o mesmo Y, e o arredondamento também descarta informação. O programa não guarda uma versão colorida para restauração. O botão “Ver original” reverte a equalização e retorna à versão em cinza preservada; ele não recupera as cores do arquivo de entrada.

### Janelas e botões

`src/ui/window.h` define `MyWindow`, reunindo janela e renderizador. `window_secondary.c` cria uma janela fixa de 400×700, solicita sua associação à principal e a posiciona em (0,0). Se a associação falhar, o código atual registra um aviso; esse comportamento ainda precisa ser validado em cada sistema.

`src/ui/button.c` define a lógica reutilizada pelos dois botões. Cada botão guarda um retângulo, um rótulo e um estado: neutro, mouse sobre o botão ou pressionado. Eventos de outras janelas são ignorados. Pressionar e soltar dentro do botão sinaliza o clique. O desenho usa primitivas SDL com diferentes tons de azul; o rótulo é centralizado dentro do retângulo. Sua textura é reutilizada entre quadros; `button_set_label` descarta o texto antigo para recriá-lo no próximo desenho.

### Cálculo do histograma e das estatísticas

`src/image/histogram.h` define `Histogram`: um vetor de 256 contagens (`bins`), total de pixels, maior contagem (`peak`), média e desvio padrão. Os contadores usam inteiros de 64 bits. `histogram_compute`, em `histogram.c`, recebe uma superfície RGBA32 já convertida para cinza e percorre cada linha respeitando seu `pitch`, a distância em bytes entre inícios de linhas. Assim, eventuais bytes de preenchimento não entram na contagem.

Cada pixel incrementa a posição correspondente à sua intensidade. Todos os pixels são contados, inclusive os transparentes: o alfa é preservado na imagem, mas não pondera esta análise dos canais RGB. Uma imagem ainda colorida é rejeitada, para impedir estatísticas calculadas apenas sobre um canal por engano. Em caso de falha, o resultado anterior é preservado.

Para `h[i]` pixels na intensidade `i` e `N` pixels ao todo:

```text
média = soma(i × h[i]) / N
variância = soma((i − média)² × h[i]) / N
desvio padrão = raiz quadrada da variância
```

A soma percorre as intensidades `i = 0, 1, ..., 255`. O total é `N = largura × altura = soma(h[i])`, e o pico é `máximo(h[i])`. Por exemplo, para os quatro pixels `[50, 50, 100, 150]`, temos `h[50] = 2`, `h[100] = 1`, `h[150] = 1`, N = 4 e pico = 2. A média é `(50 × 2 + 100 + 150) / 4 = 87,5`. A variância é `(2 × (50 − 87,5)² + (100 − 87,5)² + (150 − 87,5)²) / 4 = 1718,75`, e o desvio padrão é aproximadamente 41,46.

A variância é populacional, dividida por N: usamos todos os pixels, não uma amostra. A média indica o nível geral de intensidade; o desvio padrão mede a dispersão das intensidades e é usado como indicador de contraste global.

O enunciado pede classificações, mas não fixa limites numéricos. Este protótipo adota os seguintes critérios:

| Medida | Intervalo | Classificação |
| --- | --- | --- |
| Média | menor que 85 | escura |
| Média | de 85 até abaixo de 170 | média |
| Média | 170 ou maior | clara |
| Desvio padrão | menor que 40 | baixo |
| Desvio padrão | de 40 até abaixo de 80 | médio |
| Desvio padrão | 80 ou maior | alto |

Os limites de brilho dividem a faixa de intensidades em três partes. Os limites de contraste são uma escolha heurística de 40 e 80, dentro da faixa possível de desvio padrão de 0 a 127,5. O máximo é metade da amplitude dos níveis, `(255 − 0) / 2`, atingido quando metade dos pixels é preta e metade é branca. Essas categorias não constituem um padrão universal de qualidade visual e não medem contraste local. As funções de classificação concentram os limites para facilitar futuras alterações.

### Equalização pela distribuição acumulada

`src/image/equalization.c` implementa `equalization_create`. O método constrói uma tabela que transforma cada intensidade antiga em uma nova, usando a distribuição acumulada do histograma (CDF). O cálculo usa contagens inteiras, equivalentes à CDF normalizada após a divisão. A variante adotada subtrai a primeira contagem acumulada não nula, para levar a menor intensidade presente a 0 e a maior a 255 quando a imagem contém mais de um nível:

```text
C[i] = h[0] + h[1] + ... + h[i]
Cmin = primeiro valor não nulo de C
T[i] = arredondar(255 × (C[i] − Cmin) / (N − Cmin))
novo_pixel = (T[Y], T[Y], T[Y], alfa_original)
```

`h[i]` é a quantidade de pixels na intensidade i; `C[i]` é a quantidade com intensidade até i, inclusive; N é o total de pixels; `Cmin` corresponde à contagem da menor intensidade presente; e `T` é a tabela de 256 valores usada no remapeamento. Para níveis anteriores à primeira intensidade presente, o código define T como 0; não existem pixels desses níveis para transformar. O arredondamento usa a mesma regra de `SDL_round` descrita na conversão para cinza.

Exemplo completo com os pixels `[50, 50, 100, 150]`:

| Intensidade i | h[i] | C[i] | Cálculo de T[i] | Novo valor |
| --- | --- | --- | --- | --- |
| 50 | 2 | 2 | arredondar(255 × (2 − 2) / (4 − 2)) | 0 |
| 100 | 1 | 3 | arredondar(255 × (3 − 2) / (4 − 2)) | 128 |
| 150 | 1 | 4 | arredondar(255 × (4 − 2) / (4 − 2)) | 255 |

O resultado é `[0, 0, 128, 255]`. Todos os pixels com a mesma intensidade recebem o mesmo novo valor, e a ordem das intensidades é preservada. O histograma resultante não precisa ter barras iguais: o número de pixels e de níveis é discreto, e uma intensidade antiga não é dividida entre várias novas.

Se a imagem for constante, `Cmin = N` e o denominador seria zero. Nesse caso o código mantém todos os valores originais, inclusive em imagens totalmente pretas ou brancas. A equalização pode ampliar a distribuição global de intensidades, mas não garante melhoria visual em toda imagem e pode realçar ruído.

O algoritmo primeiro calcula o histograma da original, depois gera T e duplica a superfície com `SDL_DuplicateSurface`. Percorre as linhas respeitando o `pitch`, substitui R/G/B e preserva alfa e dimensões. Em seguida cria uma textura própria para a imagem equalizada. O resultado só substitui a saída quando todas as etapas terminam com sucesso. O custo de cálculo é proporcional ao número de pixels, `O(N + 256)`, e a nova superfície RGBA32 ocupa aproximadamente `4 × largura × altura` bytes, além do preenchimento das linhas e da textura do renderizador.

Referências: [explicação de equalização por distribuição acumulada](https://docs.opencv.org/4.x/d4/d1b/tutorial_histogram_equalization.html) e [SDL_DuplicateSurface](https://wiki.libsdl.org/SDL3/SDL_DuplicateSurface). O programa implementa o algoritmo em C com SDL; OpenCV é apenas uma referência conceitual, não uma dependência.

### Reversão e sincronização das duas janelas

A original em cinza permanece em `g_image`. No primeiro clique, a versão equalizada é criada em `g_equalizedImage`; a variável booleana `g_isEqualized` seleciona qual textura desenhar. A reversão consiste em selecionar novamente `g_image`, cujos pixels e textura não foram alterados pela equalização. Não há aplicação de fórmula inversa nem uma nova chamada a `IMG_Load`.

Os próximos cliques reutilizam as duas versões. Isso evita equalizar repetidamente o resultado anterior, preserva a comparação e dispensa a releitura do arquivo. As superfícies e texturas das duas versões são liberadas no encerramento.

`toggle_equalization`, em `main.c`, calcula o histograma da próxima imagem e prepara os textos antes de mudar a seleção. Se essa preparação falhar, a interface mantém o estado anterior e informa o erro no terminal. Quando há sucesso, troca o texto para “Ver original” ou “Equalizar”, confirma o estado e redesenha as janelas com imagem, barras e estatísticas correspondentes.

### Desenho do histograma

`src/ui/histogram_view.c` separa a apresentação dos cálculos. `HistogramView` guarda o histograma e as texturas dos textos. `histogram_view_update` prepara uma nova versão do painel quando a imagem muda e só substitui a anterior depois de preparar todos os textos com sucesso.

`histogram_view_render` desenha uma área de 320×220 pixels e 256 barras. A posição horizontal representa a intensidade; a altura é `contagem / pico × altura do gráfico`. A escala vertical é linear e relativa ao pico da imagem, cujo valor é exibido acima. Isso mantém a proporcionalidade entre as barras e permite comparar distribuições de imagens com diferentes tamanhos, observando também os valores de pico e total. Imagens uniformes produzem uma única barra, sem divisão por zero.

Para o retângulo do gráfico `(x0, y0, W, H) = (40, 112, 320, 220)`, a geometria de cada barra é:

```text
largura_barra = W / 256 = 1,25 pixel
x_barra = x0 + W × i / 256
altura_barra = H × h[i] / pico
y_barra = y0 + H − altura_barra
```

O eixo y da tela cresce para baixo; por isso a altura é subtraída para fazer a barra subir a partir da base. Barras com contagem zero não são desenhadas. Uma contagem igual à metade do pico ocupa `220 / 2 = 110` pixels de altura. As linhas de referência ficam em `y0 + H × k / 4`, com k de 0 a 4.

O painel também exibe os extremos 0/255, a referência 128 e os valores e classificações calculados. `histogram_view_destroy` libera as texturas antes da destruição do renderizador.

### Textos e carregamento da fonte nos três sistemas

`src/ui/text_render.c` utiliza SDL3_ttf e a fonte **Pixelify Sans**, incluída em `assets/fonts/PixelifySans-VariableFont_wght.ttf`. `text_open_font` usa `SDL_GetBasePath` e acrescenta o caminho da fonte. O Makefile copia o arquivo para `build/assets/fonts/` durante a compilação.

A busca é relativa à pasta do executável, não à pasta atual do terminal. Isso evita depender de uma fonte instalada no sistema e permite executar o programa de outro diretório. Ao mover o executável, mova também a pasta `assets` que está ao lado dele. Em Windows, as DLLs continuam necessárias; no caso de um futuro pacote `.app` no Mac, os recursos deverão acompanhar a estrutura de recursos do pacote.

`text_label_set` transforma uma string UTF-8 em superfície com `TTF_RenderText_Blended` e depois em textura do renderizador. A superfície temporária é liberada imediatamente; a textura fica em cache. `text_label_draw` reutiliza a textura nos quadros seguintes. `text_label_destroy` libera o recurso. O texto dos botões é centralizado com `x = x_botão + (largura_botão − largura_texto) / 2` e `y = y_botão + (altura_botão − altura_texto) / 2`. Assim, a troca entre “Equalizar” e “Ver original” mantém a centralização, embora os textos tenham larguras diferentes.

A fonte é fechada com `TTF_CloseFont` e SDL_ttf é encerrada com `TTF_Quit` durante a limpeza do programa. Uma fonte ausente gera mensagem no terminal e impede a inicialização incompleta da interface.

Referências de API: [SDL_GetBasePath](https://wiki.libsdl.org/SDL3/SDL_GetBasePath), [TTF_OpenFont](https://wiki.libsdl.org/SDL3_ttf/TTF_OpenFont) e [TTF_RenderText_Blended](https://wiki.libsdl.org/SDL3_ttf/TTF_RenderText_Blended).

### Alternância de resolução e posição da janela

`src/ui/resolution.c` separa o cálculo da geometria (`resolution_plan`) da aplicação às janelas (`resolution_apply`). O estado `g_isOriginalResolution`, em `main.c`, começa falso, representando o modo 1024×768. `resolution_button_label` retorna o nome da próxima ação: “Resolução original” nesse estado e “1024x768” no outro.

Para largura/altura originais `(Wi, Hi)`, o tamanho de janela solicitado `(Wj, Hj)` é:

```text
modo original: Wj = Wi e Hj = Hi
modo fixo:     Wj = 1024 e Hj = 768
```

A cada clique consultamos o monitor principal com `SDL_GetPrimaryDisplay` e seus limites com `SDL_GetDisplayBounds`. Isso evita fixar uma resolução de monitor no código. Se o monitor tem origem `(Xm, Ym)` e dimensões `(Wm, Hm)`, usamos:

```text
se Wj > Wm OU Hj > Hm:
    X = 0
    Y = 0
caso contrário:
    X = Xm + divisão_inteira((Wm − Wj), 2)
    Y = Ym + divisão_inteira((Hm − Hj), 2)
```

A divisão inteira descarta a parte fracionária; como as diferenças são não negativas no caso centralizado, isso equivale a arredondar para baixo. A origem do monitor entra na conta porque, em configurações com várias telas, ele pode estar deslocado, inclusive para coordenadas negativas. Quando a janela excede o monitor, usamos o (0,0) global exigido pelo enunciado, mesmo que esse monitor tenha outra origem.

Exemplo em um monitor de 1920×1080 com origem (0,0): a janela 1024×768 fica em `((1920 − 1024)/2, (1080 − 768)/2) = (448,156)`. Para a imagem de exemplo, de 768×512, fica em `(576,284)`. Uma janela 2000×600 excede a largura e solicita (0,0), embora sua altura caiba.

A comparação utiliza os limites completos do monitor, não a área útil descontando barras e dock. Os tamanhos são da área de conteúdo e as posições são expressas nas coordenadas de janela/desktop fornecidas pela SDL. Em configurações com escala de tela/HiDPI, essas unidades não devem ser confundidas automaticamente com os pixels físicos do painel.

### Escala e interpolação da imagem exibida

`render_main_window` desenha a textura selecionada com destino nulo em `SDL_RenderTexture`, que significa ocupar toda a saída atual do renderizador. Assim, o desenho acompanha tanto o botão quanto alterações manuais de tamanho e a saída efetiva em telas com escala. A superfície dos pixels não é redimensionada; apenas sua apresentação muda.

Para uma textura original `(Wi, Hi)` e saída do renderizador `(Wd, Hd)`, os fatores de escala são:

```text
Sx = Wd / Wi
Sy = Hd / Hi
```

A imagem de 768×512 exibida em 1024×768 usa `Sx = 1024/768 ≈ 1,3333` e `Sy = 768/512 = 1,5`. Como são fatores diferentes, a proporção é alterada. No modo original, os fatores são 1 quando a saída tem o mesmo número de pixels da textura; a escala do sistema pode modificar essa correspondência física.

O código seleciona `SDL_SCALEMODE_LINEAR`: a interpolação é executada pelo renderizador SDL. Conceitualmente, o filtro bilinear calcula um valor entre quatro texels vizinhos. Para uma coordenada de origem `(u,v)`, `i = piso(u)`, `j = piso(v)`, `a = u − i` e `b = v − j`:

```text
valor = (1−a)(1−b) × P[i,j]
      + a(1−b)     × P[i+1,j]
      + (1−a)b     × P[i,j+1]
      + ab        × P[i+1,j+1]
```

`P` representa o canal de um texel da textura. Os pesos somam 1; no meio dos quatro vizinhos, `a = b = 0,5` e cada um recebe peso 0,25. Os detalhes de amostragem, borda e precisão ficam a cargo do backend SDL; o programa não implementa uma segunda rotina de interpolação em CPU. Portanto, não pressupomos resultados idênticos byte a byte entre renderizadores.

Essa interpolação afeta somente os pixels desenhados na janela. As superfícies original/equalizada, suas dimensões, o histograma e as estatísticas permanecem intactos ao mudar a resolução.

### Aplicação e restrições de cada sistema

Antes de redimensionar, `resolution_apply` sai de fullscreen ou restaura a janela caso esteja maximizada/minimizada. Desativa temporariamente o redimensionamento manual, solicita posição/tamanho e reativa essa opção depois. Essa sequência resolveu, nos testes do Mac, a limitação automática da altura de uma janela redimensionável maior que o monitor, usando apenas APIs SDL compartilhadas pelos sistemas.

`SDL_SyncWindow` aguarda as mudanças em sistemas que as aplicam de forma assíncrona. O programa consulta o tamanho realmente aplicado: se o sistema não aceitar as dimensões pedidas, informa o erro e tenta restaurar a geometria anterior, mantendo o modo e o texto do botão. Essa tentativa restaura tamanho/posição; não promete recuperar o estado anterior de maximização/fullscreen. O próximo texto é preparado antes da operação para evitar uma troca sem rótulo.

O posicionamento é uma solicitação ao gerenciador de janelas. Se for recusado ou ajustado, o programa registra um aviso, mas permite usar o tamanho que foi aplicado. Depois da troca, solicita novamente (0,0) para a janela secundária, pois algumas plataformas movem janelas filhas junto com a principal.

A conferência de 15/09/2026 também observou a janela secundária fixa em (0,65), apesar da solicitação (0,0); sua relação pai/filha e dimensões 400×700 foram confirmadas. No teste local, o Mac aceitou uma janela de 1570×1056 para um monitor com limites SDL de 1470×956, mas ajustou a posição solicitada (0,0) para (0,65). Portanto, o tamanho excedente foi aplicado, enquanto o posicionamento exato permaneceu sujeito ao sistema. No Windows e no macOS, barras e regras do desktop podem ajustar a posição. No Linux com Wayland, aplicações normalmente não podem escolher a posição absoluta de janelas comuns. Para validar estritamente a centralização e o (0,0), use um ambiente que permita posicionamento, como X11/XWayland disponível na máquina, e registre esse ambiente no relatório. A fonte foi testada no Linux X11 e Windows 11; WSLg permanece pendente.

Referências: [limites do monitor](https://wiki.libsdl.org/SDL3/SDL_GetDisplayBounds), [tamanho](https://wiki.libsdl.org/SDL3/SDL_SetWindowSize), [posição](https://wiki.libsdl.org/SDL3/SDL_SetWindowPosition), [sincronização](https://wiki.libsdl.org/SDL3/SDL_SyncWindow), [desenho de textura](https://wiki.libsdl.org/SDL3/SDL_RenderTexture) e [restrições do Wayland](https://wiki.libsdl.org/SDL3/README-wayland).

### Salvamento da versão ativa em PNG

`src/image/image_save.c` concentra a exportação. `image_save_requested` aceita `SDL_EVENT_KEY_DOWN` com `SDLK_S` e `repeat = false`. O laço principal chama `handle_save_event` para eventos de qualquer uma das janelas. Eventos de liberação da tecla, outras teclas e a repetição automática do teclado não gravam arquivos.

A mesma variável usada para selecionar a textura exibida escolhe a superfície exportada:

```text
se g_isEqualized = verdadeiro: ativa = g_equalizedImage.surface
caso contrário:                ativa = g_image.surface
PNG(x, y, canal) = ativa(x, y, canal)
largura_PNG = ativa.w
altura_PNG  = ativa.h
```

O canal pode ser R, G, B ou alfa. Não há nova conversão para cinza, equalização, interpolação ou alteração de resolução durante o salvamento. `IMG_SavePNG` codifica a superfície em PNG, um formato de compressão sem perda dos valores de pixels; essa codificação é responsabilidade de SDL_image, não uma fórmula adicional implementada pelo grupo. A moldura da janela, os botões, o histograma e o fundo do desktop não fazem parte do PNG.

`image_save_png` consulta a existência do destino com `SDL_GetPathInfo`, chama `IMG_SavePNG` e só anuncia criação/sobrescrita se a gravação retornar sucesso. A consulta de existência serve para a mensagem; o resultado da gravação determina o sucesso real. A função não modifica as imagens em memória. No programa, o destino é sempre o caminho relativo `output_image.png`; os testes usam arquivos em uma pasta isolada dentro de `build/`.

Referências: [IMG_SavePNG](https://wiki.libsdl.org/SDL3_image/IMG_SavePNG) e [SDL_GetPathInfo](https://wiki.libsdl.org/SDL3/SDL_GetPathInfo).

### Conferência dos requisitos em 16/09/2026

A conferência abaixo compara a cópia local com `Requisitos_Trabalho.pdf`. “Validado no Mac/Linux X11/Windows 11” descreve macOS nativo, Ubuntu aarch64 com Xvfb/Openbox e Windows 11 x64 com GCC 15.1.0; não comprova WSLg nem a conclusão da entrega. As evidências Mac/Linux foram recebidas no ZIP; a execução Windows foi realizada nesta revisão.

| Item do enunciado | Implementação e evidência | Situação |
| --- | --- | --- |
| 1. Carregamento | SDL_image; argumentos, arquivo inexistente e formato inválido tratados no terminal | Carregamento validado no Mac/Linux X11/Windows 11; erros de entrada conferidos no Mac e Windows |
| 2. Escala de cinza | Detecção RGB, fórmula exigida, arredondamento e alfa; testes numéricos e linhas com padding | Validado no Mac/Linux X11/Windows 11 |
| 3. Duas janelas | Principal 1024×768; secundária 400×700 fixa e associada como filha; teste consulta relação e dimensões | No Mac, a secundária solicitada em (0,0) foi observada em (0,65); Linux X11 e Windows 11: (0,0) confirmado |
| 4. Histograma | 256 níveis, barras proporcionais, média, desvio e classificações; testes com distribuições conhecidas | Validado no Mac/Linux X11/Windows 11 |
| 5. Equalização | CDF, preservação da original, reversão sem releitura e rótulos/estados do botão | Validado no Mac/Linux X11/Windows 11 por testes numéricos, eventos simulados e renderização |
| 6. Resolução | Alternância, preenchimento da janela e centralização; tamanhos e posições consultados em janelas reais | Validado no Mac/Linux X11/Windows 11; no Mac, ressalva de (0,0) ajustado pelo SO em janela maior que o monitor |
| 7. Salvamento | S salva a versão ativa, sobrescreve e informa resultado; PNG reaberto e comparado pixel a pixel | Validado no Mac/Linux X11/Windows 11; dimensões originais são a interpretação adotada e documentada |
| 8. Textos | Pixelify Sans incluída e localizada junto ao executável; textos e acentos renderizados | Validado no Mac/Linux X11/Windows 11; falta registrar a fonte também no relatório final |

Requisitos técnicos e de entrega que permanecem em aberto:

- Validar WSL Ubuntu 26.04 com GCC 15.2.0 e WSLg. Windows 11 nativo com GCC 15.1.0 passou; Linux aarch64/X11 passou com GCC 15.2.0. Ainda falta o ambiente WSL específico da correção.
- Posição das janelas, relação pai/filha, fonte e escrita do PNG passaram nos testes Windows nativos. Falta conferir esses itens em WSLg e completar a inspeção manual de DPI/múltiplos monitores. A conformidade exata do posicionamento não é garantida em Wayland nem quando o SO ajusta a geometria.
- O [repositório público](https://github.com/PurpleLobster31/ComputacaoVisual_07N) foi confirmado. A main consultada em 16/09/2026 tinha quatro commits atribuídos a Matheus. Falta comprovar a participação dos quatro integrantes; a importação do ZIP preserva o histórico existente.
- Preencher as contribuições individuais no README com base no trabalho real. Os nomes e RAs já estão registrados.
- Elaborar e entregar o relatório final em PDF com link do repositório, fonte, refatorações, dificuldades e soluções, comparação com a análise inicial, referências/uso de IA, temas a estudar e pelo menos oito propostas de melhoria para os quatro integrantes. O objetivo geral do enunciado também prevê comparação com ferramentas comerciais.
- Confirmar a entrega da Etapa 1 em PDF no Moodle e os prazos das duas etapas. A pasta contém o DOCX da Etapa 1, mas os arquivos locais não comprovam a submissão.

O código utiliza C99 e mantém créditos ao exemplo `04-invert_image`; os arquivos gerados em `build/` e `output_image.png` estão no `.gitignore`. Em 15/09/2026, as versões instaladas de SDL3, SDL3_image e SDL3_ttf correspondiam às versões estáveis informadas pelo catálogo Homebrew consultado. A participação no Git, publicação e submissões não foram inferidas nem realizadas nesta etapa.

### Como o Makefile funciona

O Makefile reúne os arquivos C dos diretórios `src`, `src/image` e `src/ui`. Cada um gera um objeto `.o` dentro de `build/`, preservando a divisão em diretórios. A etapa de ligação combina esses objetos com as bibliotecas para produzir `programa` ou `programa.exe`.

A verificação de dependências apresenta erros antes de compilar. `pkg-config` fornece os caminhos e opções das bibliotecas em cada sistema. As opções `-MMD -MP` geram arquivos `.d` para recompilar os módulos afetados quando um cabeçalho muda. O padrão C99 e os avisos `-Wall -Wextra` ajudam a identificar problemas durante o desenvolvimento. O alvo `assets` copia a fonte para junto do executável. `test-build` prepara os cinco testes sem executá-los; `test` os executa. No Windows, `-mconsole` aparece após as opções das bibliotecas para preservar as mensagens do terminal, inclusive quando o SDK SDL fornece `-mwindows`. Os arquivos de build são ignorados pelo `.gitignore`.

### Testes dos cálculos e da interface

`tests/test_histogram.c` verifica imagens preta e branca, distribuição uniforme de 0 a 255, dois extremos com média e desvio de 127,5, linhas com preenchimento, inclusão de pixels transparentes, fronteiras das classificações e rejeição de entradas inválidas. O resultado uniforme esperado é média 127,5 e desvio de aproximadamente 73,90027.

`tests/test_ui.c` renderiza o painel em uma superfície de memória usando os mesmos módulos do programa. Verifica carregamento da fonte, troca e reutilização de rótulos e gera um PNG para inspeção visual. Não requer uma janela visível. `tests/test_equalization.c` verifica o exemplo numérico `[50, 50, 100, 150]`, a preservação do alfa, todos os 256 casos de imagem com um único pixel e uma distribuição uniforme de 256 níveis. Exercita a mesma função de alternância usada pela aplicação, simula pressionar/soltar o botão, confere rótulos e histogramas, verifica que a original permanece intacta e que a equalizada é reutilizada. Também testa falhas de preparação sem troca parcial de estado e gera `build/equalized-preview.png` para inspeção.

`tests/test_resolution.c` verifica centralização, monitor deslocado, dimensões iguais ou maiores que a tela, arredondamento e parâmetros inválidos. Também confere em memória que o desenho ocupa toda a saída, em vez de ficar restrito ao retângulo original da imagem. Com `make test-window`, abre janelas reais e verifica cliques, tamanhos/posições, rótulos, independência da equalização, redimensionamento manual e preservação de estado quando a preparação do texto falha. O caso maior que o monitor aceita uma recusa explícita do gerenciador com restauração do tamanho anterior; quando aceito, exige as dimensões pedidas.

`tests/test_save.c` executa em `build/save-test`. Verifica criação e sobrescrita, reabre o PNG e compara R/G/B/alfa e dimensões com a superfície ativa. Exercita tecla S em ambas as janelas, Shift+S, repetição ignorada, original/equalizada/reversão, independência do modo de resolução e recuperação após falha de escrita. Também confere numericamente a conversão de vermelho, verde, azul e branco, incluindo transparência e linhas com padding.

No Mac, os testes de `make test` também passaram com Clang e AddressSanitizer/UndefinedBehaviorSanitizer, sem erros reportados nos caminhos exercitados:

```sh
make BUILD_DIR=build/sanitize CC=clang CFLAGS='-std=c99 -Wall -Wextra -g -fsanitize=address,undefined -fno-omit-frame-pointer' test
```

Essa execução complementar não altera a compilação principal com GNU GCC e não é uma verificação completa de vazamentos. Esses testes não substituem a validação da aplicação em cada sistema operacional.

## Integrantes e contribuições

| Integrante | RA |
| --- | --- |
| Marina Cantarelli Barroca | 10740412 |
| Matheus Mendonça Lopes | 10443495 |
| Miguel Piñeiro Coratolo Simões | 10427085 |
| Thais Ferreira Canguçu | 10403283 |

As contribuições individuais ainda precisam ser registradas pelo grupo a partir do trabalho efetivamente realizado e do histórico do repositório.
