//
//  XPCSampleAppDelegate.m
//  XPCSample
//
//  Created by Dave Reed on 7/18/11.
//  Copyright 2011 dave256apps.com. All rights reserved.
//

#import "XPCSampleAppDelegate.h"
#import <xpc/xpc.h>

@implementation XPCSampleAppDelegate

@synthesize window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
}

- (void) mailCallback:(xpc_object_t) event
{
    NSLog(@"my callback");
    xpc_type_t type = xpc_get_type(event);
    
    if (XPC_TYPE_ERROR == type) {
        NSLog(@"issue with xpc\n");
        return; 
    }
    
    if (XPC_TYPE_DICTIONARY != type) {
        return;
    }
    
    BOOL ok = xpc_dictionary_get_bool(event, "success");
    if (!ok) {
        NSLog(@"error sending");
        NSBeep();
    }
    else {
        NSLog(@"message sent");
    }
}

- (void)addressEmailToAddresses:(NSArray*)emailAddresses withSender:(NSString*)sender andSubject:(NSString*)subject andBody:(NSString*)body send:(BOOL)sendAutomatically
{
    if (xpc_connection_create != NULL) {
        xpc_connection_t conn = xpc_connection_create("com.dave256apps.SendMail", NULL);
        xpc_connection_set_event_handler(conn, ^(xpc_object_t object) {
            //[self mailCallback:object];
            
        });
        xpc_connection_resume(conn);
        
        NSString *recipients = [emailAddresses componentsJoinedByString:@" "];
        xpc_object_t message = xpc_dictionary_create(NULL, NULL, 0);
        xpc_dictionary_set_string(message, "sendingAddress", [sender cStringUsingEncoding:NSUTF8StringEncoding]);
        xpc_dictionary_set_string(message, "recipients", [recipients cStringUsingEncoding:NSUTF8StringEncoding]);
        xpc_dictionary_set_string(message, "subject", [subject cStringUsingEncoding:NSUTF8StringEncoding]);
        xpc_dictionary_set_string(message, "body", [body cStringUsingEncoding:NSUTF8StringEncoding]);
        xpc_dictionary_set_bool(message, "send", sendAutomatically);
        
        xpc_connection_send_message_with_reply(conn, message, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT ,0),
                                               ^(xpc_object_t reply) {
                                                   [self mailCallback:reply];
                                               });
        xpc_release(message);
    }
    else {
        NSLog(@"no XPC");
    }
}

- (IBAction)buttonPressed:(id)sender
{
    NSLog(@"button pressed");
    [self addressEmailToAddresses:[[recipientsTF stringValue] componentsSeparatedByString:@","] withSender:[senderTF stringValue] andSubject:[subjectTF stringValue] andBody:[bodyTF stringValue] send:YES];
}

@end
