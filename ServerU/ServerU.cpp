#include <iostream>
#include <ws2tcpip.h>
#include "Winsock2.h"
#include "string.h"
#include "locale"
#include "time.h"
#include "ServerU.h"

#pragma comment(lib, "WS2_32.lib")

using namespace std;


struct SETSINCRO//запрос клиента на синхронизацию счетчика времени
{
	string cmd; //значения SINC
	int correction; //текущее значение счетчика времени
};

//TODO: to rus
//string GetErrorMsgText(int code)
//{
//	string msgText;
//
//	switch (code)
//	{
//	case WSAEINTR:				 msgText = "Работа функции прервана\n";						  break;
//	case WSAEACCES:				 msgText = "Разрешение отвергнуто\n";						  break;
//	case WSAEFAULT:				 msgText = "Ошибочный адрес\n";								  break;
//	case WSAEINVAL:				 msgText = "Ошибка в аргументе\n";							  break;
//	case WSAEMFILE:				 msgText = "Слишком много файлов открыто\n";				  break;
//	case WSAEWOULDBLOCK:		 msgText = "Ресурс временно недоступен\n";					  break;
//	case WSAEINPROGRESS:		 msgText = "Операция в процессе развития\n";				  break;
//	case WSAEALREADY: 			 msgText = "Операция уже выполняется\n";					  break;
//	case WSAENOTSOCK:   		 msgText = "Сокет задан неправильно\n";						  break;
//	case WSAEDESTADDRREQ:		 msgText = "Требуется адрес расположения\n";				  break;
//	case WSAEMSGSIZE:  			 msgText = "Сообщение слишком длинное\n";				      break;
//	case WSAEPROTOTYPE:			 msgText = "Неправильный тип протокола для сокета\n";		  break;
//	case WSAENOPROTOOPT:		 msgText = "Ошибка в опции протокола\n";					  break;
//	case WSAEPROTONOSUPPORT:	 msgText = "Протокол не поддерживается\n";					  break;
//	case WSAESOCKTNOSUPPORT:	 msgText = "Тип сокета не поддерживается\n";				  break;
//	case WSAEOPNOTSUPP:			 msgText = "Операция не поддерживается\n";					  break;
//	case WSAEPFNOSUPPORT:		 msgText = "Тип протоколов не поддерживается\n";			  break;
//	case WSAEAFNOSUPPORT:		 msgText = "Тип адресов не поддерживается протоколом\n";	  break;
//	case WSAEADDRINUSE:			 msgText = "Адрес уже используется\n";						  break;
//	case WSAEADDRNOTAVAIL:		 msgText = "Запрошенный адрес не может быть использован\n";	  break;
//	case WSAENETDOWN:			 msgText = "Сеть отключена\n";								  break;
//	case WSAENETUNREACH:		 msgText = "Сеть не достижима\n";							  break;
//	case WSAENETRESET:			 msgText = "Сеть разорвала соединение\n";					  break;
//	case WSAECONNABORTED:		 msgText = "Программный отказ связи\n";						  break;
//	case WSAECONNRESET:			 msgText = "Связь восстановлена\n";							  break;
//	case WSAENOBUFS:			 msgText = "Не хватает памяти для буферов\n";				  break;
//	case WSAEISCONN:			 msgText = "Сокет уже подключен\n";							  break;
//	case WSAENOTCONN:			 msgText = "Сокет не подключен\n";							  break;
//	case WSAESHUTDOWN:			 msgText = "Нельзя выполнить send: сокет завершил работу\n";  break;
//	case WSAETIMEDOUT:			 msgText = "Закончился отведенный интервал  времени\n";		  break;
//	case WSAECONNREFUSED:		 msgText = "Соединение отклонено\n";						  break;
//	case WSAEHOSTDOWN:			 msgText = "Хост в неработоспособном состоянии\n";			  break;
//	case WSAEHOSTUNREACH:		 msgText = "Нет маршрута для хоста\n";						  break;
//	case WSAEPROCLIM:			 msgText = "Слишком много процессов\n";						  break;
//	case WSASYSNOTREADY:		 msgText = "Сеть не доступна\n";							  break;
//	case WSAVERNOTSUPPORTED:	 msgText = "Данная версия недоступна\n";					  break;
//	case WSANOTINITIALISED:		 msgText = "Не выполнена инициализация WS2_32.DLL\n";		  break;
//	case WSAEDISCON:			 msgText = "Выполняется отключение\n";						  break;
//	case WSATYPE_NOT_FOUND:		 msgText = "Класс не найден\n";								  break;
//	case WSAHOST_NOT_FOUND:		 msgText = "Хост не найден\n";								  break;
//	case WSATRY_AGAIN:			 msgText = "Неавторизированный хост не найден\n";			  break;
//	case WSANO_RECOVERY:		 msgText = "Неопределенная ошибка\n";						  break;
//	case WSANO_DATA:			 msgText = "Нет записи запрошенного типа\n";				  break;
//	case WSA_INVALID_HANDLE:	 msgText = "Указанный дескриптор события  с ошибкой\n";		  break;
//	case WSA_INVALID_PARAMETER:	 msgText = "Один или более параметров с ошибкой\n";			  break;
//	case WSA_IO_INCOMPLETE:		 msgText = "Объект ввода-вывода не в сигнальном состоянии\n"; break;
//	case WSA_IO_PENDING:		 msgText = "Операция завершится позже\n";					  break;
//	case WSA_NOT_ENOUGH_MEMORY:	 msgText = "Не достаточно памяти\n";						  break;
//	case WSA_OPERATION_ABORTED:	 msgText = "Операция отвергнута\n";							  break;
//	case WSAEINVALIDPROCTABLE:	 msgText = "Ошибочный сервис\n";							  break;
//	case WSAEINVALIDPROVIDER:	 msgText = "Ошибка в версии сервиса\n";						  break;
//	case WSAEPROVIDERFAILEDINIT: msgText = "Невозможно инициализировать сервис\n";			  break;
//	case WSASYSCALLFAILURE:		 msgText = "Аварийное завершение системного вызова\n";		  break;
//	default:					 msgText = "Error\n";										  break;
//	};
//
//	return msgText;
//}
string GetErrorMsgText(int code)
{
	string msgText;

	switch (code)
	{
	case WSAEINTR:				 msgText = "Function aborted\n";						  break;
	case WSAEACCES:				 msgText = "Permission denied\n";						  break;
	case WSAEFAULT:				 msgText = "Invalid address\n";							  break;
	case WSAEINVAL:				 msgText = "Error in argument\n";						  break;
	case WSAEMFILE:				 msgText = "Too many files open\n";						  break;
	case WSAEWOULDBLOCK:		 msgText = "Resource temporarily unavailable\n";		  break;
	case WSAEINPROGRESS:		 msgText = "Operation in progress\n";					  break;
	case WSAEALREADY: 			 msgText = "Operation already in progress\n";			  break;
	case WSAENOTSOCK:   		 msgText = "Socket specified incorrectly\n";			  break;
	case WSAEDESTADDRREQ:		 msgText = "Location address required\n";				  break;
	case WSAEMSGSIZE:  			 msgText = "Message too long\n";						  break;
	case WSAEPROTOTYPE:			 msgText = "Invalid protocol type for socket\n";		  break;
	case WSAENOPROTOOPT:		 msgText = "Error in protocol option\n";				  break;
	case WSAEPROTONOSUPPORT:	 msgText = "Protocol not supported\n";					  break;
	case WSAESOCKTNOSUPPORT:	 msgText = "Socket type not supported\n";				  break;
	case WSAEOPNOTSUPP:			 msgText = "Operation not supported\n";					  break;
	case WSAEPFNOSUPPORT:		 msgText = "Protocol type not supported\n";			      break;
	case WSAEAFNOSUPPORT:		 msgText = "Address type not supported by protocol\n";	  break;
	case WSAEADDRINUSE:			 msgText = "Address already in use\n";					  break;
	case WSAEADDRNOTAVAIL:		 msgText = "Requested address cannot be used\n";		  break;
	case WSAENETDOWN:			 msgText = "Network down\n";							  break;
	case WSAENETUNREACH:		 msgText = "Network not reachable\n";					  break;
	case WSAENETRESET:			 msgText = "Network dropped connection\n";				  break;
	case WSAECONNABORTED:		 msgText = "Software connection failure\n";				  break;
	case WSAECONNRESET:			 msgText = "Connection restored\n";						  break;
	case WSAENOBUFS:			 msgText = "Out of memory for buffers\n";				  break;
	case WSAEISCONN:			 msgText = "Socket already connected\n";				  break;
	case WSAENOTCONN:			 msgText = "Socket not connected\n";					  break;
	case WSAESHUTDOWN:			 msgText = "Cannot send: socket terminated\n";			  break;
	case WSAETIMEDOUT:			 msgText = "Timeout expired\n";							  break;
	case WSAECONNREFUSED:		 msgText = "Connection refused\n";						  break;
	case WSAEHOSTDOWN:			 msgText = "Host in down state\n";						  break;
	case WSAEHOSTUNREACH:		 msgText = "No route for host\n";						  break;
	case WSAEPROCLIM:			 msgText = "Too many processes\n";						  break;
	case WSASYSNOTREADY:		 msgText = "Network not reachable\n";					  break;
	case WSAVERNOTSUPPORTED:	 msgText = "This version is not available\n";			  break;
	case WSANOTINITIALISED:		 msgText = "WS2_32.DLL failed to initialize\n";			  break;
	case WSAEDISCON:			 msgText = "Disconnecting\n";							  break;
	case WSATYPE_NOT_FOUND:		 msgText = "Class not found\n";							  break;
	case WSAHOST_NOT_FOUND:		 msgText = "Host not found\n";							  break;
	case WSATRY_AGAIN:			 msgText = "Unauthorized host not found\n";				  break;
	case WSANO_RECOVERY:		 msgText = "Unspecified error\n";						  break;
	case WSANO_DATA:			 msgText = "No entry for requested type\n";				  break;
	case WSA_INVALID_HANDLE:	 msgText = "The specified event handle failed\n";		  break;
	case WSA_INVALID_PARAMETER:	 msgText = "One or more parameters failed\n";			  break;
	case WSA_IO_INCOMPLETE:		 msgText = "The I/O object is not in a signaled state\n"; break;
	case WSA_IO_PENDING:		 msgText = "The operation will complete later\n";		  break;
	case WSA_NOT_ENOUGH_MEMORY:	 msgText = "Not enough memory\n";						  break;
	case WSA_OPERATION_ABORTED:	 msgText = "The operation was rejected\n";				  break;
	case WSAEINVALIDPROCTABLE:	 msgText = "Errored service\n";							  break;
	case WSAEINVALIDPROVIDER:	 msgText = "Error in service version\n";				  break;
	case WSAEPROVIDERFAILEDINIT: msgText = "Unable to initialize service\n";			  break;
	case WSASYSCALLFAILURE:		 msgText = "System call aborted\n";						  break;
	default:					 msgText = "Error\n";									  break;
	};

	return msgText;
}

string SetErrorMsgText(string msgText, int code)
{
	return  msgText + GetErrorMsgText(code);
};

//int setAverageCorrection(int averageCorrection[], int length)
//{
//реализация нахождения средней коррекции
//}

int main()
{
	setlocale(LC_ALL, "Russian");

	SETSINCRO setsincro, getsincro;//значения установки 
	ZeroMemory(&setsincro, sizeof(setsincro));
	ZeroMemory(&getsincro, sizeof(getsincro));

	setsincro.cmd = "SINCRO";//начальная установка из структуры
	setsincro.correction = 0;//начальная установка из структуры
	SYSTEMTIME tm;//получение системного времени

	clock_t c;//время работы сервера с момента запуска
	int averageCorrection[50];//значение средней коррекции

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

		int count = 1;//какой по счету эксперимент

		while (true)
		{
			SOCKADDR_IN client;
			int lc = sizeof(client);
			int average = 0;//первоначальное среднее значение коррекции
			GetSystemTime(&tm);
			recvfrom(sS, (char*)&getsincro, sizeof(getsincro), NULL, (sockaddr*)&client, &lc);
			c = clock();//отсчет времени
			setsincro.correction = c - getsincro.correction; // ЗНАЧЕНИЕ КОРРЕКЦИИ = ТЕКУЩИЕ ТИКИ СЕРВЕРА - ЗНАЧЕНИЕ ТИКОВ КЛИЕНТА
			//averageCorrection[count - 1] = реализация получения значения средней коррекции в одном эксперименте;
			//average = setAverageCorrection(averageCorrection, count);//реализация получения значения средней коррекции за все эксперименты;
			sendto(sS, (char*)&setsincro, sizeof(setsincro), 0, (sockaddr*)&client, sizeof(client));


			//нахождение адреса клиента

			//TODO: to rus
			//cout << endl << count << "." << " Date and time " << "МЕСЯЦ УКАЗАТЬ" << "/" << "ДЕНЬ УКАЗАТЬ" << "/" << "ГОД УКАЗАТЬ"//
			//	<< " " << endl << "ЧАСЫ УКАЗАТЬ" << " Hours " << "МИНУТЫ УКАЗАТЬ" << " Minutes " << "СЕКУНДЫ УКАЗАТЬ"//
			//	<< " Seconds " << "МИЛЛИСЕКУНДЫ УКАЗАТЬ" << " Milliseconds " << endl << "Correction = " << "КОРРЕКЦИЮ УКАЗАТЬ"//
			//	<< ", Average correction = " << "СРЕДНЮЮ КОРРЕКЦИЮ УКАЗАТЬ" << endl;//
			//cout << "Client's adress " << "АДРЕС КЛИЕНТА УКАЗАТЬ" << endl;//
			cout << endl << count << "." << " Date and time " << tm.wMonth << "/" << tm.wDay << "/" << tm.wYear//
				<< " " << endl << tm.wHour << " Hours " << tm.wMinute << " Minutes " << tm.wSecond//
				<< " Seconds " << tm.wMilliseconds << " Milliseconds " << endl << "Correction = " << "CORRECTION"//
				<< ", Average correction = " << "CORRECTION AVG" << endl;//
			cout << "Client's adress " << "CLIENT ADDRESS" << endl;//

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