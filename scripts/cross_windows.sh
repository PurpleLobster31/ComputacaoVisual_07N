#!/bin/sh
# No Mac: brew install mingw-w64 pkgconf; execute na raiz do projeto.
set -eu
cd "$(dirname "$0")/.."
sdk_dir="$(pwd)/build/platform-validation/windows-sdk"
mkdir -p "$sdk_dir"
fetch_sdk() {
  repo=$1 version=$2 name=$3
  archive="$sdk_dir/$name-devel-$version-mingw.tar.gz"
  curl -fL "https://github.com/libsdl-org/$repo/releases/download/release-$version/$name-devel-$version-mingw.tar.gz" -o "$archive"
  tar -xf "$archive" -C "$sdk_dir"
}
fetch_sdk SDL 3.4.16 SDL3
fetch_sdk SDL_image 3.4.6 SDL3_image
fetch_sdk SDL_ttf 3.2.2 SDL3_ttf
# Corrige apenas metadados do SDK oficial que contem prefixo de build absoluto.
python3 - "$sdk_dir" <<'PY'
from pathlib import Path
import sys
p = Path(sys.argv[1]) / 'SDL3_ttf-3.2.2/x86_64-w64-mingw32/lib/pkgconfig/sdl3-ttf.pc'
s = p.read_text()
s = '\n'.join('prefix=${pcfiledir}/../..' if line.startswith('prefix=') else line for line in s.splitlines()) + '\n'
p.write_text(s)
PY
export PKG_CONFIG_PATH=''
export PKG_CONFIG_LIBDIR="$sdk_dir/SDL3-3.4.16/x86_64-w64-mingw32/lib/pkgconfig:$sdk_dir/SDL3_image-3.4.6/x86_64-w64-mingw32/lib/pkgconfig:$sdk_dir/SDL3_ttf-3.2.2/x86_64-w64-mingw32/lib/pkgconfig"
make -j2 CC=x86_64-w64-mingw32-gcc UNAME_S=MINGW64_NT-10.0 BUILD_DIR=build/windows all test-build info
python3 scripts/package_windows.py
