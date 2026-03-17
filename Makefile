.PHONY: all build api server clean docker-up docker-down docker-prod dev test

# Default: build everything
all: build

# Build C++ binaries via CMake (CPU + API server)
build:
	@mkdir -p build
	@if command -v cmake >/dev/null 2>&1; then \
		cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j$$(nproc); \
	else \
		echo "cmake not found, building with g++ directly..."; \
		g++ -std=c++17 -O3 -o build/fractal_api src/render_api.cpp; \
		g++ -std=c++17 -O3 -o build/mandelbrot_cpu src/main.cpp src/render.cpp -Iinclude; \
	fi
	@echo "Build complete. Binaries in ./build/"

# Build only the API binary (works without cmake)
api:
	@mkdir -p build
	@if command -v cmake >/dev/null 2>&1; then \
		cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make fractal_api; \
	else \
		g++ -std=c++17 -O3 -o build/fractal_api src/render_api.cpp; \
	fi
	@echo "API binary built: ./build/fractal_api"

# Install server dependencies
server-deps:
	cd server && npm install

# Start local dev server (static files only)
dev:
	npx http-server ./docs -p 8080 -o

# Start API server locally (requires build first)
server: api server-deps
	FRACTAL_BIN=$(CURDIR)/build/fractal_api node server/index.js

# --- Docker: local build (for dev machines with enough RAM) ---
docker-up:
	docker compose up -d --build

docker-down:
	docker compose down

docker-logs:
	docker compose logs -f

# --- Docker: pre-built images (for EC2 t3a.micro/small) ---
docker-prod:
	docker compose -f docker-compose.prod.yml pull
	docker compose -f docker-compose.prod.yml up -d

docker-prod-down:
	docker compose -f docker-compose.prod.yml down

docker-prod-logs:
	docker compose -f docker-compose.prod.yml logs -f

# Quick test: render a sample image
test: api
	./build/fractal_api --fractal mandelbrot --width 800 --height 600 --iter 1000 > /tmp/fractal_test.ppm
	@echo "Test image saved to /tmp/fractal_test.ppm"
	@file /tmp/fractal_test.ppm

# Clean build artifacts
clean:
	rm -rf build/
	rm -f /tmp/fractal_test.ppm

help:
	@echo "Fractal Renderer"
	@echo ""
	@echo "Development:"
	@echo "  make build         Build all C++ binaries"
	@echo "  make api           Build API server binary only"
	@echo "  make dev           Start local static file server"
	@echo "  make server        Start API server locally (builds C++ first)"
	@echo "  make test          Render a test image"
	@echo "  make clean         Remove build artifacts"
	@echo ""
	@echo "Docker (local build — needs 2+ GB RAM):"
	@echo "  make docker-up     Build images + start containers"
	@echo "  make docker-down   Stop containers"
	@echo ""
	@echo "Docker (pre-built — for t3a.micro/small EC2):"
	@echo "  make docker-prod       Pull images + start containers"
	@echo "  make docker-prod-down  Stop containers"
	@echo "  make docker-prod-logs  Tail container logs"
	@echo ""
	@echo "One-line EC2 deploy:"
	@echo "  bash deploy.sh"
