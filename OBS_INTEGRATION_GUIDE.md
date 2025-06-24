# OBS Integration Guide

This guide explains how OBS Studio's capture engine is integrated into this Node.js addon and how to deploy it in production.

## 🏗️ Architecture Overview

### How It Works

This project embeds OBS Studio's core libraries and plugins directly into your Node.js application:

```
Your Node.js App
├── obs_screen_capture.node (Native Addon)
├── OBS Core Libraries
│   ├── libobs.so/libobs.dylib/libobs.dll
│   ├── libobs-ffmpeg.so/libobs-ffmpeg.dylib/libobs-ffmpeg.dll
│   └── libobs-util.so/libobs-util.dylib/libobs-util.dll
├── OBS Capture Plugins
│   ├── mac-capture.so (ScreenCaptureKit)
│   ├── win-capture.dll (Graphics Capture API)
│   ├── linux-capture.so (X11/Wayland)
│   └── Audio capture plugins
└── OBS Encoding Plugins
    ├── obs-x264.so/obs-x264.dll
    └── obs-ffmpeg.so/obs-ffmpeg.dll
```

### Key Benefits

1. **No External Dependencies**: End users don't need OBS Studio installed
2. **Professional Quality**: Uses OBS's battle-tested capture and encoding
3. **Cross-Platform**: Same code works on macOS, Windows, and Linux
4. **Hardware Acceleration**: Supports GPU encoding (NVENC, QuickSync, AMF)
5. **Multiple Codecs**: H.264, H.265, VP9, AV1 support

## 🔧 Development Setup

### Prerequisites

#### macOS
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew (if not already installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install OBS Studio for development
brew install obs-studio

# Install CMake
brew install cmake
```

#### Windows
```powershell
# Install Visual Studio Build Tools 2019 or later
# Download from: https://visualstudio.microsoft.com/downloads/

# Install CMake
# Download from: https://cmake.org/download/

# Install OBS Studio
# Download from: https://obsproject.com/download
# Make sure to install development headers
```

#### Linux (Ubuntu/Debian)
```bash
# Install build dependencies
sudo apt update
sudo apt install build-essential cmake ninja-build

# Install OBS development packages
sudo apt install libobs-dev libobs-ffmpeg-dev

# Install X11 development
sudo apt install libx11-dev libxrandr-dev

# Install audio development
sudo apt install libpulse-dev
```

### Building with OBS Integration

1. **Configure CMake with OBS**:
```bash
# OBS will be automatically detected if installed
cmake -B build -DUSE_SYSTEM_OBS=ON

# Or specify custom OBS paths
cmake -B build \
  -DOBS_INCLUDE_DIR=/path/to/obs/include \
  -DOBS_LIBRARY=/path/to/obs/lib/libobs.so
```

2. **Build the project**:
```bash
cmake --build build --parallel
```

3. **Test the integration**:
```bash
npm test
```

## 📦 Production Deployment

### Strategy 1: Bundle Pre-built OBS Libraries

This approach downloads and bundles OBS binaries for each platform.

#### Step 1: Download OBS Binaries

```bash
# Create a script to download OBS for each platform
mkdir -p obs-binaries/{macos,windows,linux}

# macOS
curl -L "https://github.com/obsproject/obs-studio/releases/download/30.0.2/obs-studio-30.0.2-macos-x86_64.dmg" -o obs-binaries/macos/obs-studio.dmg

# Windows
curl -L "https://github.com/obsproject/obs-studio/releases/download/30.0.2/obs-studio-30.0.2-windows-x64.exe" -o obs-binaries/windows/obs-studio.exe

# Linux (example for Ubuntu)
curl -L "https://github.com/obsproject/obs-studio/releases/download/30.0.2/obs-studio-30.0.2-linux-x86_64.tar.gz" -o obs-binaries/linux/obs-studio.tar.gz
```

#### Step 2: Extract Required Libraries

```bash
# Extract only the libraries and plugins needed
# This reduces the bundle size significantly

# macOS
hdiutil attach obs-binaries/macos/obs-studio.dmg
cp -r "/Volumes/OBS Studio/OBS Studio.app/Contents/Frameworks/" obs-binaries/macos/
cp -r "/Volumes/OBS Studio/OBS Studio.app/Contents/PlugIns/" obs-binaries/macos/
hdiutil detach "/Volumes/OBS Studio"

# Windows
# Extract using 7-Zip or similar
# Copy only the required DLLs and plugins

# Linux
tar -xzf obs-binaries/linux/obs-studio.tar.gz
cp -r obs-studio-30.0.2-linux-x86_64/lib/* obs-binaries/linux/
```

#### Step 3: Update CMakeLists.txt

```cmake
# Add bundled OBS libraries
if(APPLE)
    set(OBS_BUNDLED_DIR "${CMAKE_SOURCE_DIR}/obs-binaries/macos")
elseif(WIN32)
    set(OBS_BUNDLED_DIR "${CMAKE_SOURCE_DIR}/obs-binaries/windows")
    else()
    set(OBS_BUNDLED_DIR "${CMAKE_SOURCE_DIR}/obs-binaries/linux")
    endif()

if(EXISTS "${OBS_BUNDLED_DIR}")
    set(OBS_INCLUDE_DIR "${OBS_BUNDLED_DIR}/include")
    set(OBS_LIBRARY "${OBS_BUNDLED_DIR}/lib/libobs.dylib")
    add_compile_definitions(HAVE_OBS OBS_BUNDLED)
endif()
```

### Strategy 2: Minimal OBS Build

Build only the OBS components you need, reducing bundle size.

#### Step 1: Create Minimal OBS Build Script

```bash
#!/bin/bash
# build-minimal-obs.sh

OBS_VERSION="30.0.2"
BUILD_DIR="obs-minimal-build"

# Download OBS source
git clone --recursive https://github.com/obsproject/obs-studio.git
cd obs-studio
git checkout $OBS_VERSION

# Configure minimal build
mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_UI=OFF \
    -DENABLE_SCRIPTING=OFF \
    -DENABLE_WEBSOCKET=OFF \
    -DENABLE_BROWSER=OFF \
    -DENABLE_VST=OFF \
    -DENABLE_VLC=OFF \
    -DENABLE_NDI=OFF \
    -DENABLE_RTMPS=OFF \
    -DENABLE_SRT=OFF \
    -DENABLE_JACK=OFF \
    -DENABLE_AJA=OFF \
    -DENABLE_DECKLINK=OFF \
    -DENABLE_ALSA=OFF \
    -DENABLE_PULSEAUDIO=ON \
    -DENABLE_WASAPI=ON \
    -DENABLE_COREAUDIO=ON \
    -DENABLE_MAC_CAPTURE=ON \
    -DENABLE_WIN_CAPTURE=ON \
    -DENABLE_LINUX_CAPTURE=ON \
    -DENABLE_X264=ON \
    -DENABLE_FFMPEG=ON \
    -DENABLE_NVENC=ON \
    -DENABLE_AMF=ON \
    -DENABLE_QSV=ON \
    -DBUILD_CAPTIONS=OFF \
    -DBUILD_BROWSER=OFF \
    -DBUILD_VST=OFF \
    -DBUILD_WEBSOCKET=OFF

# Build
make -j$(nproc)
```

#### Step 2: Bundle Minimal Build

```bash
# Copy only the required components
mkdir -p minimal-obs-bundle/{lib,plugins,data}

# Core libraries
cp $BUILD_DIR/libobs/libobs.so minimal-obs-bundle/lib/
cp $BUILD_DIR/libobs-ffmpeg/libobs-ffmpeg.so minimal-obs-bundle/lib/
cp $BUILD_DIR/libobs-util/libobs-util.so minimal-obs-bundle/lib/

# Capture plugins
cp $BUILD_DIR/plugins/mac-capture/mac-capture.so minimal-obs-bundle/plugins/
cp $BUILD_DIR/plugins/win-capture/win-capture.dll minimal-obs-bundle/plugins/
cp $BUILD_DIR/plugins/linux-capture/linux-capture.so minimal-obs-bundle/plugins/

# Audio plugins
cp $BUILD_DIR/plugins/coreaudio-encoder/coreaudio-encoder.so minimal-obs-bundle/plugins/
cp $BUILD_DIR/plugins/wasapi/wasapi.dll minimal-obs-bundle/plugins/
cp $BUILD_DIR/plugins/pulseaudio/pulseaudio.so minimal-obs-bundle/plugins/

# Encoding plugins
cp $BUILD_DIR/plugins/obs-x264/obs-x264.so minimal-obs-bundle/plugins/
cp $BUILD_DIR/plugins/obs-ffmpeg/obs-ffmpeg.so minimal-obs-bundle/plugins/
```

## 🔧 Runtime Integration

### Plugin Loading

The addon automatically loads OBS plugins at runtime:

```cpp
void OBSManager::setupPluginPaths() {
#ifdef HAVE_OBS
    // Add bundled plugin paths
#ifdef OBS_BUNDLED
    std::string pluginPath = getBundledPluginPath();
    obs_add_module_path(pluginPath.c_str(), nullptr);
#endif

    // Platform-specific paths
#ifdef __APPLE__
    obs_add_module_path("/usr/local/lib/obs-plugins", "/usr/local/share/obs/obs-plugins");
    obs_add_module_path("/opt/homebrew/lib/obs-plugins", "/opt/homebrew/share/obs/obs-plugins");
#elif defined(_WIN32)
    obs_add_module_path("C:/Program Files/obs-studio/obs-plugins/64bit", "C:/Program Files/obs-studio/data/obs-plugins");
#else
    obs_add_module_path("/usr/lib/obs-plugins", "/usr/share/obs/obs-plugins");
    obs_add_module_path("/usr/local/lib/obs-plugins", "/usr/local/share/obs/obs-plugins");
#endif
#endif
}
```

### Source Creation

Different capture sources are used based on platform and requirements:

```cpp
bool OBSManager::createVideoSource(const RecordingConfig& config) {
#ifdef HAVE_OBS
    std::string source_id;
    obs_data_t* settings = obs_data_create();
    
#ifdef __APPLE__
    // Use ScreenCaptureKit on macOS
if (config.source_type == RecordingConfig::WINDOW) {
        source_id = "screen_capture";
        obs_data_set_int(settings, "type", 1); // Window
    obs_data_set_int(settings, "window", config.window_id);
} else {
        source_id = "screen_capture";
        obs_data_set_int(settings, "type", 0); // Display
    obs_data_set_string(settings, "display_uuid", config.display_id.c_str());
}
obs_data_set_bool(settings, "show_cursor", config.capture_cursor);
#elif defined(_WIN32)
    // Use Graphics Capture API on Windows
if (config.source_type == RecordingConfig::WINDOW) {
    source_id = "window_capture";
} else {
    source_id = "monitor_capture";
    obs_data_set_int(settings, "monitor", std::stoi(config.display_id));
}
obs_data_set_bool(settings, "cursor", config.capture_cursor);
#else
    // Use X11 on Linux
source_id = "xcomposite_input";
obs_data_set_int(settings, "screen", std::stoi(config.display_id));
obs_data_set_bool(settings, "show_cursor", config.capture_cursor);
#endif
    
    obs_source_t* source = obs_source_create(source_id.c_str(), "video_source", settings, nullptr);
    obs_data_release(settings);
    
    if (!source) {
        std::cerr << "Failed to create video source: " << source_id << std::endl;
        return false;
    }
    
    video_source_ = source;
    return true;
#else
    return true;
#endif
}
```

## 🎯 Platform-Specific Details

### macOS

#### ScreenCaptureKit Integration
- Uses Apple's modern ScreenCaptureKit API
- Supports display and window capture
- Hardware acceleration via VideoToolbox
- Automatic permission handling

#### Audio Capture
- CoreAudio for system audio capture
- Supports multiple audio devices
- Real-time audio processing

### Windows

#### Graphics Capture API
- Uses Microsoft's modern Graphics Capture API
- Supports display and window capture
- Hardware acceleration via NVENC/QuickSync
- No additional permissions required

#### Audio Capture
- WASAPI for system audio capture
- Supports multiple audio endpoints
- Low-latency audio processing

### Linux

#### X11/Wayland Capture
- X11 capture via xcomposite
- Wayland support via pipewire
- Hardware acceleration via VA-API
- Multiple display support

#### Audio Capture
- PulseAudio for system audio
- PipeWire compatibility
- Multiple audio device support

## 🔒 Security Considerations

### Code Signing

For production deployment, especially on macOS, you should code sign your application:

   ```bash
# macOS code signing
codesign --force --deep --sign "Developer ID Application: Your Name" your-app.app

# Windows code signing
signtool sign /f certificate.pfx /p password your-app.exe

# Linux (optional)
# Use GPG signing for packages
```

### Sandboxing

Consider sandboxing your application for additional security:

```xml
<!-- macOS sandbox entitlements -->
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>com.apple.security.device.camera</key>
    <true/>
    <key>com.apple.security.device.microphone</key>
    <true/>
    <key>com.apple.security.device.audio-input</key>
    <true/>
    <key>com.apple.security.device.audio-output</key>
    <true/>
</dict>
</plist>
```

## 📊 Performance Optimization

### Memory Management

   ```cpp
// Proper OBS resource cleanup
void OBSManager::cleanupRecording() {
#ifdef HAVE_OBS
    if (video_source_) {
        obs_source_release(static_cast<obs_source_t*>(video_source_));
        video_source_ = nullptr;
    }
    
    if (audio_source_) {
        obs_source_release(static_cast<obs_source_t*>(audio_source_));
        audio_source_ = nullptr;
    }
    
    if (obs_output_) {
        obs_output_release(static_cast<obs_output_t*>(obs_output_));
        obs_output_ = nullptr;
    }
#endif
}
```

### Hardware Acceleration

Enable hardware encoding for better performance:

```cpp
bool OBSManager::setupVideoOutput(const RecordingConfig& config) {
#ifdef HAVE_OBS
    struct obs_video_info ovi = {};
    ovi.graphics_module = "libobs-opengl"; // or "libobs-d3d11" on Windows
    ovi.fps_num = config.fps;
    ovi.fps_den = 1;
    ovi.base_width = config.width;
    ovi.base_height = config.height;
    ovi.output_width = config.width;
    ovi.output_height = config.height;
    ovi.output_format = VIDEO_FORMAT_NV12;
    ovi.adapter = 0;
    ovi.gpu_conversion = true; // Enable GPU acceleration
    ovi.colorspace = VIDEO_CS_709;
    ovi.range = VIDEO_RANGE_PARTIAL;
    ovi.scale_type = OBS_SCALE_BICUBIC;
    
    if (obs_reset_video(&ovi) != OBS_VIDEO_SUCCESS) {
        std::cerr << "Failed to reset video" << std::endl;
        return false;
    }
    
    return true;
#else
    return true;
#endif
}
```

## 🧪 Testing

### Unit Tests

```javascript
// test/obs-integration.test.js
const { OBSManager } = require('../index.js');

describe('OBS Integration', () => {
    let obs;
    
    beforeEach(() => {
        obs = OBSManager.getInstance();
    });
    
    afterEach(() => {
        if (obs.isInitialized()) {
            obs.shutdown();
        }
    });
    
    test('should initialize OBS core', () => {
        expect(obs.initialize()).toBe(true);
        expect(obs.isInitialized()).toBe(true);
    });
    
    test('should enumerate displays', () => {
        obs.initialize();
        const displays = obs.getDisplays();
        expect(displays).toBeInstanceOf(Array);
        expect(displays.length).toBeGreaterThan(0);
    });
    
    test('should start and stop recording', () => {
        obs.initialize();
        const displays = obs.getDisplays();
        
        const config = {
            source_type: 'DISPLAY',
            display_id: displays[0].id,
            width: 640,
            height: 480,
            fps: 30,
            capture_audio: false
        };
        
        expect(obs.startRecording('test.mp4', config)).toBe(true);
        expect(obs.isRecording()).toBe(true);
        
        obs.stopRecording();
        expect(obs.isRecording()).toBe(false);
    });
});
```

### Integration Tests

```bash
#!/bin/bash
# test-integration.sh

echo "Testing OBS integration..."

# Test display enumeration
node -e "
const { OBSManager } = require('./index.js');
const obs = OBSManager.getInstance();
obs.initialize();
const displays = obs.getDisplays();
console.log('Displays found:', displays.length);
obs.shutdown();
"

# Test recording
node -e "
const { OBSManager } = require('./index.js');
const obs = OBSManager.getInstance();
obs.initialize();
const displays = obs.getDisplays();
if (displays.length > 0) {
    const config = {
        source_type: 'DISPLAY',
        display_id: displays[0].id,
        width: 640,
        height: 480,
        fps: 30,
        capture_audio: false
    };
    
    if (obs.startRecording('test-output.mp4', config)) {
        console.log('Recording started successfully');
        setTimeout(() => {
            obs.stopRecording();
            obs.shutdown();
            console.log('Recording test completed');
        }, 5000);
    } else {
        console.error('Failed to start recording');
        obs.shutdown();
    }
} else {
    console.error('No displays found');
    obs.shutdown();
}
"
```

## 🚀 Deployment Checklist

### Pre-deployment

- [ ] OBS libraries are bundled or minimal build is created
- [ ] All required plugins are included
- [ ] Code signing is applied (macOS/Windows)
- [ ] Permissions are properly configured
- [ ] Tests pass on all target platforms
- [ ] Bundle size is optimized
- [ ] Documentation is updated

### Post-deployment

- [ ] Monitor for crashes and errors
- [ ] Collect performance metrics
- [ ] Gather user feedback
- [ ] Update OBS version as needed
- [ ] Monitor for security updates

## 📚 Additional Resources

- [OBS Studio Documentation](https://obsproject.com/docs/)
- [OBS Plugin Development Guide](https://obsproject.com/docs/plugins.html)
- [Node.js Native Addon API](https://nodejs.org/api/n-api.html)
- [CMake Documentation](https://cmake.org/documentation/)

---

This integration provides a powerful, professional-grade screen recording solution that leverages OBS Studio's proven technology while maintaining the simplicity of a Node.js API. 