# Remarks
First of all, this is INDIVIDUAL work, no groups. Any code sharing will be considered a
violation of ethical conduct.

Having said that, I made the whole project from scratch, so there will be mlutiple
typos/inconsistencies. The sooner you read it and study it, the sooner I will be notified of these
typos/inconsistencies and be able to resolve them.

*NOTE: your project MUST run on one of the Unix machines on campus, NOT ON
YOUR PC. I recommend {cs1,cs2}.utdallas.edu.*

You can use any language that you want, as long as it runs on the unix machines on campus.
I recomend that you DO NOT use threads! In the past, students unknowingly have created
around 200 threads which of course causes the system to kill your programs.
You will submit your source code and any instructions on how to compile it (i.e. which machine
you compiled it on and using what command) You have to submite a README file, along with
your source code, indicating: on which machine you run it, *exactly* what command you used
to compile it.

Please make sure your program uses the arguments as described below and in the order
described. Otherwise, the TA may have to modify the shell scripts to run our tests, which we
will not be happy about (hence, possible point deduction).
We will run your code in the UTD unix machine that you mention in the README file, and see
if it works.

## Overview
We will simulate an Internetwork by having a group of Unix processes running in the
background (batch mode basically). Each process will represent a node in the network: a
bridge, a router, or a host PC. Since these are processes and not actual computer equipment,
we will use text files (good 'ol Unix .txt files) to represent physical links. I.e., when a node
transmits a message, what it really is doing is appending the message to a text file that
represents the physical link (more on this later). The reason I use text files and not sockets is
twofold: a) I don't require you to know sockets in the class, and b) at the end of the execution,
each link (i.e. text file) will have stored in it all the messages that were sent between the two
nodes at its endpoints. This actually makes our grading easier.

The Internetwork consistes of three networks (networks 1, 2, and 3 :=) ), it has one host in
network 1, and one host in network 3. It also has three routers which join the networks
together. Think of each network as a bridged Ethernet, the first network has two bridges, and
networks 2 and 3 consist only of a single bridge. Think of the links as UTP (unshielded twisted
pairs) cable that connects the bridges together and connects the host and routers to the
bridges. Routers have multiple IP addresses, as usual, and the hosts only one. Bridges have
multiple ports, e.g., bridge B1 has six ports while bridge B3 has eight ports.
You will write three programs, one program for hosts, one program for routers, and one
program for bridges. The above Internetwork would be "created" by typing the following at the
Unix command line prompt (which I simply assume is >)

```bash
> host .... &
> host .... &
> bridge .... &
> bridge .... &
> bridge .... &
> bridge .... &
> bridge .... &
> router ... &
> router ... &
> router ... &
```

where "host" is the code you wrote for a host process, "bridge" is the code for a bridge process,
and "router" is the code for a router process, "..." are the arguments of each process (which I
describe further below) and "&" puts the process in the "background" (batch mode) which
frees the terminal, and you will get the command prompt again.
Each router and host will have Ethernet and IP addresses, as usual. For simplicity, Ethernet
addresses are in the range 1 .. 99, with 99 being the broadcast address. For IP addresses, both
the network number and the host number are in the range 1 .. 9 (9 is not broadcast, it's also a
host, I will not use an IP broadcast). Bridges also have id's; I assume each bridge has an ID in
the range 1 .. 9.

## Files Instead of Links
As mentioned above, the physical links will be represented by text files. In general, assume you
have two nodes, A and B, joined by a physical link. We will have two text files: one for the
messages from A to B, and another for the messages from B to A. These text files are NEVER
OVERWRITTEN. I.e., for the file from A to B, A "appends" (without overwritting) its messages
to the file, and B reads the messages from this file.

Files will be named as follows. Consider bridge B3. It has eight ports. For each port X, there
will be two files, fromB3PX.txt and toB3PX.txt. When B3 sends an "Ethernet" message over
port X, it appends it to the file toB3PX.txt. When B3 reads an Ethernet message from port X, it
reads it from fromB3PX.txt

Consider now the host in network 1. When it sends a message over the Ethernet, it appends it
to toB1P1.txt, and when it reads Ethernet messages, it reads them from fromB1P1.txt. The
behavior of the router is similar, except that it has multiple interfaces.
The special case, unfortunately, is links between bridges. Bridges will be aware of who their
neighboring bridges are (see arguments below). This doesn't happen in practice but I can't get
around it, so it will be a special case. So when B1 sends a message over to B2, it appends it to
the file B1B2.txt (and vicecersa).

Note that each text file has a single process that reads from it and a single process that
appends to it. That should work fine (no need for locking the file). In C it works great, read
from an input file until you get and EOF, then you wait a while (a second) and when you read
again either you get another EOF (the other node did not send anything) or you get more data
(the other node appended a message). Java is trickier (I am not familiar with Java, good luck :))
one way to do this is to read until an EOF, then close the file, reopen it a second later, and read
again, but skip over the bytes you read before (yes, you have to remember how many bytes you
read before)

## Arguments to each Process
So how do nodes know which files to open? The arguments of each process will give to the
process all the configuration information that it needs. I.e., you "design" a network (draw a
graph like the above), determine by hand port #'s etc., then you give these numbers to the
processes as arguments (no DHCP :)).  We next overview the arguments of each type of
process.

### Arguments to Hosts
The arguments to a host are:
Its IP address
Its Ethernet address
The IP address of its "default router"
The ID of the bridge it is attached to
the port number of the bridge to which it is attached to 
The IP address of a "destination node" to whom it wants to send a data string
The data string itself (which is just an arbitrary string of text)

For example, the host in network 1 could have the following arguments

```bash
> host 1 1 29 1 2 1 1 3 3  "This is a string I want to send to node (3, 3)"
```

Its IP address is (1,1), its Ethernet address is 29, its default router is (1, 2),  the ID of the
bridge it is attached to is 1, the port number of the bridge to which it is attached to is 1, the IP
address of the destination node to which it wants to send a data string is (3,3), and the string it
wants to send is "This is a string I want to send to node (3, 3)".

If a host is not sending a string, then its last argument is the port number of the bridge it is
attached to.

Note that a host should be ready and willing to receive a string from ANY other host. E.g., (3,
3) does not know that (1, 1) is going to send it a string; the packets simply arrive at (3, 3) and
(3, 3) should be able to deal with them.

A host will only send (at most) one string, but it could receive strings from multiple other
hosts.

### Arguments to Routers
The arguments to a router are three values for each network to which it is attached to:
1. Its IP address in that network
2. The bridge ID and port number of the bridge to which it is attached to
3. the Ethernet address of the router (in that particular network)
E.g.,  the router than joins network 4 and network 3, would have the following argumen

```bash
> router 4 2  4 5 18  3 1 5 5 75
```

The router is connected to two networks. In the first one, its IP address is (4, 2), the bridge ID
is 4 and the port of the bridge is 5, and the Ethernet address of the router is 18. In the second
network, the IP address of the router is (3, 1), the bridge id is 5, the port # is 5, and the
Ethernet address of the router is 75

### Arguments to Bridges
The arguemnts to a bridge are as follows
The ID of the bridge
The number of ports in the bridge
A list of neighboring bridges (if any)
For example, Bridge 1 will have the following arguments

```bash
> bridge 1 6 2
```

The bridge ID is 1, it has six ports (1 .. 6), and it has only one neighbor (it could have more, but
I only have one in the picture), which is B2.

## Functionality of Each node
We next overview the functionality of all the nodes. Basically, it is a simple Internetwork.

### Functionality Bridges
The job of the bridge is to forward "Ethernet" messages to their appropriate location. Ethernet
messages whose destination is the broadcast address will be sent out over all ports (except
where it came in, of course), and messages addressed to a specific Ethernet address (i.e.
unicast) will be forwarded along the appropriate port.

Bridges will learn about the location of specific Ethernet addresses in the same way as
discussed in class, i.e., by observing the source address of each message. They will keep a
cache of addresses they have learned. For simplicity, cache entries will not expire. If the
destination address is not in the cache, the message is replicated along all ports.
An Ethernet message (in the text files described above) has the following format
Ethernet-Destination Ethernet-Source Packet-Type <packet from a higher layer>

E.g.:
28 19 IP etc.

is an Ethernet message with Ethernet destination 28, Ethernet source 19, and it is an IP packet
(literally the letters "IP"), the rest of they bytes correspond to the rest of the IP packet. The
bridges don't care about the specific contents of the higher layer packet. There are several
higher layers, as you will see next; IP is just one of them.

For simplicity, packets will end with a new-line (end-of-line, or whatever you want to call it).
That will allow us to view the messages in a simple way with a regular text editor, and will also
simplify things for you when you parse the input files of a node.

### Functionality of Routers
#### Basic IP forwarding
The main function of routers is to forward "IP" messages until the message reaches its ultimate
destination (the destination host). So, routers need to find a path to reach any network. To do
so, we will use a very simple routing protocol: broadcast :)

IP messages have the following format
IP destination-IP source-IP <contents of a transport layer message>
E.g.
IP 1 1 3 3 etc.

where IP is just the string "IP", (1, 1) is the destination IP address, and (3, 3) is the source IP
address.

When the router receives an IP message whose destination is in one of its attached networks, it
most find out the Ethernet address of the destination via ARP (which is described below). Then,
the router forwards the message to the bridge by encapsulating the message into an Ethernet
packet, and appending the Ethernet packet to the input file of the bridge attached to the
router. Of course, the bridge then will forward this packet to the host.

#### ARP
For ARP, if a router does not know the Ethernet address of a node (host or router), it stores in a
buffer the message (e.g IP message) that it wanted to send, and it broadcasts an ARP request
over the Ethernet.

When the ARP reply returns, it then forwards the stored message (e.g. the IP message) to the
bridge using the appropriate Ethernet destination address.

ARP-request messages are of the following form

ARP REQ target-IP-address source-IP-address source-Ethernet-address

where target-IP-address is the IP address of the node you are looking for

An ARP reply is of the following form

ARP REP target-IP-address target-Ethernet-address source-IP-address source-Ethernet-address

where the target is the node sending the reply, and the source is the node who sent the
request.

#### Forwarding to Other Networks
When a router receives an IP message whose destination is not in any of its attached
networks,  the router will perform a broadcast along all the networks (NOT an Ethernet
broadcast, but a broadcast that will reach all routers). The router encapsulates the contents of
the IP packet into another type of packet, a broadcast (BC) packet, whose format is

BC src-router-IP-address sequence-number destination-network-number <IP packet being
encapsulated>

where src-router-IP-address is the router who performs the encapsulation, sequence number is
a number that the source router increases by one every time it encapsulates an IP packet, and
destination-network-number is the network number to which the IP packet has to be delivered.

The router then sends this BC packet to all its neighboring routers (you may have to call ARP
for them) via the Bridge.

When a router receives a BC message, and the destination-network-number is attached to this
router, the router recovers the IP packet, and delivers it to its destination host (via the bridge
and ARP of course)

If the destination-network-number is not attached to this router, and the router has not seen
this packet before (it has not seen this sequence number from the source router), then it
forwards a copy of the message (without altering it in any way) to all its neighboring routers on
networks other than the network from which this message this received (obviously).

In this way, the message will propagate to all networks until it reaches the destination, and the
sequence number prevents the packet from looping.  

#### Hello Protocol.
In order for the above to work, each router needs to be aware of all its neighbors in the same
network. Thus, in each of its networks, the router will broadcast (using Ethernet broadcast) a
"hello" message of the following form

HL router-ip-address router-Ethernet-address

This will inform all other nodes (routers, because hosts will ignore this message) that this
router exists on this network

When a router receives a HL message from another router, it adds the router to its list of
neighbors.

Hello messages will be sent by the router once every five seconds (the first hello message is
sent at time zero).

### Functionality of Hosts
The host's purpose is to transfer the string given in the argument to the destination host. To do
so, the "transport layer" (above IP) will grab the string, cut it into 5-bye pieces, and use the
conurrent logical channels protocol to transfer it to the destination. The transport layer will
thus have two message types, data (DA) and acknowledgement (AK) of the following format

DA sn cn <5 bytes of the string>

where sn is the sequence number (in our case, either 0 or 1), and cn is the channel number (in
our case, only two channels, 0 or 1)

Acknowledgments are simply of the form

AK sn cn

If an ack is not received within 30 seconds of sending the data message, then the data message
will be retransmitted.

Both the data and acknowledgments have to be given to the IP layer to be encapsulated and
turned into IP messages which will travel the network and reach the destination.
Note that we also need to do ARP, this is similar to the description given for the router.

## Protocol layers and Program Skeleton
### Layers of Hosts
Each host will have a transport layer, the IP layer, and the Ethernet layer. I overview next their
interaction. The host also has additional layers (ARP) , but you can figure that out on your own
:)

Adjacent layers talk to each other. The way I recommend, is to have subroutines for the
interfaces between layers. You can define your own if you like, you don't have to follow mine
exactly, they are just a guideline, and I leave it up to you to define the arguments, parameters,
etc between them.

- **Transport-periodic-tasks**
This procedure is called once a second by the main routine of the program. The transport layer
checks if there is anything new to do. E.g., if there is data ready to send, send it (give it to IP),
if a timer expired (if enough seconds have ellapsed from sending the message) then retransmit
the data message (by giving it again to IP), etc.
- **Transport-receive-from-IP**
The IP layer calls this routine when it has a transport-layer message that it received. The
transport layer will then examine the message (it could be a data message or an ack) and
processes the message accordingly.
- **IP-receive-from-transport**
This is a procedure called by the transport layer. In it, the IP layer receives a transport
message that is to be forwarded across the Internet until it reaches the destination. The
destination IP address is a parameter of the procedure. IP will then forward the message to the
Ethernet layer, and may have to call ARP before doing so.
- **IP-receive-from-Ethernet**
This procedure is called by the Ethernet layer after the Ethernet receives an IP message. The
IP layer  retrieves the transport layer message from inside the IP packet, and gives this
message to the transport layer, by calling the procedure Transport-receive-from-IP.
- **Ethernet-receive-from-IP**
This is a procedure called by the IP layer when it wants to give an IP message to the Ethernet
layer to be sent over the network. IP must include as a parameter the Ethernet address of the
node it wants to send the message to.
- **Ethernet-receive-from-Bridge**
This routine is called one-per-second by the main routine of the host program. It will read all
the new messages from the text file of the bridge, decapsulate the contents of the message,
and give the message to the appropriate layer. E.g., if it is an IP message,  it calls IP-receivefrom-Ethernet,
if it is an ARP message, it gives the message to ARP.

Thus, the program "skeleton" of a host could look something like this

```c++
main()
   //open the input and output text file of the host.
   do forever {
        call transport-periodic-tasks
        call Ethernet-receive-from-Bridge
        sleep 1 second   // the sleep is necessary to prevent you from using lots of CPU time
}
```

### Layers of Routers and Bridges
If you understand how hosts behave, you should be able to do routers and bridges. Bridges are
simpler, routers a little more complicated. Both of them operate once a second, just like a host
does. I.e., every second, read all input files to check for new messages (and process them), and
check if anything new needs to be done (e.g., hello messages)

## Remarks
Your program must run in the Unix machines on campus (not your laptop!!!!). I recommend
cs1, cs2, cs3, and babbage. You can use any language you want. When you turn it in, you send
us the code in elearning, along with detailed instructions on how to compile it (which command
you used, which flags, or include a makefile if you know what that is), and we will compile and
test your code. I DON'T CARE HOW NICE YOUR CODE IS (I should but you don't have the
time to make it nice) If it runs correctly on several scenarios that I post later, you get a 100.
There might be some details I did not mention, but your code must follow the typical and
obvious behavior. E.g., if a host is sending to a destination host on the same network, it sends
the messages directly to the host rather than via the default router, etc.

 Happy programming !!!
