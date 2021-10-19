#include <stdio.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <string.h>

#define MAXN 100

char *getIp(void) {
	char ipAddr[MAXN] = {};
	struct ifaddrs *ifAddrStruct = NULL;
	void * tmpAddrPtr = NULL;
	struct ifaddrs *iter = ifAddrStruct;
	while (iter != NULL) {
		if (iter->ifa_addr->sa_family == AF_INET) {
			tmpAddrPtr = &((struct sockaddr_in *)iter->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			if (strlen(ipAddr) + strlen(addressBuffer) < MAXN - 1) {
				strcpy(ipAddr, addressBuffer);
			} else {
				strcpy(ipAddr, "127.0.0.1");
				break;
			}
		}
		iter = iter->ifa_next;
	}
	freeifaddrs(ifAddrStruct);
	char *tmp = ipAddr;
	return tmp;
}

int main() {
	char *x = getIp();
	printf("%s", x);
	return 0;
}