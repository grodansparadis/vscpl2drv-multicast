% vscpl2drv-multicast(1) VSCP Level II Multicast Driver
% Ake Hedman and contributors, the VSCP Project
% October 2026

# NAME

vscpl2drv-multicast - VSCP Level II IPv4 multicast driver

# DESCRIPTION

The shared library libvscpl2drv-multicast.so exports the standard VSCP Level II
driver API. It sends and receives VSCP binary events using IPv4 multicast.
The receiver binds the configured UDP port and joins the configured multicast
group. IPv6 multicast is not supported.

# CONFIGURATION

Pass the JSON configuration path and driver GUID to VSCPOpen. A sample
configuration is installed as
/usr/share/vscpl2drv-multicast/multicast.json.

**interface**
: Required multicast group and port, for example udp://239.192.0.1:33333.
  The destination must resolve to an IPv4 multicast address (224.0.0.0/4);
  the port must be 1-65535.

**multicast.interface-address**
: Optional numeric IPv4 address assigned to the local interface used to send
  and receive multicast. If omitted or empty, the operating system selects the
  interface.

**multicast.ttl**
: Outgoing multicast TTL, integer 0-255. Defaults to 1.

**multicast.loopback**
: Controls whether outgoing multicast is delivered to local multicast
  receivers. Defaults to true.

**multicast.reuse-address**
: Enables SO_REUSEADDR before binding the receive socket, allowing multiple
  local receivers on the same multicast port where supported. Defaults to true.

**flags**
: 0 enables sending and receiving, 1 receives only, 2 sends only, and 3
  disables both directions.

**security**
: Optional object containing encryption, key and hmac. Plaintext is the
  default. Legacy top-level security settings are accepted only when the
  security object is absent; mixing both forms is rejected.

**security.encryption**
: none (default), aes128, aes192 or aes256. AES uses CBC with a fresh random
  16-byte IV.

**security.key**
: Path to a hexadecimal AES key of exactly 16, 24 or 32 bytes, as required by
  the selected cipher. Whitespace in the file is ignored.

**security.hmac**
: Optional path to a separate 32-byte hexadecimal HMAC key. HMAC requires AES.
  HMAC-SHA256 is truncated to 16 bytes and verified before decryption. Both
  communicating endpoints must use the same encryption and authentication
  settings.

**filter**
: Optional `in-filter`, `in-mask`, `out-filter` and `out-mask` VSCP filter
  strings. By default all events are accepted.

**logging**
: Console, rotating-file, syslog (except on Windows) and UDP log output are
  supported. UDP logging is separate from multicast event transport.

**write**
: Reserved; runtime configuration changes are not implemented.

Multicast does not guarantee delivery or ordering and provides no
acknowledgements or replay protection. Encryption without HMAC provides
confidentiality only. Keep configuration and key files private to the driver
user.

# TESTING

Build and run the tests from the repository root:

    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build
    ctest --test-dir build --output-on-failure

## Standalone peer

The `vscp-multicast-peer` executable uses the same JSON configuration and can
either use the built-in transport or load a driver library with `--driver`.

    vscp-multicast-peer send|receive --config FILE [--driver LIBRARY] [options]

**--count N**
: Number of events, 1-1000000; default 1.

**--timeout MS**
: Timeout per operation, 0-3600000 milliseconds; default 5000. Use `infinite`
  to continue waiting across timeouts.

**--interval MS**
: Send-only delay between events, 0-3600000 milliseconds; default 1000.

**--class N**, **--type N**
: Send-only VSCP class and type, each 0-65535; defaults 10 and 6.

**--guid GUID**
: Send-only event GUID as sixteen colon-separated hexadecimal bytes.

**--data HEX**
: Send-only payload as comma-separated two-digit hexadecimal bytes.

Create a receive configuration:

    {
      "interface": "udp://239.192.0.1:33333",
      "flags": 1,
      "multicast": { "interface-address": "127.0.0.1" },
      "security": { "encryption": "none" },
      "logging": { "console-enable": false }
    }

Use the same group, port, interface selection and security settings in a
sender configuration with `flags` set to 2. Start the receiver before sending:

    ./build/vscp-multicast-peer receive --config receive.json --count 5 --timeout 60000
    ./build/vscp-multicast-peer send --config send.json --count 5 --class 10 --type 6 --data 01,02,ff

Received events are written to stdout as JSON lines; diagnostics go to stderr.
Only load trusted driver libraries matching the peer architecture and VSCP
event ABI.

# SEE ALSO

The VSCP binary protocol specification:
<https://grodansparadis.github.io/vscp-doc-spec/#/./vscp_over_binary>
