#include <stdio.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <string.h>

#define MAXN 100

char *getIp(void) {
	char *ipAddr = NULL;
	struct ifaddrs *ifAddrStruct = NULL;
	void * tmpAddrPtr = NULL;
	if (getifaddrs(&ifAddrStruct) != 0) {
		strcpy(ipAddr, "127.0.0.1");
	}
	else {
		struct ifaddrs *iter = ifAddrStruct;
		while (iter != NULL) {
			if (iter->ifa_addr->sa_family == AF_INET) {
				tmpAddrPtr = &((struct sockaddr_in *)iter->ifa_addr)->sin_addr;
				char addressBuffer[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
				ipAddr = addressBuffer;
			}
			iter = iter->ifa_next;
		}
		freeifaddrs(ifAddrStruct);
	}
	return ipAddr;
}

int main() {
	char *x = getIp();
	printf("%s", x);
	return 0;
}