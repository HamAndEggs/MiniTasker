#/bin/bash
SERVICE_NAME="mini-tasker"
APP_NAME="mini-tasker"

echo "Building app, you have to have 'appbuild' installed to do this"
appbuild

echo "Copying app to /usr/bin will need root access"
sudo cp ./bin/release/$APP_NAME /usr/bin/

echo "Copying service file"
sudo cp ./$SERVICE_NAME.service /lib/systemd/system/

echo "Setting mode"
sudo chmpd 664  /lib/systemd/system/$SERVICE_NAME.service

echo "Enabling at boot"
sudo systemctl enable $SERVICE_NAME

echo "service is called $SERVICE_NAME, will be loaded at next boot. Keep cool! :)"
