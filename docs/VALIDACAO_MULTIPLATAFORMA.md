# Validação de portabilidade — histórico de 15/09/2026

**Atualização em 16/09/2026:** Windows 11 x64 recompilado com GCC 15.1.0;
todos os testes passaram, incluindo janelas. Consulte o [registro Windows](VALIDACAO_WINDOWS.md).
As referências a Windows pendente abaixo descrevem a rodada anterior.

Este registro compara a implementação com os requisitos e separa compilação,
execução automatizada e inspeção manual. A versão do compilador faz parte da
evidência: compilar com uma versão mais recente não certifica a versão da correção.

## macOS

Executado nativamente em macOS 26.6.2, Apple Silicon arm64, GNU GCC 16.2.0,
pkgconf 3.0.7, SDL3 3.4.16, SDL3_image 3.4.6 e SDL3_ttf 3.2.2.
`sh scripts/validate.sh --window` passou: compilação, cinco executáveis de teste,
fonte fora do diretório do projeto e teste das janelas. Logs desta rodada:
`build/validation/20260915T195953Z-Darwin/`.

O teste consulta a relação pai/filha, tamanho fixo 400×700, alternância entre
768×512 e 1024×768, centralização, redimensionamento manual e independência da
equalização. No monitor de 1470×956 unidades, o tamanho maior 1570×1056 foi aceito.
Cocoa ajustou a posição (0,0) para (0,65), tanto na filha quanto no caso maior.
Portanto, não se declara conformidade literal com (0,0). O programa registra o
ajuste; o teste aceita essa limitação documentada para a janela maior e apenas
registra a posição da filha. A centralização dos tamanhos normais é verificada.

As dimensões da janela são unidades de coordenadas SDL; uma tela Retina pode ter
mais pixels físicos. O PNG salva os pixels da superfície original/equalizada,
sem depender da densidade ou do tamanho da janela. Mac Intel não foi testado.

## Linux: execução realizada

Ubuntu **26.04.1 LTS**, aarch64, GCC **15.2.0-16ubuntu1**, pkgconf 2.5.1,
SDL3 3.4.16, SDL3_image 3.4.6, SDL3_ttf 3.2.2. Executado em container Docker
na VM Colima com virtualização Apple; kernel Linux 6.8.0-117-generic da VM.
A sessão gráfica usou X11, Openbox e Xvfb 1920×1080×24. Os fontes foram copiados
para o sistema de arquivos Linux, sem montagem da pasta do Mac.

Resultado de `sh scripts/validate.sh --window`: **PASS** para versões,
compilação, cinco executáveis de teste, fonte fora da pasta do projeto e janelas.
Não apareceram avisos do compilador nos builds Mac, Linux e Windows desta rodada.
O painel PNG gerado no Linux também foi inspecionado: barras, botões e acentos
foram renderizados corretamente. Logs: `build/validation/20260915T200309Z-Linux/`.

A filha ficou fixa em 400×700 na posição (0,0). As posições centrais foram
(448,156) para 1024×768 e (576,284) para 768×512. A janela maior de 2020×1180
foi aceita em (0,0). Também passaram reversão da equalização, preservação dos
pixels, redimensionamento manual e recuperação de falhas simuladas.

Isso valida Linux aarch64/X11 virtualizado. **Não valida WSLg, Wayland, Linux
x86_64, GPU física, diferentes gerenciadores de janelas ou DPI/múltiplos monitores.**
GCC 15.2.0 corresponde à versão requerida, mas o ambiente completo de correção
WSL ainda precisa ser testado. As bibliotecas foram compiladas dos releases oficiais;
a instalação de pacotes sozinha pode fornecer versões diferentes.

Reprodução no Mac (ferramentas instaladas nesta validação: Colima, Docker CLI e
MinGW-w64 via Homebrew):

```sh
colima start cv-validation --vm-type vz --cpus 2 --memory 4 --disk 16 --root-disk 12 --mount none --activate=false --ssh-config=false
sh validation/run-linux.sh
# Depois de guardar os logs:
docker --context colima-cv-validation stop cv-linux-validation
colima stop cv-validation
```

O script usa um container chamado `cv-linux-validation`; se ele já existir,
reutilize-o manualmente ou remova apenas esse container de teste após guardar os
resultados antes de repetir o script. Não modifica o contexto Docker padrão.
`validation/ubuntu.Dockerfile` registra as versões SDL, mas Ubuntu/apt recebem
atualizações; sempre compare os novos logs. A imagem desta execução teve ID
`sha256:ef228318f65ee434639217bfaf0a05ae52dff76d6098f1c78b5fbe2206a66f94`.
A VM foi parada ao terminar; ferramentas e imagem permanecem para reprodução.

## Windows

Os seis executáveis PE x86_64 (aplicação e cinco testes) foram compilados e ligados
no Mac com MinGW-w64/GCC 16.2.0 e os SDKs oficiais SDL nas mesmas versões acima.
Os imports PE foram inspecionados: as dependências externas ao Windows são
SDL3.dll, SDL3_image.dll e SDL3_ttf.dll, incluídas no pacote. As demais são DLLs
do sistema e contratos UCRT. A inspeção de imports não comprova carregamento
ou funcionamento em tempo de execução.

**Não houve execução Windows.** Miguel poderá fazê-la posteriormente.
O compilador de correção exigido é GCC 15.1.0; essa compilação continua pendente.
Não se confunde o pacote MinGW oficial com uma instalação MSYS2 UCRT64: o teste
nativo do código deve usar as bibliotecas e ferramentas do mesmo ambiente.

O Makefile agora passa `-mconsole` após as bibliotecas no Windows. O SDK SDL
oficial fornece `-mwindows`; a opção final mantém o terminal disponível para
mensagens, erros e resultados dos testes. Os arquivos `.exe` usam o mesmo C99,
os mesmos módulos e fórmulas do Mac/Linux.

O alvo `make test-build` compila os testes sem executá-los. Isso permite a
compilação cruzada sem tentar executar um PE no macOS. O script
`scripts/cross_windows.sh` baixa os SDKs versionados e reproduz a compilação;
`scripts/package_windows.py` prepara o ZIP. O `.pc` do SDK SDL_ttf 3.2.2 possui
um prefixo absoluto da máquina de empacotamento; o script o torna relativo a
`${pcfiledir}` somente na cópia baixada, sem alterar a biblioteca.

### Roteiro para a máquina Windows

1. Copiar `build/validacao-windows-x64.zip`, extrair **todo** o conteúdo e executar
   `VALIDAR_WINDOWS.cmd`. Não executar dentro do visualizador de ZIP.
2. Guardar a pasta `logs`. O script verifica cálculos, exportação/releitura RGBA,
   interface em memória, fonte fora da pasta do executável e janelas reais.
3. Abrir `programa.exe kodim23.png` em um Prompt de Comando na pasta extraída.
   Conferir textos/acentos, barras, cliques, equalização/reversão, resolução,
   tecla S em ambas as janelas, sobrescrita e encerramento ao fechar cada janela.
4. Repetir numa pasta com espaços e acentos, e com imagem maior que o monitor.
   Registrar versão Windows (`winver`), escala DPI, monitores, posição da filha
   e comportamento de janela maior. `PASS` automático não comprova esses itens.
5. No MSYS2 UCRT64, recompilar o código conforme README e executar
   `sh scripts/validate.sh --window`. Registrar `make info`; instalar pacotes
   atuais não garante GCC 15.1.0. Essa versão precisa ser conferida separadamente.

O pacote inclui licenças SDL e a licença OFL da fonte. O comando de testes grava
arquivos em `logs`; a aplicação grava `output_image.png` no diretório atual.
Reexecutar os testes substitui seus logs e imagens anteriores.

## Diferenças que afetam o uso

| Aspecto | macOS | Windows | Linux / WSL |
| --- | --- | --- | --- |
| Compilar | GNU GCC do Homebrew; `gcc` da Apple é Clang | Shell MSYS2 UCRT64 para Makefile; executável `.exe` | GCC/Make e pacotes da distribuição |
| Bibliotecas | Bibliotecas dinâmicas do Homebrew | DLLs no PATH do UCRT64 ou junto ao executável | `.so` instaladas e encontradas pelo carregador |
| Fonte | `assets/fonts` ao lado do executável | Mesma estrutura, inclusive no ZIP | Mesma estrutura; nomes distinguem maiúsculas/minúsculas |
| Caminho de imagem | Caminho POSIX, entre aspas se tiver espaços | `/c/Users/...` no MSYS2; `C:\Users\...` no Prompt | Caminho POSIX; no WSL disco Windows usualmente em `/mnt/c` |
| Saída PNG | Pasta atual do terminal | Pasta atual do terminal; evitar diretório protegido | Pasta atual, sujeita às permissões do usuário |
| Janelas | Cocoa pode ajustar o topo para a área utilizável | Validar barras, DPI, relação pai/filha e múltiplos monitores | X11 depende do gerenciador; Wayland pode impedir posição global; WSL precisa de sessão gráfica/WSLg |
| Cálculos e pixels | Mesmas fórmulas C99 e arredondamento | Mesmas fórmulas; execução ainda pendente | Mesmas fórmulas; backend gráfico não altera a superfície salva |

A fonte é localizada via `SDL_GetBasePath`, não pela pasta atual. O arquivo de
entrada e o PNG de saída usam a pasta atual. Não há reversão matemática de cinza
para RGB colorido: “Ver original” recupera a cópia em cinza preservada. As fórmulas
de conversão, histograma, desvio, equalização, escala de barras e centralização
continuam detalhadas no README e não mudam por sistema operacional.

## Requisitos e pendências de entrega

Os testes cobrem os cálculos dos itens 2, 4 e 5, renderização, geometria do item 6
e gravação do item 7. Janelas e textos dos itens 3 e 8 exigem confirmação por
backend. Posição literal (0,0), WSLg, Windows nativo, DPI e múltiplos monitores
continuam pontos de conferência. Salvar nas dimensões da superfície é a
interpretação documentada do item 7; não se produz uma captura redimensionada.

A validação técnica não resolve relatório final PDF, referências/IA,
contribuições individuais, publicação e histórico do repositório, submissões
Moodle ou oito propostas de melhoria. O checklist completo permanece no README.

## Referências e reprodução

- [SDL 3.4.16](https://github.com/libsdl-org/SDL/releases/tag/release-3.4.16)
- [SDL_image 3.4.6](https://github.com/libsdl-org/SDL_image/releases/tag/release-3.4.6)
- [SDL_ttf 3.2.2](https://github.com/libsdl-org/SDL_ttf/releases/tag/release-3.2.2)
- [Fonte Pixelify Sans e licença](https://github.com/google/fonts/tree/main/ofl/pixelifysans)

`sh scripts/validate.sh` compila e executa testes sem janelas visíveis;
`sh scripts/validate.sh --window` acrescenta o teste gráfico. Cada execução cria
uma pasta datada em `build/validation`, com versões, ambiente e logs separados.
Os builds nativos têm diretórios distintos por sistema/arquitetura, evitando
reutilizar objetos da compilação Windows cruzada. Ao mudar compilador/flags no
mesmo sistema, use um diretório limpo antes de repetir.
