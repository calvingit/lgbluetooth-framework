
# 数据同步
这里的数据指存储在设备的运动数据、睡眠数据、心率数据、GPS数据、血压数据等等。

## 运动数据
运动数据包括步数和卡路里，不包括距离。距离需要另外计算，计算方式请看下面介绍。

### 读取当天运动数据(LGCurrentSportDataCmd)

当天运动数据是汇总值，表示今天跑的总步数和消耗的卡路里，卡路里单位是小卡。
代码如下：
```
LGCurrentSportDataCmd *cmd = [LGCurrentSportDataCmd commandWithAgent:self.agent];
[[cmd readCurrentSportDataWithSuccess:^(NSInteger steps, NSInteger calories) {
    [self showAlert:_commands[row] msg:[NSString stringWithFormat:@"步数:%ld步\n卡路里:%ld卡", steps, calories]];
} failure:^(NSError *error) {
    //失败
}] start];

```

### 读取历史运动数据(LGHistorySportDataCmd)
历史运动数据用`LGSportData`表示有三个属性:
- date  时间
- steps  步数
- calories 卡路里

`LGSportDataBlock`是`LGHistorySportDataCmd`的成功回调，参数类型是`LGSportData`类型的数组。datas不一定有数据，当没有历史数据时返回空数组。
```
typedef void(^LGSportDataBlock)(NSArray<LGSportData *> *datas);
```

使用方法如下:
```
LGHistorySportDataCmd *cmd = [[LGHistorySportDataCmd alloc] initWithPeripheralAgent:self.agent];
[[cmd readDataWithSuccess:^(NSArray<LGSportData *> *datas) {
    //成功,保存到数据库
} failure:^(NSError *error) {
    //失败
}] start];
```

### 清空历史运动数据(LGCleanUpSportDataCmd)
手环里面的存储空间有限，如果每天跑步的话，大概可以保存一周的数据，所以及时清理旧的数据很重要。

最佳做法是读取历史运动数据成功之后，清空掉里面的数据，避免App保存的数据和设备的数据重复了。如果不清空数据，每次调用`LGHistorySportDataCmd`都会把所有的数据返回。

使用方法如下:
```
LGCleanUpSportDataCmd *cmd = [LGCleanUpSportDataCmd commandWithAgent:self.agent];
[cmd startWithSuccess:^{
    [self showAlert:_commands[row] msg: @"成功"];
} failure:^(NSError *error) {
    [self showAlert:_commands[row] msg: error.description];
}];
```

## 睡眠数据
一般来说，人在晚上睡觉会经历3种状态：
- 浅睡`LGSleepStateLight`（睡得模模糊糊，容易被吵醒）
- 深睡`LGSleepStateDeep` (睡得很沉，不容易被吵醒)
- 清醒`LGSleepStateAwake` (意识清醒，躺在床上，或者动作幅度很大)

这几种状态使用枚举`LGSleepState`表示，另外还加了`LGSleepStateStart`和`LGSleepStateEnd`来表示准备入睡和起床状态。

每一种状态的数据表示使用类`LGSleepDetailData`，有结束时间(date), 持续时长(minutes), 状态(state)三个属性表示。

一段完整的睡眠`LGSleepData`有很多个`LGSleepDetailData`组成，比如：
1. 开始(`LGSleepStateStart`)
2. 浅睡(`LGSleepStateLight`)
3. 深睡(`LGSleepStateDeep`)
4. 浅睡(`LGSleepStateLight`)
5. 深睡(`LGSleepStateDeep`)
6. 清醒(`LGSleepStateAwake`)
7. 结束(`LGSleepStateEnd`)

`LGSleepData`有很多属性对这段睡眠做了一些统计：
- deepSleepMinutes 深睡的时间汇总
- lightSleepMinutes 浅睡的时间汇总
- awakeMinutes  清醒的时间汇总
- awakeCount  清醒的次数

LGSleepData还有开始的时间`startDate`和结束的时间`endDate`，以及`LGSleepDetailData`类型的`details`数组。

### 读取历史睡眠数据(LGHistorySleepDataCmd)
`LGHistorySleepDataBlock`是成功回调，参数是`LGSleepData`类型的数组。

> 一天也可能有多个完整睡眠，不一定是指晚上睡觉，白天也有午睡。所以不能说一个`LGSleepData`就代表着某一天的睡眠，只能代表某一段时间的睡眠。

使用方法如下:

```
LGHistorySleepDataCmd *cmd = [[LGHistorySleepDataCmd alloc] initWithPeripheralAgent:self.agent];
[[cmd readDataWithSuccess:^(NSArray<LGSleepData *> *datas) {
    [self performSegueWithIdentifier:kShowTextSegueID sender:datas.description];
} failure:^(NSError *error) {
    [self showAlert:_commands[row] msg: error.description];
}] start];
```

### 清空历史睡眠数据(LGCleanUpSleepDataCmd)
如运动数据一样，同步完成之后，可以清空历史数据。
```
LGCleanUpSleepDataCmd *cmd = [LGCleanUpSleepDataCmd commandWithAgent:self.agent];
[cmd startWithSuccess:^{
    [self showAlert:_commands[row] msg: @"成功"];
} failure:^(NSError *error) {
    [self showAlert:_commands[row] msg: error.description];
}];

```

## 心率数据
心率数据类是`LGHeartRateData`，只有两个属性:
- value 心率值
- date   测量的时间

### 读取历史心率数据(LGHistoryHeartRateDataCmd)
成功回调是`LGHistoryHeartRateDataBlock`，参数返回`LGHeartRateData`类型的数组。

使用方法如下：
```
LGHistoryHeartRateDataCmd *cmd = [[LGHistoryHeartRateDataCmd alloc] initWithPeripheralAgent:self.agent];
[[cmd readDataWithSuccess:^(NSArray<LGHeartRateData *> *datas) {
    [self performSegueWithIdentifier:kShowTextSegueID sender:datas.description];
} failure:^(NSError *error) {
    [self showAlert:_commands[row] msg: error.description];
}] start];
```

### 清空历史心率数据(LGCleanUpHeartRateDataCmd)
如运动数据一样，同步完成之后，可以清空历史数据。

使用方法如下:
```
LGCleanUpHeartRateDataCmd *cmd = [LGCleanUpHeartRateDataCmd commandWithAgent:self.agent];
[cmd startWithSuccess:^{
    [self showAlert:_commands[row] msg: @"成功"];
} failure:^(NSError *error) {
    [self showAlert:_commands[row] msg: error.description];
}];
```

## GPS数据
GPS数据使用类`LGGPSData`表示，它的声明如下：
```
@interface LGGPSData : NSObject
///数据状态
@property (nonatomic, assign) LGGPSDataState state;
///运动类型
@property (nonatomic, assign) LGSportType    sportType;
///时间
@property (nonatomic, assign) NSDate         *date;
///纬度
@property (nonatomic, assign) double         latitude;
///经度
@property (nonatomic, assign) double         longitude;
///数据索引 (画轨迹的时候需要用到排序)
@property (nonatomic, assign) NSInteger      index;
@end

```

其中`state`有三种状态:
- LGGPSDataStateStarting 开始，即第一个点
- LGGPSDataStateRunning  行走
- LGGPSDataStateEnding 结束，即最后一个点

`sportType`有两种类型:
- LGSportTypeRiding 骑行
- LGSportTypeRunning 跑步

`index`在地图上画轨迹时比较重要，可以先按从小到大排序。

### 读取历史GPS数据(LGHistoryGPSDataCmd)
`LGHistoryGPSDataBlock`是成功回调，参数是`LGGPSData`类型的数组。

使用方法如下：
```
LGHistoryGPSDataCmd *cmd = [[LGHistoryGPSDataCmd alloc] initWithPeripheralAgent:self.agent];
[[cmd readDataWithSuccess:^(NSArray<LGGPSData *> *datas) {
    [self performSegueWithIdentifier:kShowTextSegueID sender:datas.description];
} failure:^(NSError *error) {
    [self showAlert:_commands[row] msg:error.description];
}] start];
```

### 清空历史GPS数据(LGCleanUpGPSDataCmd)

```
LGCleanUpGPSDataCmd *cmd = [LGCleanUpGPSDataCmd commandWithAgent:self.agent];
[cmd startWithSuccess:^{
    [self showAlert:_commands[row] msg:@"成功"];
} failure:^(NSError *error) {
    [self showAlert:_commands[row] msg:error.description];
}];
```