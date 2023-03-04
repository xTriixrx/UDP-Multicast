# UDP 1-N Multicast

<p>
receiver [multicast_group_ip] [multicast_port] [output_path] <br />
multicast [sourceHost] [sourcePort] [multicastGroup] [multicastPort] <br />
binaryPub [destinationHost] [destinationPort] [inputPath] <br />
</p>

Relative to src directory:

gcc -o receiver receiver.c
gcc -o multicast multicast.c
gcc -o binaryPub binaryFilePub.c

./receiver 224.1.1.1 5007 receive.txt
./multicast 127.0.0.1 6001 224.1.1.1 5007
./binaryPub 127.0.0.1 6001 ../data/binary_sample

(Ctrl-C out of receiver)

diff ../data/binary_sample receive.txt
