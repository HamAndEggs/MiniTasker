#/bin/bash
APP_FOLDER="/usr/share/mini-tasker"

echo "Updating repo"
git pull

echo "Updating EdgeUI repo"
cd ./EdgeUI
git pull
cd ..

./update-resources.sh

echo "Building application"
appbuild
#./makeit.sh DRM Release

echo "Updaing service"
sudo systemctl stop mini-tasker
sudo cp ./build/Release/mini-tasker /usr/bin/mini-tasker
#sudo cp ./bin/release/mini-tasker /usr/bin/mini-tasker
sudo systemctl start mini-tasker
sudo systemctl status mini-tasker

