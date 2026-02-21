FROM ubuntu:22.04 AS base

ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=en_US.UTF-8

# Setup locales and add mc-rtc cloudsmith repository
RUN apt-get update && apt-get install -y \
    locales curl gnupg lsb-release ca-certificates apt-transport-https && \
    locale-gen en_US en_US.UTF-8 && \
    update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8 && \
    curl -1sLf 'https://dl.cloudsmith.io/public/mc-rtc/stable/setup.deb.sh' | bash && \
    rm -rf /var/lib/apt/lists/*

# Install all dependencies in one layer (heavy but cached)
# libmc-rtc-dev transitively installs all JRL libraries (Tasks, TVM, RBDyn, etc.)
# mc-rtc-data provides robot/environment descriptions (JVRC1, etc.)
RUN apt-get update && apt-get install -y \
    # Build tools
    build-essential cmake git \
    # System dependencies not covered transitively
    libeigen3-dev libboost-all-dev libtinyxml2-dev \
    # All JRL deps via libmc-rtc-dev + robot descriptions
    # libgeos++-inline-dev provides missing .inl files for Ubuntu 22.04 GEOS
    libmc-rtc-dev mc-rtc-data libmesh-sampling-dev libgeos++-inline-dev \
    && rm -rf /var/lib/apt/lists/*

# Build stage - uses local source code
FROM base AS builder

WORKDIR /workspace/mc_rtc

# Copy source code (uses .dockerignore to exclude build artifacts)
COPY . .

# Build mc_rtc
RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DPYTHON_BINDING=OFF && \
    make -j$(nproc) && \
    make install

# Final stage
FROM base AS runtime

# Copy built artifacts from builder
COPY --from=builder /usr/local /usr/local

# Copy entrypoint
COPY docker/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

WORKDIR /workspace

ENV DEBIAN_FRONTEND=

ENTRYPOINT ["/entrypoint.sh"]
CMD ["/bin/bash"]
