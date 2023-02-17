# UDP 1-N Multicast

Relative to src directory:

gcc -o multicast multicast.c
gcc -o receiver receiver.c
gcc -o binaryPub binaryFilePub.c

./receiver 224.1.1.1 5007 > receive.txt
./multicast 224.1.1.1 5007 127.0.0.1 6000
./binaryPub 127.0.0.1 6000 ../data/sample.txt

(Ctrl-C out of receiver)

diff receive.txt ../data/sample.txt

As sample increases, drop packets become more likely at full speed; if publisher throttles packet submittal their should be an equilibrium point as sample data size increases.
