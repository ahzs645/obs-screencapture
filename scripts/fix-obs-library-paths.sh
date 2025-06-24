#!/bin/bash
# fix-obs-library-paths.sh - Fix all OBS library paths to use the framework consistently

echo "🔧 Fixing OBS library paths..."

INSTALL_PREFIX="/usr/local"
FRAMEWORK_PATH="$INSTALL_PREFIX/lib/libobs.framework"

# Check if framework exists
if [ ! -d "$FRAMEWORK_PATH" ]; then
    echo "❌ OBS framework not found at $FRAMEWORK_PATH"
    echo "   Please run scripts/fix-obs-data-files.sh first"
    exit 1
fi

echo "📁 Found OBS framework at: $FRAMEWORK_PATH"

# List of libraries that might need path fixing
OBS_LIBRARIES=(
    "libobs-opengl.dylib"
    "libobs-ffmpeg.dylib"
    "libobs-util.dylib"
    "libobs-graphics.dylib"
    "libobs-audio.dylib"
    "libobs-video.dylib"
)

# Fix paths for each library
for lib in "${OBS_LIBRARIES[@]}"; do
    lib_path="$INSTALL_PREFIX/lib/$lib"
    
    if [ -f "$lib_path" ]; then
        echo "🔧 Fixing paths for $lib..."
        
        # Get current dependencies
        dependencies=$(otool -L "$lib_path" | grep "libobs" | awk '{print $1}' | grep -v "$lib")
        
        for dep in $dependencies; do
            # Extract just the library name without path
            lib_name=$(basename "$dep")
            
            # Skip if it's the library itself
            if [ "$lib_name" = "$lib" ]; then
                continue
            fi
            
            # If it's a libobs library, update the path
            if [[ "$lib_name" == libobs* ]]; then
                new_path="$FRAMEWORK_PATH/Versions/A/$lib_name"
                if [ -f "$new_path" ]; then
                    echo "   Updating $dep -> $new_path"
                    sudo install_name_tool -change "$dep" "$new_path" "$lib_path"
                else
                    echo "   ⚠️  Target library not found: $new_path"
                fi
            fi
        done
        
        # Also fix the library's own install name if it's a libobs library
        if [[ "$lib" == libobs* ]]; then
            echo "   Setting install name for $lib"
            sudo install_name_tool -id "$FRAMEWORK_PATH/Versions/A/$lib" "$lib_path"
        fi
        
        echo "   ✅ Fixed $lib"
    else
        echo "   ⚠️  Library not found: $lib_path"
    fi
done

# Also fix any plugins that might reference libobs
echo "🔧 Checking for plugins that need path fixing..."
PLUGIN_DIRS=(
    "/usr/local/lib/obs-plugins"
    "/opt/homebrew/lib/obs-plugins"
)

for plugin_dir in "${PLUGIN_DIRS[@]}"; do
    if [ -d "$plugin_dir" ]; then
        echo "📁 Checking plugins in: $plugin_dir"
        for plugin in "$plugin_dir"/*.dylib; do
            if [ -f "$plugin" ]; then
                plugin_name=$(basename "$plugin")
                echo "   Checking $plugin_name..."
                
                # Check if this plugin depends on libobs
                if otool -L "$plugin" | grep -q "libobs"; then
                    echo "   🔧 Fixing libobs references in $plugin_name"
                    
                    # Fix any libobs references
                    dependencies=$(otool -L "$plugin" | grep "libobs" | awk '{print $1}')
                    for dep in $dependencies; do
                        lib_name=$(basename "$dep")
                        if [[ "$lib_name" == libobs* ]]; then
                            new_path="$FRAMEWORK_PATH/Versions/A/$lib_name"
                            if [ -f "$new_path" ]; then
                                echo "     Updating $dep -> $new_path"
                                sudo install_name_tool -change "$dep" "$new_path" "$plugin"
                            fi
                        fi
                    done
                fi
            fi
        done
    fi
done

# Verify the framework library itself
echo "🔍 Verifying framework library..."
if [ -f "$FRAMEWORK_PATH/Versions/A/libobs" ]; then
    echo "✅ Framework library found"
    
    # Check its install name
    install_name=$(otool -D "$FRAMEWORK_PATH/Versions/A/libobs" | tail -n +2)
    if [ "$install_name" = "$FRAMEWORK_PATH/Versions/A/libobs" ]; then
        echo "✅ Framework library install name is correct"
    else
        echo "🔧 Fixing framework library install name..."
        sudo install_name_tool -id "$FRAMEWORK_PATH/Versions/A/libobs" "$FRAMEWORK_PATH/Versions/A/libobs"
    fi
else
    echo "❌ Framework library not found"
fi

# Test a specific library to make sure paths are correct
echo "🧪 Testing library paths..."
if [ -f "$INSTALL_PREFIX/lib/libobs-opengl.dylib" ]; then
    echo "Testing libobs-opengl.dylib dependencies:"
    otool -L "$INSTALL_PREFIX/lib/libobs-opengl.dylib" | grep "libobs" || echo "   No libobs dependencies found"
fi

echo ""
echo "🎉 OBS library path fixing complete!"
echo ""
echo "📋 Next steps:"
echo "1. Clean previous build: rm -rf build/"
echo "2. Rebuild your project: npm run configure && npm run build"
echo "3. Test with full OBS integration: node test-screen-capture.js" 