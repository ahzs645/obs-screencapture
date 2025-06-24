#pragma once
#include <string>
#include <vector>
#include <memory>

struct DisplayInfo {
    std::string id;
    std::string name;
    int width;
    int height;
    int x;
    int y;
};

struct WindowInfo {
    uint64_t id;
    std::string name;
    std::string owner;
    int width;
    int height;
    int x;
    int y;
};

struct RecordingConfig {
    // Source type
    enum SourceType { 
        DISPLAY = 0, 
        WINDOW = 1, 
        APPLICATION = 2 
    } source_type = DISPLAY;
    
    // Source identifiers
    std::string display_id;
    uint64_t window_id = 0;
    std::string application_id;
    
    // Output settings
    int width = 1920;
    int height = 1080;
    int fps = 60;
    int video_bitrate = 8000;
    int audio_bitrate = 160;
    
    // Capture settings
    bool capture_cursor = true;
    bool capture_audio = true;
    bool hide_obs = true; // macOS only
    bool show_empty_names = false;
    bool show_hidden_windows = false;
};

class OBSManager {
public:
    static OBSManager& getInstance();
    
    // Core OBS management
    bool initialize();
    void shutdown();
    bool isInitialized() const { return initialized_; }
    
    // Source enumeration
    std::vector<DisplayInfo> getDisplays();
    std::vector<WindowInfo> getWindows();
    
    // Recording control
    bool startRecording(const std::string& output_path, const RecordingConfig& config);
    void stopRecording();
    bool isRecording() const { return recording_; }
    
    // Audio control
    bool setSystemAudioEnabled(bool enabled);
    bool isCaptureAudioSupported();
    
private:
    OBSManager() = default;
    ~OBSManager();
    
    // Prevent copying
    OBSManager(const OBSManager&) = delete;
    OBSManager& operator=(const OBSManager&) = delete;
    
    bool initialized_ = false;
    bool recording_ = false;
    
    // OBS objects (using void* to avoid including obs headers here)
    void* obs_output_ = nullptr;
    void* video_encoder_ = nullptr;
    void* audio_encoder_ = nullptr;
    void* video_source_ = nullptr;
    void* audio_source_ = nullptr;
    void* scene_ = nullptr;
    
    // Internal methods
    void setupPluginPaths();
    bool loadRequiredPlugins();
    void cleanupRecording();
    bool setupVideoOutput(const RecordingConfig& config);
    bool setupAudioOutput(const RecordingConfig& config);
    bool createVideoSource(const RecordingConfig& config);
    bool createAudioSource(const RecordingConfig& config);
    std::string getPluginPath() const;
    std::string getDataPath() const;
};