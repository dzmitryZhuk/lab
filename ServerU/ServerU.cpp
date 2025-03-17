#include <iostream>
#include <vector>
#include <unordered_set>
#include <chrono>
#include <string>
#include <numeric>
#include <ws2tcpip.h>
#include "Winsock2.h"
#include "string.h"
#include "locale"
#include "time.h"
#include "ServerU.h"

#pragma comment(lib, "WS2_32.lib")

using namespace std;


struct SETSINCRO
{
	string cmd;
	int correction;
	long long unique_id; // уникальный идентификатор клиента
};

string GetErrorMsgText(int code)
{
	string msgText;

	switch (code)
	{
	case WSAEINTR:				 msgText = "Работа функции прервана\n";						  break;
	case WSAEACCES:				 msgText = "Разрешение отвергнуто\n";						  break;
	case WSAEFAULT:				 msgText = "Ошибочный адрес\n";								  break;
	case WSAEINVAL:				 msgText = "Ошибка в аргументе\n";							  break;
	case WSAEMFILE:				 msgText = "Слишком много файлов открыто\n";				  break;
	case WSAEWOULDBLOCK:		 msgText = "Ресурс временно недоступен\n";					  break;
	case WSAEINPROGRESS:		 msgText = "Операция в процессе развития\n";				  break;
	case WSAEALREADY: 			 msgText = "Операция уже выполняется\n";					  break;
	case WSAENOTSOCK:   		 msgText = "Сокет задан неправильно\n";						  break;
	case WSAEDESTADDRREQ:		 msgText = "Требуется адрес расположения\n";				  break;
	case WSAEMSGSIZE:  			 msgText = "Сообщение слишком длинное\n";				      break;
	case WSAEPROTOTYPE:			 msgText = "Неправильный тип протокола для сокета\n";		  break;
	case WSAENOPROTOOPT:		 msgText = "Ошибка в опции протокола\n";					  break;
	case WSAEPROTONOSUPPORT:	 msgText = "Протокол не поддерживается\n";					  break;
	case WSAESOCKTNOSUPPORT:	 msgText = "Тип сокета не поддерживается\n";				  break;
	case WSAEOPNOTSUPP:			 msgText = "Операция не поддерживается\n";					  break;
	case WSAEPFNOSUPPORT:		 msgText = "Тип протоколов не поддерживается\n";			  break;
	case WSAEAFNOSUPPORT:		 msgText = "Тип адресов не поддерживается протоколом\n";	  break;
	case WSAEADDRINUSE:			 msgText = "Адрес уже используется\n";						  break;
	case WSAEADDRNOTAVAIL:		 msgText = "Запрошенный адрес не может быть использован\n";	  break;
	case WSAENETDOWN:			 msgText = "Сеть отключена\n";								  break;
	case WSAENETUNREACH:		 msgText = "Сеть не достижима\n";							  break;
	case WSAENETRESET:			 msgText = "Сеть разорвала соединение\n";					  break;
	case WSAECONNABORTED:		 msgText = "Программный отказ связи\n";						  break;
	case WSAECONNRESET:			 msgText = "Связь восстановлена\n";							  break;
	case WSAENOBUFS:			 msgText = "Не хватает памяти для буферов\n";				  break;
	case WSAEISCONN:			 msgText = "Сокет уже подключен\n";							  break;
	case WSAENOTCONN:			 msgText = "Сокет не подключен\n";							  break;
	case WSAESHUTDOWN:			 msgText = "Нельзя выполнить send: сокет завершил работу\n";  break;
	case WSAETIMEDOUT:			 msgText = "Закончился отведенный интервал  времени\n";		  break;
	case WSAECONNREFUSED:		 msgText = "Соединение отклонено\n";						  break;
	case WSAEHOSTDOWN:			 msgText = "Хост в неработоспособном состоянии\n";			  break;
	case WSAEHOSTUNREACH:		 msgText = "Нет маршрута для хоста\n";						  break;
	case WSAEPROCLIM:			 msgText = "Слишком много процессов\n";						  break;
	case WSASYSNOTREADY:		 msgText = "Сеть не доступна\n";							  break;
	case WSAVERNOTSUPPORTED:	 msgText = "Данная версия недоступна\n";					  break;
	case WSANOTINITIALISED:		 msgText = "Не выполнена инициализация WS2_32.DLL\n";		  break;
	case WSAEDISCON:			 msgText = "Выполняется отключение\n";						  break;
	case WSATYPE_NOT_FOUND:		 msgText = "Класс не найден\n";								  break;
	case WSAHOST_NOT_FOUND:		 msgText = "Хост не найден\n";								  break;
	case WSATRY_AGAIN:			 msgText = "Неавторизированный хост не найден\n";			  break;
	case WSANO_RECOVERY:		 msgText = "Неопределенная ошибка\n";						  break;
	case WSANO_DATA:			 msgText = "Нет записи запрошенного типа\n";				  break;
	case WSA_INVALID_HANDLE:	 msgText = "Указанный дескриптор события  с ошибкой\n";		  break;
	case WSA_INVALID_PARAMETER:	 msgText = "Один или более параметров с ошибкой\n";			  break;
	case WSA_IO_INCOMPLETE:		 msgText = "Объект ввода-вывода не в сигнальном состоянии\n"; break;
	case WSA_IO_PENDING:		 msgText = "Операция завершится позже\n";					  break;
	case WSA_NOT_ENOUGH_MEMORY:	 msgText = "Не достаточно памяти\n";						  break;
	case WSA_OPERATION_ABORTED:	 msgText = "Операция отвергнута\n";							  break;
	case WSAEINVALIDPROCTABLE:	 msgText = "Ошибочный сервис\n";							  break;
	case WSAEINVALIDPROVIDER:	 msgText = "Ошибка в версии сервиса\n";						  break;
	case WSAEPROVIDERFAILEDINIT: msgText = "Невозможно инициализировать сервис\n";			  break;
	case WSASYSCALLFAILURE:		 msgText = "Аварийное завершение системного вызова\n";		  break;
	default:					 msgText = "Error\n";										  break;
	};

	return msgText;
}

string SetErrorMsgText(string msgText, int code)
{
	return  msgText + GetErrorMsgText(code);
};

int setAverageCorrection(vector<int> &averageCorrection)
{
	int sum = accumulate(averageCorrection.begin(), averageCorrection.end(), 0); // НАХОДИМ СУММУ ВСЕХ ЭЛЕМЕНТОВ ВЕКТОРА
	return sum / averageCorrection.size();										 // ДЕЛИМ СУММУ ВСЕХ ЭЛЕМЕНТОВ НА ИХ КОЛИЧЕСТВО
}

int main()
{
	setlocale(LC_ALL, "Russian");

	SETSINCRO setsincro, getsincro;
	ZeroMemory(&setsincro, sizeof(setsincro));
	ZeroMemory(&getsincro, sizeof(getsincro));

	setsincro.cmd = "SINCRO";
	setsincro.correction = 0;
	setsincro.unique_id = 0;
	SYSTEMTIME tm;

	clock_t c;
	vector<int> averageCorrection;//ВЕКТОР ВСЕХ КОРРЕКЦИЙ
	std::unordered_set<string> connected_clients; // СЕТ IP АДРЕСОВ КЛИЕНТОВ, КОТОРЫЕ ПОДКЛЮЧАЛИСЬ К СЕРВЕРУ

	//cout << "Сервер запущен" << endl; // TODO: to rus
	cout << "Server running" << endl;

	try
	{
		SOCKET sS;
		WSADATA wsaData;


		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup: ", WSAGetLastError());

		if ((sS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) // ТУТ ФЛАГ SOCK_DGRAM ЗНАЧИТ ЧТО БУДЕМ ПЕРЕДАВАТЬ ДАННЫЕ ПО UDP
			throw SetErrorMsgText("Socket: ", WSAGetLastError());

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);// ПОРТ, КОТОРЫЙ СЛУШАЕТ СЕРВЕР
		serv.sin_addr.s_addr = INADDR_ANY;

		if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
			throw SetErrorMsgText("Bind_Server: ", WSAGetLastError());

		int count = 1;

		while (true)
		{
			SOCKADDR_IN client;
			int lc = sizeof(client);
			int average = 0;
			recvfrom(sS, (char*)&getsincro, sizeof(getsincro), NULL, (sockaddr*)&client, &lc);
			GetSystemTime(&tm);
			c = clock();//отсчет времени (сколько прошло тиков со старта программы)
			setsincro.correction = c - getsincro.correction; // ЗНАЧЕНИЕ КОРРЕКЦИИ = ТЕКУЩИЕ ТИКИ СЕРВЕРА - ЗНАЧЕНИЕ ТИКОВ КЛИЕНТА
			sendto(sS, (char*)&setsincro, sizeof(setsincro), 0, (sockaddr*)&client, sizeof(client));

			char clientIP[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(reinterpret_cast<sockaddr_in*>(&client)->sin_addr), clientIP, INET_ADDRSTRLEN);
			{
				string client_id = clientIP;
				client_id += to_string(getsincro.unique_id);
				if (connected_clients.find(client_id) == connected_clients.end()) // если данный клиент еще не подключался:
					// тогда не нужно эту коррекцию учитывать при подсчете
					// просто запоминаем клиента, чтобы дальше учитывали его при подсчете коррекции
				{
					connected_clients.insert(client_id);
				}
				else
				{
					averageCorrection.push_back(setsincro.correction); //ЗАПОМИНАЕМ ТЕКУЩУЮ КОРРЕКЦИЮ
					average = setAverageCorrection(averageCorrection); //подсчитываем среднее значение коррекции
					cout << endl << count << "." << " Date and time " << tm.wMonth << "/" << tm.wDay << "/" << tm.wYear
						<< " " << endl << tm.wHour << " Hours " << tm.wMinute << " Minutes " << tm.wSecond
						<< " Seconds " << tm.wMilliseconds << " Milliseconds " << endl << "Correction = " << setsincro.correction
						<< ", Average correction = " << average << endl;
					cout << "Client's adress " << clientIP << " (" << getsincro.unique_id << ")" << endl;
				}
			}


			count++;
		}

		if (closesocket(sS) == SOCKET_ERROR)
			throw SetErrorMsgText("close socket: ", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << endl << errorMsgText;
	}

	return 0;
}