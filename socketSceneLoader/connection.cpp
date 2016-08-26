#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <assert.h>
#include <mutex>
#include "connection.h"
using std::cout;
using std::endl;
using std::cerr;
using std::mutex;
using std::vector;
using std::stringstream;

SOCKET Server::ServerSocket;
stack<char> Server::charStack;
queue<string> Server::jsonQueue;
string Server::ipAddr;
string Server::port;
mutex jsonQueueMutex;

#define BATCH 100000+10

#pragma comment(lib, "Ws2_32.lib")

/*********************************************************************/
//Server
/*********************************************************************/
int Server::InitSocket()
{
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	ServerSocket = INVALID_SOCKET;
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	u_long iMode = 1;
	ioctlsocket(ServerSocket, FIONBIO, &iMode);

	if (ServerSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	if (LOG_OPEN)
	printf("init success\n");
}

void Server::SetAddr(string ip, string port)
{
	this->ipAddr = ip;
	this->port = port;

	recieveThread = thread(Start);
	recieveThread.detach();
}

void Server::Bind()
{
	if (LOG_OPEN)
	printf("binding...\n");
	int iResult;

	addrinfo hints = {}, *server_info = nullptr;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	getaddrinfo(ipAddr.c_str(), port.c_str(), &hints, &server_info);

	iResult = bind(ServerSocket, server_info->ai_addr, static_cast<int>(server_info->ai_addrlen));
	if (iResult == SOCKET_ERROR) {
		if (LOG_OPEN)
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(ServerSocket);
		freeaddrinfo(server_info);
		WSACleanup();
		return;
	}

	freeaddrinfo(server_info);

	if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR) {
		if (LOG_OPEN)
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ServerSocket);
		WSACleanup();
		return;
	}

	if (LOG_OPEN)
	printf("bind success\n");
}

void Server::Receive()
{
	int iResult;
	stringstream ss;
	ss.str("");

	//receive buffer
	do {
		char buf[BATCH];
		if(LOG_OPEN)
		printf("receiving\n");
		iResult = recv(ServerSocket, buf, 100000, 0);
		if(LOG_OPEN)
		printf("iResult %d\n", iResult);

		if (iResult > 0) {
			//json segmentation
			if(LOG_OPEN)
			printf("recv success\n");
			for (int i = 0; i < iResult; ++i)
			{
				ss << buf[i];
				switch (buf[i])
				{
				case '{':
					charStack.push(buf[i]);
					break;

				case '}':
					assert('{' == charStack.top());
					charStack.pop();
					if (charStack.empty())
					{
						jsonQueueMutex.lock();
						jsonQueue.push(ss.str());
						jsonQueueMutex.unlock();
						ss.str("");
					}
					break;

				case '[':
					charStack.push(buf[i]);
					break;

				case ']':
					assert('[' == charStack.top());
					charStack.pop();
					if (charStack.empty())
					{
						jsonQueueMutex.lock();
						jsonQueue.push(ss.str());
						jsonQueueMutex.unlock();
						ss.str("");
					}
					break;
				}
			}
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			if (LOG_OPEN)
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ServerSocket);
			WSACleanup();
			return;
		}

	} while (iResult > 0);

}

void Server::Accept()
{
	if (LOG_OPEN)
	printf("accepting...\n");
	ServerSocket = accept(ServerSocket, NULL, NULL);
	if (ServerSocket == INVALID_SOCKET) {
		if (LOG_OPEN)
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(ServerSocket);
		WSACleanup();
		return;
	}
	else
	{
		printf("accepted!\n");
	}

	return;
}

void Server::Close()
{
	closesocket(ServerSocket);
	WSACleanup();
}

//Note : the socket's creation ,connection and ... must be in the same thread
void Server::Start()
{
	while (true)
	{
		InitSocket();
		Bind();
		Accept();
		Receive();
	}
}

/*********************************************************************/
//Client
/*********************************************************************/
int Client::InitSocket()
{
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	ClientSocket = INVALID_SOCKET;
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	if (ClientSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}
}

int Client::Connect(string ip, string port)
{
	int iResult;

	addrinfo hints = {}, *server_info = nullptr;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	getaddrinfo(ip.c_str(), port.c_str(), &hints, &server_info);

	iResult = SOCKET_ERROR;
	while (iResult == SOCKET_ERROR)
	{
		if (LOG_OPEN)
		printf("connecting...\n");
		iResult = connect(ClientSocket, server_info->ai_addr, static_cast<int>(server_info->ai_addrlen));
	}

	printf("connected!\n");
	return 0;
}

int Client::Send(string buf)
{
	int iResult = send(ClientSocket, &buf[0], (int)(buf.size() * sizeof(char)), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	return 0;
}

void Client::Close()
{
	closesocket(ClientSocket);
	ClientSocket = INVALID_SOCKET;
	WSACleanup();
}