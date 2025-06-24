#include "obs_wrapper.h"
#include <filesystem>
#include <iostream>

#ifdef HAVE_OBS
#include <obs/obs.h>
#include <obs/obs-module.h>
#endif

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
    
    std::cout << "Initializing OBS core..." << std::endl;
    
#ifdef HAVE_OBS
    // Initialize OBS core
    if (!obs_startup("en-US", nullptr, nullptr)) {
        std::cerr << "Failed to initialize OBS core" << std::endl;
        return false;
    }
    
    // Setup plugin paths
    setupPluginPaths();
    
    // Load required plugins
    if (!loadRequiredPlugins()) {
        std::cerr << "Failed to load required plugins" << std::endl;
        obs_shutdown();
        return false;
    }
    
    // Reset audio and video
    struct obs_audio_info ai = {};
    ai.samples_per_sec = 48000;
    ai.speakers = SPEAKERS_STEREO;
    obs_reset_audio(&ai);
    
    struct obs_video_info ovi = {};
    ovi.base_width = 1920;
    ovi.base_height = 1080;
    ovi.output_width = 1920;
    ovi.output_height = 1080;
    ovi.output_format = VIDEO_FORMAT_NV12;
    ovi.fps_num = 30;
    ovi.fps_den = 1;
    ovi.graphics_module = "libobs-opengl";
    obs_reset_video(&ovi);
    
    std::cout << "OBS core initialized successfully" << std::endl;
#else
    std::cout << "Building without OBS - foundation only" << std::endl;
#endif
    
    initialized_ = true;
    return true;
}

void OBSManager::setupPluginPaths() {
#ifdef HAVE_OBS
    std::cout << "Setting up OBS plugin paths..." << std::endl;
    
#ifdef OBS_PLUGINS_PATH
    // Add the plugins directory
    obs_add_module_path(OBS_PLUGINS_PATH, nullptr);
#endif

#ifdef __APPLE__
    // macOS plugin paths
    obs_add_module_path("/usr/local/lib/obs-plugins", "/usr/local/share/obs/obs-plugins");
    obs_add_module_path("/opt/homebrew/lib/obs-plugins", "/opt/homebrew/share/obs/obs-plugins");
#elif defined(_WIN32)
    // Windows plugin paths
    obs_add_module_path("C:/Program Files/obs-studio/obs-plugins/64bit", "C:/Program Files/obs-studio/data/obs-plugins");
#else
    // Linux plugin paths
    obs_add_module_path("/usr/lib/obs-plugins", "/usr/share/obs/obs-plugins");
    obs_add_module_path("/usr/local/lib/obs-plugins", "/usr/local/share/obs/obs-plugins");
#endif

#endif
}

bool OBSManager::loadRequiredPlugins() {
#ifdef HAVE_OBS
    std::cout << "Loading required OBS plugins..." << std::endl;
    
    // Load all available modules
    obs_load_all_modules();
    
    obs_post_load_modules();
    std::cout << "Plugins loaded successfully" << std::endl;
    return true;
#else
    return true;
#endif
}

void OBSManager::shutdown() {
    if (!initialized_) {
        return;
    }
    
    if (recording_) {
        stopRecording();
    }
    
    std::cout << "Shutting down OBS..." << std::endl;
    
#ifdef HAVE_OBS
    cleanupRecording();
    obs_shutdown();
#endif
    
    initialized_ = false;
}

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
            info.id = std::to_string(display_id);
            info.width = CGDisplayPixelsWide(display_id);
            info.height = CGDisplayPixelsHigh(display_id);
            
            CGRect bounds = CGDisplayBounds(display_id);
            info.x = bounds.origin.x;
            info.y = bounds.origin.y;
            
            info.name = "Display " + std::to_string(i + 1);
            
            displays.push_back(info);
        }
    }
#elif defined(_WIN32)
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
    
    std::cout << "Starting OBS recording to: " << output_path << std::endl;
    
#ifdef HAVE_OBS
    // Setup video
    if (!setupVideoOutput(config)) {
        std::cerr << "Failed to setup video output" << std::endl;
        return false;
    }
    
    // Setup audio if requested
    if (config.capture_audio) {
        if (!setupAudioOutput(config)) {
            std::cerr << "Failed to setup audio output" << std::endl;
            return false;
        }
    }
    
    // Create sources
    if (!createVideoSource(config)) {
        std::cerr << "Failed to create video source" << std::endl;
        return false;
    }
    
    if (config.capture_audio && !createAudioSource(config)) {
        std::cerr << "Failed to create audio source" << std::endl;
        return false;
    }
    
    // Start recording
    obs_output_t* output = obs_output_create("ffmpeg_muxer", "recording_output", nullptr, nullptr);
    if (!output) {
        std::cerr << "Failed to create output" << std::endl;
        return false;
    }
    
    obs_data_t* settings = obs_data_create();
    obs_data_set_string(settings, "path", output_path.c_str());
    obs_output_update(output, settings);
    obs_data_release(settings);
    
    if (!obs_output_start(output)) {
        std::cerr << "Failed to start recording output" << std::endl;
        obs_output_release(output);
        return false;
    }
    
    obs_output_ = output;
    recording_ = true;
    
    std::cout << "Recording started successfully" << std::endl;
    return true;
#else
    std::cout << "Mock recording started (no OBS integration)" << std::endl;
    recording_ = true;
    return true;
#endif
}

void OBSManager::stopRecording() {
    if (!recording_) {
        return;
    }
    
    std::cout << "Stopping recording..." << std::endl;
    
#ifdef HAVE_OBS
    if (obs_output_) {
        obs_output_stop(static_cast<obs_output_t*>(obs_output_));
        obs_output_release(static_cast<obs_output_t*>(obs_output_));
        obs_output_ = nullptr;
    }
#endif
    
    recording_ = false;
    std::cout << "Recording stopped" << std::endl;
}

bool OBSManager::setupVideoOutput(const RecordingConfig& config) {
#ifdef HAVE_OBS
    // Setup video info
    struct obs_video_info ovi = {};
    ovi.graphics_module = "libobs-opengl"; // or "libobs-d3d11" on Windows
    ovi.fps_num = config.fps;
    ovi.fps_den = 1;
    ovi.base_width = config.width;
    ovi.base_height = config.height;
    ovi.output_width = config.width;
    ovi.output_height = config.height;
    ovi.output_format = VIDEO_FORMAT_NV12;
    ovi.adapter = 0;
    ovi.gpu_conversion = true;
    ovi.colorspace = VIDEO_CS_709;
    ovi.range = VIDEO_RANGE_PARTIAL;
    ovi.scale_type = OBS_SCALE_BICUBIC;
    
    if (obs_reset_video(&ovi) != OBS_VIDEO_SUCCESS) {
        std::cerr << "Failed to reset video" << std::endl;
        return false;
    }
    
    return true;
#else
    return true;
#endif
}

bool OBSManager::setupAudioOutput(const RecordingConfig& config) {
#ifdef HAVE_OBS
    struct obs_audio_info ai = {};
    ai.samples_per_sec = 44100;
    ai.speakers = SPEAKERS_STEREO;
    
    if (!obs_reset_audio(&ai)) {
        std::cerr << "Failed to reset audio" << std::endl;
        return false;
    }
    
    return true;
#else
    return true;
#endif
}

bool OBSManager::createVideoSource(const RecordingConfig& config) {
#ifdef HAVE_OBS
    std::string source_id;
    obs_data_t* settings = obs_data_create();
    
#ifdef __APPLE__
    // Use ScreenCaptureKit on macOS
    if (config.source_type == RecordingConfig::WINDOW) {
        source_id = "screen_capture";
        obs_data_set_int(settings, "type", 1); // Window
        obs_data_set_int(settings, "window", config.window_id);
    } else {
        source_id = "screen_capture";
        obs_data_set_int(settings, "type", 0); // Display
        obs_data_set_string(settings, "display_uuid", config.display_id.c_str());
    }
    obs_data_set_bool(settings, "show_cursor", config.capture_cursor);
#elif defined(_WIN32)
    // Use Graphics Capture API on Windows
    if (config.source_type == RecordingConfig::WINDOW) {
        source_id = "window_capture";
    } else {
        source_id = "monitor_capture";
        obs_data_set_int(settings, "monitor", std::stoi(config.display_id));
    }
    obs_data_set_bool(settings, "cursor", config.capture_cursor);
#else
    // Use X11 on Linux
    source_id = "xcomposite_input";
    obs_data_set_int(settings, "screen", std::stoi(config.display_id));
    obs_data_set_bool(settings, "show_cursor", config.capture_cursor);
#endif
    
    obs_source_t* source = obs_source_create(source_id.c_str(), "video_source", settings, nullptr);
    obs_data_release(settings);
    
    if (!source) {
        std::cerr << "Failed to create video source: " << source_id << std::endl;
        return false;
    }
    
    video_source_ = source;
    return true;
#else
    return true;
#endif
}

bool OBSManager::createAudioSource(const RecordingConfig& config) {
#ifdef HAVE_OBS
    std::string source_id;
    
#ifdef __APPLE__
    source_id = "coreaudio_output_capture"; // System audio
#elif defined(_WIN32)
    source_id = "wasapi_output_capture"; // WASAPI
#else
    source_id = "pulse_output_capture"; // PulseAudio
#endif
    
    obs_data_t* settings = obs_data_create();
    obs_source_t* source = obs_source_create(source_id.c_str(), "audio_source", settings, nullptr);
    obs_data_release(settings);
    
    if (!source) {
        std::cerr << "Failed to create audio source: " << source_id << std::endl;
        return false;
    }
    
    audio_source_ = source;
    return true;
#else
    return true;
#endif
}

void OBSManager::cleanupRecording() {
#ifdef HAVE_OBS
    if (video_source_) {
        obs_source_release(static_cast<obs_source_t*>(video_source_));
        video_source_ = nullptr;
    }
    
    if (audio_source_) {
        obs_source_release(static_cast<obs_source_t*>(audio_source_));
        audio_source_ = nullptr;
    }
#endif
}

bool OBSManager::setSystemAudioEnabled(bool enabled) {
    // This would control system audio capture
    return true;
}

bool OBSManager::isCaptureAudioSupported() {
    return true;
}