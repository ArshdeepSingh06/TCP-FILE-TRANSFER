# TCP File Transfer

A minimal client-server file transfer application written in **C** using the **Winsock2** API. It sends a file from a client to a server over a raw **TCP** socket connection, using a small custom protocol to frame the filename and file size ahead of the raw file data. Works with any file type (text, images, videos, etc.) since data is read/written in binary mode.

## How It Works

Communication happens in three stages over a single TCP connection:

1. **Filename** — the client sends a fixed **100-byte** buffer containing the filename.
2. **File size** — the client sends the file size as an **8-byte `long long`**.
3. **File data** — the client streams the file contents in **1024-byte chunks** until the entire file has been sent, then calls `shutdown(sock, SD_SEND)` to signal it's done sending.

The server mirrors this: it reads the 100-byte filename, then the 8-byte size, then loops on `recv()` — writing each chunk to disk with `fwrite()` — until it has received the expected number of bytes.

```
Client                              Server
------                              ------
open file (rb)
connect()  ────────────────────►    accept()
send(filename, 100 bytes)  ────►    recv(filename, 100 bytes)
send(fileSize, 8 bytes)    ────►    recv(fileSize, 8 bytes)
send(chunk, ≤1024 bytes)   ────►    recv(chunk) + fwrite()   [repeat until fileSize reached]
shutdown(SD_SEND)
```

## Project Structure

```
TCP-FILE-TRANSFER/
├── client/
│   └── client.c
├── server/
│   └── server.c
└── examples/
```

## Requirements

- Windows OS (uses `winsock2.h`)
- A C compiler with Winsock support (MinGW/GCC or MSVC)
- Linked against `ws2_32.lib` (already handled via `#pragma comment(lib, "ws2_32.lib")`)

## Building

Using MinGW/GCC:

```bash
gcc server/server.c -o server.exe -lws2_32
gcc client/client.c -o client.exe -lws2_32
```

## Running

1. **Start the server** first (it listens on port `8080` and waits for a connection):

   ```bash
   server.exe
   ```

2. **Run the client** on the same machine (or update the hardcoded IP in `client.c` for a different host), and enter the name of a file that exists in the client's working directory when prompted:

   ```bash
   client.exe
   Enter file name to send: photo.jpg
   ```

3. The server saves the incoming file to its own working directory, using the same filename it received from the client.

## Current Limitations

- **Single connection only** — the server accepts exactly one client and exits after the transfer completes.
- **Hardcoded address** — the client connects to `127.0.0.1:8080`; the target IP/port isn't configurable via arguments.
- **No error handling on `send()`/`recv()`** — return values aren't checked for partial sends/receives or connection drops.
- **No encryption or integrity check** — files are sent in plaintext with no checksum verification on arrival.
- **Filename buffer is fixed at 100 bytes** — very long filenames/paths could overflow or truncate.

## Possible Improvements

- Loop the server to accept multiple sequential (or concurrent, via threads) client connections.
- Add command-line arguments for host/port instead of hardcoding them.
- Add checksum (e.g., MD5/SHA-256) verification after transfer.
- Add basic error handling and retry logic around socket calls.
- Support directory transfers or multiple files per session.

## License

No license specified.
