#include <napi.h>
#include "obs_wrapper.h"

// Add the missing permission functions
Napi::Boolean CheckScreenPermission(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
#ifdef __APPLE__
    // Use the function from permission_manager.mm
    extern bool HasScreenCapturePermission();
    return Napi::Boolean::New(env, HasScreenCapturePermission());
#else
    return Napi::Boolean::New(env, true); // Always true on other platforms
#endif
}

Napi::Boolean RequestScreenPermission(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
#ifdef __APPLE__
    extern bool RequestScreenCapturePermission();
    return Napi::Boolean::New(env, RequestScreenCapturePermission());
#else
    return Napi::Boolean::New(env, true);
#endif
}

// Add the missing ListWindows function
Napi::Value ListWindows(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Array arr = Napi::Array::New(env);
    
    auto windows = OBSManager::getInstance().getWindows();
    for (size_t i = 0; i < windows.size(); i++) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("id", Napi::Number::New(env, windows[i].id));
        obj.Set("name", windows[i].name);
        obj.Set("owner", windows[i].owner);
        obj.Set("width", windows[i].width);
        obj.Set("height", windows[i].height);
        obj.Set("x", windows[i].x);
        obj.Set("y", windows[i].y);
        arr.Set(i, obj);
    }
    return arr;
}

Napi::Boolean InitOBS(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    bool success = OBSManager::getInstance().initialize();
    return Napi::Boolean::New(env, success);
}

Napi::Value ListDisplays(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Array arr = Napi::Array::New(env);
    
    auto displays = OBSManager::getInstance().getDisplays();
    for (size_t i = 0; i < displays.size(); i++) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("id", displays[i].id);
        obj.Set("name", displays[i].name);
        obj.Set("width", displays[i].width);
        obj.Set("height", displays[i].height);
        obj.Set("x", displays[i].x);
        obj.Set("y", displays[i].y);
        arr.Set(i, obj);
    }
    return arr;
}

Napi::Boolean StartRecording(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Output path required").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }
    
    std::string path = info[0].As<Napi::String>();
    RecordingConfig config;
    
    if (info.Length() > 1 && info[1].IsObject()) {
        Napi::Object opts = info[1].As<Napi::Object>();
        if (opts.Has("width")) config.width = opts.Get("width").As<Napi::Number>().Int32Value();
        if (opts.Has("height")) config.height = opts.Get("height").As<Napi::Number>().Int32Value();
        if (opts.Has("fps")) config.fps = opts.Get("fps").As<Napi::Number>().Int32Value();
        if (opts.Has("displayId")) config.display_id = opts.Get("displayId").As<Napi::String>();
        if (opts.Has("windowId")) {
            config.window_id = opts.Get("windowId").As<Napi::Number>().Int64Value();
            config.source_type = RecordingConfig::WINDOW;
        }
        if (opts.Has("capture_audio")) config.capture_audio = opts.Get("capture_audio").As<Napi::Boolean>();
    }
    
    bool success = OBSManager::getInstance().startRecording(path, config);
    return Napi::Boolean::New(env, success);
}

Napi::Value StopRecording(const Napi::CallbackInfo& info) {
    OBSManager::getInstance().stopRecording();
    return info.Env().Undefined();
}

Napi::Value Shutdown(const Napi::CallbackInfo& info) {
    OBSManager::getInstance().shutdown();
    return info.Env().Undefined();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("init", Napi::Function::New(env, InitOBS));
    exports.Set("shutdown", Napi::Function::New(env, Shutdown));
    exports.Set("listDisplays", Napi::Function::New(env, ListDisplays));
    exports.Set("listWindows", Napi::Function::New(env, ListWindows));
    exports.Set("startRecording", Napi::Function::New(env, StartRecording));
    exports.Set("stopRecording", Napi::Function::New(env, StopRecording));
    exports.Set("checkScreenPermission", Napi::Function::New(env, CheckScreenPermission));
    exports.Set("requestScreenPermission", Napi::Function::New(env, RequestScreenPermission));
    return exports;
}

NODE_API_MODULE(obs_screen_capture, Init)