#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
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

// функция для преобразования NTP-времени в SYSTEMTIME
SYSTEMTIME getDateTimeFromNtp(const uint32_t ntp_time[2])
{
    SYSTEMTIME tm = {0};

    // извлекаем целую часть секунд из NTP-времени
    uint32_t seconds = ntohl(ntp_time[0]);
    uint32_t fraction = ntohl(ntp_time[1]);

    // переводим NTP-время в windows/unix время
    time_t actual_time = static_cast<time_t>(seconds - NTP_TIMESTAMP_DELTA);

    // преобразуем windows/unix время в локальное время
    struct tm* local_time = localtime(&actual_time);
    if (!local_time)
        return tm;

    // заполняем структуру SYSTEMTIME
    tm.wYear = static_cast<WORD>(local_time->tm_year + 1900); // год
    tm.wMonth = static_cast<WORD>(local_time->tm_mon + 1);    // месяц (1-12)
    tm.wDay = static_cast<WORD>(local_time->tm_mday);         // день месяца
    tm.wHour = static_cast<WORD>(local_time->tm_hour);        // часы
    tm.wMinute = static_cast<WORD>(local_time->tm_min);       // минуты
    tm.wSecond = static_cast<WORD>(local_time->tm_sec);       // секунды

    // вычисляем миллисекунды из дробной части NTP-времени
    uint32_t milliseconds = static_cast<uint32_t>(((uint64_t)fraction * 1000ULL) / 0x100000000LL);
    tm.wMilliseconds = static_cast<WORD>(milliseconds);

    return tm;
}

// функция подсчета разницы времени NTP сервера и текущего устройства, возвращает разницу в миллисекундах
time_t calculateDefferenceNtpCurrent(SYSTEMTIME ntp, SYSTEMTIME current)
{
    time_t res = 0;
    res += ntp.wMilliseconds - current.wMilliseconds;
    res += 1000 * (ntp.wSecond - current.wSecond);
    res += 60 * 1000 * (ntp.wMinute - current.wMinute);
    return res;
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

    try {
        int tick_number = 1;
        while (tick_number < 11) {
            cout << "Sending NTP request to server..." << endl;
            // отправляем запрос на ntp сервер
            auto t0 = sendNtpRequest(sock, serverAddr);
            // получаем ответ от ntp сервера
            cout << "Getting NTP response from server..." << endl;
            auto response = receiveNtpResponse(sock);

            // получаем отметки времени локального устройства и сервера и выводим их в консоль
            SYSTEMTIME tm_current, tm_NTP;
            GetSystemTime(&tm_current); // tm_current сейчас в UTC формате времени, преобразуем в локальное время (например для РБ локалькое время = UTC+3)
            // преобразуем SYSTEMTIME в FILETIME
            FILETIME utc_filetime;
            SystemTimeToFileTime(&tm_current, &utc_filetime);
            // преобразуем FILETIME обратно в SYSTEMTIME, но уже в локальное время
            FileTimeToLocalFileTime(&utc_filetime, &utc_filetime);
            FileTimeToSystemTime(&utc_filetime, &tm_current);

            char buff[128]; // буффер для вывода отметки времени в консоль
            // заполняем буффер данными в таком формате [2025/01/17 08:18:16.569]
            // ТЕКУЩЕЕ ВРЕМЯ НА УСТРОЙСТВЕ
            snprintf(buff, sizeof(buff), "[%04d/%02d/%02d %02d:%02d:%02d.%03d]", tm_current.wYear, tm_current.wMonth, tm_current.wDay, tm_current.wHour, tm_current.wMinute, tm_current.wSecond, tm_current.wMilliseconds);
            cout << buff << " current machine timestamp" << endl;

            tm_NTP = getDateTimeFromNtp(response.t2_timestamp);
            // заполняем буффер данными в таком формате [2025/01/17 08:18:16.569]
            // ВРЕМЯ NTP СЕРВЕРЕ
            snprintf(buff, sizeof(buff), "[%04d/%02d/%02d %02d:%02d:%02d.%03d]", tm_NTP.wYear, tm_NTP.wMonth, tm_NTP.wDay, tm_NTP.wHour, tm_NTP.wMinute, tm_NTP.wSecond, tm_NTP.wMilliseconds);
            cout << buff << " NTP server timestamp" << endl;
            /////////////////////////////////////////////////////////////////////////////////
            cout << "Difference between current time and NTP: " << calculateDefferenceNtpCurrent(tm_NTP, tm_current) << " ms" << endl << endl;


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