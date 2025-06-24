#include "obs_wrapper.h"
#include <filesystem>
#include <iostream>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#elif defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#endif

OBSManager& OBSManager::getInstance() {
    static OBSManager instance;
    return instance;
}

OBSManager::~OBSManager() {
    if (initialized_) {
        shutdown();
    }
}

bool OBSManager::initialize() {
    if (initialized_) {
        return true;
    }
    
    // For now, just initialize without OBS
    std::cout << "Initializing screen capture manager..." << std::endl;
    initialized_ = true;
    return true;
}

void OBSManager::shutdown() {
    if (!initialized_) {
        return;
    }
    
    if (recording_) {
        stopRecording();
    }
    
    std::cout << "Shutting down screen capture manager..." << std::endl;
    initialized_ = false;
}

// Helper methods removed - will be implemented when adding actual screen capture

std::vector<DisplayInfo> OBSManager::getDisplays() {
    std::vector<DisplayInfo> displays;
    
#ifdef __APPLE__
    uint32_t count = 0;
    CGGetActiveDisplayList(0, nullptr, &count);
    
    if (count > 0) {
        std::vector<CGDirectDisplayID> display_ids(count);
        CGGetActiveDisplayList(count, display_ids.data(), &count);
        
        for (uint32_t i = 0; i < count; ++i) {
            CGDirectDisplayID display_id = display_ids[i];
            
            DisplayInfo info;
            info.id = std::to_string(display_id); // Use display ID directly
            info.width = CGDisplayPixelsWide(display_id);
            info.height = CGDisplayPixelsHigh(display_id);
            
            CGRect bounds = CGDisplayBounds(display_id);
            info.x = bounds.origin.x;
            info.y = bounds.origin.y;
            
            // Try to get display name
            info.name = "Display " + std::to_string(i + 1);
            
            displays.push_back(info);
        }
    }
#elif defined(_WIN32)
    // Windows display enumeration
    struct EnumData { std::vector<DisplayInfo>* displays; int index; } data{&displays, 0};
    
    EnumDisplayMonitors(NULL, NULL, [](HMONITOR monitor, HDC, LPRECT rect, LPARAM lparam) -> BOOL {
        EnumData* data = reinterpret_cast<EnumData*>(lparam);
        
        MONITORINFOEXA mi;
        mi.cbSize = sizeof(mi);
        if (GetMonitorInfoA(monitor, (MONITORINFO*)&mi)) {
            DisplayInfo info;
            info.id = std::to_string(reinterpret_cast<uintptr_t>(monitor));
            info.name = std::string(mi.szDevice);
            info.width = mi.rcMonitor.right - mi.rcMonitor.left;
            info.height = mi.rcMonitor.bottom - mi.rcMonitor.top;
            info.x = mi.rcMonitor.left;
            info.y = mi.rcMonitor.top;
            data->displays->push_back(info);
        }
        data->index++;
        return TRUE;
    }, reinterpret_cast<LPARAM>(&data));
    
#else // Linux
    Display* display = XOpenDisplay(nullptr);
    if (display) {
        Window root = DefaultRootWindow(display);
        XRRScreenResources* resources = XRRGetScreenResourcesCurrent(display, root);
        
        if (resources) {
            for (int i = 0; i < resources->noutput; ++i) {
                XRROutputInfo* output_info = XRRGetOutputInfo(display, resources, resources->outputs[i]);
                
                if (output_info && output_info->connection == RR_Connected && output_info->crtc) {
                    XRRCrtcInfo* crtc_info = XRRGetCrtcInfo(display, resources, output_info->crtc);
                    
                    if (crtc_info) {
                        DisplayInfo info;
                        info.id = std::to_string(resources->outputs[i]);
                        info.name = std::string(output_info->name);
                        info.width = crtc_info->width;
                        info.height = crtc_info->height;
                        info.x = crtc_info->x;
                        info.y = crtc_info->y;
                        displays.push_back(info);
                        
                        XRRFreeCrtcInfo(crtc_info);
                    }
                }
                
                if (output_info) {
                    XRRFreeOutputInfo(output_info);
                }
            }
            XRRFreeScreenResources(resources);
        }
        XCloseDisplay(display);
    }
#endif
    
    return displays;
}

std::vector<WindowInfo> OBSManager::getWindows() {
    std::vector<WindowInfo> windows;
    
#ifdef __APPLE__
    CFArrayRef window_list = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);
    CFIndex count = CFArrayGetCount(window_list);
    
    for (CFIndex i = 0; i < count; ++i) {
        CFDictionaryRef window_dict = (CFDictionaryRef)CFArrayGetValueAtIndex(window_list, i);
        
        CFNumberRef window_id_ref = (CFNumberRef)CFDictionaryGetValue(window_dict, kCGWindowNumber);
        CFStringRef owner_name_ref = (CFStringRef)CFDictionaryGetValue(window_dict, kCGWindowOwnerName);
        CFStringRef window_name_ref = (CFStringRef)CFDictionaryGetValue(window_dict, kCGWindowName);
        CFDictionaryRef bounds_ref = (CFDictionaryRef)CFDictionaryGetValue(window_dict, kCGWindowBounds);
        
        if (window_id_ref && owner_name_ref) {
            WindowInfo info;
            
            CFNumberGetValue(window_id_ref, kCFNumberSInt64Type, &info.id);
            
            char owner_cstr[256] = {0};
            CFStringGetCString(owner_name_ref, owner_cstr, sizeof(owner_cstr), kCFStringEncodingUTF8);
            info.owner = std::string(owner_cstr);
            
            if (window_name_ref) {
                char window_cstr[256] = {0};
                CFStringGetCString(window_name_ref, window_cstr, sizeof(window_cstr), kCFStringEncodingUTF8);
                info.name = std::string(window_cstr);
            }
            
            if (bounds_ref) {
                CGRect bounds;
                CGRectMakeWithDictionaryRepresentation(bounds_ref, &bounds);
                info.x = bounds.origin.x;
                info.y = bounds.origin.y;
                info.width = bounds.size.width;
                info.height = bounds.size.height;
            }
            
            windows.push_back(info);
        }
    }
    
    CFRelease(window_list);
#elif defined(_WIN32)
    // Windows window enumeration
    struct WinData { std::vector<WindowInfo>* windows; } data{&windows};
    
    EnumWindows([](HWND hwnd, LPARAM lparam) -> BOOL {
        WinData* data = reinterpret_cast<WinData*>(lparam);
        
        if (!IsWindowVisible(hwnd)) return TRUE;
        
        char title[256] = {0};
        char class_name[256] = {0};
        GetWindowTextA(hwnd, title, sizeof(title));
        GetClassNameA(hwnd, class_name, sizeof(class_name));
        
        RECT rect;
        GetWindowRect(hwnd, &rect);
        
        WindowInfo info;
        info.id = reinterpret_cast<uint64_t>(hwnd);
        info.name = std::string(title);
        info.owner = std::string(class_name);
        info.x = rect.left;
        info.y = rect.top;
        info.width = rect.right - rect.left;
        info.height = rect.bottom - rect.top;
        
        data->windows->push_back(info);
        return TRUE;
    }, reinterpret_cast<LPARAM>(&data));
#endif
    
    return windows;
}

bool OBSManager::startRecording(const std::string& output_path, const RecordingConfig& config) {
    if (!initialized_ || recording_) {
        return false;
    }
    
    std::cout << "Starting recording to: " << output_path << std::endl;
    std::cout << "Resolution: " << config.width << "x" << config.height << std::endl;
    std::cout << "FPS: " << config.fps << std::endl;
    
    // For now, just simulate recording
    recording_ = true;
    return true;
}

void OBSManager::stopRecording() {
    if (!recording_) {
        return;
    }
    
    std::cout << "Stopping recording..." << std::endl;
    recording_ = false;
}

// Placeholder implementations - will be replaced with actual screen capture logic