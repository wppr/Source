#pragma once
#include <string>
#include <queue>
#include <stack>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
using std::string;
using std::thread;
using std::queue;
using std::stack;

#define LOG_OPEN 1

class Server
{
public:

	void SetAddr(string ip, string port);
	void Close();

private:
	thread recieveThread;
	queue<char> charQueue;
	static SOCKET ServerSocket;
	static stack<char> charStack;
	static void Start();
	static void Bind();
	static void Receive();
	static void Accept();
	static int InitSocket();
	static string ipAddr;
	static string port;

public:
	static queue<string> jsonQueue;
};

class Client
{
public:
	int InitSocket();
	int Connect(string ip, string port);
	int Send(string str);
	void Close();

private:
	SOCKET ClientSocket;
};