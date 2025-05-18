#!/bin/bash

case "$1" in
  kb)
    ./install/sc-machine/bin/sc-builder -i repo.path -o kb.bin --clear
    ;;
  server)
    export LD_LIBRARY_PATH=$PWD/install/ps-common-lib/lib:$PWD/install/sc-machine/lib:$PWD/install/scl-machine/lib:$PWD/install/non-atomic-action-interpreter-module/lib/extensions:$LD_LIBRARY_PATH
    ./install/sc-machine/bin/sc-machine -s kb.bin \
      -e "install/sc-machine/lib/extensions;install/scl-machine/lib/extensions;build/Release/extensions;install/non-atomic-action-interpreter-module/lib/extensions" -c ostis-example-app.ini
    ;;
  web)
    cd interface/sc-web || exit 1
    source .venv/bin/activate
    python3 server/app.py
    ;;
  *)
    echo "Usage: $0 {kb|server|web}"
    exit 1
    ;;
esac
