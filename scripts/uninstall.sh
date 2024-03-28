#! /bin/bash

LIB_DIR="lib"
INC_DIR="include"

LIBS=()
for LIBPATH in "$LIB_DIR"/lib*.so; do
    LIBS+=("$LIBPATH")
done

HEADERS=()
for HEADERPATH in "$INC_DIR"/*.h; do
    HEADERS+=("$HEADERPATH")
done

LIB_LOCATION="/usr/local/lib"
INC_LOCATION="/usr/local/include"

#Copy library files

for HEADER  in "${HEADERS[@]}"; do
    NAME=$(basename "$HEADER")
    echo "Removing $INC_LOCATION/$NAME"
    rm -f "$INC_LOCATION/$NAME"
done

for LIB  in "${LIBS[@]}"; do
    NAME=$(basename "$LIB")
    echo "Removing $LIB_LOCATION/$NAME"
    rm -f "$LIB_LOCATION/$NAME"
done

# Setup linker

ldconfig 