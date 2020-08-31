#!/bin/bash

pip3 install pytest
pytest ../test/test_tcp.py

read PYTHON_MAJOR PYTHON_MINOR PYTHON_PATCH <<< $(python3 --version | cut -d' ' -f2 | sed 's/\./ /g')
if [[ (${PYTHON_MAJOR} -eq 3 && ${PYTHON_MINOR} -ge 6) ]]
then
  pip3 install websockets
  pytest ../test/test_ws.py
fi
