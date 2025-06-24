# OBS Screen Capture Node Addon

A powerful cross-platform screen recording library that embeds OBS Studio's battle-tested capture engine directly into your Node.js applications. Get professional-quality screen recording without requiring users to install OBS Studio.

## 🎯 Key Features

- **🚀 OBS-Powered**: Uses OBS Studio's proven capture plugins (ScreenCaptureKit, WASAPI, Graphics Capture API)
- **📦 No OBS Installation Required**: For end users - everything is bundled in your app
- **🖥️ Cross-Platform**: Works on macOS, Windows, and Linux
- **🎥 Professional Quality**: Hardware acceleration, multiple codecs, high performance
- **🔧 Flexible Configuration**: Display capture, window capture, audio recording
- **⚡ Real-time**: Low latency capture and encoding

## 🏗️ Architecture

This project embeds OBS Studio's core capture functionality directly into your application:

```
Your App
├── Node.js Addon (obs_screen_capture.node)
├── OBS Core Libraries (embedded)
├── OBS Capture Plugins (embedded)
│   ├── ScreenCaptureKit (macOS)
│   ├── Graphics Capture API (Windows)
│   ├── X11 Capture (Linux)
│   └── Audio Capture (WASAPI/PulseAudio/CoreAudio)
└── Encoding Plugins (x264, FFmpeg)
```

## 📋 Requirements

### Development Requirements
- **Node.js** 16.0.0 or higher
- **CMake** 3.24 or higher
- **OBS Studio** (for development only - not required for end users)

### Platform-Specific Requirements

#### macOS
- Xcode Command Line Tools
- OBS Studio (for development): `brew install obs-studio`

#### Windows
- Visual Studio Build Tools 2019 or later
- OBS Studio (for development): Download from [obsproject.com](https://obsproject.com/download)

#### Linux
- Build essentials: `sudo apt install build-essential cmake`
- X11 development: `sudo apt install libx11-dev libxrandr-dev`
- OBS development: `sudo apt install libobs-dev`

## 🚀 Quick Start

### 1. Install Dependencies

```bash
# Clone the repository
git clone https://github.com/yourusername/obs-screen-capture.git
cd obs-screen-capture

# Run the automated setup (recommended)
npm install
```

The setup script will:
- Check for required tools (Node.js, CMake)
- Install OBS Studio for development (if not found)
- Configure and build the project
- Run tests to verify everything works

### 2. Manual Installation (Alternative)

If you prefer manual installation:

```bash
# Install OBS Studio for development
# macOS
brew install obs-studio

# Ubuntu/Debian
sudo apt install libobs-dev

# Windows
# Download and install from https://obsproject.com/download

# Build the project
npm run configure
npm run build
npm test
```

### 3. Basic Usage

```javascript
const { OBSManager } = require('./index.js');

async function recordScreen() {
    const obs = OBSManager.getInstance();
    
    // Initialize OBS
    if (!obs.initialize()) {
        console.error('Failed to initialize OBS');
        return;
    }
    
    // Get available displays
    const displays = obs.getDisplays();
    console.log('Available displays:', displays);
    
    // Get available windows
    const windows = obs.getWindows();
    console.log('Available windows:', windows);
    
    // Start recording
    const config = {
        source_type: 'DISPLAY',
        display_id: displays[0].id,
        width: 1920,
        height: 1080,
        fps: 60,
        capture_audio: true,
        capture_cursor: true
    };
    
    if (obs.startRecording('output.mp4', config)) {
        console.log('Recording started!');
        
        // Record for 10 seconds
        setTimeout(() => {
            obs.stopRecording();
            console.log('Recording stopped!');
            obs.shutdown();
        }, 10000);
    }
}

recordScreen();
```

## 📚 API Reference

### OBSManager

The main class for managing OBS operations.

#### Methods

##### `getInstance()`
Get the singleton instance of OBSManager.

##### `initialize()`
Initialize the OBS core and load required plugins.
- **Returns**: `boolean` - true if successful

##### `shutdown()`
Shutdown OBS and cleanup resources.

##### `getDisplays()`
Get list of available displays.
- **Returns**: `DisplayInfo[]` - Array of display information

##### `getWindows()`
Get list of available windows.
- **Returns**: `WindowInfo[]` - Array of window information

##### `startRecording(outputPath, config)`
Start recording with the specified configuration.
- **Parameters**:
  - `outputPath` (string): Path for the output file
  - `config` (RecordingConfig): Recording configuration
- **Returns**: `boolean` - true if successful

##### `stopRecording()`
Stop the current recording.

##### `isRecording()`
Check if currently recording.
- **Returns**: `boolean`

### RecordingConfig

Configuration object for recording settings.

```javascript
{
    // Source type: 'DISPLAY', 'WINDOW', or 'APPLICATION'
    source_type: 'DISPLAY',
    
    // Source identifiers
    display_id: '1',           // Display ID (for display capture)
    window_id: 12345,          // Window ID (for window capture)
    application_id: 'app',     // Application ID (for app capture)
    
    // Output settings
    width: 1920,               // Output width
    height: 1080,              // Output height
    fps: 60,                   // Frame rate
    video_bitrate: 8000,       // Video bitrate (kbps)
    audio_bitrate: 160,        // Audio bitrate (kbps)
    
    // Capture settings
    capture_cursor: true,      // Include cursor in recording
    capture_audio: true,       // Include audio in recording
    hide_obs: true,           // Hide OBS from capture (macOS)
    show_empty_names: false,   // Show windows with empty names
    show_hidden_windows: false // Show hidden windows
}
```

### DisplayInfo

Information about a display.

```javascript
{
    id: '1',           // Display identifier
    name: 'Display 1', // Display name
    width: 1920,       // Display width
    height: 1080,      // Display height
    x: 0,              // Display X position
    y: 0               // Display Y position
}
```

### WindowInfo

Information about a window.

```javascript
{
    id: 12345,         // Window ID
    name: 'My App',    // Window title
    owner: 'MyApp',    // Application name
    width: 800,        // Window width
    height: 600,       // Window height
    x: 100,            // Window X position
    y: 100             // Window Y position
}
```

## 🔧 Advanced Usage

### Display Capture

```javascript
const displays = obs.getDisplays();
const config = {
    source_type: 'DISPLAY',
    display_id: displays[0].id,
    width: 1920,
    height: 1080,
    fps: 60,
    capture_audio: true
};

obs.startRecording('display_capture.mp4', config);
```

### Window Capture

```javascript
const windows = obs.getWindows();
const targetWindow = windows.find(w => w.name.includes('Chrome'));

if (targetWindow) {
    const config = {
        source_type: 'WINDOW',
        window_id: targetWindow.id,
        width: targetWindow.width,
        height: targetWindow.height,
        fps: 30,
        capture_audio: false
    };
    
    obs.startRecording('window_capture.mp4', config);
}
```

### Custom Recording Settings

```javascript
const config = {
    source_type: 'DISPLAY',
    display_id: '1',
    width: 2560,
    height: 1440,
    fps: 120,
    video_bitrate: 15000,
    audio_bitrate: 320,
    capture_cursor: true,
    capture_audio: true
};

obs.startRecording('high_quality.mp4', config);
```

## 🛠️ Development

### Building from Source

```bash
# Configure with CMake
npm run configure

# Build the project
npm run build

# Run tests
npm test

# Clean build
rm -rf build/
```

### CMake Options

```bash
# Build without OBS integration (foundation only)
cmake -B build -DUSE_SYSTEM_OBS=OFF

# Build with custom OBS path
cmake -B build -DOBS_INCLUDE_DIR=/custom/path/include -DOBS_LIBRARY=/custom/path/lib/libobs.so
```

### Debugging

```bash
# Build with debug symbols
cmake -B build -DCMAKE_BUILD_TYPE=Debug
npm run build

# Run with verbose output
DEBUG=* npm test
```

## 🚀 Deployment

### For Development
- Use system OBS installation (what we set up above)
- Full functionality available

### For Production
You have two deployment strategies:

#### Option A: Bundle Pre-built OBS Libraries
Download OBS binaries for each platform and bundle them with your app.

#### Option B: Minimal OBS Build
Build only the capture plugins you need:

```cmake
set(OBS_CMAKE_ARGS
    -DENABLE_UI=OFF                    # No UI
    -DENABLE_MAC_CAPTURE=ON           # ScreenCaptureKit only
    -DENABLE_WASAPI=ON                # Windows audio only  
    -DENABLE_X264=ON                  # Basic encoding only
    -DBUILD_CAPTIONS=OFF              # No captions
)
```

## 🔒 Permissions

### macOS
- **Screen Recording**: Required for screen capture
- **Microphone**: Required for audio capture
- **Accessibility**: May be required for window capture

### Windows
- **Graphics Capture**: Required for screen capture
- **Audio Capture**: Required for audio recording

### Linux
- **X11 Access**: Required for screen capture
- **PulseAudio**: Required for audio capture

## 🐛 Troubleshooting

### Common Issues

#### OBS Not Found
```
WARNING: OBS not found, building foundation only
```
**Solution**: Install OBS Studio for development as described in the installation section.

#### Build Failures
```
CMake Error: Could not find OBS
```
**Solution**: Ensure OBS development headers are installed and CMake can find them.

#### Permission Denied
```
Failed to start recording
```
**Solution**: Grant screen recording permissions in system settings.

#### Plugin Loading Errors
```
Failed to load required plugins
```
**Solution**: Ensure OBS plugins are installed and accessible.

### Platform-Specific Issues

#### macOS
- **Xcode Command Line Tools**: `xcode-select --install`
- **Homebrew**: `brew install obs-studio`
- **Permissions**: System Preferences → Security & Privacy → Privacy → Screen Recording

#### Windows
- **Visual Studio Build Tools**: Install from Microsoft
- **OBS Development Headers**: Download from obsproject.com
- **Graphics Capture**: Ensure Windows 10 version 1803 or later

#### Linux
- **Build Dependencies**: `sudo apt install build-essential cmake`
- **OBS Development**: `sudo apt install libobs-dev`
- **X11 Development**: `sudo apt install libx11-dev libxrandr-dev`

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch: `git checkout -b feature-name`
3. Make your changes
4. Add tests for new functionality
5. Run tests: `npm test`
6. Commit your changes: `git commit -am 'Add feature'`
7. Push to the branch: `git push origin feature-name`
8. Submit a pull request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **OBS Studio Team**: For the amazing capture engine and plugins
- **Node.js Community**: For the excellent native addon APIs
- **ScreenCaptureKit Team**: For the modern macOS capture API

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/yourusername/obs-screen-capture/issues)
- **Discussions**: [GitHub Discussions](https://github.com/yourusername/obs-screen-capture/discussions)
- **Documentation**: [Wiki](https://github.com/yourusername/obs-screen-capture/wiki)

---

**Happy Recording! 🎥✨**