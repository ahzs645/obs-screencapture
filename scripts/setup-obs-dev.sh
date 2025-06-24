#!/bin/bash
# setup-obs-dev.sh - Install OBS development environment on macOS

echo "🔧 Setting up OBS development environment for macOS..."

# Check if we have Homebrew
if ! command -v brew &> /dev/null; then
    echo "❌ Homebrew not found. Please install Homebrew first:"
    echo "   /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
    exit 1
fi

echo "✅ Homebrew found"

# Install required development dependencies
echo "📦 Installing OBS development dependencies..."

# Install core dependencies
brew install cmake ninja pkg-config

# Install FFmpeg and codecs
brew install ffmpeg x264 

# Install Qt (required for OBS)
brew install qt@5

# Install other required libraries
brew install mbedtls swig luajit

echo "✅ Dependencies installed"

# Clone and build OBS Studio from source
echo "🏗️ Building OBS Studio from source..."

# Create development directory
DEV_DIR="$HOME/development"
mkdir -p "$DEV_DIR"
cd "$DEV_DIR"

# Remove existing OBS directory if it exists
if [ -d "obs-studio" ]; then
    echo "🧹 Removing existing OBS directory..."
    rm -rf obs-studio
fi

# Clone OBS Studio
echo "📥 Cloning OBS Studio..."
git clone --recursive https://github.com/obsproject/obs-studio.git
cd obs-studio

# Create build directory
mkdir -p build
cd build

# Configure with Qt5 from Homebrew
echo "⚙️ Configuring OBS build..."
export CMAKE_PREFIX_PATH="/opt/homebrew/opt/qt@5:$CMAKE_PREFIX_PATH"

cmake .. \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_BROWSER=OFF \
    -DENABLE_WEBSOCKET=OFF \
    -DENABLE_UI=OFF \
    -DENABLE_SCRIPTING=OFF \
    -DENABLE_VST=OFF \
    -DENABLE_VIRTUALCAM=OFF \
    -DENABLE_MAC_VIRTUALCAM=OFF \
    -DENABLE_WEBRTC=OFF \
    -DENABLE_RTMP=OFF \
    -DENABLE_SRT=OFF \
    -DENABLE_RIST=OFF \
    -DENABLE_FFMPEG=ON \
    -DENABLE_X264=ON \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DQT_VERSION=5

# Build OBS
echo "🔨 Building OBS Studio..."
make -j$(sysctl -n hw.ncpu)

# Install OBS (this installs the libraries and headers)
echo "📦 Installing OBS libraries and headers..."
sudo make install

echo "✅ OBS Studio built and installed successfully!"

# Verify installation
echo "🔍 Verifying OBS installation..."

if [ -f "/usr/local/include/obs/obs.h" ]; then
    echo "✅ OBS headers found at /usr/local/include/obs/"
else
    echo "❌ OBS headers not found"
fi

if [ -f "/usr/local/lib/libobs.dylib" ]; then
    echo "✅ OBS library found at /usr/local/lib/libobs.dylib"
else
    echo "❌ OBS library not found"
fi

echo ""
echo "🎉 OBS development environment setup complete!"
echo ""
echo "📋 Next steps:"
echo "1. Rebuild your Node.js addon: npm run rebuild"
echo "2. Test with full OBS integration: npm test"
echo ""
echo "📁 OBS source code location: $DEV_DIR/obs-studio"
echo "📚 Installed libraries: /usr/local/lib/libobs.dylib"
echo "📄 Installed headers: /usr/local/include/obs/"