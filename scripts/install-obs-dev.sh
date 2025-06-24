#!/bin/bash
# install-obs-dev.sh - Install OBS development files from built source

echo "🔧 Installing OBS development files..."

OBS_SOURCE="/Users/ahzs645/development/obs-studio"
OBS_BUILD="$OBS_SOURCE/build"
INSTALL_PREFIX="/usr/local"

# Create necessary directories
echo "📁 Creating installation directories..."
sudo mkdir -p "$INSTALL_PREFIX/lib"
sudo mkdir -p "$INSTALL_PREFIX/include/obs"

# Install the main OBS library
echo "📚 Installing OBS library..."
if [ -f "$OBS_BUILD/libobs/libobs.framework/Versions/A/libobs" ]; then
    sudo cp "$OBS_BUILD/libobs/libobs.framework/Versions/A/libobs" "$INSTALL_PREFIX/lib/libobs.dylib"
    echo "✅ OBS library installed"
else
    echo "❌ OBS library not found at expected location"
    exit 1
fi

# Install all OBS headers recursively
echo "📄 Installing OBS headers..."
sudo cp -R "$OBS_SOURCE/libobs/"*.h "$INSTALL_PREFIX/include/obs/"

# Install utility headers
echo "🔧 Installing utility headers..."
sudo mkdir -p "$INSTALL_PREFIX/include/obs/util"
sudo cp -R "$OBS_SOURCE/libobs/util/"*.h "$INSTALL_PREFIX/include/obs/util/"

# Install graphics headers
echo "🎨 Installing graphics headers..."
sudo mkdir -p "$INSTALL_PREFIX/include/obs/graphics"
sudo cp -R "$OBS_SOURCE/libobs/graphics/"*.h "$INSTALL_PREFIX/include/obs/graphics/"

# Install callback headers
echo "📞 Installing callback headers..."
sudo mkdir -p "$INSTALL_PREFIX/include/obs/callback"
sudo cp -R "$OBS_SOURCE/libobs/callback/"*.h "$INSTALL_PREFIX/include/obs/callback/"

# Install media-io headers
echo "🎵 Installing media-io headers..."
sudo mkdir -p "$INSTALL_PREFIX/include/obs/media-io"
sudo cp -R "$OBS_SOURCE/libobs/media-io/"*.h "$INSTALL_PREFIX/include/obs/media-io/"

# Install SIMDE headers from dependencies
echo "🚀 Installing SIMDE headers..."
if [ -d "$OBS_SOURCE/.deps/obs-deps-2025-05-23-universal/include/simde" ]; then
    sudo cp -R "$OBS_SOURCE/.deps/obs-deps-2025-05-23-universal/include/simde" "$INSTALL_PREFIX/include/obs/"
    echo "✅ SIMDE headers installed"
else
    echo "⚠️  SIMDE headers not found, trying alternative location..."
    if [ -d "$OBS_SOURCE/libobs/util/simde" ]; then
        sudo cp -R "$OBS_SOURCE/libobs/util/simde" "$INSTALL_PREFIX/include/obs/"
        echo "✅ SIMDE headers installed from util directory"
    else
        echo "❌ SIMDE headers not found"
    fi
fi

# Install additional dependencies from obs-deps
echo "📦 Installing additional dependencies..."
DEPS_DIR="$OBS_SOURCE/.deps/obs-deps-2025-05-23-universal"
if [ -d "$DEPS_DIR" ]; then
    # Copy additional libraries
    if [ -d "$DEPS_DIR/lib" ]; then
        sudo cp "$DEPS_DIR/lib/"*.dylib "$INSTALL_PREFIX/lib/" 2>/dev/null || true
        sudo cp "$DEPS_DIR/lib/"*.a "$INSTALL_PREFIX/lib/" 2>/dev/null || true
    fi
    
    # Copy additional headers
    if [ -d "$DEPS_DIR/include" ]; then
        for header_dir in "$DEPS_DIR/include"/*; do
            if [ -d "$header_dir" ]; then
                dir_name=$(basename "$header_dir")
                if [ "$dir_name" != "simde" ]; then  # Already handled above
                    sudo mkdir -p "$INSTALL_PREFIX/include/$dir_name"
                    sudo cp -R "$header_dir/"* "$INSTALL_PREFIX/include/$dir_name/" 2>/dev/null || true
                fi
            fi
        done
    fi
    echo "✅ Additional dependencies installed"
fi

# Set proper permissions
echo "🔐 Setting permissions..."
sudo chmod 755 "$INSTALL_PREFIX/lib/libobs.dylib"
sudo chmod -R 644 "$INSTALL_PREFIX/include/obs/"*.h
sudo chmod -R 644 "$INSTALL_PREFIX/include/obs/"*/*.h

# Create pkg-config file for easier linking
echo "📋 Creating pkg-config file..."
sudo tee "$INSTALL_PREFIX/lib/pkgconfig/libobs.pc" > /dev/null << EOF
prefix=$INSTALL_PREFIX
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib
includedir=\${prefix}/include

Name: libobs
Description: OBS Studio core library
Version: 31.1.0
Libs: -L\${libdir} -lobs
Cflags: -I\${includedir}
EOF

echo "✅ pkg-config file created"

# Verify installation
echo "🔍 Verifying installation..."
if [ -f "$INSTALL_PREFIX/lib/libobs.dylib" ]; then
    echo "✅ OBS library found: $INSTALL_PREFIX/lib/libobs.dylib"
else
    echo "❌ OBS library not found"
fi

if [ -f "$INSTALL_PREFIX/include/obs/obs.h" ]; then
    echo "✅ OBS headers found: $INSTALL_PREFIX/include/obs/obs.h"
else
    echo "❌ OBS headers not found"
fi

if [ -f "$INSTALL_PREFIX/include/obs/util/c99defs.h" ]; then
    echo "✅ Utility headers found: $INSTALL_PREFIX/include/obs/util/c99defs.h"
else
    echo "❌ Utility headers not found"
fi

if [ -f "$INSTALL_PREFIX/include/obs/graphics/graphics.h" ]; then
    echo "✅ Graphics headers found: $INSTALL_PREFIX/include/obs/graphics/graphics.h"
else
    echo "❌ Graphics headers not found"
fi

echo ""
echo "🎉 OBS development environment installation complete!"
echo ""
echo "📋 Next steps:"
echo "1. Clean previous build: rm -rf build/"
echo "2. Rebuild your project: npm run configure && npm run build"
echo "3. Test with full OBS integration: node test-screen-capture.js"
echo ""
echo "📁 Installation locations:"
echo "   Library: $INSTALL_PREFIX/lib/libobs.dylib"
echo "   Headers: $INSTALL_PREFIX/include/obs/"
echo "   pkg-config: $INSTALL_PREFIX/lib/pkgconfig/libobs.pc" 