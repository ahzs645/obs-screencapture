const obs = require('..');
const path = require('path');
const fs = require('fs');

console.log('🧪 Testing OBS Screen Capture');
console.log('Platform:', process.platform);

async function runTests() {
    try {
        // Test 1: Initialize OBS
        console.log('\n1️⃣ Testing OBS initialization...');
        const initResult = obs.init();
        console.log('   ✅ OBS initialized:', initResult);
        
        if (!initResult) {
            throw new Error('Failed to initialize OBS');
        }
        
        // Test 2: List displays
        console.log('\n2️⃣ Testing display enumeration...');
        const displays = obs.listDisplays();
        console.log('   📺 Found displays:', displays.length);
        displays.forEach((display, i) => {
            console.log(`   Display ${i}: ${display.name} (${display.width}x${display.height})`);
        });
        
        // Test 3: List windows  
        console.log('\n3️⃣ Testing window enumeration...');
        const windows = obs.listWindows();
        console.log('   🪟 Found windows:', windows.length);
        windows.slice(0, 5).forEach((window, i) => {
            console.log(`   Window ${i}: "${window.name}" by ${window.owner}`);
        });
        
        // Test 4: Check permissions (platform specific)
        if (process.platform === 'darwin') {
            console.log('\n4️⃣ Testing macOS permissions...');
            const hasPermission = obs.checkScreenPermission();
            console.log('   🔐 Screen permission:', hasPermission ? '✅ Granted' : '❌ Not granted');
            
            if (!hasPermission) {
                console.log('   💡 Run: obs.requestScreenPermission() to request permission');
            }
        }
        
        // Test 5: Recording test
        console.log('\n5️⃣ Testing recording (5 second test)...');
        const outputPath = path.join(__dirname, 'test-recording.mp4');
        
        // Clean up any existing test file
        if (fs.existsSync(outputPath)) {
            fs.unlinkSync(outputPath);
        }
        
        const recordingConfig = {
            width: 1280,
            height: 720,
            fps: 30,
            displayId: displays[0] ? displays[0].id : '',
            capture_audio: false // Disable audio for simple test
        };
        
        const startResult = obs.startRecording(outputPath, recordingConfig);
        console.log('   🎬 Recording started:', startResult ? '✅ Success' : '❌ Failed');
        
        if (startResult) {
            console.log('   ⏱️ Recording for 5 seconds...');
            
            // Record for 5 seconds
            await new Promise(resolve => setTimeout(resolve, 5000));
            
            obs.stopRecording();
            console.log('   🛑 Recording stopped');
            
            // Check if file was created
            await new Promise(resolve => setTimeout(resolve, 1000)); // Wait for file to be written
            
            if (fs.existsSync(outputPath)) {
                const stats = fs.statSync(outputPath);
                console.log(`   📁 Output file created: ${outputPath} (${stats.size} bytes)`);
                
                // Clean up test file
                fs.unlinkSync(outputPath);
                console.log('   🧹 Test file cleaned up');
            } else {
                console.log('   ❌ Output file was not created');
            }
        }
        
        console.log('\n✅ All tests completed successfully!');
        
    } catch (error) {
        console.error('\n❌ Test failed:', error.message);
        console.error(error.stack);
        process.exit(1);
    } finally {
        // Always shutdown OBS
        obs.shutdown();
        console.log('🔄 OBS shutdown complete');
    }
}

// Platform-specific guidance
function showPlatformGuidance() {
    console.log('\n📋 Platform-specific notes:');
    
    if (process.platform === 'darwin') {
        console.log('🍎 macOS:');
        console.log('   - Requires Screen Recording permission');
        console.log('   - System Preferences → Security & Privacy → Privacy → Screen Recording');
        console.log('   - Uses ScreenCaptureKit for modern capture');
    } else if (process.platform === 'win32') {
        console.log('🪟 Windows:');
        console.log('   - No special permissions required');
        console.log('   - Uses Windows Graphics Capture API');
    } else if (process.platform === 'linux') {
        console.log('🐧 Linux:');
        console.log('   - Requires X11 development libraries');
        console.log('   - PulseAudio for audio capture');
        console.log('   - May need to run with elevated permissions');
    }
}

showPlatformGuidance();
runTests();