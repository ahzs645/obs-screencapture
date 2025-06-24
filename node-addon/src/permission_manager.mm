#include <Foundation/Foundation.h>

#ifdef __APPLE__
#import <AVFoundation/AVFoundation.h>

// For macOS 10.15+
bool HasScreenCapturePermission() {
    if (@available(macOS 10.15, *)) {
        return CGPreflightScreenCaptureAccess();
    }
    return true; // Assume permission on older versions
}

bool RequestScreenCapturePermission() {
    if (@available(macOS 10.15, *)) {
        return CGRequestScreenCaptureAccess();
    }
    return true;
}

bool HasMicrophonePermission() {
    AVAuthorizationStatus status = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeAudio];
    return status == AVAuthorizationStatusAuthorized;
}

bool RequestMicrophonePermission() {
    AVAuthorizationStatus status = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeAudio];
    
    if (status == AVAuthorizationStatusNotDetermined) {
        dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
        __block BOOL granted = NO;
        
        [AVCaptureDevice requestAccessForMediaType:AVMediaTypeAudio completionHandler:^(BOOL granted_inner) {
            granted = granted_inner;
            dispatch_semaphore_signal(semaphore);
        }];
        
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        return granted;
    }
    
    return status == AVAuthorizationStatusAuthorized;
}

#else

bool HasScreenCapturePermission() { return true; }
bool RequestScreenCapturePermission() { return true; }
bool HasMicrophonePermission() { return true; }
bool RequestMicrophonePermission() { return true; }

#endif