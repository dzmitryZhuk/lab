#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include <string>
#include <numeric>
#include <vector>
#include <chrono>
#pragma comment(lib, "WS2_32.lib")

using namespace std;

// смещение между NTP временем (1 января 1900) и временем на операционках Unix, Windows (1 января 1970):
// на серверах NTP время считается с 1 января 1900 и отправляется клиентам, соответственно сколько прошло времени с 1 января 1900
// на операционках Windows, Unix системное время считается с 1 января 1970
constexpr long long NTP_TIMESTAMP_DELTA = 2208988800ull;

// структура SNTP-пакета, взята отсюда -> https://en.wikipedia.org/wiki/Network_Time_Protocol#NTP_packet_header_format
struct NtpPacket {
    uint8_t li_vn_mode;         // Leap Indicator (2 бита), Version Number (3 бита), Mode (3 бита)
    uint8_t stratum;            // слой синхронизации
    uint8_t poll;               // интервал опроса сервера
    uint8_t precision;          // точность системных часов
    uint32_t root_delay;        // задержка сигнала
    uint32_t root_dispersion;   // дисперсия источника синхронизации на сервере (заполняется на сервере, не нужно в этой лабе)
    uint32_t ref_id;            // id источника синхронизации на сервере (заполняется на сервере, не нужно в этой лабе)
    uint32_t last_upd_timestamp[2]; // отметка времени, когда системные часы последний раз устанавливались или корректировались
    uint32_t t0_timestamp[2];   // отметка времени, когда клиент отправляет запрос на сервер (устанавливается клиентом по местным часам)
    uint32_t t1_timestamp[2];   // отметка времени, когда сервер принял запрос от клиента (устанавливается сервером по часам сервера)
    uint32_t t2_timestamp[2];   // отметка времени, когда сервер отправил ответ клиенту (устанавливается сервером по часам сервера)
};

// функция для отправки SNTP-запроса
time_t sendNtpRequest(SOCKET sock, const sockaddr_in& serverAddr) {
    NtpPacket packet;
    ZeroMemory(&packet, sizeof(packet));
    // инициализируем пакет для отправки. нужно использовать именно такие данные
    packet.li_vn_mode = 0b00'100'011; // Mode = 3 (тройка тут означает клиентский запрос)

    auto now = std::chrono::system_clock::now();
    time_t t0 = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count(); // кол-во мс с 1 января 1970 (отметка времени, когда отправляем запрос)

    // отправляем пакет на сервер
    sendto(sock, (char*)(&packet), sizeof(packet), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

    return t0;
}

// функция для получения ответа от сервера
NtpPacket receiveNtpResponse(SOCKET sock) {
    NtpPacket packet;
    sockaddr_in serverAddr;
    int addrLen = sizeof(serverAddr);

    // Получаем ответ от сервера
    int bytesReceived = recvfrom(sock, (char*)&packet, sizeof(packet), 0, (sockaddr*)&serverAddr, &addrLen);
    if (bytesReceived == SOCKET_ERROR) {
        cerr << "Failed to receive NTP response: " << WSAGetLastError() << endl;
    }

    return packet;
}

// функция для подсчета коррекции
time_t calculateCorrection(const NtpPacket& response, time_t t0) {

    // лямбда функция для подсчета временной отметки. считаем из времени формата NTP
    auto getTimeFromNtpTime = [](const uint32_t timestamp[2]) -> time_t {
        // извлекаем временную метку
        uint32_t seconds = ntohl(timestamp[0]);
        uint32_t fraction = ntohl(timestamp[1]); // fraction - дробная часть секунды по стандарту NTP

        // преобразуем в привычное время (в миллисекундах)
        uint64_t res = ((uint64_t)seconds - NTP_TIMESTAMP_DELTA) * 1000;
        uint64_t ms_fraction = ((uint64_t)fraction * (uint64_t)1000) / 0x100000000LL;
        res += ms_fraction;

        return res;
    };
    /////////////////////////////////////////////////////////////////////////////////

    auto now = std::chrono::system_clock::now();
    time_t t3 = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    time_t t1 = getTimeFromNtpTime(response.t1_timestamp);
    time_t t2 = getTimeFromNtpTime(response.t2_timestamp);

    auto res = ((t1 - t0) + (t2 - t3)) / 2; // считаем коррекцию по формуле -> https://en.wikipedia.org/wiki/Network_Time_Protocol#Clock_synchronization_algorithm

    return res;
}

int calculateAverageCorrection(vector<int> &corrections)
{
	int sum = accumulate(corrections.begin(), corrections.end(), 0); // НАХОДИМ СУММУ ВСЕХ ЭЛЕМЕНТОВ ВЕКТОРА
	return sum / corrections.size();								 // ДЕЛИМ СУММУ ВСЕХ ЭЛЕМЕНТОВ НА ИХ КОЛИЧЕСТВО
}

int main(int argc, char* argv[]) {
    setlocale(LC_CTYPE, "Russian");

    string IP = "17.253.38.253"; // time.apple.com // 1.by.pool.ntp.org
    int PORT = 123; // NTP port
    int Tc = 1000;

    if (argc != 4) {
        cout << "Должно быть 3 параметра: адрес сервера, порт сервера, значение задержки в тиках" << endl;
        cout << "Пример: 17.253.38.253 123 1000" << endl;
        return 0;
    }
    else {
        IP = argv[1];
        PORT = atoi(argv[2]);
        Tc = atoi(argv[3]);
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
        cerr << "WSAStartup failed: " << WSAGetLastError() << endl;
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        cerr << "Socket creation failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(IP.c_str());

    vector<int> corrections;//ВЕКТОР ВСЕХ КОРРЕКЦИЙ

    try {
        int maxcor = INT_MIN;
		int mincor = INT_MAX;
        int tick_number = 1;
        while (tick_number < 11) {
            cout << "Sending NTP request to server..." << endl;
            // отправляем запрос на ntp сервер
            auto t0 = sendNtpRequest(sock, serverAddr);
            // получаем ответ от ntp сервера
            cout << "Getting NTP response from server..." << endl;
            auto response = receiveNtpResponse(sock);
            auto correction = calculateCorrection(response, t0);
            maxcor = (maxcor < correction) ? correction : maxcor;//нахождение максимальной коррекции
			mincor = (mincor > correction) ? correction : mincor;//нахождение минимальной коррекции
            cout << "Correction = " << correction << ", min = " << mincor << ", max = " << maxcor << endl;
            corrections.push_back(correction); //ЗАПОМИНАЕМ ТЕКУЩУЮ КОРРЕКЦИЮ
            int average = calculateAverageCorrection(corrections); //подсчитываем среднее значение коррекции
            cout << "Average correction = " << average << endl;

            Sleep(Tc);
            tick_number++;
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}