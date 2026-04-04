//
// Created by tendan on 3.04.2026.
//

#include "weather_transformer.h"

#include <time.h>
#include <math.h>

static double to_rad(double deg)
{
    return deg * M_PI / 180.0;
}

enum TransformResult estimate_irradiance(const struct RawWeatherData *raw_weather_data,
                                         struct CalculatedWeatherData *parsed_weather_data)
{
    struct tm *t = gmtime(&raw_weather_data->unix_time);

    // 1. Dzień roku (d)
    int d = t->tm_yday + 1;

    // 2. Deklinacja słoneczna (delta)
    double delta = 23.45 * sin(to_rad(360.0 / 365.0 * (d - 81)));

    // 3. Kąt godzinny (H)
    // Uproszczony: południe słoneczne jest ok. 12:00 czasu UTC + (lon/15)
    double hour_utc = t->tm_hour + t->tm_min / 60.0 + t->tm_sec / 3600.0;
    double solar_noon_utc = 12.0 - (raw_weather_data->longitude / 15.0);
    double h_angle = 15.0 * (hour_utc - solar_noon_utc);

    // 4. Cosinus kąta zenitalnego (cos_theta)
    double phi = to_rad(raw_weather_data->latitude);
    double d_rad = to_rad(delta);
    double h_rad = to_rad(h_angle);

    double cos_theta = sin(phi) * sin(d_rad) + cos(phi) * cos(d_rad) * cos(h_rad);

    parsed_weather_data->longitude = raw_weather_data->longitude;
    parsed_weather_data->latitude = raw_weather_data->latitude;
    parsed_weather_data->temperature = raw_weather_data->temperature;

    // Jeśli cos_theta <= 0, słońce jest pod horyzontem (noc)
    if (cos_theta <= 0) {
        parsed_weather_data->irradiance = 0;
        return TRANSFORM_OK;
    }

    // 5. Model czystego nieba (Haurwitz)
    // Gcs = 1098 * cos_theta * exp(-0.057 / cos_theta)
    double g_cs = 1098.0 * cos_theta * exp(-0.057 / cos_theta);

    // 6. Korekta o zachmurzenie (Kasten-Czeplak)
    // G = Gcs * (1 - 0.75 * (C^3.4))
    double c = raw_weather_data->cloudiness / 100.0;
    parsed_weather_data->irradiance = g_cs * (1.0 - 0.75 * pow(c, 3.4));

    return TRANSFORM_OK;
}
