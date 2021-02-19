#/bin/bash
git pull
appbuild
sudo systemctl stop mini-tasker
sudo cp ./bin/release/mini-tasker /usr/bin/mini-tasker
sudo systemctl start mini-tasker
sudo systemctl status mini-tasker

