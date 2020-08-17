#!/usr/bin/env python
# -*- coding: utf-8 -*-

import pytest


@pytest.fixture
def tcp_client():
    import socket
    import subprocess
    import time
    from pathlib import Path
    import os

    proc = subprocess.Popen(
        str(Path(os.getcwd(), "examples/tcp_echo/tcp_echo").resolve()))

    client = None
    retries = 0
    while True:
        assert retries < 5

        host_ip, server_port = "127.0.0.1", 5555
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


def test_echo(tcp_client):
    data = " Hello how are you?\n"
    tcp_client.sendall(data.encode())
    assert tcp_client.recv(1024).decode() == data


def test_multi_echo(tcp_client):
    for i in range(1000):
        data = " Hello how are you?\n"
        tcp_client.sendall(data.encode())
        assert tcp_client.recv(1024).decode() == data
