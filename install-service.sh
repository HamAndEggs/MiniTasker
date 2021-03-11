#/bin/bash
SERVICE_NAME="mini-tasker"
APP_NAME="mini-tasker"
APP_FOLDER="/usr/share/$APP_NAME"

echo "This script uses sudo, please review first if you're not sure."
echo "Do you wish to continue?"
read -p "(y/n)?" answer

if [ $answer == "y" ] || [ $answer == "Y" ] ;then
    echo "Building application"
    appbuild

    ./update-resources.sh

    echo "Copying configuration file"
    sudo cp ./task-file.json &APP_FOLDER

    echo "Copying app to /usr/bin"
    sudo cp ./bin/release/$APP_NAME /usr/bin/

    echo "Copying service file"
    sudo cp ./$SERVICE_NAME.service /lib/systemd/system/

    echo "Setting mode"
    sudo chmpd 664  /lib/systemd/system/$SERVICE_NAME.service

    echo "Enabling at boot"
    sudo systemctl enable $SERVICE_NAME

    echo "service is called $SERVICE_NAME, will be loaded at next boot. Keep cool! :)"
    echo "You now need to go an modify the task-file.json configuration json file, it is loaded from $APP_FOLDER"
    echo "A copy is already waiting for you. Add your weather API key too."

else
    echo "Application not installed"
fi

