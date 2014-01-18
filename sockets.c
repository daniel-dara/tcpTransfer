/*
 * The following functions were adapted from:
 * "TCP/IP Sockets in C" 2nd Ed. by Michael Donahoo and Kenneth Calvert
 * pgs. 42-48 
*/

#include "globals.h"

// Finds and successfully connects to a socket on the server (specified by
// arguments) and returns the socket descriptor.
int setupTCPClientSocket(const char* host, const char* service)
{
	const struct addrinfo search_criteria = 
	{
		// Search for...
		.ai_family = AF_UNSPEC,     // IPv4 or IPv6
		.ai_socktype = SOCK_STREAM, // stream sockets 
		.ai_protocol = IPPROTO_TCP  // TCP protocol
	};

	struct addrinfo* addr_list; // pointer to hold search results

	// Search for a socket.
	int ret_val = getaddrinfo(host, service, &search_criteria, &addr_list);

	// error handling
	if (ret_val)
	{
		dieWithUserMessage("getaddrinfo() failed", gai_strerror(ret_val));
	}

	struct addrinfo* addr; // address list iterator
	int sock = -1; // socket descriptor

	// Loop over resulting addresses until a connection is established.
	for (addr = addr_list; addr != NULL; addr = addr->ai_next)
	{
		// Create a socket from the current address.
		sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

		if (sock < 0)
		{
			// Socket creation failed so try next address.
			continue;
		}

		// Connect to the socket (on the server).
		if (connect(sock, addr->ai_addr, addr->ai_addrlen) == 0)
		{
			// Socket connection was successful; end loop.
			break;
		}

		// Socket connection failed; try next address.

		close(sock);

		// Reset the socket descriptor.
		sock = -1;
	}

	// Free the memory allocated for the address list results.
	freeaddrinfo(addr_list);

	return sock;
}

// Finds and successfully creates a socket on the server at the given port (or
// service) and returns the socket descriptor.
int setupTCPServerSocket(const char *service)
{
	// Search for...
	const struct addrinfo search_criteria = {
		.ai_family = AF_UNSPEC,     // IPv4 or IPv6
		.ai_flags = AI_PASSIVE,     // Any address/port
		.ai_socktype = SOCK_STREAM, // stream sockets
		.ai_protocol = IPPROTO_TCP  // TCP protocol
	};

	struct addrinfo* addr_list; // pointer to hold search results

	// Search for a socket.
	int ret_val = getaddrinfo(NULL, service, &search_criteria, &addr_list);

	// error handling
	if (ret_val)
	{
		dieWithUserMessage("getaddrinfo() failed", gai_strerror(ret_val));
	}

	struct addrinfo* addr; // address list iterator
	int server_socket = -1; // socket descriptor

	// Loop over resulting addresses until a listening socket is
	// successfully created.
	for (addr = addr_list; addr != NULL; addr = addr->ai_next) {
		// Create a socket from the current address.
		server_socket = socket(addr->ai_family, addr->ai_socktype,addr->ai_protocol);

		if (server_socket < 0)
		{
			// Socket creation failed so try next address.
			continue;
		}

		// Bind to the local address and set the socket to listen.
		if ((bind(server_socket, addr->ai_addr, addr->ai_addrlen) == 0) &&
			(listen(server_socket, MAXPENDING) == 0))
		{
			// Print the socket addresss
			struct sockaddr_storage local_addr;
			socklen_t addrSize = sizeof(local_addr);

			if (getsockname(server_socket, (struct sockaddr *) &local_addr, &addrSize) < 0)
			{
				dieWithSystemMessage("getsockname() failed");
			}

			printf("Binding to ");
			printSocketAddress((struct sockaddr *) &local_addr, stdout);
			printf("\n");

			break; // Bind and listen were successful
		}

 		// Socket connection failed; try next address.

		close(server_socket);

		// Reset the socket descriptor.
		server_socket = -1;
	}

	// Free the memory allocated for the address list results.
	freeaddrinfo(addr_list);

	return server_socket;
}


// Accepts connections on the given server_socket and returns the client_socket
// that connected to it.
int acceptTCPConnection(const int server_socket)
{
	struct sockaddr_storage client_addr;

	// in-out parameter
	// IN: size of given address structure
	// OUT: size of returned address structure (not used here)
	socklen_t client_addr_len = sizeof(client_addr);

	// Wait for a client to connect.
	int client_socket = accept(server_socket, (struct sockaddr *) &client_addr, \
	                      &client_addr_len);

	// error handling
	if (client_socket < 0)
	{
		dieWithSystemMessage("accept() failed");
	}

	// connection successful
	printf("Handling client ");
	printSocketAddress((struct sockaddr *) &client_addr, stdout);
	printf("\n");

	return client_socket;
}

// Prints the given address to the given output stream in human-readable format
void printSocketAddress(const struct sockaddr* address, FILE* stream)
{
	if (address == NULL || stream == NULL)
		return;

	void* binary_address;

	// buffer to contain result (IPv6 sufficient to hold IPv4)
	char addr_buf[INET6_ADDRSTRLEN];

	in_port_t port; // port to print

	switch (address->sa_family)
	{
		case AF_INET:
			binary_address = &((struct sockaddr_in *) address)->sin_addr;
			port = ntohs(((struct sockaddr_in *) address)->sin_port);
			break;
		case AF_INET6:
			binary_address = &((struct sockaddr_in6 *) address)->sin6_addr;
			port = ntohs(((struct sockaddr_in6 *) address)->sin6_port);
			break;
		default:
			fprintf(stream, "[unknown type]");
			return;
	}

	// Convert binary representation to human-readable address.
	if (inet_ntop(address->sa_family, binary_address, addr_buf, \
	              sizeof(addr_buf)) == NULL)
	{
		fprintf(stream, "[invalid address]");
	}
	else
	{
		fprintf(stream, "%s", addr_buf);

		// Zero is not valid in any socket address.
		if (port != 0)
		{
			fprintf(stream, "-%u", port);
		}
	}
}

