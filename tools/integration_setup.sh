#!/bin/bash

MOSQUITTO_CONF="tools/mosquitto_test.conf"
FAKE_SERVER="tools/fake_server/server.py"
MOSQUITTO_PID_FILE="/tmp/test_mosquitto.pid"
FAKE_SERVER_PID_FILE="/tmp/test_fake_server.pid"

start() {
    echo "Starting test Mosquitto..."
    mosquitto -c "$MOSQUITTO_CONF" -d \
        --pid-file "$MOSQUITTO_PID_FILE"
    sleep 0.5

    echo "Starting fake HTTP server..."
    python3 "$FAKE_SERVER" &
    echo $! > "$FAKE_SERVER_PID_FILE"
    sleep 0.5

    echo "Infrastructure ready"
}

stop() {
    echo "Stopping fake HTTP server..."
    if [ -f "$MOSQUITTO_PID_FILE" ]; then
        kill "$(cat $MOSQUITTO_PID_FILE)" 2>/dev/null
        rm "$MOSQUITTO_PID_FILE"
    fi

    echo "Stopping Mosquitto..."
    if [ -f "$FAKE_SERVER_PID_FILE" ]; then
        kill "$(cat $FAKE_SERVER_PID_FILE)" 2>/dev/null
        rm "$FAKE_SERVER_PID_FILE"
    fi

    echo "Infrastructure stopped"
}

case "$1" in
    start) start ;;
    stop)  stop  ;;
    *)     echo "Usage: $0 {start|stop}" ;;
esac