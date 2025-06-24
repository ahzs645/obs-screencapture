# OBS Screen Capture

A cross-platform Node.js package for high-quality screen and audio recording, powered by OBS Studio. This package provides a simple API for capturing displays, windows, and system audio without requiring users to install OBS Studio.

## ✨ Features

- 🖥️ **Display Capture** - Record entire screens/monitors
- 🪟 **Window Capture** - Record specific application windows  
- 🎵 **System Audio** - Capture system audio output
- 🎯 **Cross-Platform** - Works on macOS, Windows, and Linux
- 🚀 **Modern APIs** - Uses ScreenCaptureKit on macOS 13+
- 📦 **Self-Contained** - No external OBS installation required
- ⚡ **High Performance** - Hardware-accelerated encoding
- 🔒 **Permission Handling** - Automatic permission management on macOS

## 🚀 Quick Start

```bash
npm install obs-screen-capture
```

```javascript
const obs = require('obs-screen-capture');

// Initialize OBS
obs.init();

// List available displays
const displays = obs.listDisplays();
console.log('Available displays:', displays);

// Start recording
obs.startRecording('/path/to/output.mp4', {
    displayId: displays[0].id,
    width: 1920,
    height: 1080,
    fps: 60,
    capture_audio: true
});

// Record for 10 seconds
setTimeout(() => {
    obs.stopRecording();
    obs.shutdown();
    console.log('Recording complete!');
}, 10000);
```

## 📋 Requirements

### macOS

- macOS 10.15+ (macOS 13+ recommended for best features)
- Xcode Command Line Tools
- Screen Recording permission (automatically requested)

### Windows

- Windows 10 1903+ or Windows 11
- Visual Studio Build Tools or Visual Studio Community
- No additional permissions required

### Linux

- Modern Linux distribution with X11 or Wayland
- Build tools (gcc, cmake)
- PulseAudio or PipeWire for audio

## 🛠️ Installation

### For Users

```bash
npm install obs-screen-capture
```

The package will automatically build native components during installation.

### For Developers

```bash
git clone https://github.com/yourusername/obs-screen-capture.git
cd obs-screen-capture

# Install dependencies
npm install

# Build native components
npm run build

# Run tests
npm test
```

## 📖 API Reference

### Core Functions

#### `obs.init() → boolean`

Initialize OBS Studio core. Must be called before other functions.

```javascript
const success = obs.init();
if (!success) {
    console.error('Failed to initialize OBS');
}
```

#### `obs.shutdown()`

Clean up OBS resources. Call when done recording.

#### `obs.listDisplays() → Display[]`

Get available displays/monitors.

```javascript
const displays = obs.listDisplays();
// Returns: [{ id: string, name: string, width: number, height: number, x: number, y: number }]
```

#### `obs.listWindows() → Window[]`

Get available windows for capture.

```javascript
const windows = obs.listWindows();
// Returns: [{ id: number, name: string, owner: string, width: number, height: number, x: number, y: number }]
```

#### `obs.startRecording(outputPath, config) → boolean`

Start recording with specified configuration.

```javascript
const success = obs.startRecording('/path/to/video.mp4', {
    // Source selection (choose one)
    displayId: 'display-uuid',           // Capture entire display
    windowId: 12345,                     // Capture specific window
    
    // Output settings
    width: 1920,                         // Video width
    height: 1080,                        // Video height
    fps: 60,                             // Frame rate
    video_bitrate: 8000,                 // Video bitrate (kbps)
    audio_bitrate: 160,                  // Audio bitrate (kbps)
    
    // Capture options
    capture_cursor: true,                // Include mouse cursor
    capture_audio: true,                 // Include system audio
    hide_obs: true,                      // Hide OBS from capture (macOS)
});
```

#### `obs.stopRecording()`

Stop current recording.

#### `obs.isRecording() → boolean`

Check if currently recording.

### macOS-Specific Functions

#### `obs.checkScreenPermission() → boolean`

Check if screen recording permission is granted.

#### `obs.requestScreenPermission() → boolean`

Request screen recording permission from user.

```javascript
if (!obs.checkScreenPermission()) {
    console.log('Requesting screen permission...');
    const granted = obs.requestScreenPermission();
    if (!granted) {
        console.log('Permission denied. Please grant in System Preferences.');
    }
}
```

## 🎯 Recording Examples

### Display Recording

```javascript
const obs = require('obs-screen-capture');

obs.init();
const displays = obs.listDisplays();

obs.startRecording('./screen-recording.mp4', {
    displayId: displays[0].id,
    width: 1920,
    height: 1080,
    fps: 60,
    capture_audio: true,
    capture_cursor: true
});

// Record for 30 seconds
setTimeout(() => {
    obs.stopRecording();
    obs.shutdown();
}, 30000);
```

### Window Recording

```javascript
const obs = require('obs-screen-capture');

obs.init();
const windows = obs.listWindows();

// Find a specific window (e.g., browser)
const browserWindow = windows.find(w => 
    w.name.includes('Chrome') || w.name.includes('Firefox')
);

if (browserWindow) {
    obs.startRecording('./window-recording.mp4', {
        windowId: browserWindow.id,
        width: browserWindow.width,
        height: browserWindow.height,
        fps: 30,
        capture_audio: false // Window audio not always available
    });
}
```

### Audio-Only Recording

```javascript
const obs = require('obs-screen-capture');

obs.init();
const displays = obs.listDisplays();

obs.startRecording('./audio-only.mp4', {
    displayId: displays[0].id,
    width: 640,    // Minimal video
    height: 480,
    fps: 15,       // Low frame rate
    video_bitrate: 500,  // Low video bitrate
    audio_bitrate: 320,  // High audio bitrate
    capture_audio: true,
    capture_cursor: false
});
```

## 🔧 Configuration Options

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| displayId | string | - | Display UUID to capture (macOS) or monitor ID |
| windowId | number | - | Window ID to capture |
| width | number | 1920 | Output video width |
| height | number | 1080 | Output video height |
| fps | number | 60 | Frame rate |
| video_bitrate | number | 8000 | Video bitrate in kbps |
| audio_bitrate | number | 160 | Audio bitrate in kbps |
| capture_cursor | boolean | true | Include mouse cursor |
| capture_audio | boolean | true | Include system audio |
| hide_obs | boolean | true | Hide OBS from capture (macOS only) |

## 🧪 Testing

```bash
# Run all tests
npm test

# Platform-specific tests
npm run test:macos     # macOS ScreenCaptureKit features
npm run test:windows   # Windows Graphics Capture API
npm run test:linux     # Linux X11/Wayland capture

# Build tests
npm run configure      # Test CMake configuration
npm run build         # Test compilation
```

## 🐛 Troubleshooting

### macOS Issues

#### Permission Denied

```bash
# Grant screen recording permission manually:
# System Preferences → Security & Privacy → Privacy → Screen Recording
# Add your terminal app or Node.js
```

#### Build Failures

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Update CMake
brew upgrade cmake
```

### Windows Issues

#### Build Tools Missing

```powershell
# Install Visual Studio Build Tools
# https://visualstudio.microsoft.com/downloads/

# Or install via Chocolatey
choco install visualstudio2022buildtools
```

#### CMake Not Found

```powershell
# Add CMake to PATH or reinstall
choco install cmake
```

### Linux Issues

#### Missing Libraries

```bash
# Ubuntu/Debian
sudo apt-get install libx11-dev libxrandr-dev libpulse-dev

# CentOS/RHEL
sudo yum install libX11-devel libXrandr-devel pulseaudio-libs-devel

# Arch Linux
sudo pacman -S libx11 libxrandr pulseaudio
```

#### Permission Issues

```bash
# Add user to audio group
sudo usermod -a -G audio $USER

# Restart session after adding to group
```

## 🏗️ Architecture

This package builds a minimal version of OBS Studio with only the capture plugins needed:

- **macOS**: ScreenCaptureKit, CoreAudio
- **Windows**: Graphics Capture API, WASAPI
- **Linux**: X11/Wayland capture, PulseAudio/PipeWire

The build process:

1. Downloads OBS Studio source code
2. Configures minimal build (no UI, only capture plugins)
3. Builds libobs and required plugins
4. Creates Node.js native addon linking to OBS
5. Bundles everything into npm package

## 🤝 Contributing

1. Fork the repository
2. Create feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Add tests for new functionality
5. Commit changes (`git commit -m 'Add amazing feature'`)
6. Push to branch (`git push origin feature/amazing-feature`)
7. Open Pull Request

### Development Setup

```bash
git clone https://github.com/yourusername/obs-screen-capture.git
cd obs-screen-capture

# Install dependencies
npm install

# Configure build
npm run configure

# Build in development mode
cmake --build build --config Debug

# Run tests
npm test
```

## 📄 License

MIT License - see LICENSE file for details.

## 🙏 Acknowledgments

- [OBS Studio](https://obsproject.com/) - The amazing streaming/recording software this is built on
- [node-addon-api](https://github.com/nodejs/node-addon-api) - For modern Node.js native addon development
- Apple's ScreenCaptureKit - For modern macOS screen capture
- Microsoft's Graphics Capture API - For modern Windows screen capture

## 📚 Related Projects

- [OBS Studio](https://obsproject.com/) - Full-featured streaming/recording software
- [Electron DesktopCapturer](https://www.electronjs.org/docs/latest/api/desktop-capturer) - Browser-based screen capture
- [node-screenshots](https://github.com/nashaofu/node-screenshots) - Cross-platform screenshot library