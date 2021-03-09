#!/bin/bash
BASE_URL="http://openweathermap.org/img/wn/"

ICONS=(
    "01d.png"
    "01n.png"
    "02d.png"
    "02n.png"
    "03d.png"
    "03n.png"
    "04d.png"
    "04n.png"
    "09d.png"
    "09n.png"
    "10d.png"
    "10n.png"
    "11d.png"
    "11n.png"
    "13d.png"
    "13n.png"
    "50d.png"
    "50n.png"
)

for t in ${ICONS[@]}; do
    wget "$BASE_URL$t"
done
