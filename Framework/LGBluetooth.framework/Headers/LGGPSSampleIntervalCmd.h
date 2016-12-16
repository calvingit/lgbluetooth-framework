//
// LGBluetooth
//
// Created by : zhangwen
// Copyright (C) 2016 zhangwen. All rights reserved.
//

#import "LGBaseCommand.h"
/**
 *  GPS采样间隔，单位为秒, NSInteger
 */
@interface LGGPSSampleIntervalCmd : LGBaseCommand
//设置间隔时间
- (instancetype)setInterval:(NSInteger)interval success:(LGBlockWithVoid)success failure:(LGBlockWithError)failure;
//读取间隔时间
- (instancetype)readIntervalWithSuccess:(LGBlockWithInteger)success failure:(LGBlockWithError)failure;
@end
