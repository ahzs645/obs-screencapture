#!/bin/bash
# test-build.sh

echo "🧪 Testing OBS Screen Capture Build"
echo "Platform: $(uname -s)"

# Check prerequisites
echo "Checking prerequisites..."

if ! command -v node &> /dev/null; then
    echo "❌ Node.js not found"
    exit 1
fi

if ! command -v cmake &> /dev/null; then
    echo "❌ CMake not found" 
    exit 1
fi

echo "✅ Prerequisites OK"

# Test configuration
echo "Testing CMake configuration..."
if npm run configure; then
    echo "✅ Configuration successful"
else
    echo "❌ Configuration failed"
    exit 1
fi

# Test build
echo "Testing build..."
if npm run build; then
    echo "✅ Build successful"
else
    echo "❌ Build failed"
    exit 1
fi

# Test loading
echo "Testing module loading..."
if node -e "const obs = require('./index.js'); console.log('Module loaded successfully')"; then
    echo "✅ Module loads successfully"
else
    echo "❌ Module loading failed"
    exit 1
fi

echo "🎉 All build tests passed!"