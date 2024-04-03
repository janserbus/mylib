#! /bin/bash

LIB_DIR="lib"
INC_DIR="include"

LIBS=()
while IFS= read -r -d '' LIBPATH; do
    LIBS+=("$LIBPATH")
done < <(find "$LIB_DIR" -maxdepth 1 -type f -name 'lib*.so' -print0)

HEADERS=()
while IFS= read -r -d '' HEADERPATH; do
    HEADERS+=("$HEADERPATH")
done < <(find "$INC_DIR" -maxdepth 1 -type f -name '*.h' -print0)

LIB_LOCATION="/usr/local/lib"
INC_LOCATION="/usr/local/include"

#Copy library files

if [ ${#HEADERS[@]} -gt 0 ]; then
    for HEADER in "${HEADERS[@]}"; do
        NAME=$(basename "$HEADER")
        echo "Removing $INC_LOCATION/$NAME"
        rm -f "$INC_LOCATION/$NAME"
    done
else
    echo "No header files found in $INC_DIR"
fi

if [ ${#LIBS[@]} -gt 0 ]; then
    for LIB in "${LIBS[@]}"; do
        NAME=$(basename "$LIB")
        echo "Removing $LIB_LOCATION/$NAME"
        rm -f "$LIB_LOCATION/$NAME"
    done
else
    echo "No library files found in $LIB_DIR"
fi

# Setup linker

ldconfig 