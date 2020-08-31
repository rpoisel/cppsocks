#!/usr/bin/env python
# -*- coding: utf-8 -*-

import asyncio

import websockets

import pytest


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


def test_echo(ws_fixture):
    asyncio.get_event_loop().run_until_complete(communicate_echo())

def test_ping(ws_fixture):
    asyncio.get_event_loop().run_until_complete(communicate_ping())

