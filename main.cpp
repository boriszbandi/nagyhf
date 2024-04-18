#include <iostream>
#include <sstream>
#include <ctime>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "include/inja/inja.hpp"
#include "backend.hpp"

// HTTP response kezelése
// A WeatherData lekérdezése után a válasz elküldése a kliensnek

class HttpResponse {
    BackendDataFetcher bdf;

public:
    // A kliens socketjének fogadása és a válasz elküldése
    void operator()(int clientSocket) {
        // Lekérjük az időjárás adatokat
        WeatherData weatherData = bdf.fetchWeather();   
        std::vector<WeatherData> forecast = bdf.fetchForecast(); 

        //@debug Debug céljából kiírjuk a teljes választ 
        std::cout << "[i] User's IP address:\t " << bdf.publicIp << std::endl;
        std::cout << "[i] User's socket:\t " << clientSocket << std::endl;
        std::cout << "[i] RAW json from OpenWeather:\t " << weatherData._raw << std::endl;

        std::time_t t = std::time(0);
        std::tm* now = std::localtime(&t);

        //A HTML template beolvasása, majd inja segítéségével a helyettesítés
        //A helyettesítéshez szükséges adatokat a weatherData-ból kiolvastuk és egy JSON objektumba helyeztük
        std::ifstream file("skeleton.html");
        std::stringstream buffer;

        //Bufferbe olvassuk a fájl tartalmát
        buffer << file.rdbuf();

        //A buffer tartalmát stringbe helyezzük
        std::string template_html = buffer.str();
        file.close();

        // Injában létrehozunk egy környezetet, majd a template-et beolvassuk és a helyettesítést elvégezzük
        inja::Environment env;

        // A template-et beolvassuk és a helyettesítést elvégezzük
        inja::Template temp = env.parse(template_html);

        // Itt történik a helyettesítés
        // A data egy JSON objektum, amely tartalmazza HTML adatait
        nlohmann::json data;    

        std::string monthNames[] = {"Január", "Február", "Március", "Április", "Május", "Június", "Július", "Augusztus", "Szeptember", "Október", "November", "December"};
        std::string monthName = monthNames[now->tm_mon];
        std::string datestring = monthName + " " + std::to_string(now->tm_mday) + ".";    
        data["date"] = datestring;
        data["city"] = weatherData.city;
        data["temp"] = weatherData.temp;
        data["max_temp"] = weatherData.temp_max;
        data["min_temp"] = weatherData.temp_min;
        data["description"] = weatherData.description;
        data["icon"] = weatherData.icon;
        data["wind_speed"] = weatherData.wind_speed;
        data["wind_direction"] = weatherData.wind_direction;
        data["pressure"] = weatherData.pressure;
        data["sunset"] = weatherData.sunset;
        data["sunrise"] = weatherData.sunrise;
        data["ip"] = bdf.publicIp; 
        for (const auto& forecastData : forecast) {
            inja::json item;
            std::time_t t = forecastData.dt;
            std::tm* timePtr = std::localtime(&t);
            std::stringstream ss;
            ss << std::put_time(timePtr, "%B %d. %H:%M");
            item["date"] = ss.str();
            item["city"] = forecastData.city;
            item["description"] = forecastData.description;
            item["temp"] = forecastData.temp;
            item["temp_min"] = forecastData.temp_min;
            item["temp_max"] = forecastData.temp_max;
            item["pressure"] = forecastData.pressure;
            item["humidity"] = forecastData.humidity;
            item["wind_speed"] = forecastData.wind_speed;
            item["wind_direction"] = forecastData.wind_direction;
            item["icon"] = forecastData.icon;
            data["forecast"].push_back(item);
        }
        // A behelyettesített template-t lerenedereljük, majd a válaszhoz hozzáadjuk a HTTP fejlécet
        std::string response = env.render(temp, data);
        response = "HTTP/1.1 200 OK\r\n"
                   "Content-Type: text/html\r\n\r\n" + response;

        // A választ elküldjük a kliensnek
        send(clientSocket, response.c_str(), response.size(), 0);
    }
};
class Server {
    //@param server_fd: a szerver socketje
    //@param new_socket: a kliens socketje
    //@param address: a szerver címe
    //@param opt: a socket beállításai
    //@param addrlen: a cím mérete
    //@param httpResponse: az HTTP válasz kezelése

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    HttpResponse httpResponse;

public:
    Server() {
        //Itt az egész https://www.geeksforgeeks.org/socket-programming-cc/ alapján készült

        // Létrehozzuk a szerver socketet
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("[!] socket failed (@Server socket creation)");
            exit(EXIT_FAILURE);
        }

        // A socket beállításai, hogy újra lehessen használni a portot és a címet
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            perror("[!] setsockopt failed (@Server socket options)");
            exit(EXIT_FAILURE);
        }

        // A szerver címe és portja beállítása (127.0.0.1:8080)
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(8080);

        // A socket bindelése a címre, majd a hallgató módba állítása
        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
            perror("[!] bind failed (@Server socket bind)");
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 3) < 0) {
            perror("[!] listen failed (@Server socket");
            exit(EXIT_FAILURE);
        }

        //Ha minden sikerült, kiírjuk, hogy a szerver fut (mert fut :D)
        std::cout << "[i] Server is running on port 8080\tPress Ctrl+C to stop server" << std::endl; 
    }

    // A szerver futtatása
    // A szerver folyamatosan vár a kliensekre, majd a kliens socketjét átadja a httpResponse-nek
    // A kliens socketjét bezárja a válasz elküldése után
    void run() {
        while(true) {           
            std::cout << "[i] Listening for newcomers.." << std::endl;
            // A kliens socketjének fogadása, majd a válasz elküldése
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
                perror("[!] socket accept failed (@Server respone)");
                exit(EXIT_FAILURE);
            }           

            // A válasz elküldése a kliensnek, majd a kliens socketjének bezárása
            httpResponse(new_socket);
            close(new_socket);
        }
    }
};


int main() {   
    std::setlocale(LC_ALL, "hu_HU.utf8");
    Server server;
    server.run();
    return 0;
}