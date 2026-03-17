# =============================================================================
# Single-container Dockerfile — Nginx + Node.js + C++ renderer
# Optimized for small EC2 instances (t3a.micro: 1GB RAM, t3a.small: 2GB RAM)
# =============================================================================

# Stage 1: Build C++ binary (static link for alpine)
FROM alpine:3.19 AS cpp-builder

RUN apk add --no-cache build-base

WORKDIR /app
COPY src/render_api.cpp src/render_api.cpp
RUN g++ -std=c++17 -O3 -static -o fractal_api src/render_api.cpp

# Stage 2: Install Node.js dependencies
FROM node:20-alpine AS node-builder

WORKDIR /app/server
COPY server/package.json server/package-lock.json ./
RUN npm ci --omit=dev

# Stage 3: Production image
FROM node:20-alpine

RUN apk add --no-cache nginx libstdc++

# Copy C++ binary
COPY --from=cpp-builder /app/fractal_api /usr/local/bin/fractal_api

# Copy Node.js server
WORKDIR /app/server
COPY --from=node-builder /app/server/node_modules ./node_modules
COPY server/index.js .
COPY server/package.json .

# Copy web static files
COPY docs/ /var/www/fractal/

# Copy Nginx config
RUN mkdir -p /etc/nginx/http.d
COPY nginx/nginx.conf /etc/nginx/http.d/default.conf

ENV PORT=3000
ENV FRACTAL_BIN=/usr/local/bin/fractal_api
ENV NODE_ENV=production
ENV MAX_RENDERS=2

COPY docker-entrypoint.sh /docker-entrypoint.sh
RUN chmod +x /docker-entrypoint.sh

EXPOSE 80

CMD ["/docker-entrypoint.sh"]
