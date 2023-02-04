#!/bin/sh

set -e

SRC="$(find src -type f \( -name '*.cpp' -o -name "*.c" \))"
PRGNAME="align"

echo $SRC

debug() {
  g++ -std=c++17 -pedantic -ggdb -o $PRGNAME $SRC \
    || exit 1
  echo "Build debug complete!"
}

release() {
  g++ -O3 -std=c++17 -o $PRGNAME $SRC
}

install() {
  cp align /usr/bin/$PRGNAME
  chmod 755 /usr/bin/$PRGNAME
}

clean() {
  echo cleaning
  set -x
  rm -rf $PRGNAME
}

usage() {
    echo "Usage: $0 [subcmd]\n"
    echo ""
    echo "Subcmds:"
    echo "    build|debug   building in debug mode (Default)"
    echo "    install       building in release and intaling the executable"
    echo "    clean         deleting build files"
}

if [ -z "$1" ]; then 
    debug
else
  case "$1" in
    build|debug) 
      debug ;;
    install) 
      release;
      install ;;
    clean)
      clean ;;
    -h|--help|help) 
        usage ;;
  esac
fi


