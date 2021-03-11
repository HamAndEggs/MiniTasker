#/bin/bash
APP_FOLDER=$1
RESOURCE=$2
RESOURCE_TYPE=$3

RESOURCE_FOLDER="$APP_FOLDER/$RESOURCE"

echo "Removing current $RESOURCE folder"
sudo rm -drf $RESOURCE_FOLDER/

echo "Creating new empty $RESOURCE folder $RESOURCE_FOLDER"
sudo mkdir -p $RESOURCE_FOLDER

echo "Copying $RESOURCE too $RESOURCE_FOLDER"
sudo cp -v ./$RESOURCE/*.$RESOURCE_TYPE $RESOURCE_FOLDER/
