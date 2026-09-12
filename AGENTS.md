# AGENTS.md — Projeto 1 (Computação Visual): Processador de Imagens em C/SDL3

> Este arquivo serve dois propósitos: (1) é o `AGENTS.md` que orienta agentes de IA (GitHub Copilot, etc.)
> dentro deste repositório; (2) pode ser copiado e colado como prompt completo em qualquer outra
> ferramenta de IA (Claude, ChatGPT etc.) caso o grupo precise trocar de ferramenta no meio do
> desenvolvimento — basta colar este arquivo inteiro como contexto inicial.

## Contexto do projeto

Este é o Projeto 1 (Proj1) da disciplina **Computação Visual**, ministrada pelo Prof. André Kishimoto
na Universidade Presbiteriana Mackenzie (Faculdade de Computação e Informática, Ciência da Computação).
É um trabalho em grupo (2 a 4 pessoas) que consiste em desenvolver um software de processamento de
imagens em **linguagem C**, usando a biblioteca **SDL3** (Simple DirectMedia Layer, versão 3) e
**SDL_image**, a partir de um código-base fornecido pelo professor no repositório da disciplina.

A Etapa 1 (análise inicial, sem implementação) já foi entregue. Estamos agora na **Etapa 2**:
implementação completa + relatório final.

## Stack técnica

- **Linguagem:** C, padrão C99 ou mais recente.
- **Bibliotecas obrigatórias:** SDL3 e SDL_image (versões mais recentes e estáveis disponíveis,
  não necessariamente as usadas no material da disciplina — checar changelog/documentação para
  mudanças de API).
- **Fonte de texto:** SDL_ttf, com uma fonte específica escolhida pelo grupo e embutida no
  repositório (não depender de fontes instaladas no sistema).
- **Compilador:** GCC. Ambiente oficial de correção: **gcc 15.1.0 no Windows 10/11** e
  **gcc 15.2.0 no WSL Ubuntu 26.04**. O grupo também desenvolve em macOS — atenção: no macOS,
  o comando `gcc` é por padrão um alias para o Clang da Apple (via Xcode Command Line Tools),
  não o GNU GCC real. Se possível, instalar o GCC real via Homebrew (`brew install gcc`) para
  reduzir divergências de comportamento/erros de compilação em relação ao ambiente de correção.
- **Sistemas operacionais do grupo:** Linux (WSL) e macOS. Editor padronizado: VS Code.
- **Build:** linha de comando (Makefile recomendado, com detecção de SO via `uname` para ajustar
  caminhos de include/lib entre Homebrew no macOS e apt/instalação manual no Linux/WSL).

## Uso do programa

```
programa caminho_da_imagem.ext
```

## Funcionalidades obrigatórias (resumo do escopo)

1. **Carregamento de imagem** via SDL_image, com tratamento de erro para arquivo inexistente ou
   formato inválido (mensagem pertinente no terminal).
2. **Detecção e conversão para escala de cinza**: verificar se a imagem já é grayscale ou colorida
   (mensagem no terminal); se colorida, converter usando `Y = 0.2125*R + 0.7154*G + 0.0721*B`.
   A imagem em escala de cinza é a base para todas as operações seguintes.
3. **GUI com duas janelas**:
   - Janela principal: exibe a imagem, 1024x768 px inicialmente, centralizada no monitor principal.
   - Janela secundária (filha da principal): tamanho fixo definido pelo grupo, posicionada em (0,0),
     exibe histograma + informações + dois botões.
4. **Histograma**: calcular e exibir na janela secundária; classificar média de intensidade
   ("clara"/"média"/"escura") e desvio padrão/contraste ("alto"/"médio"/"baixo").
5. **Equalização do histograma**: botão desenhado com primitivas SDL abaixo do histograma. Clique
   equaliza; clique de novo reverte para a versão original em escala de cinza (sem recarregar
   arquivo). Texto do botão muda ("Equalizar" / "Ver original"). Estado visual reflete
   neutro/hover/clicado (ex. azul / azul claro / azul escuro).
6. **Alternância de resolução**: outro botão abaixo do anterior. Alterna entre resolução original
   e 1024x768. Janela principal é redimensionada e recentralizada (ou posicionada em (0,0) se
   exceder a resolução do monitor). Mesmo padrão de texto/estado visual do botão anterior.
7. **Salvar imagem**: tecla `S` salva a imagem atualmente exibida como `output_image.png`
   (sobrescrevendo se já existir), com mensagem de resultado no terminal.
8. **Texto na interface**: fonte/família escolhida deve ser documentada no README e carregar
   corretamente independentemente do SO.

## Pontos técnicos de atenção conhecidos pelo grupo

- **GCC vs Clang no macOS**: ver acima — testar compilação também em ambiente Linux/WSL real
  antes de cada entrega, já que é o ambiente oficial de correção.
- **Case sensitivity**: WSL/Linux é case-sensitive, Windows e macOS (por padrão) são
  case-insensitive. Cuidado com nomes de arquivos/caminhos (ex. fontes, assets) que diferem só
  em maiúsculas/minúsculas.
- **Botões interativos são um componente à parte**: desenhar o retângulo do botão é simples;
  a lógica de hover/clique/estado é código de UI que deve ser escrito do zero (não vem pronto
  no código-base) e é reutilizada pelos dois botões (equalizar e resolução).
- **API de janelas do SDL3** para relação pai/filha pode diferir entre versões do próprio SDL3
  usadas pelo grupo vs. a versão de referência do professor — checar documentação da versão
  instalada antes de assumir comportamento.

## Convenções de código e de repositório (para o agente seguir)

- Organizar o código em módulos com responsabilidades claras (ex. carregamento, escala de cinza,
  histograma, equalização, UI/botões, janelas, renderização de texto) — não tudo em um `main.c`.
- Gerenciamento de memória correto: toda alocação (`SDL_CreateTexture`, buffers de pixel, etc.)
  deve ter liberação correspondente; preferir checar com Valgrind/AddressSanitizer quando possível.
- Commits pequenos e frequentes, com mensagens descritivas, refletindo a participação de todos os
  integrantes (a avaliação penaliza repositórios com 1-2 commits gigantes).
- Não commitar artefatos de build (binários, `.o`, pastas `build/`) — usar `.gitignore`.
- Ao gerar código, preferir C99 puro e explicitar quando algo depende de uma API específica da
  versão do SDL3 instalada.
- Ao sugerir soluções, indicar quando uma abordagem é específica de SO e precisa de
  `#ifdef`/detecção de plataforma.

## Entregáveis finais

- Código-fonte completo no repositório público (GitHub).
- `README.md` explicando o projeto, como funciona, contribuições de cada integrante, e processo
  completo de compilação/execução por SO + versões de compilador e bibliotecas usadas.
- Relatório da Etapa 2 em PDF (fora do repositório de código, entregue via Moodle).
