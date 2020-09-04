#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os

import pytest
import requests

from fixtures import tcp_client

FILE_DIR = os.path.dirname(os.path.realpath(__file__))


@pytest.fixture
def http_server():
    import subprocess
    import time

    proc = subprocess.Popen(
        os.path.join(os.getcwd(), "examples/http_server/http_server"),
        cwd=FILE_DIR)
    time.sleep(.1)

    yield
    proc.terminate()
    try:
        proc.communicate(1)
    except TimeoutExpired:
        proc.kill()

def test_http_get_root_200(http_server):
    r = requests.get("http://localhost:8080/")
    assert r.status_code == 200
    with open(os.path.join(FILE_DIR, "index.html"), "r") as fh:
        contents = fh.read()
        assert r.text == contents

def test_http_get_root_without_slash_200(http_server):
    r = requests.get("http://localhost:8080")
    assert r.status_code == 200
    with open(os.path.join(FILE_DIR, "index.html"), "r") as fh:
        contents = fh.read()
        assert r.text == contents

def test_http_get_404(http_server):
    r = requests.get("http://localhost:8080/doesnotexist")
    assert r.status_code == 404

@pytest.mark.fixt_data(["examples/http_server/http_server", 8080])
def test_http_get_illegal_path_404(tcp_client):
    tcp_client.sendall("GET /../.. HTTP/1.1\r\nHost: localhost\r\n\r\n".encode())
    assert tcp_client.recv(1024).decode().startswith("HTTP/1.1 403")

@pytest.mark.fixt_data(["examples/http_server/http_server", 8080])
def test_http_incomplete_request(tcp_client):
    tcp_client.sendall("G\r\n\r\n".encode())
    assert tcp_client.recv(1024) == b''

@pytest.mark.fixt_data(["examples/http_server/http_server", 8080])
def test_http_get_incomplete_request(tcp_client):
    tcp_client.sendall("GET\r\n\r\n".encode())
    assert tcp_client.recv(1024) == b''

@pytest.mark.fixt_data(["examples/http_server/http_server", 8080])
def test_http_get_incomplete_request_path(tcp_client):
    tcp_client.sendall("GET /asdf\r\n\r\n".encode())
    assert tcp_client.recv(1024) == b''

@pytest.mark.fixt_data(["examples/http_server/http_server", 8080])
def test_http_unsupported_method(tcp_client):
    tcp_client.sendall("PUT\r\n\r\n".encode())
    assert tcp_client.recv(1024) == b''

