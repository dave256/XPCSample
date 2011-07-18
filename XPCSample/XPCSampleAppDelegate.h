//
//  XPCSampleAppDelegate.h
//  XPCSample
//
//  Created by Dave Reed on 7/18/11.
//  Copyright 2011 dave256apps.com. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface XPCSampleAppDelegate : NSObject <NSApplicationDelegate> {
    NSWindow *window;
    IBOutlet NSTextField *senderTF;
    IBOutlet NSTextField *recipientsTF;
    IBOutlet NSTextField *subjectTF;
    IBOutlet NSTextField *bodyTF;
}

@property (assign) IBOutlet NSWindow *window;

- (IBAction)buttonPressed:(id)sender;

@end
