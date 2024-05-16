#include <iostream>
#include <sstream>
#include <ctime>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "include/inja/inja.hpp"
#include "backend.hpp"
#include <signal.h>

volatile sig_atomic_t stop = 0;

void inthand(int signum) {
    stop = 1;
}

//#include "include/memtrace/memtrace.h"

static bool DEBUG_MODE = false;
// HTTP response kezelése
// A WeatherData lekérdezése után a válasz elküldése a kliensnek

class HttpResponse {
    public:
        enum DateType {
            HO_NAP,
            ALL
        };
        template<DateType T>
        std::string dateFormatter(int hoIdx = 1, int napIdx = 1, WeatherData ForecastData = {}){
            std::string monthNames[] = {"Január", "Február", "Március", "Április", "Május", "Június", "Július", "Augusztus", "Szeptember", "Október", "November", "December"};
            
            
            if constexpr (T == HO_NAP){
                return monthNames[hoIdx] + " " + std::to_string(napIdx) + ".";
            }
            else if constexpr(T == ALL){
                std::time_t t = ForecastData.dt;
                int ho = std::localtime(&t)->tm_mon;
                int nap = std::localtime(&t)->tm_mday;
                std::tm* tm = std::localtime(&t);
                std::stringstream oraperc;
                oraperc <<std::put_time(tm, "%H:%M");
                
                return monthNames[ho] + " " + std::to_string(nap) + ". " + oraperc.str();
            }
            return "";
        }
       
    BackendDataFetcher bdf;

public:
    // A kliens socketjének fogadása és a válasz elküldése
    void operator()(int clientSocket) {
        // Lekérjük az időjárás adatokat
        WeatherData weatherData = bdf.fetchWeather();   
        std::vector<WeatherData> forecast = bdf.fetchForecast(); 

        //@debug Debug céljából kiírjuk a teljes választ 
        #if DEBUG_MODE
            std::cout << "[i] User's IP address:\t " << bdf.publicIp << std::endl;
            std::cout << "[i] User's socket:\t " << clientSocket << std::endl;
            std::cout << "[i] RAW json from OpenWeather:\t " << weatherData._raw << std::endl;
        #endif

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

        std::string datestring = dateFormatter<HO_NAP>(now->tm_mon, now->tm_mday);    
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
        data["hosszusag"] = bdf.hosszusag;
        data["szelesseg"] = bdf.szelesseg;
        data["raw"] = weatherData._raw;
        for (const auto& forecastData : forecast) {
            inja::json item;
            std::string datestring = dateFormatter<ALL>(0, 0, forecastData);
            item["date"] = datestring;
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

    void stop() {
        // Close the server socket
        close(server_fd);

        // Perform any necessary cleanup here

        std::cout << "[i] Server has been stopped" << std::endl;
    }

    // A szerver futtatása
    // A szerver folyamatosan vár a kliensekre, majd a kliens socketjét átadja a httpResponse-nek
    // A kliens socketjét bezárja a válasz elküldése után
    void run() {
        #if !DEBUG_MODE
            while(true) {           
                std::cout << "[i] Listening for newcomers.." << std::endl;
                // A kliens socketjének fogadása, majd a válasz elküldése
                if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
                    perror("[!] socket accept failed (@Server respone)");
                    return;                   
                }                          

                // A válasz elküldése a kliensnek, majd a kliens socketjének bezárása
                httpResponse(new_socket);
                close(new_socket);            
            }
            return;
        #endif   
        #if DEBUG_MODE   
            std::cout << "[DBG_MODE] Debug mode is enabled [DBG_MODE]" << std::endl;               
            std::cout << "[i] Listening for newcomers.." << std::endl;
            // A kliens socketjének fogadása, majd a válasz elküldése
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
                perror("[!][DBG_MODE] socket accept failed (@Server respone)");
                exit(EXIT_FAILURE);
            }           

            // A válasz elküldése a kliensnek, majd a kliens socketjének bezárása
            httpResponse(new_socket);
            close(new_socket);    
            return;
        #endif
    }
};


int main() { 
    //https://stackoverflow.com/questions/16959377/how-to-detect-kill-of-my-process

    struct sigaction act;
    act.sa_handler = inthand;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction");
        return 1;
    } 
    //========eddig
    std::setlocale(LC_ALL, "hu_HU.utf8");
    Server server;
    while (!stop) {
        server.run();
    }
    server.stop();
    return 0;
}