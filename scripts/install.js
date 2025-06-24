#!/usr/bin/env node

const { execSync } = require('child_process');
const fs = require('fs');
const path = require('path');
const os = require('os');

console.log('🚀 Setting up OBS Screen Capture Node Addon...\n');

// Check if we're on a supported platform
const platform = os.platform();
if (!['darwin', 'win32', 'linux'].includes(platform)) {
    console.error('❌ Unsupported platform:', platform);
    process.exit(1);
}

console.log(`📋 Platform: ${platform}`);

// Check for required tools
function checkCommand(command, name) {
    try {
        execSync(`${command} --version`, { stdio: 'ignore' });
        console.log(`✅ ${name} found`);
        return true;
    } catch (error) {
        console.log(`❌ ${name} not found`);
        return false;
    }
}

console.log('\n🔍 Checking required tools...');
const hasNode = checkCommand('node', 'Node.js');
const hasNpm = checkCommand('npm', 'npm');
const hasCmake = checkCommand('cmake', 'CMake');

if (!hasNode || !hasNpm || !hasCmake) {
    console.error('\n❌ Missing required tools. Please install:');
    if (!hasNode) console.error('  - Node.js: https://nodejs.org/');
    if (!hasCmake) console.error('  - CMake: https://cmake.org/download/');
    process.exit(1);
}

// Check for OBS installation
console.log('\n🔍 Checking for OBS installation...');
let obsFound = false;

if (platform === 'darwin') {
    // macOS - check for Homebrew OBS
    try {
        execSync('brew list obs-studio', { stdio: 'ignore' });
        console.log('✅ OBS Studio found (Homebrew)');
        obsFound = true;
    } catch (error) {
        console.log('❌ OBS Studio not found via Homebrew');
        
        // Check for manual installation
        const obsPaths = [
            '/Applications/OBS Studio.app',
            '/usr/local/include/obs/obs.h',
            '/opt/homebrew/include/obs/obs.h'
        ];
        
        for (const obsPath of obsPaths) {
            if (fs.existsSync(obsPath)) {
                console.log(`✅ OBS Studio found at: ${obsPath}`);
                obsFound = true;
                break;
            }
        }
    }
    
    if (!obsFound) {
        console.log('\n📦 Installing OBS Studio for development...');
        console.log('   This is required for building with OBS integration.');
        console.log('   Users of your final app will NOT need OBS installed.\n');
        
        try {
            execSync('brew install obs-studio', { stdio: 'inherit' });
            console.log('✅ OBS Studio installed successfully');
            obsFound = true;
        } catch (error) {
            console.error('❌ Failed to install OBS Studio via Homebrew');
            console.log('\n💡 Alternative installation methods:');
            console.log('   1. Download from https://obsproject.com/download');
            console.log('   2. Install manually and ensure headers are available');
            console.log('   3. Build without OBS integration (limited functionality)');
        }
    }
} else if (platform === 'win32') {
    // Windows - check for OBS installation
    const obsPaths = [
        'C:\\Program Files\\obs-studio\\include\\obs\\obs.h',
        'C:\\Program Files (x86)\\obs-studio\\include\\obs\\obs.h'
    ];
    
    for (const obsPath of obsPaths) {
        if (fs.existsSync(obsPath)) {
            console.log(`✅ OBS Studio found at: ${obsPath}`);
            obsFound = true;
            break;
        }
    }
    
    if (!obsFound) {
        console.log('\n📦 OBS Studio not found on Windows');
        console.log('   Please install OBS Studio from: https://obsproject.com/download');
        console.log('   Make sure to install the development headers.');
    }
} else if (platform === 'linux') {
    // Linux - check for OBS installation
    try {
        execSync('pkg-config --exists libobs', { stdio: 'ignore' });
        console.log('✅ OBS Studio found (pkg-config)');
        obsFound = true;
    } catch (error) {
        console.log('❌ OBS Studio not found via pkg-config');
        
        // Check common installation paths
        const obsPaths = [
            '/usr/include/obs/obs.h',
            '/usr/local/include/obs/obs.h'
        ];
        
        for (const obsPath of obsPaths) {
            if (fs.existsSync(obsPath)) {
                console.log(`✅ OBS Studio found at: ${obsPath}`);
                obsFound = true;
                break;
            }
        }
    }
    
    if (!obsFound) {
        console.log('\n📦 OBS Studio not found on Linux');
        console.log('   Please install OBS Studio development packages:');
        console.log('   Ubuntu/Debian: sudo apt install libobs-dev');
        console.log('   Fedora: sudo dnf install obs-studio-devel');
        console.log('   Arch: sudo pacman -S obs-studio');
    }
}

// Install npm dependencies
console.log('\n📦 Installing npm dependencies...');
try {
    execSync('npm install', { stdio: 'inherit' });
    console.log('✅ Dependencies installed');
} catch (error) {
    console.error('❌ Failed to install dependencies');
    process.exit(1);
}

// Configure and build
console.log('\n🔨 Configuring build...');
try {
    execSync('npm run configure', { stdio: 'inherit' });
    console.log('✅ Build configured');
} catch (error) {
    console.error('❌ Build configuration failed');
    process.exit(1);
}

console.log('\n🔨 Building project...');
try {
    execSync('npm run build', { stdio: 'inherit' });
    console.log('✅ Build completed');
} catch (error) {
    console.error('❌ Build failed');
    process.exit(1);
}

// Test the build
console.log('\n🧪 Testing build...');
try {
    execSync('npm test', { stdio: 'inherit' });
    console.log('✅ Tests passed');
} catch (error) {
    console.error('❌ Tests failed');
    process.exit(1);
}

console.log('\n🎉 Setup completed successfully!');
console.log('\n📚 Next steps:');
console.log('   1. Check the README.md for usage examples');
console.log('   2. Run "npm test" to verify everything works');
console.log('   3. Start building your screen capture app!');

if (!obsFound) {
    console.log('\n⚠️  Note: OBS integration is not available');
    console.log('   The addon will work with basic functionality only.');
    console.log('   Install OBS Studio for full recording capabilities.');
} else {
    console.log('\n✅ OBS integration is ready!');
    console.log('   You have full screen recording capabilities.');
}

console.log('\n🚀 Happy coding!');