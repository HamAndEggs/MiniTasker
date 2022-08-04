#!/bin/bash
function ShowHelp() {
	echo "usage: makeit.sh [options]"
	echo "  -r rebuild"
	echo "  -x execute after build"
	echo "  --help This help"
	echo ""
}

OUTPUT_EXEC="MiniTasker"
OUTPUT_FOLDER="./build/debug"

# Process the params
while [ "$1" != "" ];
do
    if [ "$1" == "-r" ]; then
        REBUILD_SOMETHING="TRUE"
    elif [ "$1" == "-x" ]; then
        EXECUTE="TRUE"
    elif [ "$1" == "--help" ]; then
        ShowHelp
        exit 0
    fi
    # Got to next param. shift is a shell builtin that operates on the positional parameters.
    # Each time you invoke shift,
    shift 
done

# Remove exec so if build fails, we don't run old version.
rm -f $OUTPUT_FOLDER/$OUTPUT_EXEC

cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug --target MiniTasker -- -j8

if [ -n "$EXECUTE" ]; then
    ls -lha $OUTPUT_FOLDER/$OUTPUT_EXEC
    $OUTPUT_FOLDER/$OUTPUT_EXEC
fi
