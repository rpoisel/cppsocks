#!/usr/bin/env python
# -*- coding: utf-8 -*-

import asyncio

import pytest
import websockets


@pytest.fixture
def ws_fixture():
    import subprocess
    from pathlib import Path
    import os
    import time

    proc = subprocess.Popen(
        str(Path(os.getcwd(), "examples/ws_echo/ws_echo").resolve()))
    time.sleep(1)

    yield

    proc.terminate()
    try:
        proc.communicate(1)
    except TimeoutExpired:
        proc.kill()

async def communicate_echo():
    uri = "ws://127.0.0.1:8080/"
    async with websockets.connect(uri) as websocket:
        request = "Some Test"

        for i in range(0, 100):
            await websocket.send(request)
            response = await websocket.recv()
            assert request == response

async def communicate_ping():
    uri = "ws://127.0.0.1:8080/"
    async with websockets.connect(uri) as websocket:

        for i in range(0, 10):
            await websocket.ping()

async def communicate_large_msg():
    uri = "ws://127.0.0.1:8080/"
    async with websockets.connect(uri) as websocket:
        request = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Sed elementum tempus egestas sed sed risus pretium quam. Lacinia quis vel eros donec ac odio tempor orci dapibus. Sem viverra aliquet eget sit amet tellus cras. Euismod lacinia at quis risus sed vulputate odio. Ipsum consequat nisl vel pretium. Egestas erat imperdiet sed euismod nisi porta lorem mollis aliquam. Suscipit tellus mauris a diam. Blandit massa enim nec dui nunc mattis enim ut tellus. Tempus iaculis urna id volutpat lacus laoreet non curabitur. Nam at lectus urna duis convallis convallis tellus. Sit amet consectetur adipiscing elit ut aliquam purus sit amet. Donec enim diam vulputate ut pharetra sit amet aliquam."

        for i in range(0, 2):
            await websocket.send(request)
            response = await websocket.recv()
            assert request == response



def test_echo(ws_fixture):
    asyncio.get_event_loop().run_until_complete(communicate_echo())

def test_ping(ws_fixture):
    asyncio.get_event_loop().run_until_complete(communicate_ping())

def test_large_msg(ws_fixture):
    asyncio.get_event_loop().run_until_complete(communicate_large_msg())

