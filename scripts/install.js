const fs = require('fs');
const path = require('path');
const { execSync } = require('child_process');

console.log('OBS Screen Capture - Post Install');

const platform = process.platform;
const buildDir = path.join(__dirname, '..', 'build');

// Check if we need to build
if (!fs.existsSync(path.join(buildDir, 'node-addon', 'obs_screen_capture.node'))) {
    console.log('Native addon not found, building...');
    
    try {
        // Configure CMake
        console.log('Configuring CMake...');
        execSync('cmake -B build -DBUILD_FOR_NODE=ON', { 
            stdio: 'inherit',
            cwd: path.dirname(__dirname)
        });
        
        // Build
        console.log('Building native addon...');
        execSync('cmake --build build --parallel', { 
            stdio: 'inherit',
            cwd: path.dirname(__dirname)
        });
        
        console.log('Build completed successfully!');
        
    } catch (error) {
        console.error('Build failed:', error.message);
        
        // Provide platform-specific help
        if (platform === 'darwin') {
            console.error('\nOn macOS, make sure you have:');
            console.error('- Xcode Command Line Tools: xcode-select --install');
            console.error('- CMake: brew install cmake');
        } else if (platform === 'linux') {
            console.error('\nOn Linux, make sure you have:');
            console.error('- Build tools: sudo apt-get install build-essential cmake ninja-build');
            console.error('- X11 dev: sudo apt-get install libx11-dev libxrandr-dev');
            console.error('- Audio dev: sudo apt-get install libpulse-dev');
        } else if (platform === 'win32') {
            console.error('\nOn Windows, make sure you have:');
            console.error('- Visual Studio Build Tools');
            console.error('- CMake');
        }
        
        process.exit(1);
    }
} else {
    console.log('Native addon already built.');
}

// Check permissions on macOS
if (platform === 'darwin') {
    console.log('\n⚠️  macOS Notice:');
    console.log('This package requires Screen Recording permission.');
    console.log('You will be prompted to grant permission when first using screen capture.');
    console.log('You can also manually grant permission in:');
    console.log('System Preferences → Security & Privacy → Privacy → Screen Recording');
}

console.log('\n✅ Installation complete!');