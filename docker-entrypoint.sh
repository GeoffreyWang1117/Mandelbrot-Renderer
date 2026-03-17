#!/bin/bash
set -e

echo "Starting Fractal Renderer..."

# Start Nginx in background
nginx -g "daemon off;" &
NGINX_PID=$!
echo "Nginx started (PID: $NGINX_PID)"

# Start Node.js API server
cd /app/server
node index.js &
NODE_PID=$!
echo "API server started (PID: $NODE_PID)"

# Handle shutdown
trap "kill $NGINX_PID $NODE_PID; exit 0" SIGTERM SIGINT

# Wait for either process to exit
wait -n $NGINX_PID $NODE_PID
EXIT_CODE=$?

# If one died, kill the other
kill $NGINX_PID $NODE_PID 2>/dev/null || true
exit $EXIT_CODE
