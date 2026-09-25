---
title: The Hyperboria Protocol
abbrev: Hyperboria
docname: RFC-0001
category: info
ipr: trust200902
area: Routing
workgroup: HyperboriaNS
std: false

stand_alone: true
pi:
  toc: yes
  sortrefs: yes
---

# The Hyperboria Protocol

## Status of This Memo

This document is an informational overview of the wire, routing and
cryptographic conventions used by the Hyperboria network. It is written in
the style of an RFC so that the protocol can be discussed, implemented and
extended against an agreed-upon reference, independent of any particular
implementation.

The terminology used in this memo defines the Hyperboria network and the
HyperboriaNS software stack. This memo does not specify an Internet standard;
it is distributed for information and review.

This memo is associated with the HyperboriaNS project, an independent,
autotools-based, single- daemon mesh networking daemon and its routing
infrastructure. The authoritative behaviour of the reference is the
HyperboriaNS source tree, which this document describes.

## Abstract

Hyperboria is a peer-to-peer, self-forming, cryptographically-addressed mesh
network intended to provide end-to-end confidentiality, integrity and
availability of routing without dependence on central allocation authorities.
Every node is identified by a public key; its IPv6 address is derived
unforgeably from that key. Traffic is encrypted and authenticated on every
hop of its journey, from an unauthenticated hello to full session keys.

The network is composed of two cooperating planes:

* **The switch plane**, a connectionless, label-switched fabric in which
  packets are forwarded by small, stateless switches without any memory
  lookups or per-destination state. Switch labels are self-describing paths
  through the fabric that can be reversed cheaply, enabling bidirectional
  traffic over a single label.

* **The DHT plane**, a distributed hash table whose nodes are the mesh nodes
  themselves and whose keys are cryptographically-derived Hyperboria
  addresses. The DHT is used to discover the label (path) to a given node.

This memo describes: addressing and identity derivation; the link
encryption scheme (CryptoAuth); the switch label format and encoding; the
encapsulation headers and content types; session management; the
HyperboriaDHT query/reply wire format; tunnelling of legacy IP; and protocol
version negotiation.

## 1.  Introduction

The Hyperboria network follows a small number of design invariants:

1.  Every node owns a long-lived keypair. The node's identity and its
    addressable location in the network are both derived from (or bound to)
    that keypair)Skip existing.
2.  There is no central address allocation. An address is a function of a
    public key; a node's address is the same regardless of where it attaches
    to the network, and possession of the private key is the only thing that
    gives a node the right to "be" that address.
3.  Every packet is cryptographically authenticated and, except for
    handshake data, encrypted in transit hop by hop.
4.  Forwarding in the core is connectionless and memoryless: a switch only
    needs to read a few bits of a self-describing label to forward, and can
    compute the reverse label without storing state.

The protocol described here is implemented by HyperboriaNS ("Hyperboria
Network Stack"), a single daemon, `hyperboria-route`, composed in C with no
external runtime dependencies beyond the system's build toolshare. The
protocol version of the software is 1.0.0 and the on-the-wire protocol
version is 21.

### 1.1.  Requirements Language

The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT",
"SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" in this
document are to be interpreted as described in RFC 2119.

### 1.2.  Terminology

Hyperboria
: The network and the set of protocols it transmits over shared
interconnects. The network is colloquially referred to as "the network".

HyperboriaNS (Hyperboria Network Stack)
: The software project that implements this protocol as a single daemon,
including the administrative and diagnostic tools that are shipped with it.

`hyperboria-route`
: The core daemon of HyperboriaNS. It provides the switch, the DHT, the
CryptoAuth engine, and the tunnelling interfaces that together make up one
node in the Hyperboria network. Historically this component was called
"cjdroute".

Hyperboria address
: A 16-byte IPv6 address within the `fc00::/8` range, derived from a node's
public key (see Section 3).

Label / path label
: A 64-bit value describing the path from one switch to another switch
through the fabric (Section 6).

Interface
: A bidirectional attachment point between two switches, or between a node
and its switch.

Encoding scheme
: A description (a set of "forms") of how labels are laid out and how many
bits each label segment occupies.

Session
: The CryptoAuth-protected, replay-protected association between two nodes
over a fixed path, identified by a pair of handles.

Node
: A single Hyperboria node: a switch plus a routing engine plus a DHT
client operating as one administrative unit.

## 2.  Conventions

* All integers are unsigned.
* On the wire, multi-byte integers that appear in headers are big-endian
  ("network byte order") unless explicitly stated otherwise. This is noted
  per-field in each header definition.
* All cryptographic keys are 32 bytes.
* All addresses are 16 bytes and are written in standard IPv6 textual form
  (see RFC 4291).
* Labels are printed as four groups of four hex digits separated by dots,
  e.g. `0000.0000.0000.4005`.
* A "content type" is a 16-bit unsigned integer carried in the data header
  of a packet.
* All bit and byte positions in this document are zero-indexed from the most
  significant end of the field as displayed.

## 3.  Identity and Addressing

### 3.1.  Node Identity

A node is identified by a 32-byte public key. This key is the node's
permanent public key and is used for:

* Onionised key agreement (Section 5): the key is used as the Curve25519 key
  of the node in the shared-secret computations.
* Identity: the same key is carried in the `CryptoHeader` of every handshake
  packet so the recipient can authenticate the sender (via Poly1305
  authenticator), and so the recipient can derive the sender's Hyperboria
  address.

### 3.2.  Address Derivation

A node's Hyperboria address is a deterministic function of its public key
`P` (32 bytes). The algorithm is:

1.  Compute `H = SHA-512(P)`.
2.  Compute `Hash = SHA-512(H)` (i.e. a double SHA-512).
3.  The address candidate is the first 16 bytes of `Hash`.
4.  The address is only valid if its leading address-prefix bits are
    correct (see below). Because `P` must produce an address beginning with
    the `fc00::/8` prefix, not every key yields an address; when a node's key
    does not, the node MUST draw a fresh keypair. This is the mechanism by
    which the "difficulty" of the `fc00::/8` puzzle is implemented---on
    average 256 key generations are needed.

The prefix is expressed by two compile-time constants:

* `ADDRESS_PREFIX`, default `0xFC`, and
* `ADDRESS_PREFIX_BITS`, default `8`.

The top `ADDRESS_PREFIX_BITS` bits of the address are forced to the value
`ADDRESS_PREFIX`. With the defaults, the high byte of every valid address is
`0xFC`, so all Hyperboria addresses lie in the IPv6 `fc00::/8` space. The
prefix is applied by clearing the top `ADDRESS_PREFIX_BITS` bits of the
derived bytes and setting them to `ADDRESS_PREFIX`.

Validity check: a 16-byte `A` is a valid Hyperboria address if

~~~
BigEndian64( first 8 bytes of A )
    & PrefixMask == PrefixValue
~~~

When constructing the "return" path for a packet the router writes the
address prefix back into the source address field so that the source address
always looks valid.

### 3.3.  Address Form

Within the DHT and in packet headers, an address is not carried as a
variable set of text octets but fixed and known-width. The serialized form of
a reachable peer ("CPE"), used in node lists, is:

| Field                | Size (bytes) | Meaning                                  |
|----------------------|--------------|------------------------------------------|
| IPv6 address         | 16           | The node's Hyperboria address (Section 3) |
| Public key           | 32           | The node's permanent public key           |
| Path label (64-bit)  | 8            | Label of the path to the node (Section 6) |

The serialized size of one node is 56 bytes; the version of the protocol in
use at that node is carried in a parallel array (see the DHT section).

## 4.  Protocol Layering

Each node is expected to implement the following layering, from the physical
to the application:

| Layer   | Name                    | Purpose                                              |
|---------|-------------------------|------------------------------------------------------|
| L2      | Switch / CryptoAuth     | Encrypted, label-switched hop protocol                |
| L3      | HyperboriaDHT / IP6     | Cryptographic routing and content carriage            |
| L4+     | TCP/UDP/ICMP (guest) /  | End-to-end datagrams for the network's own services   |

In terms of the on-the-wire packet, the lowest framing is:

~~~
0                8
+-+-+-+-+-+-+-+-+          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  SwitchHeader   |  (12 bytes: label, version, flags etc.) 
+-+-+-+-+-+-+-+-+          |     CryptoHeader (120 bytes)  |
| CryptoHeader +   |       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| handshake/traffic|       |   ...encrypted content...     |
+-+-+-+-+-+-+-+-+          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
~~~

The outer header is the switch header (only present between switches over
the fabric); next is the CryptoHeader (inside which packets are
authenticated and, after the handshake completes, encrypted); inside that is
the data header (4 bytes: version + content type) followed by the content.

For traffic (non-handshake) packets a 4-byte nonce counter is carried in the
CryptoHeader; the full 24-byte nonce for the packet's encryption is built
from that counter (Section 5.3).

## 5.  Link Encryption: CryptoAuth

Link encryption protects the packet on *each hop* between two directly
connected CryptoAuth endpoints (i.e. between a node and its peers and any
pair of switches that has established a session). It provides:

* **Confidentiality**: after the handshake, packet payloads are encrypted
  with the equal of NaCl's `crypto_box_curve25519xsalsa20poly1305`
  construction.
* **Integrity and authenticity**: every packet is authenticated with Poly1305.
  While the handshake is still in progress the amount of data that can be
  sent is limited to the handshake exchange; content cannot pass before the
  session is established.
* **Replay protection**: a per-session, windowed replay protector rejects
  replayed packets and requires monotonically increasing counters within a
  sliding window.

### 5.1.  Session State

A CryptoAuth session is a state machine over the following phases:

* A session `permanentKey` is randomly drawn.
* `nextNonce` starts at xyz.
* No state is stored until the first packet is received.

Sessions distinguish three functional roles by the "session state" recorded
in the first (CryptoHeader) field of the packet:

| Value                 | Meaning                                                                     |
|-----------------------|-----------------------------------------------------------------------------|
| `1` or `2`            | A "hello" packet (handshake initiation / repeat hello)                      |
| `2` or `3`            | A "key" packet (handshake response / repeat key)                            |
| `UINT32_MAX`          | A "connectToMe" packet (see Section 5.6)                                    |
| any other             | A "traffic" packet; the value is the nonce counter for the packet           |

The same values in the "Session State" field indicate a repeated
transmission of the same packet; repeats are used for retransmission and for
the situation where an initiator must wait for key material (see below).

### 5.2.  Handshake Packets

The handshake is carried in a `CryptoHeader` of 120 bytes, subdivided as
follows:

~~~
0                   4                   8                  12
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    +-+-+-+-+-+-+
|                         Session State                        |    | Auth |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ -> | ...  |
|                         Auth Challenge                       |    +-+-+-+-+-+
|                                                              |      12 bytes
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    
|                     handshake Nonce (24 bytes)               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|               Permanent Public Key (32 bytes)                |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Authenticator (16 bytes)                  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|        Encrypted/Authenticated Temporary PubKey (32 bytes)   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   (content follows up to MTU, encrypted+authenticated)       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
~~~

Where:

* **Session State**: as tabulated above.
* **Auth Challenge** (12 bytes): used to authenticate the sender to a specific
  user of the recipient, used for password or login authentication; its
  fields are a type byte, a 7-byte lookup hash, a derivation count and an
  additional byte (the "A"uthenticate and "S"ession-setup bits and the
  derivations/additional fields are deprecated and SHOULD be ignored).
* **handshakeNonce** (24 bytes): a per-handshake random nonce, encrypted with
  the recipient's permanent public key, so the recipient can recover it using
  its private key without needing a prior session.
* **Permanent Public Key** (32 bytes): the sender's identity key.
* **Authenticator** (16 bytes): a Poly1305 authenticator over the sender's
  temporary (ephemeral, per-session) public key and the rest of the header,
  generated using the shared secret computed from the sender's
  `handshakeNonce` and the recipient's permanent key, so that the public key
  is bound to the authenticated session.
* **EncryptedTempKey** (32 bytes): the sender's ephemeral public key,
  encrypted (with the per-packet crypto_box) so that it is protected from
  plaintext eavesdropping, but also bound by the authenticator.

On receipt of a hello or key packet:

1.  The recipient decrypts `handshakeNonce` using its permanent private key.
2.  From `handshakeNonce`, the sender's identity public key, the authenticator
    and (for a key packet) the encrypted temp key, the recipient derives the
    shared secret and completes the session.
3.  The session can then carry traffic encrypted with `crypto_box` using a
    24-byte nonce derived per packet (Section 5.3).

### 5.3.  Per-Packet Nonce and Encryption of Traffic

For traffic packets the 4-byte "Session State"-field is repurposed as a
*monotonic nonce counter*. The full nonce passed to the cipher is 24 bytes;
given a 32-bit counter `c` and a flag `isInitiator`:

* the counter is stored big-endian into the [initiator] half of the nonce
  for packets sent by the initiator, and into the [non-initiator] half for
  packets sent by the responder. The other half is zero. This direction-
  dependent placement prevents reflection attacks (a packet captured on one
  side and replayed at the other is not decryptable because the nonce shifts
  to the other half).

Concretely, the nonce bytes are built as:

~~~
nonce[4..8]  or  nonce[20..24]   <-- the 32-bit counter, big-endian
~~~

The remainder of the nonce is zero. In NaCl terms, the ciphertext is:

~~~
Crypto_box_afternm( plaintext = [content], sharedSecret, nonce24 )
~~~

which is XSalsa20 encryption with a Poly1305 MAC appended (16 bytes). Each
session derives the shared secret once after the handshake and then uses the
`*afternm` form of the box for speed.

For Handshake packets the same construction is used but with the per-packet
random 24-byte nonce; the "header" itself (the nonce, the handshake nonce,
the auth challenge and the key fields up to the authenticator) is
authenticated with a separate Poly1305 MAC as described above, so that no
part of the handshake is malleable.

### 5.4.  Replay Protection

ReplayProtector implements a sliding window over the 4-byte per-packet
nonce. Each traffic packet includes its nonce counter; the receiver stores
the initial counter (xor `/initState`) and subsequently:

* accepts packets whose counter is greater than the largest accepted counter
  minus the window width, and
* tracks which counters within the window have been seen to reject replays.

The window width is compile-time configurable (default maps to a few dozen
counters). A session that has been inactive for `InactivityTimeout` seconds
(60s by default; shorter for setup) is torn down, its symmetric state freed,
and the next packet begins a fresh handshake.

### 5.5.  Authentication

When a node requires that sessions be authenticated with a password or
login, it registers users (a `password:auth type` pair). A client that wishes
to use such a session includes an Auth Challenge. Two authentication types
are defined:

* type 1: the challenge contains a SHA-256 (applied twice) of the password
  as its lookup key;
* type 2: the challenge contains a hash derived from the login string.

The recipient recomputes the lookup key from its stored user set and, on
match, retrieves the associated user (returned by
`CryptoAuth_getUser`), which the routing layer may use for rate limiting and
identity.

### 5.6.  connectToMe and NAT Traversal

If a node `A` wishes to speak to node `B` but the initial "hello" would be
blocked by a NAT/firewall at `B`, it sends a "connectToMe" packet
(`Session State == UINT32_MAX`), which the receiver's routing layer can learn
about and instruct `B` to initiate an outbound hello to `A`. The connectToMe
packet carries no encrypted content; its only field that is read is the
Permanent Public Key of `A` (identity, Section 5.2), all other fields are
ignoredchers. This mechanism is the same one that allows rejoining one's own
address from behind a NAT after restart.

## 6.  The Switch and Path Labels

### 6.1.  The Switch Header

The switch header is 12 bytes and is the outermost header between two
switches:

~~~
 0                   1                   2                   3
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                       path label (64-bit, BE)                       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  congestion/  |  suppress  |  version    |  label  | traffic        |
|  (unused)     |  errors     |  + shift    |  shift  | class          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
~~~

In the code this is `struct SwitchHeader`:

| Field                 | Size      | Meaning                                           |
|-----------------------|-----------|---------------------------------------------------|
| `label_be`            | 8         | The 64-bit path label, big-endian                 |
| `congestAndSuppressErrors` | 1    | Congestion signal; suppresses error replies       |
| `versionAndLabelShift`| 1         | Upper bits: switch version; lower 6 bits: label shift |
| `trafficClass_be`     | 2         | Traffic class (QoS hint), big-endian              |

`SwitchHeader_CURRENT_VERSION` is 1.

### 6.2.  Labels

A label is a 64-bit value that names a path through the switch fabric. Labels
are read starting from the least significant bits. The low-order bits encode,
via the receiving switch's encoding scheme (Section 6.3), the interface
number of the next hop; the rest of the label describes how to get to the
current hop.

The forwarding procedure at a switch is:

1.  The switch receives a packet whose label is `L`.
2.  It consults its own encoding scheme (specifically the form whose bit
    count applies to the interface from which the label was received) to
    determine `b`, the number of bits used by the next segment of the label.
3.  The next-hop interface is determined from the low `b` bits of `L`.
4.  The packet label is right-shifted by `b` and the switch's own interface
    number is spliced back into the low `b` bits by XOR (`L' =
    (L >> b) XOR iface`). This reversal means the *response* packet can be
    routed back along the same fabric by simply using the negative label,
    without storing forwarding state at the switch.

A valid label has the property that after removing all the segments (each
possibly of differing bit counts according to the encoding scheme) the
remaining leading bits are a known sentinel (a run of `1`s of the length of
the final form), which is how termination is detected.

The "label shift" byte in the switch header records how many bits of the
label were consumed by the forwarding operation so far, allowing a receiving
endpoint to know how the label of the current packet relates to the path.

Labels may be *spliced*: given a label `A` that reaches some switch and a
label `B` from that switch on to another node, the two may be concatenated
(here "spliced") to form a single label `A|B`. Splicing is done by the
routing/DHT layer when a node reports a path that passes through the
reporting node (Section 8.4).

### 6.3.  Encoding Schemes (NumberCompress)

Because different parts of the network may have different numbers of
interfaces per switch, the label is not a fixed-width bitmap of interface
indices. Instead the label is divided into *forms*: each form describes a
segment of the label with:

| Field        | Meaning                                                        |
|--------------|----------------------------------------------------------------|
| `bitCount`   | Number of bits the label uses for this form's interface index   |
| `prefixLen`  | Number of bits in the (bit-)prefix that identifies this form    |
| `prefix`     | The identifying prefix value in those `prefixLen` bits          |

Two canonical schemes exist:

* **Fixed-width schemes** (e.g. a 4-bit `f4` or 8-bit `f8` form): every
  interface index in the label occupies the same, fixed number of bits, and
  no per-segment prefix is present (`prefixLen == 0`). A single scheme of 8
  bits gives labels that carry at most 8 interfaces south of the point at
  which they read the label.

* **Dynamic (variable-width) schemes**, of which the default is
  `NumberCompress_TYPE = v3x5x8`. v3x5x8 interleaves three forms:
  * a form with 4 bits for interfaces `0..15` (`0x4`),
  * a form with 6 bits for interfaces `0..255`, and
  * a form with 8 bits, giving a per-interface-label sentinel.

The reference selects the default scheme at build time via the
`NumberCompress_TYPE` macro (the default is `v3x5x8`).

When a node sends a reply that includes paths, it also includes the size of
the encoding (see Section 8, the `es`/`ei` fields) so the recipient can
decode the label segments correctly. The upshot is that a 64-bit label is
enough space to describe reasonably large paths, while small networks pay
only a few bits per hop.

## 7.  Encapsulation and Content Types

### 7.1.  The Data Header

After crypto, every packet carries a 4-byte data header:

~~~
 0                   1                   2                   3
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  ver  | unused |   unused     |    Content Type (16-bit, BE)  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
~~~

* The high nibble is a version (`DataHeader_CURRENT_VERSION == 1`).
* The low 16 bits are the Content Type.

### 7.2.  Content Types

The content type identifies the payload that follows the data header.
Types `0..255` are reserved for IPv6 protocol numbers (RFC 8200 next-header
values) and carry inner IPv6 packets; e.g. `ContentType_IP6_ICMP == 1`,
`ContentType_IP6_TCP == 6`, `ContentType_IP6_UDP == 17`,
`ContentType_IP6_IPV6 == 41`, `ContentType_IP6_ICMPV6 == 58`.

| Content type    | Value | Meaning                                        |
|-----------------|-------|------------------------------------------------|
| IP6 protocol    | 0-255 | Inner IPv6 packet (next-header value)          |
| HyperboriaDHT   | 256   | Hyperboria DHT inter-node message (Section 8)  |
| IpTunnel        | 257   | Tunnelling control (Section 9)                 |
| RESERVED        | 258   | Reserved                                      |
| RESERVED_MAX    | 0x7fff| Reserved range for future allocation           |
| AVAILABLE ...   | 0x8000+| Application-defined; usable after mutual agreement between endpoints |
| CTRL            | 0xffff + 1 | Unencrypted control frames (not seen in the wild) |

A packet whose content type is in the AVAILABLE range (>= 0x8000) is not
interpreted by the network; it is delivered to any peer that both ends have
agreed to use it forGit (like a port number for Hyperboria subsystems).

## 8.  HyperboriaDHT

The DHT uses end-to-end Bencoded dictionaries (RFC 7049-style maps). All DHT
queries and responses are standard exchange messages that can be parsed as
plain Bencode without length prefixes; the network can be diagnosed simply
by inspecting the raw messages.

### 8.1.  Message Format

A DHT message is a Bencoded dictionary with these top-level keys:

| Key | Name                | Meaning                                                       |
|-----|---------------------|---------------------------------------------------------------|
| `q` | Query type          | One of `fn`, `gp`, `pn`, `nh` (Section 8.2)                   |
| `p` | Protocol version    | The version of the Hyperboria protocol the sender runs (21)   |
| `a` | Arguments           | A nested dictionary of query arguments                        |
| `txid`| Transaction id     | Echoed by the responder; used to match replies                |
| `n` | Nodes               | A compact, concatenated node list (Section 8.4)               |
| `es`, `ei` | Encoding scheme + form index for the sender | Allow the receiver to interpret `n` labels |
| `nh`, `np` | Next-hop info      | Present in `nh` responses    |

Every query type also carries a `q` (query) and `txid` (transaction id); the
response echoes the `txid`. A ping response additionally carries the sender's
protocol version (`p`).

### 8.2.  Query Types

| Query    | Name       | Arguments            | Purpose                                   |
|----------|------------|----------------------|-------------------------------------------|
| `fn`     | find node  | `tar` (target addr), `n` (nodes carried) | Return the nodes known to be closest to `tar` |
| `gp`     | get peers  | `tar` (target), `n`  | Return the peers of the receiver closest to `tar` |
| `pn`     | ping       | (none)               | Reachability check; returns version        |
| `nh`     | next hop   | `tar` (target), `n`  | Forward the packet toward `tar`           |

The DHT key space is the set of 128-bit Hyperboria addresses; "closest" is
measured by the XOR distance between the 128-bit addresses, matching the
Kademlia-style approach. Nodes are stored in a routing table whose cores
index is the XOR-metric and which is maintained by the `RouterModule` in the
`dhtcore` engine.

### 8.3.  Node List Encoding

A "nodes" field (`n`) is a sequence of zero or more serialized node
addresses, each exactly 56 bytes:

~~~
[ ip6 (16 bytes) ][ publicKey (32 bytes) ][ pathLabel (8 bytes) ]
~~~

When the sender is listing nodes *through* which it has a path, the `path`
of each serialized node is the label of the path from the *querying node* to
that node (after splicing through the responder, Section 6.2), and the
per-node protocol version is carried in a parallel bencoded list (`np`), one
entry per node, so the recipient knows each node's protocol version without
parsing variable fields.

In responses to `findNode` (`fn`), the node list gives the closest nodes to
the search `tar`. In responses to `ping` (`pn`) the node list may be empty.

### 8.4.  Path Splicing

The "greedy" DHT operation that makes routing scale is `next hop` (`nh`): a
node may forward a query toward the target rather than simply answering, and
the response includes the label for the requester to use. When a node X has
a path `PX -> ...` and returns a node Y that is reachable via X, it reports
Y's label as the path from the *requesting* node through X to Y. On the wire
a response containing such spliced paths also carries:
* `es`: the encoding scheme (bit-format description) in use at `X`, and
* `ei`: the index of the smallest encoding-form that can represent the
  interface behind which the requesting node sits,
* `pes`/`pei`: the encoding scheme and index for the closest peer along the
  path.

These allow the requester to (a) decode the returned label, and (b) re-encode
its own continuation label when the reply passes back through a switch whose
interface uses a different form.

## 9.  Tunnelling of Legacy IP

Hyperboria native traffic is IPv6 in the `fc00::/8` space. For nodes that
also carry legacy IPv4/IPv6 traffic (e.g. a gateway to the regular Internet,
or a mobile node using tunnelling), the IpTunnel facility terminates
tunnels:

* An IPv6-in-IPv6 (or IPv4-in-IPv6) packet addressed to a node's Hyperboria
  address (or to a routed prefix) is intercepted by the node's own IpTunnel
  frame, which unpacks it and forwards the inner packet to the destination.
* `ContentType_IPTUN (257)` messages carry tunnel *control* (session setup,
  address allocation, MTU negotiation) between a tunnel client and server.

Internally, tunnel connections are described by `struct IpTunnel_Connection`
holding the IPv6 address of the far end, an optional IPv4 address, and prefix
lengths (`connectionIp6Prefix`, `connectionIp6Alloc`, etc.). These allow the
endpoint to assign the far end an IPv4/IPv6 address within its own routed
spaceley and to route replies back over the same cryptographic session.

## 10.  Protocol Versioning

### 10.1.  Version Numbers

Every node runs a *protocol version*. The current protocol version required
by HyperboriaNS is **21**, and the minimum protocol version it will talk to
is **20**; i.e. the current software requires peers to run protocol version
20 or 21, and it itself presents version 21 as its protocol version in every
DHT message (`p`), in the CryptoAuth session header, and in the
`ping`/`findNode` replies.

The version pair used by HyperboriaNS is:

| Constant                       | Value |
|--------------------------------|-------|
| `Version_CURRENT_PROTOCOL`     | 21    |
| `Version_MINIMUM_COMPATIBLE`   | 20    |

### 10.2.  Version Negotiation

Versions are learned from:

* the `p` key in DHT pings and queries,
* the `Version` field of the `CryptoHeader` handshake (via the session),
* the header of `findNode`/`getPeers` responses (the parallel node-version
  list `np`).

A node that receives a packet claiming a version older than its minimum
compatible SHOULD drop it and, if possible, indicate version
incompatibility to the sender. Because the protocol has evolved over time,
the minimum is separate from the working version: a node running version 21
may interoperate with nodes running version 20 (both directions), and the
protocol selection is the "safe" lowest common denominator.

### 10.3.  Switch Version

The switch header also carries a (smaller) version in its high bits
(`SwitchHeader_CURRENT_VERSION == 1`). This is the version of the *switch
layer*, independent of the DHT/application protocol version, and governs
how the label shift field and the label format are to be interpreted.

## 11.  Session Management

Sessions are established end to end when one node sends a hello to another
(CryptoAuth) over a path label. Because links are changed by the DHT at
routing time, and because a node may be reachable by more than one path, the
system tracks up to `SessionManager_PATH_COUNT` (3) candidate paths per peer,
each with:

* `label` — the path label,
* `timeLastValidated` — when the path was last confirmed,
* `metric` — the switch distance (a lower metric is better, used to pick
  the primary path).

These per-peer paths are kept so that if a primary path breaks, a secondary
path is already known and traffic can continue without a fresh DHT
lookupSync. Sessions time out after 120 seconds of inactivity and are
"reset" after 60 seconds (or 10 seconds during setup); the `timeOfLastUsage`
and `lastSearchTime` fields drive these timers.

## 12.  Security Considerations

* **Identity**: an address is unforgeable because it is a cryptographic hash
  of the permanent public key, and the handshake proves possession of the
  corresponding private key (authenticator over the encrypted temp key binds
  the packet to that key). Spoofing another node's address requires either
  the private key or breaking SHA-512/Curve25519/Poly1305.
* **Confidentiality**: after the handshake, content is encrypted with
  XSalsa20-Poly1305 per packet, with unique per-session nonces. This prevents
  observers, and because each hop is separately secured it provides
  end-to-end-*ish* protection free of a PKI: you only need to know the
  address/key of the far end.
* **Replay**: the windowed replay protector and monotonically increasing
  nonce counter prevent replay within a session.
* **Password authentication**: passwords are never sent in the clear; the
  auth challenge carries only a (twice-hashed) hash, and the shared secret
  for the session is derived from the password via the "passwordHash" branch
  (a 64-byte buffer of the hash of the password appended to the shared
  secret) so the password is the only input needed to complete the session.
* **Label manipulation**: the switch never trusts the label as data for
  routing decisions except via the encoding scheme; the splice/XOR
  construction and the label-shift sentinel are designed so a malicious node
  cannot forge a path to a node whose label it cannot compute. Labels are
  not secrets and can be observed; they are used only to reach the DHT's
  advertised node.
* **DoS**: CryptoAuth session setup is expensive by design (a public-key
  operation per handshake); the timeout of sessions (Section 11) bounds
  resource use from half-open sessions. Because routing is cryptographically
  addressed and DHT queries are version- and key-bound, a node can only
  "own" addresses for which it holds private keys.

## 13.  IANA Considerations

This document defines no IANA registries. The `fc00::/8` prefix allocation
is shared with the documentation/ULA space; nodes SHOULD NOT be allocated
addresses outside legitimate use of the protocol. Content types in the
`0x8000..0xffff` ("AVAILABLE") range are intrinsically non-global and MUST be
negotiated per-session between the endpoints using them.

## 14.  References

* RFC 2119 — Key words for use in RFCs to Indicate Requirement Levels
* RFC 4291 — IP Version 6 Addressing Architecture
* RFC 8200 — Internet Protocol, Version 6 (IPv6) Specification
* The HyperboriaNS source tree, in particular:
  * `crypto/AddressCalc.c` / `crypto/AddressCalc.h` — address derivation
  * `crypto/CryptoAuth.c` / `crypto/CryptoAuth.h` — CryptoAuth
  * `wire/CryptoHeader.h` — CryptoHeader wire format
  * `wire/SwitchHeader.h` — switch header
  * `switch/EncodingScheme.h`, `switch/NumberCompress.h` — label encoding
  * `wire/DataHeader.h`, `wire/ContentType.h` — content carrying
  * `dht/dhtcore/RouterModule.c`, `dht/dhtcore/ReplySerializer.c`,
    `dht/HyperboriaDHTConstants.h` — DHT protocol
  * `net/SessionManager.h` — session and path management
  * `tunnel/IpTunnel.h` — tunnelling
  * `util/version/Version.h` — version constants

## Author's Address

The HyperboriaNS development community.

Comments to this document should be directed to the HyperboriaNS project.
