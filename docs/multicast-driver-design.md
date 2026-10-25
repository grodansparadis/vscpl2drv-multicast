# Multicast transport

The multicast driver reuses VSCP binary framing, CRC validation, encryption,
authentication, filters, queueing, and Level II API handling from the UDP
driver. Its transport is IPv4 UDP multicast; multicast changes socket setup and
destination addressing, not the event wire format.

## Supported behavior

- `interface` is a `udp://group:port` URL. The resolved IPv4 address must be
  within `224.0.0.0/4` and the port must be 1-65535.
- Send sockets target that group and port. Optional
  `multicast.interface-address`, `multicast.ttl`, and `multicast.loopback`
  configure the local outgoing interface, TTL, and local delivery.
- Receive sockets bind the wildcard address on the configured port, enable
  `SO_REUSEADDR` by default, then explicitly join the group with
  `IP_ADD_MEMBERSHIP`. `multicast.interface-address` selects the membership
  interface as well as the outgoing interface.
- `flags` values retain the original meanings: 0 is duplex, 1 receive-only, 2
  send-only, and 3 disables both directions. Disabled directions do not join a
  group or configure send-only options.
- IPv6 multicast is not currently supported. A resolved unicast or broadcast
  address is rejected rather than sent as ordinary UDP.
- UDP logging remains a separate logging sink; it is not routed through the
  event multicast group.

The default group is `239.192.0.1:33333`, TTL is 1, multicast loopback and
address reuse are enabled. Select a group and scope appropriate for the local
network. A local interface address must be assigned to the machine. When left
empty, interface selection is delegated to the operating system.

## Reliability and security

Multicast does not provide delivery acknowledgement, ordering, replay
protection, or guaranteed delivery. The application-level receive filters and
queue limits apply unchanged. Encryption and optional HMAC protect packet
contents according to the existing VSCP frame format; every endpoint must use
matching settings and keys. Encryption without HMAC does not authenticate a
sender.

The standalone `vscp-multicast-peer` shares the driver transport and config
parser. Integration tests exercise multicast loopback through both its
standalone mode and the exported Level II API, including plaintext, AES,
HMAC, malformed frames, receive timeouts, and disabled directions.
