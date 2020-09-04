#!/bin/bash

exitCode=0
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

pip3 install pytest requests
pytest ${SCRIPT_DIR}/test_tcp.py || exitCode=1
pytest ${SCRIPT_DIR}/test_http.py || exitCode=1

read PYTHON_MAJOR PYTHON_MINOR PYTHON_PATCH <<< $(python3 --version | cut -d' ' -f2 | sed 's/\./ /g')
if [[ (${PYTHON_MAJOR} -eq 3 && ${PYTHON_MINOR} -ge 6) ]]
then
  pip3 install websockets
  pytest ${SCRIPT_DIR}/test_ws.py || exitCode=1
fi

exit ${exitCode}
