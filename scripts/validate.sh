#!/bin/sh
# Executar no macOS/Linux ou no shell MSYS2 UCRT64 do Windows.
set -eu
case "${1:-}" in
  "") window_tests=false ;;
  --window) window_tests=true ;;
  *) echo "Uso: sh scripts/validate.sh [--window]"; exit 2 ;;
esac
cd "$(dirname "$0")/.."
repo_dir=$(pwd)
platform=$(uname -s)
architecture=$(uname -m)
build_dir="build/native-$platform-$architecture"
log_dir="$repo_dir/build/validation/$(date -u +%Y%m%dT%H%M%SZ)-$platform"
mkdir -p "$log_dir"
exe_ext=""
case "$platform" in MINGW*|MSYS*|UCRT*) exe_ext=.exe ;; esac

run_check() {
  name=$1
  shift
  if "$@" > "$log_dir/$name.log" 2>&1; then
    echo "PASS $name" | tee -a "$log_dir/results.txt"
  else
    result=$?
    echo "FAIL $name (exit $result)" | tee -a "$log_dir/results.txt"
    cat "$log_dir/$name.log"
    echo "Logs: $log_dir"
    exit "$result"
  fi
}

{
  date -u
  uname -a
  if [ -f /etc/os-release ]; then cat /etc/os-release; fi
  if command -v sw_vers >/dev/null 2>&1; then sw_vers; fi
  printf 'SDL_VIDEODRIVER=%s\n' "${SDL_VIDEODRIVER:-automatico}"
} > "$log_dir/environment.txt"
run_check versions make info
run_check build make -j2 "BUILD_DIR=$build_dir" all test-build
run_check tests make "BUILD_DIR=$build_dir" test
font_check() {
  cd "$log_dir"
  "$repo_dir/$build_dir/test_ui$exe_ext" "$repo_dir/kodim23.png" "$log_dir/font-preview.png"
}
run_check font-outside-project font_check
# font_check muda o diretorio desta shell, portanto voltamos ao projeto.
cd "$repo_dir"
if "$window_tests"; then
  run_check windows make "BUILD_DIR=$build_dir" test-window
else
  echo "NOT RUN windows (use --window em uma sessao grafica)" | tee -a "$log_dir/results.txt"
fi
printf 'Logs: %s\n' "$log_dir"
