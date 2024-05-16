#include <iostream>
#include <curl/curl.h>
#include <cstdlib> 
#include "backend.hpp"
#include "include/nlohmann/json.hpp"
//#include "include/memtrace/memtrace.h"

#define MEMTRACE_H

using json = nlohmann::json;

DataFetcher::DataFetcher() {
    curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }
}
DataFetcher::~DataFetcher() {
    std::cout << "[i] DataFetcher: Bye-bye!" << std::endl;
    if (curl) { curl_easy_cleanup(curl); }
}

void BackendDataFetcher::fetchIP() {
    //IP lekérése
    std::string url = "https://api64.ipify.org?format=json";
    //A kapott választ a writeCallback függvény írja a stream-be
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);

    //A curl_easy_perform hívás végzi a lekérést
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) { throw std::runtime_error("[!] CURL error (@IP fetch): " + std::string(curl_easy_strerror(res))); }

    //A stream tartalmát JSON formátumra próbáljuk alakítani
    auto response = json::parse(stream, nullptr, false);
    if (response.is_discarded()) { throw std::runtime_error("[!] Failed to parse IP fetch response (@IP fetch)"); }
    //Az IP-t a response-ból kiolvasva eltároljuk
    publicIp = response.value("ip", "");
    //Fontos minden alkalommal törölni a stream tartalmát, hogy ne legyen benne felesleges adat
    stream.clear();
}
void BackendDataFetcher::fetchGeo() {
    //Geo koordináták lekérése
    std::string url = "http://ip-api.com/json/" + publicIp;
    //A kapott választ a writeCallback függvény írja a stream-be
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);    

    //A curl_easy_perform hívás végzi a lekérést
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) { throw std::runtime_error("[!] CURL error (@Geo): " + std::string(curl_easy_strerror(res))); }

    auto response = json::parse(stream, nullptr, false);
    if (response.is_discarded()) { throw std::runtime_error("[!] Failed to parse IP lookup response (@Geo)"); }
    //A koordinátákat a response-ból kiolvasva eltároljuk
    lat = response.value("lat", 0.0);
    lon = response.value("lon", 0.0);
    stream.clear();
}
WeatherData BackendDataFetcher::fetchWeather() {
    //Időjárás adatok lekérése
    //Az URL-ben a lat és lon helyére a koordinátákat helyettesítjük be
    //Az appid helyére az OpenWeatherMap API kulcsát
    //A units=metric paraméterrel a mértékegységeket metrikusra állítjuk
    //A lang=hu paraméterrel a válasz nyelvét magyarra állítjuk
    std::string url = "https://api.openweathermap.org/data/2.5/weather?lat=" + std::to_string(lat) +
                    "&lon=" + std::to_string(lon) + "&appid=" + weatherAccessKey + "&units=metric&lang=hu";

    //A kapott választ a writeCallback függvény írja a stream-be
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);

    //A curl_easy_perform hívás végzi a lekérést
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) { throw std::runtime_error("[!] CURL error (@Weather lookup): " + std::string(curl_easy_strerror(res))); }

    //A stream tartalmát JSON formátumra próbáljuk alakítani    
    nlohmann::json jsonWeatherData = nlohmann::json::parse(stream);
    //Az adatokat egy WeatherData objektumba tároljuk
    WeatherData data;
    //Ha a weather mező nem üres, akkor a description mező tartalmát eltároljuk
    if (!jsonWeatherData["weather"].empty()) data.description = jsonWeatherData["weather"][0]["description"];

    //Az adatokat a JSON objektumból kiolvasva eltároljuk
    data._raw = stream;
    szelesseg = jsonWeatherData["coord"]["lat"];
    hosszusag = jsonWeatherData["coord"]["lon"];
    data.city = jsonWeatherData["name"];
    data.description = jsonWeatherData["weather"][0]["description"];

    //A számokat kerekítjük, hogy ne legyenek tizedesek a végén
    data.temp = std::round(jsonWeatherData["main"]["temp"].get<double>());
    data.temp_min = std::round(jsonWeatherData["main"]["temp_min"].get<double>());
    data.temp_max = std::round(jsonWeatherData["main"]["temp_max"].get<double>());
    data.pressure = std::round(jsonWeatherData["main"]["pressure"].get<double>());
    data.humidity = std::round(jsonWeatherData["main"]["humidity"].get<double>());
    data.wind_speed = std::round(jsonWeatherData["wind"]["speed"].get<double>());

    data.wind_direction = jsonWeatherData["wind"]["deg"];
    data.sunrise = jsonWeatherData["sys"]["sunrise"];
    data.sunset = jsonWeatherData["sys"]["sunset"];
    data.timezone = jsonWeatherData["timezone"];
    data.icon = jsonWeatherData["weather"][0]["icon"];

    stream.clear();
    //Az adatokat tartalmazó objektumot visszaadjuk a hívónak
    return data;
}
std::vector<WeatherData> BackendDataFetcher::fetchForecast() {
    std::string url = "https://api.openweathermap.org/data/2.5/forecast?lat=" + std::to_string(lat) +
                    "&lon=" + std::to_string(lon) + "&appid=" + weatherAccessKey + "&units=metric&lang=hu";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) { throw std::runtime_error("[!] CURL error (@Weather lookup): " + std::string(curl_easy_strerror(res))); }

    nlohmann::json jsonWeatherData = nlohmann::json::parse(stream);

    std::vector<WeatherData> forecastData;
    for (const auto& item : jsonWeatherData["list"]) {
        WeatherData data;
        if (!item["weather"].empty()) data.description = item["weather"][0]["description"];

        data._raw = stream;
        data.city = jsonWeatherData["city"]["name"];
        data.description = item["weather"][0]["description"];

        data.temp = std::round(item["main"]["temp"].get<double>());
        data.temp_min = std::round(item["main"]["temp_min"].get<double>());
        data.temp_max = std::round(item["main"]["temp_max"].get<double>());
        data.pressure = std::round(item["main"]["pressure"].get<double>());
        data.humidity = std::round(item["main"]["humidity"].get<double>());
        data.wind_speed = std::round(item["wind"]["speed"].get<double>());
        data.dt = item["dt"];

        data.wind_direction = item["wind"]["deg"];
        data.icon = item["weather"][0]["icon"];

        forecastData.push_back(data);
    }

    stream.clear();
    return forecastData;
}
BackendDataFetcher::BackendDataFetcher() {
    // Az osztály konstruktorában hívjuk meg a fetchIP(), fetchGeo() és fetchWeather() függvényeket, illetve a curl-t itt inicializáljuk
    if (!curl) { throw std::runtime_error("[!] Failed to initialize CURL (@bdf constructor)"); }

    //Az adatokat egy stringbe gyűjtjük össze a könnyebb kezelhetőség érdekében
    try {
        //Az IP-t a fetchedData stringbe írjuk ki a többi adattal együtt
        fetchIP();
        fetchedData += "IP: " + publicIp + "\n";
        fetchGeo();
        fetchedData += "Latitude: " + std::to_string(lat) + ", Longitude: " + std::to_string(lon) + "\n";
        fetchWeather();
        WeatherData weatherData = fetchWeather();
        fetchedData += "Weather: " + weatherData.city + weatherData.description + std::to_string(weatherData.temp) + 
                 std::to_string(weatherData.temp_min) + std::to_string(weatherData.temp_max) + 
                 std::to_string(weatherData.pressure) + std::to_string(weatherData.humidity) + 
                 std::to_string(weatherData.wind_speed) + std::to_string(weatherData.wind_direction) + 
                 std::to_string(weatherData.sunrise) + std::to_string(weatherData.sunset) + 
                 std::to_string(weatherData.timezone) + "\n"; 
    } catch (const std::exception& e) { std::cerr << "[!] Exception caught: (@bdf constructor)" << e.what() << std::endl; }     
}
BackendDataFetcher::~BackendDataFetcher() {
    std::cout << "[i] BackendDataFetcher: Bye-bye!" << std::endl;
    //Itt nem kell curl szabadítás, mivel a DataFetcher ősosztályban már megtörtént.
    //A stringek amiket használtunk, azokat nem kell felszabadítani, mert a destruktor automatikusan felszabadítja őket
}


//A writeCallback függvény a curl_easy_perform hívások során a válaszokat írja a stream-be, ez fontos a válaszok kezeléséhez
//@param ptr: a válasz tartalma (char*) 
//@param size: a válasz egy darabjának mérete (size_t)
//@param nmemb: a válasz darabjainak száma (size_t)
//@param stream: a stream, amibe a válaszokat írjuk (std::string*)
size_t BackendDataFetcher::writeCallback(char* ptr, size_t size, size_t nmemb, std::string* stream) {
    //A válasz egy darabjának méretét és darabszámát összeszorozva megkapjuk a válasz hosszát
    //A stream-hoz hozzáfűzzük a válasz tartalmát
    //A függvény visszatérési értéke a válasz hossza
    size_t realsize = size * nmemb;    
    stream->append(ptr, realsize);
    return realsize;
}