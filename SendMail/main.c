//
//  main.c
//  SendMail
//
//  Created by Dave Reed on 7/18/11.
//  Copyright 2011 dave256apps.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <xpc/xpc.h>
#include <assert.h>
/*
- (void)addressEmailToAddresses:(NSArray*)emailAddresses withSender:(NSString*)sender andSubject:(NSString*)subject andBody:(NSString*)body send:(BOOL)sendAutomatically
{
    //subject = [subject escapeBackslashAndQuotes];
    //body = [body escapeBackslashAndQuotes];
    
    // use main queue since there seem to be some issues with messages not getting sent when using multiple threads (or try a serial queue)
    [[NSOperationQueue mainQueue] addOperationWithBlock:^{
     NSMutableArray *lines = [[NSMutableArray alloc] init];
     [lines addObject:[NSString stringWithFormat:@"tell application \"Mail\"\nset theNewMessage to make new outgoing message with properties {subject:\"%@\", sender:\"%@\", content:\"%@\", visible:true}", subject, sender, body]];
     for (NSString *email in emailAddresses) {
     if (email && (! [email isEqualToString:@""])) {
     [lines addObject:[NSString stringWithFormat:@"tell theNewMessage\nmake new to recipient at end of to recipients with properties {address:\"%@\"}\nend tell", email]];
     }
     }
     
     if (sendAutomatically) {
     [lines addObject:@"tell theNewMessage\nsend\nend tell\n"];
     }
     else {
     [lines addObject:@"activate"];
     }
     
     [lines addObject:@"end tell"];
     NSString *cmd = [lines componentsJoinedByString:@"\n"];
     [lines release];
     DLog(@"%@", cmd);
     
     // AppleScript call
     NSDictionary* errorDict;
     NSAppleEventDescriptor* returnDescriptor = NULL;
     NSAppleScript* scriptObject = [[NSAppleScript alloc] initWithSource:cmd];
     returnDescriptor = [scriptObject executeAndReturnError: &errorDict];
     [scriptObject release];
     if (returnDescriptor == NULL) {
     NSBeep();
     }
     }];
}
*/

static BOOL addressEmail(NSArray *emailAddresses, NSString *sender, NSString *subject, NSString *body, BOOL sendAutomatically)
{
    __block BOOL ok;
    [[NSOperationQueue mainQueue] addOperationWithBlock:^{
     NSMutableArray *lines = [[NSMutableArray alloc] init];
     [lines addObject:[NSString stringWithFormat:@"tell application \"Mail\"\nset theNewMessage to make new outgoing message with properties {subject:\"%@\", sender:\"%@\", content:\"%@\", visible:true}", subject, sender, body]];
     for (NSString *email in emailAddresses) {
     if (email && (! [email isEqualToString:@""])) {
     [lines addObject:[NSString stringWithFormat:@"tell theNewMessage\nmake new to recipient at end of to recipients with properties {address:\"%@\"}\nend tell", email]];
     }
     }
     
     if (sendAutomatically) {
     [lines addObject:@"tell theNewMessage\nsend\nend tell\n"];
     }
     else {
     [lines addObject:@"activate"];
     }
     
     [lines addObject:@"end tell"];
     NSString *cmd = [lines componentsJoinedByString:@"\n"];
     [lines release];
     //DLog(@"%@", cmd);
     
     // AppleScript call
     NSDictionary* errorDict;
     NSAppleEventDescriptor* returnDescriptor = NULL;
     NSAppleScript* scriptObject = [[NSAppleScript alloc] initWithSource:cmd];
     returnDescriptor = [scriptObject executeAndReturnError: &errorDict];
     [scriptObject release];
     if (returnDescriptor == NULL) {
     ok = NO;
     }
     ok = YES;
     }];
    return ok;
}

static void SendMail_peer_event_handler(xpc_connection_t peer, xpc_object_t event) 
{
	xpc_type_t type = xpc_get_type(event);
	if (type == XPC_TYPE_ERROR) {
		if (event == XPC_ERROR_CONNECTION_INVALID) {
			// The client process on the other end of the connection has either
			// crashed or cancelled the connection. After receiving this error,
			// the connection is in an invalid state, and you do not need to
			// call xpc_connection_cancel(). Just tear down any associated state
			// here.
		} else if (event == XPC_ERROR_TERMINATION_IMMINENT) {
			// Handle per-connection termination cleanup.
		}
	} else {
		assert(type == XPC_TYPE_DICTIONARY);
		// Handle the message.        
        xpc_connection_t remote = xpc_dictionary_get_remote_connection(event);
        
        xpc_object_t reply = xpc_dictionary_create_reply(event);
        
        const char *cSender = xpc_dictionary_get_string(event, "sendingAddress");
        const char *cRecipients = xpc_dictionary_get_string(event, "recipients");
        const char *cSubject = xpc_dictionary_get_string(event, "subject");
        const char *cBody = xpc_dictionary_get_string(event, "body");
        const BOOL send = xpc_dictionary_get_bool(event, "send");
        
        NSString *sender = [NSString stringWithCString:cSender encoding:NSUTF8StringEncoding];
        NSString *recipients = [NSString stringWithCString:cRecipients encoding:NSUTF8StringEncoding];
        NSString *subject = [NSString stringWithCString:cSubject encoding:NSUTF8StringEncoding];
        NSString *body = [NSString stringWithCString:cBody encoding:NSUTF8StringEncoding];
        
        BOOL ok = addressEmail([recipients componentsSeparatedByString:@" "], sender, subject, body, send);
        
        xpc_dictionary_set_bool(reply, "success", ok);
        
        xpc_connection_send_message(remote, reply);
        
        xpc_release(reply);
	}
}

static void SendMail_event_handler(xpc_connection_t peer) 
{
	// By defaults, new connections will target the default dispatch
	// concurrent queue.
	xpc_connection_set_event_handler(peer, ^(xpc_object_t event) {
		SendMail_peer_event_handler(peer, event);
	});
	
	// This will tell the connection to begin listening for events. If you
	// have some other initialization that must be done asynchronously, then
	// you can defer this call until after that initialization is done.
	xpc_connection_resume(peer);
}

int main(int argc, const char *argv[])
{
	xpc_main(SendMail_event_handler);
	return 0;
}
