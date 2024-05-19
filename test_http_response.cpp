#include <gtest/gtest.h>
#include "backend.hpp"
#include "include/inja/inja.hpp"
#include <ctime>
#include <string>

// Sajnos, mivel a main.cpp-ben lett HttpRespone osztály, így a teszteléshez szükséges a HttpResponse osztály "mockolt" implementációja is
class HttpResponse {
public:
    enum DateType {
        HO_NAP,
        ALL,
        SUNRISE,
        SUNSET
    };

    template<DateType T>
    std::string dateFormatter(int hoIdx = 1, int napIdx = 1, WeatherData ForecastData = {});

    void operator()(int clientSocket);

private:
    BackendDataFetcher bdf;
};

// A HttpResponse osztály dateFormatter függvényének implementációja
template<HttpResponse::DateType T>
std::string HttpResponse::dateFormatter(int hoIdx, int napIdx, WeatherData ForecastData) {
    std::string monthNames[] = {"Jan.", "Feb.", "Már.", "Ápr.", "Máj.", "Jún.", "Júl.", "Aug.", "Szept.", "Okt.", "Nov.", "Dec."};

    if constexpr (T == HO_NAP) {
        return monthNames[hoIdx] + " " + std::to_string(napIdx) + ".";
    } else if constexpr (T == ALL) {
        std::time_t t = ForecastData.dt;
        int ho = std::localtime(&t)->tm_mon;
        int nap = std::localtime(&t)->tm_mday;
        std::tm* tm = std::localtime(&t);
        std::stringstream oraperc;
        oraperc << std::put_time(tm, "%H:%M");

        return monthNames[ho] + " " + std::to_string(nap) + ". " + oraperc.str();
    } else if constexpr (T == SUNSET || T == SUNRISE) {
        std::time_t t;
        if (T == SUNSET) t = ForecastData.sunset;
        else if (T == SUNRISE) t = ForecastData.sunrise;
        std::tm* now = std::localtime(&t);
        std::stringstream nap_intervallumjai;
        nap_intervallumjai << std::put_time(now, "%H:%M:%S");

        return nap_intervallumjai.str();
    }
    return "";
}

class HttpResponseTest : public ::testing::Test {
protected:
    HttpResponse httpResponse;

    void SetUp() override { /* Ha kell még extra lépstét hozzáadni a teszthez */ }

    void TearDown() override { }
};

// Teszteljük a dateFormatter függvényt
TEST_F(HttpResponseTest, DateFormatter_HONAP) {
    int month = 5; // Június
    int day = 15;
    std::string formattedDate = httpResponse.dateFormatter<HttpResponse::HO_NAP>(month, day);
    EXPECT_EQ(formattedDate, "Jún. 15.");
}

