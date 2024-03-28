#! /bin/bash

# Destination and files 

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
    echo "Installing $HEADER -> $INC_LOCATION"
    cp $HEADER "$INC_LOCATION"
done

for LIB  in "${LIBS[@]}"; do
    echo "Installing $LIB -> $LIB_LOCATION"
    cp $LIB "$LIB_LOCATION"
done

# Setup linker

ldconfig 