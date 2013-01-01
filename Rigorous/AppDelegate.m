//
//  AppDelegate.m
//  Rigorous
//
//  Created by Phil Plückthun on 15.12.12.
//  Copyright (c) 2012 Phil Plückthun. All rights reserved.
//

#import "AppDelegate.h"

static const int ddLogLevel = LOG_LEVEL_VERBOSE;

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    isOn = false;
    
    [DDLog addLogger:[DDTTYLogger sharedInstance]];
	[DDLog addLogger:[DDASLLogger sharedInstance]];
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
    [self stopServer];
}

-(void)awakeFromNib{
    [[NSUserNotificationCenter defaultUserNotificationCenter] setDelegate:self];
    
    statusItem = [[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength];
    
    NSBundle *bundle = [NSBundle mainBundle];
    statusOnImage = [[NSImage alloc] initWithContentsOfFile:[bundle pathForResource:@"on" ofType:@"png"]];
    statusOffImage = [[NSImage alloc] initWithContentsOfFile:[bundle pathForResource:@"off" ofType:@"png"]];
    statusHighlightImage = [[NSImage alloc] initWithContentsOfFile:[bundle pathForResource:@"highlight" ofType:@"png"]];
    
    [statusItem setImage:statusOffImage];
    [statusItem setAlternateImage:statusHighlightImage];
    
    [statusItem setMenu:statusMenu];
    [statusItem setHighlightMode:YES];
}

- (BOOL)userNotificationCenter:(NSUserNotificationCenter *)center shouldPresentNotification:(NSUserNotification *)notification{
    return YES;
}

-(IBAction)switchonoff:(id)sender {
    if (isOn == false) {
        [self setupServer];
        isOn = true;
        [statusItem setImage:statusOnImage];
        [menuOnOff setTitle:@"Turn Off"];
    } else {
        [self stopServer];
        isOn = false;
        [statusItem setImage:statusOffImage];
        [menuOnOff setTitle:@"Turn On"];
    }
}

- (void)setupServer
{
    asyncSocket = [[GCDAsyncSocket alloc] initWithDelegate:self delegateQueue:dispatch_get_main_queue()];
    connectedSockets = [[NSMutableArray alloc] init];
    NSError *err = nil;
	if ([asyncSocket acceptOnPort:0 error:&err])
	{
		UInt16 port = [asyncSocket localPort];
		netService = [[NSNetService alloc] initWithDomain:@"" type:@"_Rigorous._tcp." name:@"" port:port];
		
		[netService setDelegate:self];
		[netService publish];
	} else {
		DDLogError(@"Error in acceptOnPort:error: -> %@", err);
	}
}

- (void)stopServer
{
    [netService stop];
    [asyncSocket disconnect];

    @synchronized(connectedSockets)
    {
        NSUInteger i;
        for (i = 0; i < [connectedSockets count]; i++)
        {
            [[connectedSockets objectAtIndex:i] disconnect];
        }
        connectedSockets = [[NSMutableArray alloc] init];
    }
}

- (void)socket:(GCDAsyncSocket *)sock didAcceptNewSocket:(GCDAsyncSocket *)newSocket
{
	DDLogInfo(@"Accepted new socket from %@:%hu", [newSocket connectedHost], [newSocket connectedPort]);
    
    NSUserNotification *notification = [[NSUserNotification alloc] init];
    notification.title = @"Client Connected";
    notification.informativeText = [NSString stringWithFormat:@"A new client connected from %@:%d", [newSocket connectedHost], [newSocket connectedPort]];
    notification.soundName = nil;
    [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:notification];
	
	[connectedSockets addObject:newSocket];
    [newSocket readDataWithTimeout:READ_TIMEOUT tag:0];
}

- (void)socketDidDisconnect:(GCDAsyncSocket *)sock withError:(NSError *)err
{
    DDLogInfo(@"Disconnected a socket");
        
    NSUserNotification *notification = [[NSUserNotification alloc] init];
    notification.title = @"Client Disconnected";
    notification.informativeText = [NSString stringWithFormat:@"A client disconnected"];
    notification.soundName = nil;
    [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:notification];
    
	[connectedSockets removeObject:sock];
}

- (void)netServiceDidPublish:(NSNetService *)ns
{
	DDLogInfo(@"Bonjour Service Published: domain(%@) type(%@) name(%@) port(%i)", [ns domain], [ns type], [ns name], (int)[ns port]);
}

- (void)netService:(NSNetService *)ns didNotPublish:(NSDictionary *)errorDict
{
	DDLogError(@"Failed to Publish Service: domain(%@) type(%@) name(%@) - %@",
               [ns domain], [ns type], [ns name], errorDict);
}

- (void)socket:(GCDAsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag
{
    dispatch_async(dispatch_get_main_queue(), ^{
		@autoreleasepool {
            DDLogInfo(@"Incoming data");
            
            NSString* rawmessage = [[NSString alloc] initWithData:data encoding:NSASCIIStringEncoding];
            NSString* message = [AESCrypt decrypt:rawmessage password:@KEYPHRASE];
            NSArray *words = [message componentsSeparatedByString:@" "];
            NSMutableDictionary *command = [[NSMutableDictionary alloc] init];
            for (int i = 0; i < words.count; i++) {
                if (i == 0) {
                    [command setObject:words[i] forKey:@"cmd"];
                } else {
                    NSArray *attribute = [words[i] componentsSeparatedByString:@":"];
                    if (attribute.count == 2) {
                        [command setObject:[attribute[1] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]] forKey:attribute[0]];
                    } else if (attribute.count > 2) {
                        [command setObject:[attribute[1] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]] forKey:attribute[0]];
                        for (int i = 2; i < attribute.count; i++) {
                            [command setObject:[[NSString stringWithFormat:@"%@:%@",[command objectForKey:attribute[0]],attribute[i]] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]] forKey:attribute[0]];
                        }
                    }
                }
            }
            
            [self processCommand:command];
            [sock readDataWithTimeout:READ_TIMEOUT tag:0];
		}
	});
}

- (NSTimeInterval)socket:(GCDAsyncSocket *)sock shouldTimeoutReadWithTag:(long)tag elapsed:(NSTimeInterval)elapsed bytesDone:(NSUInteger)length
{
    DDLogInfo(@"Timeout! Extending read time.");
    
    [sock readDataWithTimeout:READ_TIMEOUT_EXTENSION tag:0];
		
    return READ_TIMEOUT_EXTENSION;
}

- (NSTimeInterval)socket:(GCDAsyncSocket *)sock shouldTimeoutWriteWithTag:(long)tag elapsed:(NSTimeInterval)elapsed bytesDone:(NSUInteger)length
{
    DDLogInfo(@"Timeout! Extending write time.");
    
    [sock readDataWithTimeout:WRITE_TIMEOUT_EXTENSION tag:0];
    
    return WRITE_TIMEOUT_EXTENSION;
}

- (BOOL)processCommand:(NSMutableDictionary*)command
{
    if ([[command objectForKey:@"cmd"] isEqualToString:@"itunes"]) {
        if ([[command objectForKey:@"action"] isEqualToString:@"playpause"]) {
            iTunesApplication *iTunes = [SBApplication applicationWithBundleIdentifier:@"com.apple.iTunes"];
            [iTunes playpause];
            return true;
        } else if ([[command objectForKey:@"action"] isEqualToString:@"nexttrack"]) {
            iTunesApplication *iTunes = [SBApplication applicationWithBundleIdentifier:@"com.apple.iTunes"];
            [iTunes nextTrack];
            return true;
        } else if ([[command objectForKey:@"action"] isEqualToString:@"previoustrack"]) {
            iTunesApplication *iTunes = [SBApplication applicationWithBundleIdentifier:@"com.apple.iTunes"];
            [iTunes previousTrack];
            return true;
        }
    } else if ([[command objectForKey:@"cmd"] isEqualToString:@"safari"]) {
        if ([[command objectForKey:@"action"] isEqualToString:@"openurl"]) {
            SafariApplication *Safari = [SBApplication applicationWithBundleIdentifier:@"com.apple.iTunes"];
            [Safari activate];
            [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[command objectForKey:@"url"]]];
            return true;
        }
    } else if ([[command objectForKey:@"cmd"] isEqualToString:@"keynote"]) {
        if ([[command objectForKey:@"action"] isEqualToString:@"nextslide"]) {
            KeynoteApplication *Keynote = [SBApplication applicationWithBundleIdentifier:@"com.apple.iWork.Keynote"];
            if ([Keynote slideshows].count > 0) {
                SBElementArray *temp = [Keynote slideshows];
                for (int i = 0; i < temp.count; i++) {
                    KeynoteSlideshow *tempSlideshow = temp[0];
                    if ([tempSlideshow playing]) {
                        [tempSlideshow showNext];
                    }
                }
            }
            return true;
        } else if ([[command objectForKey:@"action"] isEqualToString:@"previousslide"]) {
            KeynoteApplication *Keynote = [SBApplication applicationWithBundleIdentifier:@"com.apple.iWork.Keynote"];
            if ([Keynote slideshows].count > 0) {
                SBElementArray *temp = [Keynote slideshows];
                for (int i = 0; i < temp.count; i++) {
                    KeynoteSlideshow *tempSlideshow = temp[i];
                    if ([tempSlideshow playing]) {
                        [tempSlideshow showPrevious];
                    }
                }
            }
            return true;
        } else if ([[command objectForKey:@"action"] isEqualToString:@"playpause"]) {
            KeynoteApplication *Keynote = [SBApplication applicationWithBundleIdentifier:@"com.apple.iWork.Keynote"];
            if ([Keynote slideshows].count > 0) {
                SBElementArray *temp = [Keynote slideshows];
                for (int i = 0; i < temp.count; i++) {
                    KeynoteSlideshow *tempSlideshow = temp[i];
                    if ([tempSlideshow playing]) {
                        [tempSlideshow stopSlideshow];
                    } else {
                        [tempSlideshow start];
                    }
                }
            }
            return true;
        }
    } else if ([[command objectForKey:@"cmd"] isEqualToString:@"powerpoint"]) {
        if ([[command objectForKey:@"action"] isEqualToString:@"playpause"]) {
            PowerPointApplication *PowerPoint = [SBApplication applicationWithBundleIdentifier:@"com.microsoft.Powerpoint"];
            if (currentSlideShow == nil) {
                if ([PowerPoint presentations].count > 0) {
                    PowerPointDocumentWindow *tempDocWindow = [PowerPoint activeWindow];
                    PowerPointPresentation *tempPresentation = [tempDocWindow presentation];
                    PowerPointSlideShowSettings *tempSettings = [tempPresentation slideShowSettings];
                    PowerPointSlideShowWindow *tempWindow = [tempSettings runSlideShow];
                    currentSlideShow = [tempWindow slideshowView];
                }
            } else {
                [currentSlideShow exitSlideShow];
                currentSlideShow = nil;
            }
        } else if ([[command objectForKey:@"action"] isEqualToString:@"nextslide"]) {
            if (currentSlideShow != nil) {
                [currentSlideShow goToNextSlide];
            }
        } else if ([[command objectForKey:@"action"] isEqualToString:@"previousslide"]) {
            if (currentSlideShow != nil) {
                [currentSlideShow goToPreviousSlide];
            }
        }
        return true;
    } else if ([[command objectForKey:@"cmd"] isEqualToString:@"vlc"]) {
        if ([[command objectForKey:@"action"] isEqualToString:@"playpause"]) {
            VLCApplication *VLC = [SBApplication applicationWithBundleIdentifier:@"org.videolan.vlc"];
            SBElementArray *temp = [VLC windows];
            for (int i = 0; i < temp.count; i++) {
                VLCWindow *tempWindow = temp[i];
                if ([tempWindow visible]) {
                    VLCDocument *tempDocument = [tempWindow document];
                    [tempDocument play];
                }
            }
        } else if ([[command objectForKey:@"action"] isEqualToString:@"mute"]) {
            VLCApplication *VLC = [SBApplication applicationWithBundleIdentifier:@"org.videolan.vlc"];
            SBElementArray *temp = [VLC windows];
            for (int i = 0; i < temp.count; i++) {
                VLCWindow *tempWindow = temp[i];
                if ([tempWindow visible]) {
                    VLCDocument *tempDocument = [tempWindow document];
                    [tempDocument mute];
                }
            }
        } else if ([[command objectForKey:@"action"] isEqualToString:@"stop"]) {
            VLCApplication *VLC = [SBApplication applicationWithBundleIdentifier:@"org.videolan.vlc"];
            SBElementArray *temp = [VLC windows];
            for (int i = 0; i < temp.count; i++) {
                VLCWindow *tempWindow = temp[i];
                if ([tempWindow visible]) {
                    VLCDocument *tempDocument = [tempWindow document];
                    [tempDocument stop];
                }
            }
        } else if ([[command objectForKey:@"action"] isEqualToString:@"volumeup"]) {
            VLCApplication *VLC = [SBApplication applicationWithBundleIdentifier:@"org.videolan.vlc"];
            SBElementArray *temp = [VLC windows];
            for (int i = 0; i < temp.count; i++) {
                VLCWindow *tempWindow = temp[i];
                if ([tempWindow visible]) {
                    VLCDocument *tempDocument = [tempWindow document];
                    [tempDocument volumeUp];
                }
            }
        } else if ([[command objectForKey:@"action"] isEqualToString:@"volumedown"]) {
            VLCApplication *VLC = [SBApplication applicationWithBundleIdentifier:@"org.videolan.vlc"];
            SBElementArray *temp = [VLC windows];
            for (int i = 0; i < temp.count; i++) {
                VLCWindow *tempWindow = temp[i];
                if ([tempWindow visible]) {
                    VLCDocument *tempDocument = [tempWindow document];
                    [tempDocument volumeDown];
                }
            }
        } else if ([[command objectForKey:@"action"] isEqualToString:@"fullscreen"]) {
            VLCApplication *VLC = [SBApplication applicationWithBundleIdentifier:@"org.videolan.vlc"];
            SBElementArray *temp = [VLC windows];
            for (int i = 0; i < temp.count; i++) {
                VLCWindow *tempWindow = temp[i];
                if ([tempWindow visible]) {
                    VLCDocument *tempDocument = [tempWindow document];
                    [tempDocument fullscreen];
                }
            }
        }
        return true;
    } else if ([[command objectForKey:@"cmd"] isEqualToString:@"safari"]) {
        if ([[command objectForKey:@"action"] isEqualToString:@"openurl"]) {
            SafariApplication *Safari = [SBApplication applicationWithBundleIdentifier:@"com.apple.safari"];
            [Safari activate];
            NSWorkspace * ws = [NSWorkspace sharedWorkspace];
            [ws openURL: [NSURL URLWithString:[command objectForKey:@"url"]]];
        }
    }
    return false;
}

- (void)sendMessage:(NSString *)msg
{
    NSLog(@"Message to send: %@", msg);
    NSString *message = [NSString stringWithFormat:@"%@ \n", msg];
    NSString *encryptedData = [AESCrypt encrypt:message password:@KEYPHRASE];
    NSData* data = [encryptedData dataUsingEncoding: NSASCIIStringEncoding];
    [asyncSocket writeData:data withTimeout:WRITE_TIMEOUT tag:0];
}

- (IBAction)quitapp:(id)sender {
    if (isOn == true) {
        isOn = false;
        [self stopServer];
    }
    [[NSApplication sharedApplication] terminate:nil];
}

@end
