/**********************************************************************
 * Copyright (c) 2023 John Woo
 * All rights reserved.
 *
 * This software is distributed under the terms of the LICENSE file
 * found in the root directory of this distribution
 *
 * DISCLAIMER:
 * This software is intended for authorized use only and should not be
 * used to target any individual or entity without their express consent.
 * The author, John Woo, holds no liability for misuse of this software
 * or any derivative thereof. Unauthorized use, replication, or modification
 * of this software is strictly prohibited and may be punishable under law.
 * 
 * Side Note:
 * This project actually started out as a speed test app but my friend
 * told me it made his internet stop working so I decided to make it
 * more powerful lol. 
 * 
 **********************************************************************/

#include <iostream>
#include <string>
#include <signal.h>
#include <unistd.h>
#include <chrono>

#ifdef _WIN64
    #include <winsock2.h>
    #include <windows.h>
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
#endif

// Creating socket file descriptor
int sockfd;

void safe_exit()
{
    close(sockfd);
    #ifdef _WIN64
        WSACleanup();
    #endif
    exit(0);
}

void sigint_handler(int s) {
    safe_exit();
}

void print_bps(int data_lensum)
{
    std::string unit = "B";
    std::string unit2 = "b";
    float data_len = data_lensum;
    if (data_len > 1024)
    {
        data_len /= 1024;
        unit = "KiB";
        unit2 = "Kb";
    }
    if (data_len > 1024)
    {
        data_len /= 1024;
        unit = "MiB";
        unit2 = "Mb";
    }
    if (data_len > 1024)
    {
        data_len /= 1024;
        unit = "GiB";
        unit2 = "Gb";
    }
    std::cout << "\33[2K\rDEBUG: " << data_len << " " << unit << "/s" << " [" << data_len * 8 << " " << unit2 << "/s]" << std::flush;
}

int main(int argc, char** argv) {
    int c;
    bool debug = false;

    while ((c = getopt(argc, argv, "d")) != -1)
    {
        switch (c)
        {
        case 'd':
            debug = true;
            break;
        default:
            break;
        }
    }

    if (argc - optind != 1) {
        std::cout << "Usage: " << argv[0] << " [-d] target-ip" << std::endl;
        return 0;
    }

    #ifdef _WIN64
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "Failed to initialize WinSock." << std::endl;
            return -1;
        }
    #endif

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    signal(SIGINT, sigint_handler);

    // Setup the server address
    const char* target_addr = argv[optind];
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080); // Port doesnt matter
    servaddr.sin_addr.s_addr = inet_addr(target_addr);

    if (servaddr.sin_addr.s_addr == INADDR_NONE) {
        std::cerr << "Invalid IP address" << std::endl;
        safe_exit();
    }

    // Send message to server
    std::string message(512, 'a');
    int loops = 0;
    auto start = std::chrono::high_resolution_clock::now();

    while (true) {
        sendto(sockfd, message.c_str(), message.size(),
           0, (const struct sockaddr *)&servaddr,
            sizeof(servaddr));
        
        // Debugging spaghetti
        if (debug)
        {
            loops++;
            if (loops < 1000)
                continue;

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
            if (duration.count() >= 1)
            {
                int data_lensum = message.size() * loops;
                float bps = data_lensum / duration.count();

                print_bps(bps);

                loops = 0;
                start = std::chrono::high_resolution_clock::now();
            }
        }
    }

    // I think this is redundant but I'll just leave it in here
    safe_exit();
    return 0;
}