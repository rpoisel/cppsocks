#!/usr/bin/env python
# -*- coding: utf-8 -*-

import pytest

from fixtures import tcp_client


@pytest.mark.fixt_data(["examples/tcp_echo/tcp_echo", 5555])
def test_echo(tcp_client):
    data = " Hello how are you?\n"
    tcp_client.sendall(data.encode())
    assert tcp_client.recv(1024).decode() == data


@pytest.mark.fixt_data(["examples/tcp_echo/tcp_echo", 5555])
def test_multi_echo(tcp_client):
    for i in range(1000):
        data = " Hello how are you?\n"
        tcp_client.sendall(data.encode())
        assert tcp_client.recv(1024).decode() == data
