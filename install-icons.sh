#/bin/bash
SERVICE_NAME="mini-tasker"
APP_NAME="mini-tasker"
ICON_FOLDER="/usr/share/mini-tasker/icons"

echo "Creating icon folder $ICON_FOLDER for application $APP_NAME"
sudo mkdir -p $ICON_FOLDER

echo "Copying icons too $ICON_FOLDER"
sudo cp -v ./icons/*.png $ICON_FOLDER/ 
