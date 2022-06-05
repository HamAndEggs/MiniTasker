#!/bin/bash
function ShowHelp() {
	echo "usage: makeit.sh [options]"
	echo "  -r rebuild"
	echo "  -x execute after build"
	echo "  --help This help"
	echo ""
}

OUTPUT_EXEC="MiniTasker"
OUTPUT_FOLDER="./build"

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

# If output folder not found, force rebuild. Maybe a fresh fetch of the code.
if [ ! -d $OUTPUT_FOLDER ]; then
    REBUILD_SOMETHING="TRUE"
fi

if [ -n "$REBUILD_SOMETHING" ]; then
    echo "Cleaning folders"
    rm -drf $OUTPUT_FOLDER
    mkdir -p $OUTPUT_FOLDER
    cd $OUTPUT_FOLDER
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    cd ..
    ls
fi

cd $OUTPUT_FOLDER
make -j4

cd ..
if [ -n "$EXECUTE" ]; then
    $OUTPUT_FOLDER/$OUTPUT_EXEC
fi
