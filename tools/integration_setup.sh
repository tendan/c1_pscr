#!/bin/bash

PROJECT_DIR="${2:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)}"
MOSQUITTO_CONF="$PROJECT_DIR/tools/mosquitto_test.conf"
FAKE_SERVER="$PROJECT_DIR/tools/fake_server/server.py"
MOSQUITTO_PID_FILE="/tmp/test_mosquitto.pid"
FAKE_SERVER_PID_FILE="/tmp/test_fake_server.pid"

wait_for_port() {
    local port=$1
    local retries=20
    local i=0
    while [ $i -lt $retries ]; do
        if nc -z localhost "$port" 2>/dev/null; then
            return 0
        fi
        sleep 0.3
        i=$((i + 1))
    done
    return 1
}

start() {
    echo "PROJECT_DIR=$PROJECT_DIR"         >> /tmp/integration_debug.log
    echo "MOSQUITTO_CONF=$MOSQUITTO_CONF"   >> /tmp/integration_debug.log
    echo "FAKE_SERVER=$FAKE_SERVER"         >> /tmp/integration_debug.log

    # Zatrzymaj poprzednie instancje
    if [ -f "$MOSQUITTO_PID_FILE" ]; then
        kill "$(cat $MOSQUITTO_PID_FILE)" 2>/dev/null
        rm -f "$MOSQUITTO_PID_FILE"
    fi
    if [ -f "$FAKE_SERVER_PID_FILE" ]; then
        kill "$(cat $FAKE_SERVER_PID_FILE)" 2>/dev/null
        rm -f "$FAKE_SERVER_PID_FILE"
    fi

    echo "Starting test Mosquitto..."
    setsid mosquitto -c "$MOSQUITTO_CONF" \
        > /tmp/mosquitto_test.log 2>&1 &
    MOSQ_PID=$!
    echo $MOSQ_PID > "$MOSQUITTO_PID_FILE"

    echo "Waiting for Mosquitto on port 1884..."
    if ! wait_for_port 1884; then
        echo "ERROR: Mosquitto failed to start"
        exit 1
    fi
    echo "Mosquitto ready"

    echo "Starting fake HTTP server..."
    setsid python3 "$FAKE_SERVER" \
        > /tmp/fake_server.log 2>&1 &
    FAKE_PID=$!
    echo $FAKE_PID > "$FAKE_SERVER_PID_FILE"

    echo "Waiting for fake server on port 8080..."
    if ! wait_for_port 8080; then
        echo "ERROR: Fake server failed to start"
        kill "$MOSQ_PID" 2>/dev/null
        exit 1
    fi
    echo "Fake server ready"

    echo "Infrastructure ready"
    exit 0
}

stop() {
    echo "Stopping fake HTTP server..."
    if [ -f "$FAKE_SERVER_PID_FILE" ]; then
        kill "$(cat $FAKE_SERVER_PID_FILE)" 2>/dev/null
        rm -f "$FAKE_SERVER_PID_FILE"
    fi

    echo "Stopping Mosquitto..."
    if [ -f "$MOSQUITTO_PID_FILE" ]; then
        kill "$(cat $MOSQUITTO_PID_FILE)" 2>/dev/null
        rm -f "$MOSQUITTO_PID_FILE"
    fi

    echo "Infrastructure stopped"
    exit 0
}

case "$1" in
    start) start ;;
    stop)  stop  ;;
    *)
        echo "Usage: $0 {start|stop} [project_dir]"
        exit 1
    ;;
esac