#pragma once

// pragma
#pragma comment(lib, "ws2_32.lib")

// boost include

// include
#include <iostream>
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <Windows.h>
#include <string>
#include <vector>
#include <utility>	//pair
#include <thread>
#include <queue>
#include <string>

// define
#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#define PACKET_SIZE 1024
#define PACKET_SIZE sizeof PacketInfo
#define	PORT 4578
#define SERVER_IP "172.30.85.182"
//#define MESSAGE_SIZE sizeof PacketInfo

#define GOMOKU_SIZE 15
#define BLACK 1
#define WHITE 2

// Version
#define MATCHING_ROOM	// ¸ÅÄª·ë ¸¸µé¾îº¸±â