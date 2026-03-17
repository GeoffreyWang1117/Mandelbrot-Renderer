#!/bin/bash
# =============================================================================
# Fractal Renderer — EC2 deploy script
# Works on t3a.micro (1GB RAM) and above
#
# Usage:
#   curl -fsSL https://raw.githubusercontent.com/GeoffreyWang1117/Mandelbrot-Renderer/main/deploy.sh | bash
#
# Or clone and run:
#   git clone https://github.com/GeoffreyWang1117/Mandelbrot-Renderer.git
#   cd Mandelbrot-Renderer && bash deploy.sh
# =============================================================================

set -e

REPO="GeoffreyWang1117/Mandelbrot-Renderer"
GHCR_OWNER="geoffreywang1117"

echo "=== Fractal Renderer — EC2 Deployment ==="
echo ""

# 1. Install Docker if missing
if ! command -v docker &>/dev/null; then
    echo "[1/4] Installing Docker..."
    curl -fsSL https://get.docker.com | sh
    sudo usermod -aG docker "$USER"
    echo "Docker installed. You may need to log out and back in for group changes."
else
    echo "[1/4] Docker already installed."
fi

# 2. Install docker compose plugin if missing
if ! docker compose version &>/dev/null 2>&1; then
    echo "[2/4] Installing Docker Compose plugin..."
    sudo apt-get update -qq && sudo apt-get install -y -qq docker-compose-plugin 2>/dev/null || \
    sudo yum install -y docker-compose-plugin 2>/dev/null || \
    echo "Please install docker-compose-plugin manually."
else
    echo "[2/4] Docker Compose already installed."
fi

# 3. Get compose file
echo "[3/4] Fetching production compose file..."
if [ ! -f docker-compose.prod.yml ]; then
    curl -fsSL "https://raw.githubusercontent.com/${REPO}/main/docker-compose.prod.yml" -o docker-compose.prod.yml
fi

# 4. Pull and start
echo "[4/4] Pulling images and starting services..."
docker compose -f docker-compose.prod.yml pull
docker compose -f docker-compose.prod.yml up -d

echo ""
echo "=== Deployment complete! ==="
echo ""
echo "Services:"
docker compose -f docker-compose.prod.yml ps --format "table {{.Name}}\t{{.Status}}\t{{.Ports}}"
echo ""

# Get public IP
PUBLIC_IP=$(curl -s --connect-timeout 3 http://169.254.169.254/latest/meta-data/public-ipv4 2>/dev/null || \
            curl -s --connect-timeout 3 https://ifconfig.me 2>/dev/null || \
            echo "YOUR_EC2_IP")

echo "Access your Fractal Renderer:"
echo "  Web UI:  http://${PUBLIC_IP}/"
echo "  API:     http://${PUBLIC_IP}/api"
echo ""
echo "Useful commands:"
echo "  docker compose -f docker-compose.prod.yml logs -f    # View logs"
echo "  docker compose -f docker-compose.prod.yml down       # Stop"
echo "  docker compose -f docker-compose.prod.yml pull       # Update images"
echo "  docker stats                                          # Monitor resources"
