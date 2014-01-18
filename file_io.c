#include "globals.h"

// Sends the given file (file name/path) to the server at the given socket and
// returns number of bytes sent.
int uploadFile(const int socket, const char* fileName)
{
	FILE* input = fopen(fileName, "r");
	int num_bytes_read, num_bytes_sent, total_bytes_sent = 0;

	if (input == NULL)
	{
		dieWithUserMessage("File IO Error", "Unable to open file or file does not exist.");
	}

	char* buffer[BUFSIZE];

	// Loop through file and read bytes using a buffer.
	while (!feof(input))
	{
		num_bytes_read = fread(buffer, sizeof(char), BUFSIZE, input);
		num_bytes_sent = send(socket, buffer, num_bytes_read, 0);

		// Check that send was successful.
		if (num_bytes_sent < 0)
		{
			dieWithSystemMessage("send() failed");
		}

		// check that all bytes sent successfully
		if (num_bytes_sent != num_bytes_read)
		{
			dieWithUserMessage("send()", "sent unexpected number of bytes");
		}

		total_bytes_sent += num_bytes_sent;
	}

	fclose(input);

	return total_bytes_sent;
}

// Recieves bytes from given socket and writes them to given file returning
// the total number of received bytes.
int downloadFile(const int socket, const char* fileName)
{
	FILE* output = fopen(fileName, "w");
	int num_bytes_rcvd, num_bytes_written, total_bytes_rcvd = 0;

	if (output == NULL)
	{
		dieWithUserMessage("File IO Error", "Unable to open/create file.");
	}

	char* buffer[BUFSIZE];

	// Accept data from socket and write to file.
	do
	{
		// Recieve data.
		num_bytes_rcvd = recv(socket, buffer, BUFSIZE, 0);

		// Write the data to the output file.
		num_bytes_written = fwrite(buffer, sizeof(char), num_bytes_rcvd, output);

		// Check that recv was successful.
		if (num_bytes_rcvd < 0)
		{
			dieWithSystemMessage("recv() failed");
		}

		// Check that all bytes were written successfully.
		if (num_bytes_written != num_bytes_rcvd)
		{
			dieWithUserMessage("fwrite()", "wrote unexpected number of bytes");
		}

		total_bytes_rcvd += num_bytes_rcvd;
	}
	while (num_bytes_rcvd != 0); // recv returns 0 when the client closes the
	                             // socket

	fclose(output);

	return total_bytes_rcvd;
}

