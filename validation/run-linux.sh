#!/bin/sh
# Executar no Mac com a VM cv-validation ativa (ver documento de validacao).
set -eu
cd "$(dirname "$0")/.."
context=colima-cv-validation
container=cv-linux-validation
# Nomes dedicados: nao altera o contexto Docker global.
docker --context "$context" build -t cv-validation:ubuntu26 -f validation/ubuntu.Dockerfile validation
docker --context "$context" run -d --name "$container" cv-validation:ubuntu26 sleep infinity
archive=$(mktemp -t cv-source.XXXXXX)
trap 'rm -f "$archive"' EXIT
COPYFILE_DISABLE=1 tar -czf "$archive" Makefile src tests assets scripts kodim23.png
docker --context "$context" cp "$archive" "$container:/tmp/source.tar.gz"
docker --context "$context" exec "$container" tar -xf /tmp/source.tar.gz -C /project
docker --context "$context" exec "$container" dbus-run-session -- xvfb-run -a -s '-screen 0 1920x1080x24' sh -c '
  openbox >/tmp/openbox.log 2>&1 &
  for i in 1 2 3 4 5 6 7 8 9 10; do
    xprop -root _NET_SUPPORTING_WM_CHECK | grep -q "window id" && break
    sleep 0.2
  done
  sh scripts/validate.sh --window
'
mkdir -p build/linux-validation
docker --context "$context" cp "$container:/project/build/validation/." build/linux-validation/
echo 'Logs copiados. Ao terminar: docker --context colima-cv-validation stop cv-linux-validation'
