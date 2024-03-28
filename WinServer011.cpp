// WinServer011.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <thread>
#include <sstream>
#include "http.h"
#include "cam.h"
#include "lidar.h"

// Отправка ответа на страницу index
// conn - номер пользователя
// req - запрос
void cap_stream(int conn, HTTPreq* req);
void lidar_stream(int conn, HTTPreq* req);
void com_stream(int conn, HTTPreq* req);
void status_post(int conn, HTTPreq* req);
void ping(int conn, HTTPreq* req);

// Переменные для обмена информацией между потоками
CAP_STE cap_ste;
CAP_STE com_ste;
CAP_STE lid_ste;

int main()
{
    // Потоки
    // 1 - Камера и камера глубины
    // 2 - Лидар, управление колесами, вещание видео в TCP
    // 3 - Ответ на новые подключения

    // Запускаем log
    log_init();

    // Настраиваем и запускаем потоки
    // 1 - Поток для камеры (в будущем камера и карты глубины) (без запуска будет работать в основном потоке)
    // Камера
    cap_ste.flag = 0;
    cap_ste.mat = new cv::Mat;
    // Карта глубиныы
    com_ste.flag = 0;
    com_ste.mat = new cv::Mat(480, 640, CV_8UC1, cv::Scalar(0));

    // 2 - Поток для лидара, управления колесами и вещания видео в TCP
    // Лидар
    lid_ste.flag = 0;
    lid_ste.mat = new cv::Mat;
    std::thread mc_lidar_sv([]()
        {
            // Подготовка базового изображения
            cv::Mat base_mat_lidar(360, 360, CV_8UC1, cv::Scalar(255));
            for (float deg = 0; deg < 360; deg+= 0.1)
            {
                base_mat_lidar.data[(int)(sin(deg * (3.1415926535 / 180)) * 180 + 180) * 360 + (int)(cos(deg * (3.1415926535 / 180)) * 180 + 180)] = 100;
                base_mat_lidar.data[(int)(sin(deg * (3.1415926535 / 180)) * 90 + 180) * 360 + (int)(cos(deg * (3.1415926535 / 180)) * 90 + 180)] = 100;
                base_mat_lidar.data[(int)(sin(deg * (3.1415926535 / 180)) * 30 + 180) * 360 + (int)(cos(deg * (3.1415926535 / 180)) * 30 + 180)] = 100;
            }
            for (uint16_t i = 0; i < 360; i++)
            {
                base_mat_lidar.data[i * 360 + 180] = 100;
                base_mat_lidar.data[180 * 360 + i] = 100;
            }
            while (1)
            {
                // Лидар
                *(lid_ste.mat) = base_mat_lidar.clone();
                // Тут будет ункция получения данных с лидара и переноса их на изоражения

                // Управление колесами
                // Тут будет функция управления колесами

                // Вещание видео в TCP
                streaming();
            }
        });

    // 3 - Поток для ответа на новые подключения
    std::thread nc([]()
        {
            // Указываем на какие запросы как реагировать
            HTTP* server = new_http((char*)"0.0.0.0:8000");
            handle_http(server, (char*)"/", (char*)"index.html");
            handle_http(server, (char*)"/script.js", (char*)"script.js");
            handle_http(server, (char*)"/style.css", (char*)"style.css");
            handle_http(server, (char*)"/cap_path", (char*)"cap_path.html");
            handle_http(server, (char*)"/cap_path.css", (char*)"cap_path.css");
            handle_http(server, (char*)"/cap_path.js", (char*)"cap_path.js");
            handle_http(server, (char*)"/log.txt", name_file_log());
            handle_http(server, (char*)"/cap", cap_stream);
            handle_http(server, (char*)"/lidar", lidar_stream);
            handle_http(server, (char*)"/compute", com_stream);
            handle_http(server, (char*)"/status", status_post);
            handle_http(server, (char*)"/4x2c.jpg", (char*)"4x2c.jpg");
            handle_http(server, (char*)"/4x2l.jpg", (char*)"4x2l.jpg");
            handle_http(server, (char*)"/4x2r.jpg", (char*)"4x2r.jpg");
            handle_http(server, (char*)"/4x4c.jpg", (char*)"4x4c.jpg");
            handle_http(server, (char*)"/4x4l.jpg", (char*)"4x4l.jpg");
            handle_http(server, (char*)"/4x4r.jpg", (char*)"4x4r.jpg");
            handle_http(server, (char*)"/ping", ping);
            new_stream(cap_ste.mat, &(cap_ste.flag), 0, 30);
            new_stream(lid_ste.mat, &(lid_ste.flag), 1, 50);
            new_stream(com_ste.mat, &(com_ste.flag), 2, 30);

            // Запуск прослушивания
            listen_http(server);

            // Окончание работы (дойти не должно)
            free_http(server);
        });

    // Бесконечная функция получения изображения с камеры (тут будет код Богдана получения изображения и построения карты глубины)
    cam_work(&cap_ste, &com_ste);

    // Если закончился код то ожидаем окончания потоков 
    mc_lidar_sv.join();
    nc.join();

    log_free();
    return 0;
}

// Функция обработки запроса /cap
void cap_stream(int conn, HTTPreq* req)
{
    // Серверная обработка html файла

    // Добавление пользователя в список для стримов
    new_conn_stream(conn, 0);
}

// Функция обработки запроса /lidar
void lidar_stream(int conn, HTTPreq* req)
{
    // Серверная обработка html файла

    // Добавление пользователя в список для стримов
    new_conn_stream(conn, 1);
}

// Функция обработки запроса /compute
void com_stream(int conn, HTTPreq* req)
{
    // Серверная обработка html файла

    // Добавление пользователя в список для стримов
    new_conn_stream(conn, 2);
}

// Функция обработки статуса
void status_post(int conn, HTTPreq* req)
{
    // Серверная обработка html файла


    // Добавление пользователя в список для стримов
}

// Функция ответа для расчета пинга
void ping(int conn, HTTPreq* req)
{
    char buffer[BUFSIZ] = "HTTP/1.1 200 OK\nContent-type: text/plain\n\n";
    size_t readsize = strlen(buffer);
    send_tcp(conn, buffer, readsize);
    sprintf(buffer, "ping");
    send_tcp(conn, buffer, readsize);
    close_tcp(conn);
}
