#include "globals.h"

int main(int argc, char* argv[])
{

	// Validate argument count.
	if (argc < 3 || argc > 4)
	{
		dieWithUserMessage("Usage", "tcpTransfer <file> <port> [server_address]");
	}

	// Transfer arguments to local variables
	const char* file_name = argv[1]; // Path of file to transfer
	const char* port  = argv[2]; // Port (or service) to communicate on
	const char* server_address = (argc == 4 ? argv[3] : 0); // ip or hostname

	// check if the server_address argument was populated
	if (server_address)
	{
		/* Client branch:
			Opens the file, reads it, and sends it to the server */

		printf("Connecting to server...\n");

		// Create a connected TCP socket
		int serv_sock = setupTCPClientSocket(server_address, port);

		// Check that the socket was succesfully created
		if (serv_sock < 0)
		{
			dieWithUserMessage("SetupTCPClientSocket() failed", "unable to connect");
		}

		int bytesSent = uploadFile(serv_sock, file_name);

		printf("Uploaded %d bytes of file \"%s\".\n", bytesSent, file_name);

		close(serv_sock);
	}
	else
	{
		/* Server branch:
			Opens a new file, receives data, and writes it to the file */

		printf("Initializing server socket.\n");

		// Create a socket for incoming connections.
		int serv_sock = setupTCPServerSocket(port);

		// Check the socket
		if (serv_sock < 0)
		{
			dieWithUserMessage("SetupTCPServerSocket() failed", port);
		}

		printf("Waiting for connection...\n");

		// Accept an incoming connection.
		int clnt_sock = acceptTCPConnection(serv_sock);

		// Recieve the file.
		int bytes_recvd = downloadFile(clnt_sock, file_name);

		printf("Downloaded %d bytes to file %s\n", bytes_recvd, file_name);

		// Close sockets
		close(clnt_sock);
		close(serv_sock);
	}

	return 0;
}
