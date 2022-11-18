#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include <thread>
#include <atomic>
#include <string>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define PACKET_SIZE 1024
#define	PORT 4578
#define SERVER_IP "172.30.85.182"

#define UP 72
#define DOWN 80
#define RIGHT 77
#define LEFT 75
#define SPACE 32

#define DRAW_BLACK printf("¡Û");
#define DRAW_WHITE printf("¡Ü")

#define GOMOKU_SIZE 15
#define BLACK 1
#define WHITE 2

// Version
#define MATCHING_ROOM	// ¸ÅÄª·ë ¸¸µé¾îº¸±â