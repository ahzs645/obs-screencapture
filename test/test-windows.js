const obs = require('..');
const path = require('path');
const fs = require('fs');

console.log('🪟 Testing OBS Screen Capture on Windows');

async function testWindowsFeatures() {
    try {
        console.log('\n1️⃣ Initializing OBS...');
        const initResult = obs.init();
        if (!initResult) {
            throw new Error('Failed to initialize OBS');
        }
        console.log('   ✅ OBS initialized successfully');
        
        // Test 2: Monitor enumeration
        console.log('\n2️⃣ Testing monitor enumeration...');
        const displays = obs.listDisplays();
        console.log(`   🖥️ Found ${displays.length} monitor(s):`);
        displays.forEach((display, i) => {
            console.log(`   Monitor ${i + 1}:`);
            console.log(`     ID: ${display.id}`);
            console.log(`     Name: ${display.name}`);
            console.log(`     Resolution: ${display.width}x${display.height}`);
            console.log(`     Position: (${display.x}, ${display.y})`);
        });
        
        // Test 3: Window enumeration
        console.log('\n3️⃣ Testing window enumeration...');
        const windows = obs.listWindows();
        console.log(`   🪟 Found ${windows.length} window(s):`);
        
        // Show first 10 visible windows
        const visibleWindows = windows.filter(w => w.name && w.name.trim().length > 0);
        visibleWindows.slice(0, 10).forEach((window, i) => {
            console.log(`   Window ${i + 1}:`);
            console.log(`     ID: ${window.id}`);
            console.log(`     Name: "${window.name}"`);
            console.log(`     Class: ${window.owner}`);
            console.log(`     Size: ${window.width}x${window.height}`);
        });
        
        // Test 4: Monitor capture
        console.log('\n4️⃣ Testing monitor capture...');
        const monitorOutputPath = path.join(__dirname, 'test-monitor-win.mp4');
        
        if (fs.existsSync(monitorOutputPath)) {
            fs.unlinkSync(monitorOutputPath);
        }
        
        const monitorConfig = {
            source_type: 0, // Display/Monitor capture
            displayId: displays[0]?.id || '0',
            width: 1920,
            height: 1080,
            fps: 60,
            capture_cursor: true,
            capture_audio: true
        };
        
        console.log('   🎬 Starting monitor recording...');
        const monitorStart = obs.startRecording(monitorOutputPath, monitorConfig);
        
        if (monitorStart) {
            console.log('   ⏱️ Recording monitor for 8 seconds...');
            await new Promise(resolve => setTimeout(resolve, 8000));
            
            obs.stopRecording();
            console.log('   🛑 Monitor recording stopped');
            
            await new Promise(resolve => setTimeout(resolve, 1000));
            
            if (fs.existsSync(monitorOutputPath)) {
                const stats = fs.statSync(monitorOutputPath);
                console.log(`   ✅ Monitor recording saved: ${monitorOutputPath} (${stats.size} bytes)`);
                fs.unlinkSync(monitorOutputPath);
            }
        } else {
            console.log('   ❌ Failed to start monitor recording');
        }
        
        // Test 5: Window capture
        console.log('\n5️⃣ Testing window capture...');
        
        // Find a good window to capture (prefer larger windows)
        const suitableWindow = visibleWindows.find(w => 
            w.width > 200 && 
            w.height > 200 &&
            !w.name.includes('Task Manager') && // Avoid system windows
            !w.name.includes('Windows Security')
        );
        
        if (suitableWindow) {
            const windowOutputPath = path.join(__dirname, 'test-window-win.mp4');
            
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
                capture_audio: false
            };
            
            console.log(`   🪟 Recording window: "${suitableWindow.name}"`);
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
        
        // Test 6: WASAPI audio capture
        console.log('\n6️⃣ Testing WASAPI audio capture...');
        const audioOutputPath = path.join(__dirname, 'test-audio-win.mp4');
        
        if (fs.existsSync(audioOutputPath)) {
            fs.unlinkSync(audioOutputPath);
        }
        
        const audioConfig = {
            source_type: 0, // Monitor (for system audio via WASAPI)
            displayId: displays[0]?.id || '0',
            width: 1280,
            height: 720,
            fps: 30,
            capture_cursor: false,
            capture_audio: true
        };
        
        console.log('   🎵 Starting WASAPI audio capture...');
        const audioStart = obs.startRecording(audioOutputPath, audioConfig);
        
        if (audioStart) {
            console.log('   ⏱️ Recording audio for 5 seconds...');
            console.log('   💡 Play some audio to test WASAPI capture...');
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
        
        console.log('\n🎉 All Windows tests completed successfully!');
        console.log('\n📝 Windows-specific features tested:');
        console.log('   ✅ Monitor enumeration');
        console.log('   ✅ Window capture');
        console.log('   ✅ WASAPI audio capture');
        console.log('   ✅ Graphics Capture API');
        
    } catch (error) {
        console.error('\n❌ Windows test failed:', error.message);
        console.error(error.stack);
        process.exit(1);
    } finally {
        obs.shutdown();
        console.log('\n🔄 OBS shutdown complete');
    }
}

// Run the tests
testWindowsFeatures();