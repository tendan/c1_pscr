import json
import time
from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse, parse_qs

def make_weather_response(lat, lon):
    return {
        "coord": {"lat": lat, "lon": lon},
        "main": {
            "temp": 20.5,
            "feels_like": 19.0,
            "temp_min": 18.0,
            "temp_max": 22.0,
            "pressure": 1013,
            "humidity": 60
        },
        "wind": {"speed": 3.2, "deg": 270},
        "clouds": {"all": 20},
        "dt": int(time.time())
    }

class FakeWeatherHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        parsed  = urlparse(self.path)
        params  = parse_qs(parsed.query)

        try:
            lat = float(params["lat"][0])
            lon = float(params["lon"][0])
        except (KeyError, ValueError, IndexError):
            self.send_response(400)
            self.end_headers()
            return

        body = json.dumps(make_weather_response(lat, lon)).encode()
        self.send_response(200)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, format, *args):
        pass  # wycisz logi podczas testów

if __name__ == "__main__":
    server = HTTPServer(("localhost", 8080), FakeWeatherHandler)
    print("Fake weather server running on port 8080")
    server.serve_forever()