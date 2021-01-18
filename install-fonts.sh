#/bin/bash
SERVICE_NAME="mini-tasker"
APP_NAME="mini-tasker"
FONT_FOLDER="/usr/share/mini-tasker/liberation_serif_font"

echo "Creating font folder $FONT_FOLDER for application $APP_NAME"
sudo mkdir -p $FONT_FOLDER

echo "Copying fonts too $FONT_FOLDER"
sudo cp -v ./liberation_serif_font/*.ttf $FONT_FOLDER/ 
