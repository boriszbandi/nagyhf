#ifndef BACKEND_DATA_FETCHER_H
#define BACKEND_DATA_FETCHER_H

#include <curl/curl.h>
#include "include/nlohmann/json.hpp"

using json = nlohmann::json;

// Absztrakt osztály, amelyből a WeatherData, illetve a ForecastData származik
class Data {
public:
    //virtual destruktor a leszármazottak miatt (pl. WeatherData), [0-t írtam volna, de nem fogadta el, így default-ra hagytam]
    virtual ~Data() = default;
};

//Az időjárás adatokat tároló osztály
//@param _raw: a kapott válasz teljes tartalma
//@param city: a város neve
//@param description: az időjárás leírása
//@param icon: az időjárás ikonja
//@param temp: a hőmérséklet
//@param temp_min: a minimális hőmérséklet
//@param temp_max: a maximális hőmérséklet
//@param pressure: a légnyomás
//@param humidity: a páratartalom
//@param wind_speed: a szél sebessége
//@param wind_direction: a szél iránya
//@param sunrise: a napkelte ideje
//@param sunset: a napnyugta ideje
//@param timezone: az időzóna
class WeatherData : public Data {
public:
    std::string _raw, city, description, icon;
    int temp, temp_min, temp_max, pressure, humidity, wind_speed, wind_direction, sunrise, sunset, timezone, dt;
};

//Az adatok lekérdezéséért felelős osztály
//Az osztály tartalmazza a curl-t, a stream-et, a publicIp-t, a lat-t és a lon-t
//Az osztály absztrakt, a leszármazottakban kell implementálni a fetchIP(), fetchGeo() és fetchWeather() függvényeket
//A fetchIP() függvény felelős az IP cím lekérdezéséért
//A fetchGeo() függvény felelős a koordináták lekérdezéséért
//A fetchWeather() függvény felelős az időjárás adatok lekérdezéséért
//A writeCallback() függvény felelős a curl válaszának feldolgozásáért

class DataFetcher {
protected:
    CURL* curl;
    std::string stream;
    CURLcode res;
    const char* weatherAccessKey = "c27e7fc04775ef3b49e5e69b77c89ee0";
    double lat, lon;
    
    static size_t writeCallback(char* ptr, size_t size, size_t nmemb, std::string* stream);

public:
    std::string publicIp;
    DataFetcher();

    //A virtuális destruktor miatt a leszármazottakban is szükséges a destruktor, a 0-val az alapértelmezett implementációt használjuk
    virtual ~DataFetcher();
    virtual void fetchIP() = 0;
    virtual void fetchGeo() = 0;
    virtual WeatherData fetchWeather() = 0;
    virtual std::vector<WeatherData> fetchForecast() = 0;
};
//Az adatok lekérdezéséért felelős osztály
//@function fetchIP: az IP cím lekérdezéséért felelős függvény
//@function fetchGeo: a koordináták lekérdezéséért felelős függvény
//@function fetchWeather: az időjárás adatok lekérdezéséért felelős függvény
//@function writeCallback: a curl válaszának feldolgozásáért felelős függvény

class BackendDataFetcher : public DataFetcher {
public:
    std::string fetchedData; 

    BackendDataFetcher();
    ~BackendDataFetcher();
    void fetchIP() override;
    void fetchGeo() override;
    WeatherData fetchWeather() override;
    std::vector<WeatherData> fetchForecast() override;
    static size_t writeCallback(char* ptr, size_t size, size_t nmemb, std::string* stream);
};

#endif // DATA_FETCHER_H