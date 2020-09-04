#!/usr/bin/env python
# -*- coding: utf-8 -*-

import pytest


@pytest.fixture
def tcp_client(request):
    import socket
    import subprocess
    import time
    from pathlib import Path
    import os

    data = request.node.get_closest_marker("fixt_data")
    if data is None:
        data = None
    else:
        data = data.args[0]
    proc = subprocess.Popen(
        str(Path(os.getcwd(), data[0]).resolve()))

    client = None
    retries = 0
    while True:
        assert retries < 5

        host_ip, server_port = "127.0.0.1", data[1]
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            client.connect((host_ip, server_port))
        except (ConnectionRefusedError, OSError):
            time.sleep(.1)
            retries += 1
            continue
        break
    yield client
    client.close()
    proc.terminate()
    try:
        proc.communicate(1)
    except TimeoutExpired:
        proc.kill()
