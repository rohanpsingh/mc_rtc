#!/bin/bash
set -e

REPO=${1:-rohanpsingh/mc_rtc}
BRANCH=${2:-topic/docker}

# Install ROS Humble (pre-installed on GitHub Actions ubuntu-22.04 runner, not in our base image)
curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key \
    -o /usr/share/keyrings/ros-archive-keyring.gpg
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] \
    http://packages.ros.org/ros2/ubuntu jammy main" \
    > /etc/apt/sources.list.d/ros2.list
apt-get update -qq
apt-get install -y ros-humble-ros-base python3-colcon-common-extensions

# Clone superbuild
git clone --recurse-submodules https://github.com/mc-rtc/mc-rtc-superbuild /mc-rtc-superbuild
cd /mc-rtc-superbuild

# Bootstrap (installs cmake, pre-commit, etc.)
./utils/bootstrap-linux.sh
git config --global user.email "ci@local"
git config --global user.name "Local CI"

# Source ROS environment (mirrors the fix in build.yml)
source /opt/ros/humble/setup.bash
export PYTHONPATH

# Configure superbuild
mkdir -p /tmp/build-mc-rtc
cmake -S /mc-rtc-superbuild \
      -B /tmp/build-mc-rtc \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DINSTALL_DOCUMENTATION=OFF \
      "-DMC_RTC_SUPERBUILD_OVERRIDE_mc_rtc_GITHUB=${REPO}" \
      "-DMC_RTC_SUPERBUILD_OVERRIDE_mc_rtc_GIT_TAG=origin/${BRANCH}"

# Build
cmake --build /tmp/build-mc-rtc
