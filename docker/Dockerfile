FROM ubuntu:22.04

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
# libgeos++-inline-dev provides missing .inl files for Ubuntu 22.04 GEOS
RUN apt-get update && apt-get install -y \
    build-essential cmake ninja-build ccache git \
    libeigen3-dev libboost-all-dev libtinyxml2-dev \
    libmc-rtc-dev mc-rtc-data libmesh-sampling-dev libgeos++-inline-dev \
    && rm -rf /var/lib/apt/lists/*

ENV DEBIAN_FRONTEND=
ENV CMAKE_C_COMPILER_LAUNCHER=ccache
ENV CMAKE_CXX_COMPILER_LAUNCHER=ccache

COPY docker/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

WORKDIR /workspace/mc_rtc

ENTRYPOINT ["/entrypoint.sh"]
CMD ["/bin/bash"]
