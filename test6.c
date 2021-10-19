#include <stdio.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <cstring>

#define MAX_LENGTH 254

int main(void)
{
    char ipAddr[MAX_LENGTH];

    ipAddr[0] = '\0';

    struct ifaddrs * ifAddrStruct = NULL;
    void * tmpAddrPtr = NULL;

    if (getifaddrs(&ifAddrStruct) != 0)
    {
        //if wrong, go out!
        printf("Somting is Wrong!\n");
        return -1;
    }

    struct ifaddrs * iter = ifAddrStruct;

    while (iter != NULL) {
        if (iter->ifa_addr->sa_family == AF_INET) { //if ip4
            // is a valid IP4 Address
            tmpAddrPtr = &((struct sockaddr_in *)iter->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if (strlen(ipAddr) + strlen(addressBuffer) < MAX_LENGTH - 1)
            {
                if (strlen(ipAddr) > 0)
                {
                     strcat(ipAddr, ";");
                }
                strcat(ipAddr, addressBuffer);
            }
            else
            {
                printf("Too many ips!\n");
                break;
            }
        }
        //else if (ifaddrstruct->ifa_addr->sa_family == af_inet6) { // check it is ip6

        /* deal ip6 addr */
        //    tmpaddrptr = &((struct sockaddr_in *)ifaddrstruct->ifa_addr)->sin_addr;
        //    char addressbuffer[inet6_addrstrlen];
        //    inet_ntop(af_inet6, tmpaddrptr, addressbuffer, inet6_addrstrlen);

        //}
        iter = iter->ifa_next;
    }
    //releas the struct
    freeifaddrs(ifAddrStruct);

    printf("The ips: %s\n", ipAddr);

    return 0;
}