#!/usr/bin/env node

const obs = require('./index.js');
const path = require('path');

async function captureMainScreen() {
    console.log('🎬 Starting screen capture test...\n');
    
    // Initialize OBS
    console.log('1️⃣ Initializing OBS...');
    if (!obs.init()) {
        console.error('❌ Failed to initialize OBS');
        return;
    }
    console.log('✅ OBS initialized successfully\n');
    
    // Get available displays
    console.log('2️⃣ Enumerating displays...');
    const displays = obs.listDisplays();
    if (displays.length === 0) {
        console.error('❌ No displays found');
        obs.shutdown();
        return;
    }
    
    console.log(`📺 Found ${displays.length} display(s):`);
    displays.forEach((display, index) => {
        console.log(`   Display ${index}: ${display.name} (${display.width}x${display.height})`);
    });
    console.log();
    
    // Use the first display (main screen)
    const mainDisplay = displays[0];
    console.log(`🎯 Using main display: ${mainDisplay.name} (${mainDisplay.width}x${mainDisplay.height})\n`);
    
    // Create output filename with timestamp
    const timestamp = new Date().toISOString().replace(/[:.]/g, '-').slice(0, -5);
    const outputPath = path.join(__dirname, 'recordings', `screen-capture-${timestamp}.mp4`);
    
    // Recording configuration
    const config = {
        displayId: mainDisplay.id,
        width: Math.min(mainDisplay.width, 1920),  // Cap at 1920 for reasonable file size
        height: Math.min(mainDisplay.height, 1080), // Cap at 1080 for reasonable file size
        fps: 30,
        capture_audio: true
    };
    
    console.log('3️⃣ Starting recording...');
    console.log(`   📁 Output: ${outputPath}`);
    console.log(`   📐 Resolution: ${config.width}x${config.height}`);
    console.log(`   🎬 FPS: ${config.fps}`);
    console.log(`   🎵 Audio: ${config.capture_audio ? 'Yes' : 'No'}`);
    console.log();
    
    if (obs.startRecording(outputPath, config)) {
        console.log('✅ Recording started successfully!');
        console.log('⏱️  Recording for 10 seconds...\n');
        
        // Record for 10 seconds
        setTimeout(() => {
            console.log('🛑 Stopping recording...');
            obs.stopRecording();
            console.log('✅ Recording stopped');
            
            // Check if file was created
            const fs = require('fs');
            if (fs.existsSync(outputPath)) {
                const stats = fs.statSync(outputPath);
                const fileSizeMB = (stats.size / (1024 * 1024)).toFixed(2);
                console.log(`📁 File created: ${outputPath}`);
                console.log(`📊 File size: ${fileSizeMB} MB`);
                console.log('🎉 Screen capture completed successfully!');
            } else {
                console.log('⚠️  Note: File was not created (this is expected in foundation mode)');
                console.log('   To get actual video files, you need OBS integration enabled');
            }
            
            obs.shutdown();
            console.log('🔄 OBS shutdown complete');
        }, 10000);
        
    } else {
        console.error('❌ Failed to start recording');
        obs.shutdown();
    }
}

// Handle errors
process.on('uncaughtException', (err) => {
    console.error('❌ Uncaught Exception:', err);
    process.exit(1);
});

process.on('unhandledRejection', (reason, promise) => {
    console.error('❌ Unhandled Rejection at:', promise, 'reason:', reason);
    process.exit(1);
});

// Run the capture
captureMainScreen(); 