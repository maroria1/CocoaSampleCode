/*

File: CFNetworkLoader.h

Abstract: CFNetwork ImageClient Sample

Version: 1.0

Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple
Computer, Inc. ("Apple") in consideration of your agreement to the
following terms, and your use, installation, modification or
redistribution of this Apple software constitutes acceptance of these
terms.  If you do not agree with these terms, please do not use,
install, modify or redistribute this Apple software.

In consideration of your agreement to abide by the following terms, and
subject to these terms, Apple grants you a personal, non-exclusive
license, under Apple's copyrights in this original Apple software (the
"Apple Software"), to use, reproduce, modify and redistribute the Apple
Software, with or without modifications, in source and/or binary forms;
provided that if you redistribute the Apple Software in its entirety and
without modifications, you must retain this notice and the following
text and disclaimers in all such redistributions of the Apple Software. 
Neither the name, trademarks, service marks or logos of Apple Computer,
Inc. may be used to endorse or promote products derived from the Apple
Software without specific prior written permission from Apple.  Except
as expressly stated in this notice, no other rights or licenses, express
or implied, are granted by Apple herein, including but not limited to
any patent rights that may be infringed by your derivative works or by
other works in which the Apple Software may be incorporated.

The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.

IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

Copyright (c) 2005 Apple Computer, Inc., All Rights Reserved

*/ 

#import <Cocoa/Cocoa.h>
#import <SystemConfiguration/SystemConfiguration.h>

@class ImageClient;

@interface CFNetworkLoader : NSObject {
    ImageClient *imageClient;
    CFHTTPMessageRef request;
    CFReadStreamRef readStream;
    CFMutableDataRef data;
    
    // The current proxy settings
    CFDictionaryRef proxyDictionary;
    
    // The dynamic store we're using to monitor system proxy changes
    SCDynamicStoreRef systemDynamicStore;

    BOOL haveExaminedHeaders;    // Whether we've seen the headers of the response yet
	
	CFMutableArrayRef authArray; // List of saved authentication objects for re-use
	CFMutableArrayRef proxyAuthArray; // Same as the authArray, but proxy specific
	CFMutableDictionaryRef credentialsDict; // Map of authentication objects to credentials for that object
	
	BOOL saveCredentials;	// Held long enough to know that authentication succeeded and credentials should be saved.
	BOOL ignoreKeychain;	// Used to bypass Keychain entries in the case where they are known to be wrong.
	
	// Authentication is only ever in flux with one server at a time.
	// It will be a proxy server that is being navigated or the far
	// end host.  Once through the proxy, it is know that it is the
	// far end that is being negotiated.  This boolean is use to know
	// if all the prompting and the current credentials are being
	// used to navigate the proxy.  After it has successfully negotiated
	// the proxy, this boolean will be false and it is known that we
	// are then negotiating with the far end host.
	BOOL isProxyAuthentication;
}

- (id)initWithImageClient:(ImageClient *)imgClient;

// Called by ImageClient to start and stop loads
- (void)loadURL:(NSURL *)url;
- (void)cancelLoad;

// Internal routine that does the actual work of a load
- (void)loadRequest;

// Called when the readStream signals new events
- (void)handleStreamEvent:(CFStreamEventType)event;

// Called when the system proxy changes
- (void)proxyChanged; 

// Examines the server response to determine if it is an authentication challenge
- (BOOL)isAuthorizationFailure;

// Cancels the current load and handles authentication failure
- (void)retryAfterAuthorizationFailure;

// Restarts a request after user is prompted for credentials
- (void)resumeWithCredentials;

// Find the pertinent authentication object in the list of them.
- (CFHTTPAuthenticationRef)findAuthenticationForRequest;

// Find the credentials dictionary for authentication
- (CFMutableDictionaryRef)findCredentialsForAuthentication:(CFHTTPAuthenticationRef)auth;

// Will find and save the credentials for the recent request.
- (void)saveCredentialsForRequest;

// Find the pertinent authentication object in the list of them.
- (CFHTTPAuthenticationRef)findProxyAuthenticationForRequest;

// Find the credentials dictionary for authentication
- (CFMutableDictionaryRef)findCredentialsForProxyAuthentication:(CFHTTPAuthenticationRef)auth;

@end
