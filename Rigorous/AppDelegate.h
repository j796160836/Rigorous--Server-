//
//  AppDelegate.h
//  Rigorous
//
//  Created by Phil Plückthun on 15.12.12.
//  Copyright (c) 2012 Phil Plückthun. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "GCDAsyncSocket.h"
#import "DDLog.h"
#import "DDTTYLogger.h"
#import "DDASLLogger.h"
#import "AESCrypt.h"

#import "iTunes.h"
#import "Safari.h"
#import "Keynote5.h"
#import "Keynote.h"
#import "PowerPoint.h"
#import "VLC.h"

@interface AppDelegate : NSObject <NSApplicationDelegate, NSNetServiceDelegate, NSUserNotificationCenterDelegate> {
    IBOutlet NSMenu *statusMenu;
    IBOutlet NSMenuItem *menuOnOff;
    IBOutlet NSMenuItem *menuQuit;
    
    NSStatusItem * statusItem;
    NSImage *statusOnImage;
    NSImage *statusHighlightImage;
    NSImage *statusOffImage;
    BOOL isOn;
    
	NSNetService *netService;
	GCDAsyncSocket *asyncSocket;
	NSMutableArray *connectedSockets;
    
    PowerPointSlideShowView *currentSlideShow;
}

- (IBAction)switchonoff:(id)sender;

- (BOOL)processCommand:(NSMutableDictionary*)command;
- (void)sendMessage:(NSString*)msg;
- (void)setupServer;
- (void)stopServer;

@end
