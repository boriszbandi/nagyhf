#include <gtest/gtest.h>
#include "backend.hpp"
#include <string>
#include <stdexcept>
#include <vector>

class DataFetcherTest : public ::testing::Test {
protected:
    void SetUp() override {
        df = new BackendDataFetcher();
    }

    void TearDown() override {
        delete df;
    }

    BackendDataFetcher* df;
};

// Teszteljük a DataFetcher osztály konstruktorát
TEST_F(DataFetcherTest, Initialization) {
    EXPECT_NE(df->getIP(), "");
    EXPECT_NE(df->getLatitude(), 0.0);
    EXPECT_NE(df->getLongitude(), 0.0);
}

// Teszteljük a fetchIP függvényt
TEST_F(DataFetcherTest, FetchIP) {
    df->fetchIP();
    EXPECT_NE(df->getIP(), "");
}

// Teszteljük a fetchGeo függvényt
TEST_F(DataFetcherTest, FetchGeo) {
    df->fetchIP();
    df->fetchGeo();
    EXPECT_NE(df->getLatitude(), 0.0);
    EXPECT_NE(df->getLongitude(), 0.0);
}

// Teszteljük a fetchWeather függvényt
TEST_F(DataFetcherTest, FetchWeather) {
    df->fetchIP();
    df->fetchGeo();
    WeatherData weather = df->fetchWeather();
    EXPECT_NE(weather.city, "");
    EXPECT_NE(weather.temp, 0);
}

// Teszteljük a fetchForecast függvényt
TEST_F(DataFetcherTest, FetchForecast) {
    df->fetchIP();
    df->fetchGeo();
    std::vector<WeatherData> forecast = df->fetchForecast();
    EXPECT_GT(forecast.size(), 0);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
