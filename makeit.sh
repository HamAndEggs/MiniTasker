#!/bin/bash
function ShowHelp() {
	echo "usage: makeit.sh [options]"
	echo "  -r rebuild"
	echo "  -x execute after build"
	echo "  --help This help"
	echo ""
}

BUILD_TARGET="mini-tasker"
OUTPUT_EXEC="mini-tasker"
TARGET_PLATFORM="DRM"
NUMBER_OF_THREADS=$(nproc --all)
CMAKE_BUILD_TYPE="Debug"

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
    elif [ "$1" == "DRM" ]; then
        TARGET_PLATFORM="DRM"
    elif [ "$1" == "GTK4" ]; then
        TARGET_PLATFORM="GTK4"
    elif [ "$1" == "X11" ]; then
        TARGET_PLATFORM="X11"
    elif [ "$1" == "Release" ]; then
        CMAKE_BUILD_TYPE="Release"
    else
        echo "Unknown param $1"
    fi
    # Got to next param. shift is a shell builtin that operates on the positional parameters.
    # Each time you invoke shift,
    shift 
done

echo "Building $TARGET_PLATFORM $CMAKE_BUILD_TYPE"
OUTPUT_FOLDER="./build/$CMAKE_BUILD_TYPE"

# Remove exec so if build fails, we don't run old version.
rm -f $OUTPUT_FOLDER/$OUTPUT_EXEC

if [ -n "$REBUILD_SOMETHING" ]; then
    rm -drf $OUTPUT_FOLDER
fi

cmake -D TARGET_PLATFORM=$TARGET_PLATFORM -S . -B $OUTPUT_FOLDER -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE
cmake --build $OUTPUT_FOLDER --target $BUILD_TARGET -- -j$NUMBER_OF_THREADS

if [ -n "$EXECUTE" ]; then
    ls -lha $OUTPUT_FOLDER/$OUTPUT_EXEC
    $OUTPUT_FOLDER/$OUTPUT_EXEC
fi

