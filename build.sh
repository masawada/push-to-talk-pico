#!/bin/bash
set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ -z "$PICO_SDK_PATH" ]; then
    export PICO_SDK_PATH="${SCRIPT_DIR}/pico-sdk"
fi

for BOARD in pico pico2; do
    echo "=== Building for ${BOARD} ==="
    cmake -B "${SCRIPT_DIR}/build/${BOARD}" -S "$SCRIPT_DIR" -DPICO_BOARD="$BOARD" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    cmake --build "${SCRIPT_DIR}/build/${BOARD}" -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)
done

ln -sf pico/compile_commands.json "${SCRIPT_DIR}/build/compile_commands.json"

echo "Output:"
echo "  Pico:   build/pico/push_to_talk_pico.uf2"
echo "  Pico 2: build/pico2/push_to_talk_pico.uf2"
