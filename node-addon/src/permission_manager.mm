// macOS permissions manager
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <ScreenCaptureKit/ScreenCaptureKit.h>

extern "C" {
    bool RequestScreenCapturePermission() {
        if (@available(macOS 11.0, *)) {
            // Request screen capture permission
            return true; // Placeholder
        }
        return false;
    }
    
    bool HasScreenCapturePermission() {
        if (@available(macOS 11.0, *)) {
            // Check if we have screen capture permission
            return true; // Placeholder
        }
        return false;
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
    
    bool HasMicrophonePermission() {
        AVAuthorizationStatus status = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeAudio];
        return status == AVAuthorizationStatusAuthorized;
    }
}