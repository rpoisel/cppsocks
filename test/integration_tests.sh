#!/bin/bash

pip3 install pytest
pytest ../test/test_tcp.py

if [[ "$TRAVIS_OS_NAME" == "linux" ]]
then
  cat /etc/os-release | grep '^UBUNTU_CODENAME' | cut -d'=' -f2 | read _ UBUNTU_VERSION_NAME
else
  set UBUNTU_VERSION_NAME = "undefined";
fi
echo "Ubuntu version: ${UBUNTU_VERSION_NAME}"
if [[ "$TRAVIS_OS_NAME" == "osx" || "$UBUNTU_VERSION_NAME" == "bionic" ]]
then
  pip3 install websockets
  pytest ../test/test_ws.py
fi
