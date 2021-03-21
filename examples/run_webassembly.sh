#! /bin/bash
python3 -m http.server 8888 &
P1=$!
trap 'kill 0' SIGINT; P1
python3 -m webbrowser http://localhost:8888
wait $P1
