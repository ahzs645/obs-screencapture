#!/bin/bash
# fix-obs-data-files.sh - Fix OBS data files by creating proper framework bundle structure

echo "🔧 Fixing OBS data files structure..."

OBS_SOURCE="/Users/ahzs645/development/obs-studio"
INSTALL_PREFIX="/usr/local"

# Create the framework bundle structure
echo "📁 Creating OBS framework bundle structure..."
sudo mkdir -p "$INSTALL_PREFIX/lib/libobs.framework/Versions/A/Resources"
sudo mkdir -p "$INSTALL_PREFIX/lib/libobs.framework/Versions/A/Headers"

# Copy the library to the framework structure
echo "📚 Installing OBS library in framework structure..."
if [ -f "$INSTALL_PREFIX/lib/libobs.dylib" ]; then
    sudo cp "$INSTALL_PREFIX/lib/libobs.dylib" "$INSTALL_PREFIX/lib/libobs.framework/Versions/A/libobs"
    echo "✅ OBS library copied to framework"
else
    echo "❌ OBS library not found at $INSTALL_PREFIX/lib/libobs.dylib"
    exit 1
fi

# Copy all data files to the Resources directory
echo "📄 Copying OBS data files..."
if [ -d "$OBS_SOURCE/libobs/data" ]; then
    sudo cp -R "$OBS_SOURCE/libobs/data/"* "$INSTALL_PREFIX/lib/libobs.framework/Versions/A/Resources/"
    echo "✅ OBS data files copied"
else
    echo "❌ OBS data directory not found at $OBS_SOURCE/libobs/data"
    exit 1
fi

# Copy headers to the framework structure
echo "📋 Copying OBS headers..."
if [ -d "$INSTALL_PREFIX/include/obs" ]; then
    sudo cp -R "$INSTALL_PREFIX/include/obs/"* "$INSTALL_PREFIX/lib/libobs.framework/Versions/A/Headers/"
    echo "✅ OBS headers copied"
else
    echo "❌ OBS headers not found at $INSTALL_PREFIX/include/obs"
    exit 1
fi

# Create the framework Info.plist
echo "📋 Creating framework Info.plist..."
sudo tee "$INSTALL_PREFIX/lib/libobs.framework/Versions/A/Info.plist" > /dev/null << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>en</string>
    <key>CFBundleExecutable</key>
    <string>libobs</string>
    <key>CFBundleIdentifier</key>
    <string>com.obsproject.libobs</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>libobs</string>
    <key>CFBundlePackageType</key>
    <string>FMWK</string>
    <key>CFBundleShortVersionString</key>
    <string>31.1.0</string>
    <key>CFBundleVersion</key>
    <string>31.1.0</string>
    <key>CFBundleSignature</key>
    <string>????</string>
</dict>
</plist>
EOF

# Create symbolic links for the framework
echo "🔗 Creating framework symbolic links..."
sudo ln -sf "Versions/A/libobs" "$INSTALL_PREFIX/lib/libobs.framework/libobs"
sudo ln -sf "Versions/A/Headers" "$INSTALL_PREFIX/lib/libobs.framework/Headers"
sudo ln -sf "Versions/A/Resources" "$INSTALL_PREFIX/lib/libobs.framework/Resources"
sudo ln -sf "Versions/A/Info.plist" "$INSTALL_PREFIX/lib/libobs.framework/Info.plist"
sudo ln -sf "A" "$INSTALL_PREFIX/lib/libobs.framework/Versions/Current"

# Update the library's install name to point to the framework
echo "🔧 Updating library install name..."
sudo install_name_tool -id "@rpath/libobs.framework/Versions/A/libobs" "$INSTALL_PREFIX/lib/libobs.framework/Versions/A/libobs"

# Set proper permissions
echo "🔐 Setting permissions..."
sudo chmod 755 "$INSTALL_PREFIX/lib/libobs.framework/Versions/A/libobs"
sudo chmod -R 644 "$INSTALL_PREFIX/lib/libobs.framework/Versions/A/Resources/"*
sudo chmod -R 644 "$INSTALL_PREFIX/lib/libobs.framework/Versions/A/Headers/"*
sudo chmod 644 "$INSTALL_PREFIX/lib/libobs.framework/Versions/A/Info.plist"

# Verify the framework structure
echo "🔍 Verifying framework structure..."
if [ -f "$INSTALL_PREFIX/lib/libobs.framework/Versions/A/libobs" ]; then
    echo "✅ Framework library found"
else
    echo "❌ Framework library not found"
fi

if [ -f "$INSTALL_PREFIX/lib/libobs.framework/Versions/A/Resources/default.effect" ]; then
    echo "✅ Framework data files found"
else
    echo "❌ Framework data files not found"
fi

if [ -f "$INSTALL_PREFIX/lib/libobs.framework/Versions/A/Info.plist" ]; then
    echo "✅ Framework Info.plist found"
else
    echo "❌ Framework Info.plist not found"
fi

echo ""
echo "🎉 OBS framework bundle setup complete!"
echo ""
echo "📋 Next steps:"
echo "1. Clean previous build: rm -rf build/"
echo "2. Rebuild your project: npm run configure && npm run build"
echo "3. Test with full OBS integration: node test-screen-capture.js"
echo ""
echo "📁 Framework location: $INSTALL_PREFIX/lib/libobs.framework" 