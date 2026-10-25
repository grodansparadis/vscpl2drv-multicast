# vscpl2drv-multicast

VSCP Level II driver that sends and receives VSCP events over IPv4 multicast.
It exports the standard Level II driver API (`VSCPOpen`, `VSCPClose`,
`VSCPRead`, `VSCPWrite`, and `VSCPGetVersion`) and uses UDP datagrams for
transport. The receiver explicitly joins the configured multicast group.

The driver supports multicast groups in `224.0.0.0/4`, including AES-128,
AES-192, AES-256 encryption, optional HMAC-SHA256 authentication, and VSCP
event input/output filters. Multicast does not provide acknowledgements,
ordering, replay protection, or guaranteed delivery. IPv6 multicast is not
currently supported.

## Configuration

The driver accepts a JSON configuration file. `interface` specifies the
multicast group and UDP port; `multicast.interface-address` optionally selects
a local IPv4 interface address. Leave it empty to use the operating system's
default route/interface.

```json
{
  "interface": "udp://239.192.0.1:33333",
  "flags": 0,
  "multicast": {
    "interface-address": "192.168.1.20",
    "ttl": 1,
    "loopback": true,
    "reuse-address": true
  },
  "security": {
    "encryption": "none",
    "key": "",
    "hmac": ""
  },
  "logging": {
    "console-enable": true,
    "console-level": "info"
  }
}
```

Use an address assigned to the local host for `interface-address`, not the
multicast group. Use the same group, port, and security configuration on
senders and receivers. A sender may omit `interface-address` to let the OS
choose an outgoing interface.

| Setting | Meaning |
|---------|---------|
| `interface` | IPv4 multicast destination and port, e.g. `udp://239.192.0.1:33333`. A multicast address and port 1-65535 are required. |
| `flags` | `0`: send and receive; `1`: receive only; `2`: send only; `3`: both directions disabled. |
| `multicast.interface-address` | Optional numeric local IPv4 interface address. Defaults to OS interface selection. |
| `multicast.ttl` | Outgoing IPv4 multicast TTL, integer 0-255; default `1`. |
| `multicast.loopback` | Whether outgoing multicast is looped back locally; default `true`. |
| `multicast.reuse-address` | Allow multiple local multicast receivers on the port where supported; default `true`. |
| `security.encryption` | `none`, `aes128`, `aes192`, or `aes256`. Defaults to `none`. |
| `security.key` | Path to a hexadecimal AES key file: 16, 24, or 32 bytes for the selected cipher. |
| `security.hmac` | Optional path to a 32-byte hexadecimal HMAC key file. HMAC requires AES. |
| `filter.in-filter`, `filter.in-mask` | Optional VSCP receive filter and mask. |
| `filter.out-filter`, `filter.out-mask` | Optional VSCP send filter and mask. |

The filter values use the VSCP filter string syntax. See the VSCP documentation
for filter details. Empty filters accept all events.

Logging can be directed to the console, rotating files, syslog (not available
on Windows), and a separate UDP log collector. The UDP logging settings
(`udp-enable-log`, `udp-host`, `udp-port`, `udp-log-level`, and `udp-pattern`)
are independent from multicast event transport.

An installed sample is provided as `multicast.json`. Configure the daemon's
Level II driver entry with the path to `libvscpl2drv-multicast.so` (or the
platform equivalent), the JSON configuration path, and a unique driver GUID.

## Build and test

Requirements: CMake 3.18 or newer, a C++17 compiler, OpenSSL, and Expat. CMake downloads the
pinned VSCP sources, spdlog, and nlohmann/json dependencies at configure time.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Install with `cmake --install build --prefix /usr/local`; the driver is placed
in the VSCP Level II driver directory and the sample JSON in the shared data
directory. On Linux, install the driver into `/var/lib/vscp/drivers/level2`
according to your VSCP daemon setup.

## Standalone peer

`vscp-multicast-peer` uses the same JSON configuration, so it is useful for
manual send/receive tests without running a VSCP daemon:

```sh
./build/vscp-multicast-peer receive --config receive.json --count 5 --timeout 60000
./build/vscp-multicast-peer send --config send.json --count 5 \
  --class 10 --type 6 --data 01,02,ff
```

Use `flags: 1` in the receive config and `flags: 2` in the send config. Peer
diagnostics are printed to stderr; received events are printed as JSON lines.
Set `logging.console-enable` to `false` to keep stdout JSON-only. The peer can
also load a built driver library with `--driver /path/to/libvscpl2drv-multicast.so`
to test the exported Level II API.

## License

MIT. See [LICENSE](LICENSE).
