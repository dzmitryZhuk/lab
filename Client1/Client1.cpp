#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Winsock2.h"
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <numeric>
#include <ctime>
#pragma comment(lib, "WS2_32.lib")

using namespace std;

struct GETSINCHRO
{
	string cmd;
	int curvalue;
	long long unique_id; // уникальный идентификатор для клиента
						 // будет использоваться в сервере для подсчетов
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
	return msgText + GetErrorMsgText(code);
};

int calculateAverageCorrection(vector<int>& corrections)
{
	int sum = accumulate(corrections.begin(), corrections.end(), 0); // НАХОДИМ СУММУ ВСЕХ ЭЛЕМЕНТОВ ВЕКТОРА
	return sum / corrections.size();								 // ДЕЛИМ СУММУ ВСЕХ ЭЛЕМЕНТОВ НА ИХ КОЛИЧЕСТВО
}

int main(int argc, char* argv[])
{
	setlocale(LC_CTYPE, "Russian");

	string IP = "127.0.0.1";
	int PORT = 2000;
	int Tc = 1000;

	if (argc != 4)
	{
		// TODO: to rus
		// cout << "Должно быть 3 параметра: адрес сервера, порт сервера, значение задержки в тиках" << endl;
		// cout << "Пример: 192.168.1.3 2000 1000" << endl;
		cout << "3 parameters must be\n";
		return 0;
	}
	else
	{
		IP = argv[1];
		PORT = atoi(argv[2]);
		Tc = atoi(argv[3]);
	}


	SYSTEMTIME tm;
	GETSINCHRO getsincro, setsincro;
	ZeroMemory(&setsincro, sizeof(setsincro));
	ZeroMemory(&getsincro, sizeof(getsincro));
	getsincro.cmd = "SINC";
	getsincro.curvalue = 0;
	auto now = chrono::system_clock::now();
	getsincro.unique_id = chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
	vector<int> corrections;//ВЕКТОР ВСЕХ КОРРЕКЦИЙ

	//cout << "Клиент запущен" << endl; // TODO: to rus
	cout << "Client running" << endl;

	try
	{
		SOCKET cS;
		WSADATA wsaData;

		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup: ", WSAGetLastError());

		if ((cS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) // ТУТ ФЛАГ SOCK_DGRAM ЗНАЧИТ ЧТО БУДЕМ ПЕРЕДАВАТЬ ДАННЫЕ ПО UDP
			throw SetErrorMsgText("Socket: ", WSAGetLastError());

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(PORT); // ПОРТ, КОТОРЫЙ СЛУШАЕТ СЕРВЕР
		serv.sin_addr.s_addr = inet_addr(IP.c_str());
		int maxcor = INT_MIN;
		int mincor = INT_MAX;
		int sum_correction = 0;
		int lensockaddr = sizeof(serv);
		sendto(cS, (char*)&getsincro, sizeof(getsincro), 0, (sockaddr*)&serv, sizeof(serv));
		recvfrom(cS, (char*)&setsincro, sizeof(setsincro), 0, (sockaddr*)&serv, &lensockaddr);
		getsincro.curvalue += setsincro.curvalue;//получение текущего значения расхождения времени

		int  tick_number = 1;
		while (tick_number < 11)
		{
			GetSystemTime(&tm);
			sendto(cS, (char*)&getsincro, sizeof(getsincro), 0, (sockaddr*)&serv, sizeof(serv));
			recvfrom(cS, (char*)&setsincro, sizeof(setsincro), 0, (sockaddr*)&serv, &lensockaddr);
			maxcor = (maxcor < setsincro.curvalue) ? setsincro.curvalue : maxcor;//нахождение максимальной коррекции
			mincor = (mincor > setsincro.curvalue) ? setsincro.curvalue : mincor;//нахождение минимальной коррекции

			cout << " Date and time " << tm.wMonth << "/" << tm.wDay << "/" << tm.wYear << " " << endl
				<< tm.wHour << " Hours " << tm.wMinute << " Minutes " << tm.wSecond << " Seconds " << tm.wMilliseconds
				<< " Milliseconds " << endl << tick_number++ << " " << getsincro.curvalue
				<< " Correction = " << setsincro.curvalue << " Maximum/minimum correction: "
				<< maxcor << "/" << mincor << endl << endl;

			getsincro.curvalue += setsincro.curvalue + Tc; //нахождение текущего значения счетчика времени
			corrections.push_back(setsincro.curvalue); //ЗАПОМИНАЕМ ТЕКУЩУЮ КОРРЕКЦИЮ

			Sleep(Tc);
		}
		//cout << "Средняя коррекция: " << avgcorr / 10 << endl; // TODO: to rus
		int average = calculateAverageCorrection(corrections); //подсчитываем среднее значение коррекции
		cout << "Avg correction: " << average << endl;

		if (closesocket(cS) == SOCKET_ERROR)
			throw SetErrorMsgText("Closesocket: ", WSAGetLastError());

		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << endl << errorMsgText << endl;
	}
	system("pause");

	return 0;
}