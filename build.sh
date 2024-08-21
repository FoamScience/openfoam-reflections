#!/usr/bin/env bash
#sudo apt install libasio-dev git -y
git clone https://github.com/CrowCpp/Crow --depth 1
wmakeLnInclude src/
wmake src/testlib
wclean applications/endpoint
cd TUI || exit 1
go build -o tui main.go
#npm start --prefix reflect-json-app/
#./interactiveUI/interactiveUI
