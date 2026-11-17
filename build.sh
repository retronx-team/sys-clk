#!/bin/bash
set -e

ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DIST_DIR="$ROOT_DIR/dist"
CORES="$(nproc --all)"

if [[ -n "$1" ]]; then
    DIST_DIR="$1"
fi

# HOS 21.0.0+ changed the TLS ABI. Atmosphere's compatibility patch for homebrew
# built with older libnx no longer works on HOS 23.x, so every binary must be
# built with libnx >= v4.10.0, which embeds an "LNY2" marker in its crt0.
check_libnx_abi() {
    local elf="$1"
    if ! grep -qa "LNY2" "$elf"; then
        echo "[!] $elf was built with an outdated libnx (missing LNY2 marker)."
        echo "[!] It will crash on HOS 23.x. Update devkitPro: (dkp-)pacman -Syu libnx switch-tools"
        exit 1
    fi
    echo "libnx ABI OK: $(basename "$elf")"
}

echo "DIST_DIR: $DIST_DIR"
echo "CORES: $CORES"

echo "*** sysmodule ***"
TITLE_ID="$(grep -oP '"title_id":\s*"0x\K(\w+)' "$ROOT_DIR/sysmodule/perms.json")"

pushd "$ROOT_DIR/sysmodule"
make -j$CORES
popd > /dev/null
check_libnx_abi "$ROOT_DIR/sysmodule/out/sys-clk.elf"

mkdir -p "$DIST_DIR/atmosphere/contents/$TITLE_ID/flags"
cp -vf "$ROOT_DIR/sysmodule/out/sys-clk.nsp" "$DIST_DIR/atmosphere/contents/$TITLE_ID/exefs.nsp"
>"$DIST_DIR/atmosphere/contents/$TITLE_ID/flags/boot2.flag"

echo "*** manager ***"
pushd "$ROOT_DIR/manager"
make -j$CORES
popd > /dev/null
check_libnx_abi "$ROOT_DIR/manager/sys-clk-manager.elf"

mkdir -p "$DIST_DIR/switch"
cp -vf "$ROOT_DIR/manager/sys-clk-manager.nro" "$DIST_DIR/switch/sys-clk-manager.nro"

echo "*** overlay ***"
pushd "$ROOT_DIR/overlay"
make -j$CORES
popd > /dev/null
check_libnx_abi "$ROOT_DIR/overlay/out/sys-clk-overlay.elf"

mkdir -p "$DIST_DIR/switch/.overlays"
cp -vf "$ROOT_DIR/overlay/out/sys-clk-overlay.ovl" "$DIST_DIR/switch/.overlays/sys-clk-overlay.ovl"

echo "*** assets ***"
mkdir -p "$DIST_DIR/config/sys-clk"
cp -vf "$ROOT_DIR/config.ini.template" "$DIST_DIR/config/sys-clk/config.ini.template"
cp -vf "$ROOT_DIR/README.md" "$DIST_DIR/README.md"
