//
//  CompareView.h
//  MediaInfo
//
//  Created by Jerome on 03/05/2019.
//  Copyright © 2019 MediaArea.net. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "oMediaInfoList.h"

NS_ASSUME_NONNULL_BEGIN

typedef enum CompareViewMode {
    CompareViewModeAll,
    CompareViewModeDiffers,
    CompareViewModeEquals
} CompareViewMode;

@interface CompareView : NSView <NSTableViewDataSource, NSTableViewDelegate, NSOutlineViewDataSource, NSOutlineViewDelegate>
@property (strong) IBOutlet NSView *contentView;
@property (strong) IBOutlet NSOutlineView *outlineView;
@property (nonatomic, strong) oMediaInfoList *files;
@property (strong) NSArray *fields;
@property (readonly) CompareViewMode mode;

-(void)reload;
@end

NS_ASSUME_NONNULL_END
