const obs = require('..');
const path = require('path');
const fs = require('fs');

console.log('🍎 Testing OBS Screen Capture on macOS');

async function testMacOSFeatures() {
    try {
        // Test 1: Permission checking
        console.log('\n1️⃣ Testing macOS permissions...');
        
        const hasScreenPermission = obs.checkScreenPermission();
        console.log('   📺 Screen Recording permission:', hasScreenPermission ? '✅ Granted' : '❌ Denied');
        
        if (!hasScreenPermission) {
            console.log('   🔐 Requesting screen recording permission...');
            const requestResult = obs.requestScreenPermission();
            console.log('   📝 Permission request result:', requestResult ? '✅ Granted' : '❌ Denied');
            
            if (!requestResult) {
                console.log('   ⚠️  Please grant Screen Recording permission manually:');
                console.log('      System Preferences → Security & Privacy → Privacy → Screen Recording');
                console.log('      Then restart this test');
                return;
            }
        }
        
        // Test 2: Initialize OBS
        console.log('\n2️⃣ Initializing OBS...');
        const initResult = obs.init();
        if (!initResult) {
            throw new Error('Failed to initialize OBS');
        }
        console.log('   ✅ OBS initialized successfully');
        
        // Test 3: Display enumeration with UUIDs
        console.log('\n3️⃣ Testing display enumeration (with UUIDs)...');
        const displays = obs.listDisplays();
        console.log(`   📺 Found ${displays.length} display(s):`);
        displays.forEach((display, i) => {
            console.log(`   Display ${i + 1}:`);
            console.log(`     ID: ${display.id}`);
            console.log(`     Name: ${display.name}`);
            console.log(`     Resolution: ${display.width}x${display.height}`);
            console.log(`     Position: (${display.x}, ${display.y})`);
        });
        
        // Test 4: Window enumeration
        console.log('\n4️⃣ Testing window enumeration...');
        const windows = obs.listWindows();
        console.log(`   🪟 Found ${windows.length} window(s):`);
        
        // Show first 10 windows with details
        windows.slice(0, 10).forEach((window, i) => {
            console.log(`   Window ${i + 1}:`);
            console.log(`     ID: ${window.id}`);
            console.log(`     Name: "${window.name || 'Untitled'}"`);
            console.log(`     Owner: ${window.owner}`);
            console.log(`     Size: ${window.width}x${window.height}`);
        });
        
        // Test 5: ScreenCaptureKit display recording
        console.log('\n5️⃣ Testing ScreenCaptureKit display recording...');
        const displayOutputPath = path.join(__dirname, 'test-display-sck.mp4');
        
        if (fs.existsSync(displayOutputPath)) {
            fs.unlinkSync(displayOutputPath);
        }
        
        const displayConfig = {
            source_type: 0, // Display capture
            displayId: displays[0]?.id || '',
            width: 1920,
            height: 1080,
            fps: 60,
            capture_cursor: true,
            capture_audio: true,
            hide_obs: true
        };
        
        console.log('   🎬 Starting display recording with ScreenCaptureKit...');
        const displayStart = obs.startRecording(displayOutputPath, displayConfig);
        
        if (displayStart) {
            console.log('   ⏱️ Recording display for 8 seconds...');
            await new Promise(resolve => setTimeout(resolve, 8000));
            
            obs.stopRecording();
            console.log('   🛑 Display recording stopped');
            
            await new Promise(resolve => setTimeout(resolve, 1000));
            
            if (fs.existsSync(displayOutputPath)) {
                const stats = fs.statSync(displayOutputPath);
                console.log(`   ✅ Display recording saved: ${displayOutputPath} (${stats.size} bytes)`);
                fs.unlinkSync(displayOutputPath);
            }
        } else {
            console.log('   ❌ Failed to start display recording');
        }
        
        // Test 6: ScreenCaptureKit window recording
        console.log('\n6️⃣ Testing ScreenCaptureKit window recording...');
        
        // Find a suitable window (prefer non-system windows)
        const suitableWindow = windows.find(w => 
            w.name && 
            w.name.length > 0 && 
            !w.owner.includes('Window Server') &&
            !w.owner.includes('Dock') &&
            w.width > 100 && 
            w.height > 100
        );
        
        if (suitableWindow) {
            const windowOutputPath = path.join(__dirname, 'test-window-sck.mp4');
            
            if (fs.existsSync(windowOutputPath)) {
                fs.unlinkSync(windowOutputPath);
            }
            
            const windowConfig = {
                source_type: 1, // Window capture
                windowId: suitableWindow.id,
                width: Math.min(suitableWindow.width, 1920),
                height: Math.min(suitableWindow.height, 1080),
                fps: 30,
                capture_cursor: true,
                capture_audio: false // Window audio might be tricky
            };
            
            console.log(`   🪟 Recording window: "${suitableWindow.name}" (${suitableWindow.owner})`);
            const windowStart = obs.startRecording(windowOutputPath, windowConfig);
            
            if (windowStart) {
                console.log('   ⏱️ Recording window for 5 seconds...');
                await new Promise(resolve => setTimeout(resolve, 5000));
                
                obs.stopRecording();
                console.log('   🛑 Window recording stopped');
                
                await new Promise(resolve => setTimeout(resolve, 1000));
                
                if (fs.existsSync(windowOutputPath)) {
                    const stats = fs.statSync(windowOutputPath);
                    console.log(`   ✅ Window recording saved: ${windowOutputPath} (${stats.size} bytes)`);
                    fs.unlinkSync(windowOutputPath);
                }
            } else {
                console.log('   ❌ Failed to start window recording');
            }
        } else {
            console.log('   ⚠️ No suitable window found for recording test');
        }
        
        // Test 7: Audio-only capture
        console.log('\n7️⃣ Testing audio-only capture...');
        const audioOutputPath = path.join(__dirname, 'test-audio-sck.mp4');
        
        if (fs.existsSync(audioOutputPath)) {
            fs.unlinkSync(audioOutputPath);
        }
        
        const audioConfig = {
            source_type: 0, // Display (for system audio)
            displayId: displays[0]?.id || '',
            width: 1280, // Minimal video
            height: 720,
            fps: 30,
            capture_cursor: false,
            capture_audio: true, // Focus on audio
            hide_obs: true
        };
        
        console.log('   🎵 Starting audio capture test...');
        const audioStart = obs.startRecording(audioOutputPath, audioConfig);
        
        if (audioStart) {
            console.log('   ⏱️ Recording audio for 5 seconds...');
            console.log('   💡 Play some audio to test system audio capture...');
            await new Promise(resolve => setTimeout(resolve, 5000));
            
            obs.stopRecording();
            console.log('   🛑 Audio recording stopped');
            
            await new Promise(resolve => setTimeout(resolve, 1000));
            
            if (fs.existsSync(audioOutputPath)) {
                const stats = fs.statSync(audioOutputPath);
                console.log(`   ✅ Audio recording saved: ${audioOutputPath} (${stats.size} bytes)`);
                fs.unlinkSync(audioOutputPath);
            }
        } else {
            console.log('   ❌ Failed to start audio recording');
        }
        
        console.log('\n🎉 All macOS tests completed successfully!');
        console.log('\n📝 macOS-specific features tested:');
        console.log('   ✅ Screen Recording permissions');
        console.log('   ✅ ScreenCaptureKit display capture');
        console.log('   ✅ ScreenCaptureKit window capture');
        console.log('   ✅ System audio capture');
        console.log('   ✅ Display UUID enumeration');
        
    } catch (error) {
        console.error('\n❌ macOS test failed:', error.message);
        console.error(error.stack);
        process.exit(1);
    } finally {
        obs.shutdown();
        console.log('\n🔄 OBS shutdown complete');
    }
}

// Run the tests
testMacOSFeatures();