# 锁屏管理服务

## 简介
### 内容介绍
锁屏管理服务是OpenHarmony中系统服务，为锁屏应用提供注册亮屏、灭屏、开启屏幕、结束休眠、退出动画、请求解锁结果监听，并提供回调结果给锁屏应用。锁屏管理服务向三方应用提供请求解锁、查询锁屏状态、查询是否设置锁屏密码的能力。

**图 1** 架构图

![](figures/subsystem_architecture_zh.png "子系统架构图")

### 框架图介绍 
1.三方应用支持操作请求解锁、查询锁屏状态、查询是否设置锁屏密码接口调用。\
2.锁屏应用注册亮屏、灭屏、开启屏幕、结束休眠、退出动画、请求解锁结果监听等事件 \
3.框架层API用来处理三方应用和锁屏应用的js接口请求处理，NAPI层进行js调用的处理 \
4.框架层IDL用来处理NAPI接口向锁屏管理服务之间的桥梁，进行IPC通讯 \
5.锁屏管理服务用来处理三方应用和锁屏应用接口请求，并作出对应处理，提供相应的返回结果。

## 目录

```
/base/theme/screenlock_mgr
├── figures                  # 构架图
├── frameworks/kitsimpl      # 对应用提供的接口
├── interfaces/kits          # 组件对外提供的接口代码
│   ├── jskits               # 服务间接口
│   └── napi                 # js接口解析成napi接口
├── sa_profile               # 组件包含的系统服务的配置文件和进程的配置文件
├── services                 # 锁屏管理服务实现
├── test                     # 接口的单元测试
└── utils                    # 组件包含日志打印和有序公共事件定义的常量
```

## 说明

### 接口说明

**表 1**   锁屏管理服务的主要方法说明

| 接口名                      | 描述                       |
| -------------------------- | -------------------------- |
| isScreenLocked(callback: AsyncCallback&lt;boolean&gt;): void; | 判断屏幕是否锁屏。callback方式 |
| isScreenLocked(): Promise&lt;boolean&gt;; | 判断屏幕是否锁屏。Promise方式 |
| isLocked(): boolean; | 判断屏幕是否锁屏。返回true表示屏幕已锁屏；返回false表示屏幕未锁屏。同步方式 |
| isSecureMode(callback: AsyncCallback&lt;boolean&gt;): void; | 判断当前设备的屏幕锁定是否安全(安全屏幕锁定意味着解锁屏幕需要密码、图案或其他用户身份识别)。callback方式 |
| isSecureMode(): Promise&lt;boolean&gt;; | 判断当前设备的屏幕锁定是否安全(安全屏幕锁定意味着解锁屏幕需要密码、图案或其他用户身份识别)。Promise方式 |
| isSecure(): boolean; | 判断当前设备的屏幕锁定是否安全(安全屏幕锁定意味着解锁屏幕需要密码、图案或其他用户身份识别)。返回true表示当前设备的屏幕锁定安全；返回false表示当前设备的屏幕锁定不安全。同步方式 |
| unlockScreen(callback: AsyncCallback&lt;void&gt;): void; | 三方应用解锁屏幕。callback方式 |
| unlockScreen(): Promise&lt;void&gt;; | 三方应用解锁屏幕。Promise方式 |
| unlock(callback: AsyncCallback&lt;boolean&gt;): void; | 三方应用解锁屏幕。如果屏幕解锁成功，则返回true，否则返回false。callback方式 |
| unlock():Promise&lt;boolean&gt;; | 三方应用解锁屏幕。如果屏幕解锁成功，则返回true，否则返回false。Promise方式 |
| lock(callback: AsyncCallback&lt;boolean&gt;): void; | 系统API，锁定屏幕。如果屏幕锁定成功，则返回true，否则返回false。callback方式 |
| lock():Promise&lt;boolean&gt;; | 系统API，锁定屏幕。如果屏幕锁定成功，则返回true，否则返回false。Promise方式 |
| SystemEvent { eventType: EventType, params: string } | 定义了系统事件回调参数结构，包含事件类型以及string类型的参数 |
| onSystemEvent(callback: Callback&lt;SystemEvent&gt;): boolean; | 系统API，注册与系统屏幕锁定相关的系统事件。如果注册系统事件成功，则返回true，否则返回false。callback方式 |
| sendScreenLockEvent(event: String, parameter: number, callback: AsyncCallback&lt;boolean&gt;): void; | 系统API，锁屏应用给锁屏管理服务发送事件。callback方式 |
| sendScreenLockEvent(event: String, parameter: number): Promise&lt;boolean&gt;; | 系统API，锁屏应用给锁屏管理服务发送事件。promise方式 |


**表 2**   EventType-事件类型说明

| 事件类型                     | 描述                       |
| -------------------------- | -------------------------- |
| beginWakeUp | 表示设备开始唤醒。 |
| endWakeUp | 表示设备结束唤醒。 |
| beginScreenOn | 表示设备开始亮屏。 |
| endScreenOn | 表示设备结束亮屏。 |
| beginScreenOff | 表示设备开始灭屏。 |
| endScreenOff | 表示设备结束灭屏。 |
| unlockScreen | 表示请求屏幕解锁。 |
| lockScreen | 表示请求屏幕锁定。 |
| beginExitAnimation | 表示开始退场动画。 |
| beginSleep | 表示设备开始休眠。 |
| endSleep | 表示设备结束休眠。 |
| changeUser | 表示切换用户。 |
| screenlockEnabled | 表示锁屏是否启用。 |
| serviceRestart | 表示锁屏服务进行重启。 |

### JS 接口使用示例

三方应用向锁屏管理服务进行查询屏幕锁屏状态

```js
导入模块
import screenLock from '@ohos.screenlock';

// Promise方式，在异步回调里面获取锁屏状态结果
screenLock.isScreenLocked()
    .then((data) => {
        // 异步回调打印查询锁屏状态的结果
        console.log(`Obtain whether the screen is locked successfully. result: ${data}`);
    }).catch((err) => {
        // 打印错误信息
        console.error(`Failed to obtain whether the screen is locked, because: ${err.message}`)
});

// callback方式，在异步回调里面获取锁屏状态结果
screenLock.isScreenLocked((err, data) => {
    if (err) {
        // 打印错误信息
        console.error(`Failed to obtain whether the screen is locked, because: ${err.message}`)
        return;
        }
    // 打印查询锁屏状态的结果
    console.log(`Obtain whether the screen is locked successfully. result: ${data}`);
});

 // 同步方式里面获取锁屏状态结果。如果屏幕当前已锁定，则返回true，否则返回false
let isLocked = screenLock.isLocked();

```

判断当前设备的屏幕锁定是否安全

```js

// Promise方式，在异步回调里面获取当前设备的屏幕锁定是否安全结果
screenLock.isSecureMode().then((data) => {
    console.log(`Obtain whether the device is in secure mode successfully. result: ${data}`);
}).catch((err) => {
    console.error(`Failed to obtain whether the device is in secure mode, because: ${err.message}`);
});

// callback方式，在异步回调里面获取当前设备的屏幕锁定是否安全结果
screenLock.isSecureMode((err, data)=>{      
    if (err) {
        console.error(`Failed to obtain whether the device is in secure mode, because: ${err.message}`);
        return;    
    }
    console.info(`Obtain whether the device is in secure mode successfully. result: ${data}`);
});

 // 同步方式里面获取当前设备的屏幕锁定是否安全结果。如果当前设备的屏幕锁定安全，则返回true，否则返回false
let isSecure = screenLock.isSecure();

```

锁定屏幕

```js

// Promise方式，在异步回调里面获取锁屏是否成功的结果
screenLock.lock().then((data) => {
    console.log(`lock the screen successfully. result: ${data}`);
}).catch((err) => {
    console.error(`Failed to lock the screen, because: ${err.message}`);
});

// callback方式，在异步回调里面获取锁屏是否成功的结果
screenLock.lock((err, data) => {      
    if (err) {
        console.error(`Failed to lock the screen, because: ${err.message}`);
        return;    
    }
    console.info(`lock the screen successfully. result: ${data}`);
});

```

锁屏应用注册事件说明:锁屏应用向锁屏管理服务注册相关监听事件

 ```js
try {
    let isSuccess = screenLock.onSystemEvent((event) => {
        console.log(`Register the system event which related to screenlock successfully. eventType: ${event.eventType}`)
    });
} catch (err) {
    console.error(`Failed to register the system event which related to screenlock, because: ${err.message}`)
}
 ```

三方应用向锁屏管理服务发起解锁屏幕请求

 ```js

// 三方应用callback方式调用请求解锁
screenLock.unlockScreen((err) => {
    if (err) {
        console.error(`Failed to unlock the screen, because: ${err.message}`);
        return;
    }
    console.info('unlock the screen success successfully.');
});

screenLock.unlock((err, data) => {      
    if (err) {
        console.error(`Failed to unlock the screen, because: ${err.message}`);
        return;    
    }
    console.info(`unlock the screen success successfully. result: ${data}`);
});

// 三方应用Promise方式调用请求解锁
screenLock.unlockScreen().then(() => {
    console.info('unlock the screen success successfully.');
}).catch((err) => {
    console.error(`Failed to unlock the screen, because: ${err.message}`);
});

screenLock.unlock().then((data) => {
    console.info(`unlock the screen success successfully. result: ${data}`);
}).catch((err) => {
    console.error(`Failed to unlock the screen, because: ${err.message}`);
});

```

## 相关仓

**主题框架子系统**

[theme\_screenlock_mgr](https://gitee.com/openharmony/theme_screenlock_mgr)

