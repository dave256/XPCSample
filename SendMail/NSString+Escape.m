//
//  NSString+Escape.m
//  GradeA
//
//  Created by David Reed on 2/2/11.
//  Copyright 2011 David M. Reed. All rights reserved.
//

#import "NSString+Escape.h"

@implementation NSString (EscapeExtensions)

- (NSString*)escapeBackslashAndQuotes {
    NSString *result;
    
    result = [self stringByReplacingOccurrencesOfString:@"\\" withString:@"\\\\"];
    result = [result stringByReplacingOccurrencesOfString:@"\"" withString:@"\\\""];
    return result;
}


@end
