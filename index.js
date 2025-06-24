const path = require('path');
const fs = require('fs');

// Find the compiled addon
const addonPath = path.join(__dirname, 'build', 'node-addon', 'obs_screen_capture.node');

if (!fs.existsSync(addonPath)) {
    throw new Error('Native addon not found. Run "npm run build" first.');
}

// Set library paths for runtime
const libPath = path.join(__dirname, 'build', 'node-addon', 'lib');
if (process.platform === 'darwin') {
    process.env.DYLD_LIBRARY_PATH = libPath + ':' + (process.env.DYLD_LIBRARY_PATH || '');
} else if (process.platform === 'linux') {
    process.env.LD_LIBRARY_PATH = libPath + ':' + (process.env.LD_LIBRARY_PATH || '');
} else if (process.platform === 'win32') {
    process.env.PATH = libPath + ';' + (process.env.PATH || '');
}

module.exports = require(addonPath);