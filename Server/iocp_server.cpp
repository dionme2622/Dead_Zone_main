#include <iostream>
#include <unordered_map>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <chrono>

#include "game_header.h"

#pragma comment (lib, "WS2_32.LIB")
#pragma comment (lib, "MSWSock.LIB")

enum IO_OP { IO_RECV, IO_SEND, IO_ACCEPT };

HANDLE g_hIOCP;


class EXP_OVER
{
public:
	EXP_OVER(IO_OP op) : _io_op(op)
	{
		ZeroMemory(&_over, sizeof(_over));

		_wsabuf[0].buf = reinterpret_cast<CHAR *>(_buffer);
		_wsabuf[0].len = sizeof(_buffer);
	}

	WSAOVERLAPPED	_over;
	IO_OP			_io_op;
	SOCKET			_accept_socket;
	unsigned char			_buffer[1024];
	WSABUF			_wsabuf[1];
};

class SESSION;

std::unordered_map<long long, SESSION> g_users;

void print_error_message(int s_err)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, s_err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::wcout << lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
}

class SESSION {
public:
	SOCKET			_c_socket;
	long long		_id;

	EXP_OVER		_recv_over{ IO_RECV };
	unsigned char	_remained;

	float _x = 0.0f, _y = 0.0f, _z = 0.0f;
	float _rx = 0.0f, _ry = 0.0f, _rz = 0.0f;
	float _speed;
	bool _isJumping;
	bool _aaa;
	bool _bbb;
	std::string		_name;



public:
	SESSION() {
		std::cout << "DEFAULT SESSION CONSTRUCTOR CALLED!!\n";
		exit(-1);
	}
	SESSION(long long session_id, SOCKET s) : _id(session_id), _c_socket(s)
	{
		_remained = 0;
		do_recv();
	}
	~SESSION()
	{
		stoc_packet_leave lp;
		lp.size = sizeof(lp);
		lp.type = SToC_PLAYER_LEAVE;
		lp.id = _id;
		for (auto& u : g_users) {
			if (_id != u.first)
				u.second.do_send(&lp);
		}
		closesocket(_c_socket);
	}

	void do_recv()
	{
		DWORD recv_flag = 0;
		ZeroMemory(&_recv_over._over, sizeof(_recv_over._over));
		_recv_over._wsabuf[0].buf = reinterpret_cast<CHAR *>(_recv_over._buffer + _remained);
		_recv_over._wsabuf[0].len = sizeof(_recv_over._buffer) - _remained;

		auto ret = WSARecv(_c_socket, _recv_over._wsabuf, 1, NULL, 
			&recv_flag, &_recv_over._over, NULL);
		if (0 != ret) {
			auto err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no) {
				print_error_message(err_no);
				exit(-1);
			}
		}
	}
	void do_send(void *buff)
	{
		EXP_OVER* o = new EXP_OVER(IO_SEND);
		const unsigned char packet_size = reinterpret_cast<unsigned char *>(buff)[0];
		memcpy(o->_buffer, buff, packet_size);
		o->_wsabuf[0].len = packet_size;
		DWORD size_sent;
		WSASend(_c_socket, o->_wsabuf, 1, &size_sent, 0, &(o->_over), NULL);
	}

	void send_player_info_packet()
	{
		stoc_packet_player_info p;
		p.size = sizeof(p);
		p.type = SToC_PLAYER_INFO;
		p.id = _id;
		p.x = _x;
		p.y = _y;
		p.z = _z;
		do_send(&p);
	}

	void process_packet(unsigned char* p)
	{
		const unsigned char packet_type = p[1];
		switch (packet_type) {
		case CToS_PLAYER_LOGIN :
		{
			ctos_packet_login* packet = reinterpret_cast<ctos_packet_login*>(p);

			

			_name = packet->name;
			_x = 0.0f;
			_y = 0.0f;
			_z = 0.0f;
			send_player_info_packet();

			stoc_packet_enter ep;
			ep.size = sizeof(ep);
			ep.type = SToC_PLAYER_ENTER;
			ep.id = _id;
			strcpy_s(ep.name, _name.c_str());
			ep.o_type = 0;
			ep.x = _x;
			ep.y = _y;
			ep.z = _z;

			for (auto& u : g_users) {
				if (u.first != _id)
					u.second.do_send(&ep);
			}

			for (auto& u : g_users) {
				if (u.first != _id) {
					stoc_packet_enter ep;
					ep.size = sizeof(ep);
					ep.type = SToC_PLAYER_ENTER;
					ep.id = u.first;
					strcpy_s(ep.name, u.second._name.c_str());
					ep.o_type = 0;
					ep.x = u.second._x;
					ep.y = u.second._y;
					ep.z = u.second._z;
					do_send(&ep);
				}
			}
			break;
		}
		case CToS_PLAYER_POS: {
			ctos_packet_position* packet = reinterpret_cast<ctos_packet_position*>(p);
			_x = packet->x;
			_y = packet->y;
			_z = packet->z;
			_rx = packet->rx;
			_ry = packet->ry;
			_rz = packet->rz;
			_speed = packet->speed;
			_isJumping = packet->isJumping;
			_aaa = packet->aaa;
			_bbb = packet->bbb;
			break;
		}

		default: 
			std::cout << "Error Invalid Packet Type\n";
			exit(-1);
		}
	}
};

void do_accept(SOCKET s_socket, EXP_OVER *accept_over)
{
	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);;
	accept_over->_accept_socket = c_socket;
	AcceptEx(s_socket, c_socket, accept_over->_buffer, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		NULL, &accept_over->_over);
}

void pos_sender_thread() {
	while (true) {
		stoc_packet_all_position p{};
		p.type = SToC_ALL_POSITION;

		short i = 0;
		for (auto& [id, session] : g_users) {
			p.players[i].id = id;
			p.players[i].x = session._x;
			p.players[i].y = session._y;
			p.players[i].z = session._z;
			p.players[i].rx = session._rx; 
			p.players[i].ry = session._ry; 
			p.players[i].rz = session._rz;
			p.players[i].speed = session._speed;
			p.players[i].isJumping = session._isJumping;
			p.players[i].aaa = session._aaa;
			p.players[i].bbb = session._bbb;
			++i;
		}

		p.count = i;
		p.size = sizeof(p) - sizeof(p.players) + sizeof(p.players[0]) * i;

		for (auto& [_, session] : g_users) {
			session.do_send(&p);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}


int main()
{
	std::wcout.imbue(std::locale("korean"));

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);;
	if (s_socket <= 0) std::cout << "ERRPR" << "¿øÀÎ";
	else std::cout << "Socket Created.\n";

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(GAME_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(s_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(SOCKADDR_IN));
	listen(s_socket, SOMAXCONN);
	INT addr_size = sizeof(SOCKADDR_IN);

	g_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(s_socket), g_hIOCP, -1, 0);

	EXP_OVER	accept_over(IO_ACCEPT);

	do_accept(s_socket, &accept_over);

	std::thread(pos_sender_thread).detach();

	int new_id = 0;
	while (true) {
		DWORD io_size;
		WSAOVERLAPPED* o;
		ULONG_PTR key;
		BOOL ret = GetQueuedCompletionStatus(g_hIOCP, &io_size, &key, &o, INFINITE);
		EXP_OVER* eo = reinterpret_cast<EXP_OVER*>(o);
		if (FALSE == ret) {
			auto err_no = WSAGetLastError();
			print_error_message(err_no);
			if (g_users.count(key) != 0)
				g_users.erase(key);
			continue;
		}
		if ((eo->_io_op == IO_RECV || eo->_io_op == IO_SEND) && (0 == io_size)) {
			if (g_users.count(key) != 0)
				g_users.erase(key);
			continue;
		}
		switch (eo->_io_op) {
		case IO_ACCEPT :
			CreateIoCompletionPort(reinterpret_cast<HANDLE>(eo->_accept_socket),
				g_hIOCP, new_id, 0);
			g_users.try_emplace(new_id, new_id, eo->_accept_socket);

			std::cout << "New client ID: " << new_id << std::endl;

			new_id++;
			do_accept(s_socket, &accept_over);
			break;
		case IO_SEND :
			delete eo;
			break;
		case IO_RECV :
			SESSION &user = g_users[key];
			unsigned char* p = eo->_buffer;
			int data_size = io_size + user._remained;

			while (p < eo->_buffer + data_size) {
				unsigned char packet_size = *p;
				if (p + packet_size > eo->_buffer + data_size)
					break;
				user.process_packet(p);
				p = p + packet_size;
			}

			if (p < eo->_buffer + data_size) {
				user._remained = static_cast<unsigned char>(eo->_buffer + data_size - p);
				memcpy(p, eo->_buffer, user._remained);
			}
			else
				user._remained = 0;

			user.do_recv();
			break;
		}
	}
	closesocket(s_socket);
	WSACleanup();
}