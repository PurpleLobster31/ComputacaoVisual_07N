# Validação Windows — 16/09/2026

## Resultado

**PASS**: compilação nativa dos fontes desta revisão e execução dos cinco
programas de teste, incluindo janelas reais. Não foi necessário alterar os
algoritmos de processamento do ZIP recebido.

- Windows 11 Home Single Language, versão 10.0.26200, x64.
- GCC 15.1.0, WinLibs MinGW-w64 UCRT POSIX SEH, revisão r4, MinGW-w64 13.0.0.
- C99, `-Wall -Wextra`, sem avisos de compilação.
- SDKs oficiais SDL3 3.4.16, SDL3_image 3.4.6 e SDL3_ttf 3.2.2, x86_64.
- Tela reportada pela SDL: 1920×1080. Outras escalas DPI e múltiplos monitores
  não foram exercitados.
- Projeto executado na pasta original, cujo caminho contém espaços e acentos.

O pacote Windows recebido no ZIP, compilado no Mac com GCC 16.2.0, também passou
na mesma bateria antes da recompilação. O resultado nativo GCC 15.1.0 é a evidência
principal desta rodada. Este build usa PowerShell e SDKs oficiais; não certifica
a instalação MSYS2 descrita como alternativa no README.

## Verificações

| Verificação | Resultado observado |
| --- | --- |
| Histograma | Contagens, média, desvio, limites, pitch e alfa corretos |
| Conversão para cinza | Fórmula exigida, detecção e preservação do alfa passaram |
| Equalização | CDF, imagens constantes, reversão, cache, cliques simulados e falha atômica passaram |
| Interface em memória | Fonte, acentos, barras e botões renderizados; PNG inspecionado |
| Fonte fora da pasta | Teste executado em `logs/` encontrou a fonte junto ao executável |
| Salvamento | Criação, sobrescrita, RGBA e dimensões exatos, tecla S, repetição e recuperação de erro passaram |
| Janela secundária | Relação pai/filha confirmada; fixa em 400×700, posição efetiva (0,0) |
| Janela principal | 1024×768 em (448,156); original 768×512 em (576,284) |
| Imagem maior que a tela | 2020×1180 aceitos, posição solicitada (0,0) |
| Independência dos modos | Troca de resolução preserva equalização; resize manual e falha de texto passaram |
| Linha de comando | Sem argumento, imagem ausente e arquivo inválido: mensagem pertinente e saída 1 |

Na aplicação do pacote recebido, houve também inspeção visual da imagem em cinza,
salvamento pela tecla S (PNG 768×512) e encerramento pela janela principal.
Os cliques dos testes são eventos simulados; não se declara uma inspeção manual
completa de ambos os botões, das duas janelas e de todas as escalas DPI.

Logs locais: `build/native-build.log`, `build/windows-native/logs/` e
`build/windows-validation/logs/`. Logs, prévias, SDKs, compiladores, DLLs,
executáveis e ZIPs não são versionados. Este documento registra o resumo.

## Reproduzir no PowerShell

1. Baixe o [WinLibs GCC 15.1.0 UCRT r4](https://github.com/brechtsanders/winlibs_mingw/releases/tag/15.1.0posix-13.0.0-ucrt-r4)
   (x86_64, ZIP) e extraia, por exemplo, em `C:\tools\gcc15`.
   **O caminho do compilador deve ser sem acentos.** O linker desta distribuição
   falhou quando o compilador estava dentro de `Computação Visual`; um junction
   não resolveu. Usar uma cópia real em caminho ASCII resolveu.
2. Na raiz do projeto, obtenha os SDKs oficiais. O comando `tar` deve estar
   disponível (incluído nas versões recentes de Windows 10/11):

```powershell
New-Item -ItemType Directory -Force build/platform-validation/windows-sdk | Out-Null
$sdk = 'build/platform-validation/windows-sdk'
$releases = @(
    @('SDL', 'SDL3', '3.4.16'),
    @('SDL_image', 'SDL3_image', '3.4.6'),
    @('SDL_ttf', 'SDL3_ttf', '3.2.2')
)
foreach ($release in $releases) {
    $repo, $name, $version = $release
    $archive = "$sdk/$name-devel-$version-mingw.tar.gz"
    Invoke-WebRequest "https://github.com/libsdl-org/$repo/releases/download/release-$version/$name-devel-$version-mingw.tar.gz" -OutFile $archive
    tar -xf $archive -C $sdk
    if ($LASTEXITCODE -ne 0) { throw "Falha ao extrair $archive" }
}
```

3. Compile, valide e abra uma imagem:

```powershell
./scripts/build_windows.ps1 -Gcc C:/tools/gcc15/mingw64/bin/gcc.exe
./build/windows-native/VALIDAR_WINDOWS.cmd --no-pause
./build/windows-native/programa.exe kodim23.png
```

Ajuste `-Gcc` à localização real. O script usa caminhos relativos para fontes,
SDKs e saída, evitando os erros do linker com argumentos absolutos acentuados.
Ele copia DLLs, fonte, licenças e imagem de exemplo junto aos seis executáveis.
Não modifica PATH global nem instala bibliotecas no sistema. O arquivo PNG salvo
pela aplicação fica na pasta atual do terminal. Não use `make clean` antes de
guardar os logs: ele remove `build/`, incluindo SDKs locais.

## Pendências para concluir o projeto

1. **WSL Ubuntu 26.04 / GCC 15.2.0 / WSLg:** falta executar no ambiente específico;
   WSL não está instalado nesta máquina. O registro anterior de Linux aarch64/X11
   não substitui essa verificação.
2. **Inspeção manual complementar:** ambos os botões (neutro/hover/pressionado),
   S com cada janela em foco, fechamento pela janela secundária e diferentes
   escalas/monitores. O comportamento automatizado passou no ambiente acima.
3. **Relatório final da Etapa 2 em PDF:** ainda ausente. Deve incluir integrantes,
   RAs, link público, Pixelify Sans, refatorações, problemas/soluções, comparação
   com a análise inicial, referências e uso de IA, assuntos a estudar e pelo
   menos oito melhorias. O objetivo geral também pede comparação com opções
   comerciais. Entregar no Moodle, fora do repositório de código.
4. **Contribuições individuais e histórico:** preencher o README com fatos do
   grupo. A `main` consultada tinha quatro commits, todos atribuídos a Matheus.
   Os commits desta importação não reconstituem o histórico de desenvolvimento
   nem demonstram participação dos quatro integrantes.
5. **Prazos e submissões:** `AGENTS.md` informa Etapa 1 entregue; existe o DOCX,
   mas não há comprovante local da submissão em PDF. Confirmar no Moodle, junto
   com os prazos e a entrega final. Não alterar a análise inicial retroativamente.
6. **Publicação final:** a revisão deve ser conferida na nova branch e integrada
   à branch de entrega antes do prazo. Reavaliar as versões estáveis das SDLs na
   data da entrega; este registro comprova as versões testadas, não versões futuras.

O enunciado não define os limites de brilho/contraste nem se o PNG deve acompanhar
o tamanho da janela. As escolhas atuais (limites documentados e pixels originais
da versão ativa) estão explicadas no README; confirmar com o professor somente
se houver orientação diferente.
