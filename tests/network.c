/**
@authors Copyright (c) 2017 Jaakko Keränen <jaakko.keranen@iki.fi>

@par License

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

<small>THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.</small>
*/

#include <c_plus/address.h>
#include <c_plus/audience.h>
#include <c_plus/commandline.h>
#include <c_plus/string.h>
#include <c_plus/service.h>
#include <c_plus/socket.h>

//#include <stdio.h>
//#include <string.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netdb.h>
//#include <arpa/inet.h>
//#include <netinet/in.h>

static void hostLookedUp(iAny *d, const iAddress *address) {
    iUnused(d);
    if (isValid_Address(address)) {
        iString *ip = toString_Address(address);
        printf("%i IP addresses for %s; chosen: %s \n",
               count_Address(address),
               cstr_String(hostName_Address(address)),
               cstr_String(ip));
        delete_String(ip);
    }
}

static void incomingConnection(iAny *d, iService *sv, iSocket *sock) {
    iString *addr = toString_Address(address_Socket(sock));
    printf("incoming connecting from %s\n", cstr_String(addr));
    delete_String(addr);
}

int main(int argc, char *argv[]) {
    init_CPlus();
    //struct addrinfo hints, *res, *p;
    //int status;
    //char ipstr[INET6_ADDRSTRLEN];

//    if (argc != 2) {
//        fprintf(stderr,"usage: showip hostname\n");
//        return 1;
//    }

//    memset(&hints, 0, sizeof hints);
//    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
//    hints.ai_socktype = SOCK_STREAM;

    iCommandLine *cmdline = iClob(new_CommandLine(argc, argv));

    if (contains_CommandLine(cmdline, "s;server")) {
        iService *sv = iClob(new_Service(14666));
        insert_Audience(incomingAccepted_Service(sv), sv, (iObserverFunc) incomingConnection);
        if (!open_Service(sv)) {
            puts("Failed to start service");
            return 1;
        }
        puts("Press Enter to quit..."); {
            char line[2];
            fgets(line, sizeof(line), stdin);
        }
    }
    else if (contains_CommandLine(cmdline, "c;client")) {

    }
    else {
        iConstForEach(CommandLine, i, cmdline) {
            if (i.argType == value_CommandLineArgType) {
                printf("\nLooking up \"%s\"...\n", cstr_String(value_CommandLineConstIterator(&i)));
                iAddress *addr = new_Address();
                insert_Audience(lookupFinished_Address(addr), addr, (iObserverFunc) hostLookedUp);
                lookupHost_Address(addr, value_CommandLineConstIterator(&i), 0);
                iRelease(addr);
            }
        }
    }

//    if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
//        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
//        return 2;
//    }

    //printf("IP addresses for %s:\n\n", argv[1]);

//    for(p = res;p != NULL; p = p->ai_next) {
//        void *addr;
//        char *ipver;

//        // get the pointer to the address itself,
//        // different fields in IPv4 and IPv6:
//        if (p->ai_family == AF_INET) { // IPv4
//            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
//            addr = &(ipv4->sin_addr);
//            ipver = "IPv4";
//        } else { // IPv6
//            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
//            addr = &(ipv6->sin6_addr);
//            ipver = "IPv6";
//        }

//        // convert the IP to a string and print it:
//        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
//        printf("  %s: %s\n", ipver, ipstr);
//    }

//    freeaddrinfo(res); // free the linked list

    return 0;
}
