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

# Install mc_rtc build dependencies
# JRL libraries from cloudsmith, system libs from Ubuntu
# libgeos++-inline-dev provides missing .inl files for Ubuntu 22.04 GEOS
# mc-rtc-data provides robot/environment descriptions (JVRC1, etc.)
RUN apt-get update && apt-get install -y \
    build-essential cmake ninja-build ccache git \
    libeigen3-dev libboost-all-dev libtinyxml2-dev \
    libyaml-cpp-dev libspdlog-dev libnanomsg-dev libltdl-dev libnotify-dev \
    libtasks-dev libtvm-dev libeigen-quadprog-dev libstate-observation-dev \
    libndcurves-dev libmesh-sampling-dev libgeos++-inline-dev \
    mc-rtc-data \
    && rm -rf /var/lib/apt/lists/*

# Build stage - uses local source code
FROM base AS builder

WORKDIR /workspace/mc_rtc

COPY . .

RUN cmake -S . -B build -G Ninja \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DCMAKE_C_COMPILER_LAUNCHER=ccache \
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
        -DPYTHON_BINDING=OFF && \
    cmake --build build && \
    cmake --install build

# Runtime stage
FROM base AS runtime

COPY --from=builder /usr/local /usr/local

COPY docker/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

WORKDIR /workspace/mc_rtc

ENV DEBIAN_FRONTEND=
ENV CMAKE_C_COMPILER_LAUNCHER=ccache
ENV CMAKE_CXX_COMPILER_LAUNCHER=ccache

ENTRYPOINT ["/entrypoint.sh"]
CMD ["/bin/bash"]
