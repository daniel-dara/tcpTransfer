all:
	gcc -g -Wall main.c file_io.c sockets.c utility.c -o tcpTransfer

clean:
	rm -f tcpTransfer
