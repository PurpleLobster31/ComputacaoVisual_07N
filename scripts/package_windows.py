"""Empacota executaveis ja compilados e DLLs oficiais, sem executar Windows."""
from pathlib import Path
import shutil
import subprocess
import zipfile

root = Path(__file__).resolve().parent.parent
build = root / 'build'
sdk = build / 'platform-validation/windows-sdk'
package = build / 'windows-validation'
package.mkdir(exist_ok=True)
for binary in (build / 'windows').glob('*.exe'):
    shutil.copy2(binary, package)
for library in sdk.glob('*/x86_64-w64-mingw32/bin/*.dll'):
    shutil.copy2(library, package)
for licenses in sdk.glob('*/x86_64-w64-mingw32/share/licenses'):
    shutil.copytree(licenses, package / 'licenses', dirs_exist_ok=True)
shutil.copytree(root / 'assets', package / 'assets', dirs_exist_ok=True)
shutil.copy2(root / 'kodim23.png', package)
(package / 'VALIDAR_WINDOWS.cmd').write_bytes((root / 'scripts/VALIDAR_WINDOWS.cmd').read_text().replace('\n', '\r\n').encode('utf-8'))
shutil.copy2(root / 'README.md', package)
shutil.copytree(root / 'docs', package / 'docs', dirs_exist_ok=True)
(package / 'LEIA-ME.txt').write_text('''PACOTE DE VALIDACAO - WINDOWS 10/11 x64
Compilado no macOS por cross-compilacao: MinGW-w64 GCC 16.2.0.
SDL3 3.4.16, SDL3_image 3.4.6, SDL3_ttf 3.2.2 (DLLs oficiais).
Ainda nao executado no Windows. Nao substitui recompilar com GCC 15.1.0.

1. Extraia todo o ZIP para uma pasta em que voce possa gravar arquivos.
2. Execute VALIDAR_WINDOWS.cmd. Ele abre janelas brevemente e grava logs.
3. No Prompt de Comando nessa pasta: programa.exe kodim23.png
4. Confira textos e acentos, histograma e duas janelas. Alterne Equalizar /
   Ver original e Resolucao original / 1024x768 varias vezes.
5. Pressione S com cada janela em foco. Confira output_image.png e a
   mensagem de sobrescrita. A imagem salva conserva 768x512 pixels.
6. Feche cada janela em execucoes separadas: o programa deve encerrar.
7. Repita a abertura em uma pasta com espacos e acentos, e com outra imagem
   maior que o monitor. Anote escala de tela, monitores e posicoes obtidas.
8. Envie a pasta logs e suas observacoes. Registre tambem a versao do
   Windows (winver). PASS automatico nao substitui essa inspecao manual.

Nao separe executaveis, DLLs e assets. Os testes sobrescrevem arquivos
somente em logs; o programa salva na pasta atual do terminal.
Licencas das bibliotecas em licenses; fonte em assets/fonts/OFL.txt.
''', encoding='utf-8')
# Inventario dos imports PE para auditoria das dependencias estaticas.
with (package / 'imports.txt').open('w') as report:
    for binary in sorted(package.iterdir()):
        if binary.suffix in ('.exe', '.dll'):
            data = subprocess.check_output(['x86_64-w64-mingw32-objdump', '-p', str(binary)], text=True)
            report.write(binary.name + '\n')
            report.write('\n'.join(line.strip() for line in data.splitlines() if 'DLL Name:' in line or 'Subsystem' in line) + '\n\n')
archive = build / 'validacao-windows-x64.zip'
with zipfile.ZipFile(archive, 'w', zipfile.ZIP_DEFLATED) as zipped:
    for path in sorted(package.rglob('*')):
        if path.is_file():
            zipped.write(path, path.relative_to(build))
print(archive)
