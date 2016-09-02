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

//#define LOG_OPEN
#ifdef LOG_OPEN
#define LOG(X) printf(X)
#else
#define LOG(X) 
#endif
class Server
{
public:

	void SetAddr(string ip, string port);
	void Bind();
	void Receive();
	void Accept();
	int InitSocket();
	void Close();

private:
	thread recieveThread;
	queue<char> charQueue;
	SOCKET ServerSocket;
	stack<char> charStack;
	string ipAddr;
	string port;

public:
	queue<string> jsonQueue;
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