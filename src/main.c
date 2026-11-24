#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t addr_len;
	int new_sockfd;
	char buffer[1024];
	int n;
	
	if (argc < 2) {
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr = INADDR_ANY;
	memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));


	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(1);
	}

	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("bind");
		exit(1);
	}

	if (listen(sockfd, 5) < 0) {
		perror("listen");
		exit(1);
	}

	printf("Server listening on port %s\n", argv[1]);

	while (1) {
		addr_len = sizeof(client_addr);
		new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);
		if (new_sockfd < 0) {
			perror("accept");
			continue;
		}

		printf("New client connected from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		while (1) {
			memset(buffer, 0, sizeof(buffer));
			n = read(new_sockfd, buffer, sizeof(buffer));
			if (n < 0) {
				perror("read");
				break;
			}
			if (n == 0) {
				printf("Client disconnected\n");
				break;
			}
			printf("Received: %s\n", buffer);
			write(new_sockfd, buffer, n);
		}
		close(new_sockfd);
	}

	close(sockfd);
	printf("Server stopped\n");
	return 0;
}