/*

File: CFNetworkLoader.m

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

#import "CFNetworkLoader.h"
#import "ImageClient.h"
#import "NSURLLoader.h"

#define BUFSIZE 4096

@implementation CFNetworkLoader

- (id)initWithImageClient:(ImageClient *)imgClient {
    if (self = [super init]) {
        imageClient = imgClient; // No retain because the ImageClient instance is retaining us
    }
    return self;
}

- (void)dealloc {
    if (request) CFRelease(request);
    if (readStream) {
        CFReadStreamClose(readStream);
        CFRelease(readStream);
    }
    if (data) CFRelease(data);
    [super dealloc];
}

- (void)loadURL:(NSURL *)url {
    // set the request for the new transaction
    if (request) CFRelease(request);
    request = CFHTTPMessageCreateRequest(NULL, CFSTR("GET"), (CFURLRef)url, kCFHTTPVersion1_1);
    
    // Start the load
    [self loadRequest];
}

- (void)loadRequest {
    // Cancel any load currently in progress
    [self cancelLoad];

    // Start a fresh data to hold the downloaded image
    data = CFDataCreateMutable(NULL, 0);
    
    readStream = CFReadStreamCreateForHTTPRequest(NULL, request);
    
    if (CFReadStreamOpen(readStream)) {
        BOOL done = NO;
        do {
            UInt8 buf[BUFSIZE];
            int bytesRead = CFReadStreamRead(readStream, buf, BUFSIZE);
            if (bytesRead > 0) {
                CFDataAppendBytes(data, buf, bytesRead);
            } else if (bytesRead == 0) {
                // All done
                [imageClient setImageData:(NSData *)data];
                done = YES;
            } else {
                // Error occurred
                [imageClient errorOccurredLoadingImage];
                done = YES;
            }
        } while (!done);
    } else {
        [imageClient errorOccurredLoadingImage];
    }

    // Clean up
    CFReadStreamClose(readStream);
    CFRelease(readStream);
    readStream = nil;

    CFRelease(data);
    data = NULL;
}

- (void)cancelLoad {
    // Synchronous; nothing to do.
}


@end
