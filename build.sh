#!/usr/bin/env bash
set -euo pipefail

# build.sh - Configure and build hgdb plus its external dependency (libmigdb)
# Usage:
#   ./build.sh                # default build (Release-like flags specified in CMakeLists)
#   JOBS=8 ./build.sh         # override parallel jobs
#   CMAKE_OPTS="-DDEBUG=ON" ./build.sh
#   BUILD_DIR=out ./build.sh  # custom build directory name
#
# Environment variables:
#   BUILD_DIR   - build directory name (default: build)
#   JOBS        - parallel build jobs (default: nproc)
#   CMAKE_OPTS  - extra options forwarded to cmake configuration step
#   VERBOSE=1   - enable verbose make/ninja output
#
# The ExternalProject in CMake handles cloning & building libmigdb automatically.

SCRIPT_DIR=$(cd -- "$(dirname "${BASH_SOURCE[0]}")" && pwd)
BUILD_DIR_NAME=${BUILD_DIR:-build}
BUILD_DIR_PATH="${SCRIPT_DIR}/${BUILD_DIR_NAME}"
JOBS_COUNT=${JOBS:-$(command -v nproc >/dev/null 2>&1 && nproc || echo 4)}

mkdir -p "${BUILD_DIR_PATH}"

echo "[build.sh] Configuring (build dir: ${BUILD_DIR_PATH})" >&2
cmake -S "${SCRIPT_DIR}" -B "${BUILD_DIR_PATH}" ${CMAKE_OPTS:-}

echo "[build.sh] Building hgdb (and libmigdb_ext) with ${JOBS_COUNT} jobs" >&2
if [[ "${VERBOSE:-0}" == "1" ]]; then
  cmake --build "${BUILD_DIR_PATH}" --target hgdb -j"${JOBS_COUNT}" -- VERBOSE=1
else
  cmake --build "${BUILD_DIR_PATH}" --target hgdb -j"${JOBS_COUNT}"
fi

# Provide a quick success message and location of binary
if [[ -f "${BUILD_DIR_PATH}/hgdb" ]]; then
  echo "[build.sh] Build succeeded: ${BUILD_DIR_PATH}/hgdb" >&2
else
  echo "[build.sh] Build completed but hgdb executable not found (check for errors above)." >&2
fi
