#!/usr/bin/env bash
set -euo pipefail

APP_NAME="Stellar Player"
APP_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${APP_DIR}/build"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

log()   { echo -e "${GREEN}[✓]${NC} $1"; }
warn()  { echo -e "${YELLOW}[!]${NC} $1"; }
error() { echo -e "${RED}[✗]${NC} $1"; }
info()  { echo -e "${CYAN}[i]${NC} $1"; }

detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO="$ID"
        DISTRO_LIKE="$ID_LIKE"
    elif [ -f /etc/debian_version ]; then
        DISTRO="debian"
    elif [ -f /etc/arch-release ]; then
        DISTRO="arch"
    elif [ -f /etc/fedora-release ]; then
        DISTRO="fedora"
    else
        DISTRO="unknown"
    fi
}

check_command() {
    command -v "$1" &>/dev/null
}

install_packages_debian() {
    info "Detected Debian/Ubuntu-based system"
    info "Updating package lists..."
    sudo apt-get update -qq || true

    local packages=(
        build-essential
        cmake
        pkg-config
        qt6-base-dev
        qt6-base-dev-tools
        qt6-svg-dev
        libmpv-dev
        libgl1-mesa-dev
        yt-dlp
    )

    info "Installing build dependencies..."
    sudo apt-get install -y "${packages[@]}"
}

install_packages_fedora() {
    info "Detected Fedora-based system"

    local packages=(
        gcc-c++
        cmake
        pkgconf-pkg-config
        qt6-qtbase-devel
        qt6-qtsvg-devel
        mpv-libs-devel
        libglvnd-devel
        yt-dlp
    )

    info "Installing build dependencies..."
    sudo dnf install -y "${packages[@]}"
}

install_packages_arch() {
    info "Detected Arch-based system"

    local packages=(
        base-devel
        cmake
        qt6-base
        qt6-svg
        mpv
        yt-dlp
    )

    info "Installing build dependencies..."
    sudo pacman -Sy --noconfirm "${packages[@]}"
}

install_packages_opensuse() {
    info "Detected openSUSE-based system"

    local packages=(
        gcc-c++
        cmake
        pkg-config
        qt6-base-devel
        qt6-svg-devel
        mpv-devel
        Mesa-libGL-devel
        yt-dlp
    )

    info "Installing build dependencies..."
    sudo zypper install -y "${packages[@]}"
}

build_app() {
    info "Configuring build with CMake..."
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"

    cmake "$APP_DIR" -DCMAKE_BUILD_TYPE=Release

    info "Building ${APP_NAME}..."
    cmake --build . -j"$(nproc)"

    log "Build complete!"
}

install_app() {
    info "Installing ${APP_NAME} to /usr/local/bin/..."
    sudo cmake --install "$BUILD_DIR" --prefix /usr/local 2>/dev/null || {
        sudo cp "$BUILD_DIR/StellarPlayer" /usr/local/bin/
    }

    local desktop_file="/usr/local/share/applications/StellarPlayer.desktop"
    if [ ! -f "$desktop_file" ]; then
        info "Creating desktop entry..."
        sudo mkdir -p /usr/local/share/applications
        sudo bash -c "cat > '$desktop_file'" <<EOF
[Desktop Entry]
Type=Application
Name=Stellar Player
Comment=A modern video player inspired by Deepin Movie
Exec=/usr/local/bin/StellarPlayer
Icon=${APP_DIR}/icons/app_icon.png
Categories=AudioVideo;Player;
Terminal=false
EOF
    fi

    log "${APP_NAME} installed successfully!"
    info "You can run it with: StellarPlayer"
}

# --- Main ---

echo ""
echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}  ${APP_NAME} - Dependency Installer${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

detect_distro

case "$DISTRO" in
    debian|ubuntu|linuxmint|neon|pop|elementary|zorin)
        install_packages_debian
        ;;
    fedora|rhel|centos)
        install_packages_fedora
        ;;
    arch|manjaro|endeavouros|artix|garuda)
        install_packages_arch
        ;;
    opensuse*|suse)
        install_packages_opensuse
        ;;
    *)
        error "Unsupported distribution: $DISTRO"
        error "Please install the required packages manually:"
        echo "  - cmake, g++, pkg-config"
        echo "  - Qt6 (Base, SVG, OpenGLWidgets)"
        echo "  - libmpv (development)"
        echo "  - libgl1-mesa-dev"
        echo "  - yt-dlp"
        exit 1
        ;;
esac

echo ""
build_app

echo ""
install_app

echo ""
log "All done! Enjoy ${APP_NAME}."
