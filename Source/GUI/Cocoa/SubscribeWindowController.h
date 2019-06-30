//
//  SubscribeWindowController.h
//  MediaInfo
//
//  Created by Jerome on 19/05/2019.
//  Copyright © 2019 MediaArea.net. All rights reserved.
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@interface SubscribeWindowController : NSWindowController {
    NSMutableArray *observers;
}
@property (assign) IBOutlet NSBox *loadingLayer;
@property (assign) IBOutlet NSTextField *loadingMessage;
@property (assign) IBOutlet NSProgressIndicator *loadingIndicator;
@property (assign) IBOutlet NSButton *retryButton;
@property (assign) IBOutlet NSTextField *statusText;
@property (assign) IBOutlet NSButton *subscribeButton;
@property (assign) IBOutlet NSTextField *legalText;

+(instancetype)controller;
-(void)show;

-(void)updateSubscriptionDetails;
@end

NS_ASSUME_NONNULL_END
