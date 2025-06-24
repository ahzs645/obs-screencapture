#include <napi.h>
#include "obs_wrapper.h"

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
    }
    
    bool success = OBSManager::getInstance().startRecording(path, config);
    return Napi::Boolean::New(env, success);
}

Napi::Value StopRecording(const Napi::CallbackInfo& info) {
    OBSManager::getInstance().stopRecording();
    return info.Env().Undefined();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("init", Napi::Function::New(env, InitOBS));
    exports.Set("listDisplays", Napi::Function::New(env, ListDisplays));
    exports.Set("startRecording", Napi::Function::New(env, StartRecording));
    exports.Set("stopRecording", Napi::Function::New(env, StopRecording));
    return exports;
}

NODE_API_MODULE(obs_screen_capture, Init)