# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
"""Small sequential JSON-RPC client shared by simulator examples and tests.

Use one caller per Peer. Replies retain JSON-RPC errors; notifications received
during a call are queued for notification(). Binary uploads are Backend-only.
The reader must be a buffered binary stream (socket.makefile("rb") or a pipe).
"""
from contextlib import contextmanager
import json
import queue
import socket
import threading

def read_frame(stream):
    length = None
    while True:
        line = stream.readline()
        if not line:
            raise EOFError("RPC stream closed")
        if line == b"\r\n":
            break
        key, value = line.decode("ascii").split(":", 1)
        if key.lower() == "content-length":
            length = int(value)
    if length is None or not 0 < length <= 16 * 1024 * 1024:
        raise ValueError("invalid frame length")
    body = bytearray()
    while len(body) < length:
        chunk = stream.read(length - len(body))
        if not chunk:
            raise EOFError("truncated response")
        body.extend(chunk)
    return json.loads(body)


class Peer:
    def __init__(self, reader, writer):
        self.writer = writer
        self.next_id = 0
        self.notifications = []
        self.inbox = queue.Queue()
        def receive():
            try:
                while True:
                    self.inbox.put(read_frame(reader))
            except BaseException as exc:
                self.inbox.put(exc)
        self.receiver = threading.Thread(target=receive, daemon=True)
        self.receiver.start()

    def call(self, method, params=None):
        self.next_id += 1
        payload = json.dumps({"jsonrpc": "2.0", "id": self.next_id,
                              "method": method, "params": params or {}}).encode()
        self.writer.write(f"Content-Length: {len(payload)}\r\n\r\n".encode() + payload)
        self.writer.flush()
        while True:
            reply = self.inbox.get(timeout=8)
            if isinstance(reply, BaseException):
                raise reply
            if reply.get("id") == self.next_id:
                return reply
            if "method" in reply:
                self.notifications.append(reply)

    def notification(self, method=None, timeout=8):
        for index, value in enumerate(self.notifications):
            if method is None or value.get("method") == method:
                message = self.notifications.pop(index)
                return message if method is None else message["params"]
        while True:
            reply = self.inbox.get(timeout=timeout)
            if isinstance(reply, BaseException):
                raise reply
            if "method" in reply and (method is None or reply["method"] == method):
                return reply if method is None else reply["params"]
            self.notifications.append(reply)

    def binary(self, headers, payload):
        self.next_id += 1
        transfer = str(self.next_id)
        fields = {"Content-Length": str(len(payload)),
                  "Content-Type": "application/vnd.esp-gsp.binary",
                  "X-GSP-Transfer-ID": transfer, **headers}
        self.writer.write("".join(f"{k}: {v}\r\n" for k, v in fields.items()).encode() + b"\r\n" + payload)
        self.writer.flush()
        result = self.notification("binary_result")
        assert result["transfer_id"] == transfer, result
        return result

    def scalar(self, op, *args):
        reply = self.call("bridge_call", {"op": op, "args": list(args) + [0] * (8 - len(args))})
        assert "result" in reply, reply
        return reply["result"]


@contextmanager
def connect(host="127.0.0.1", port=8266):
    """Connect over TCP; own the socket, buffered streams and receiver lifetime."""
    with socket.create_connection((host, port), timeout=8) as sock:
        sock.settimeout(None)
        with sock.makefile("rb") as reader, sock.makefile("wb") as writer:
            peer = Peer(reader, writer)
            try:
                yield peer
            finally:
                try:
                    sock.shutdown(socket.SHUT_RDWR)
                except OSError:
                    # A disconnected socket may already be shut down. Preserve
                    # the caller's original error and still join the receiver.
                    pass
                peer.receiver.join()
