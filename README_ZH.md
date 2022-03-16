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
/base/miscservices/screenlock
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
## JS 接口说明

| 接口名                      | 描述                       |
| -------------------------- | -------------------------- |
| isScreenLocked(callback: AsyncCallback<boolean>): void; | 判断屏幕是否锁屏，callback方式 |
| isScreenLocked(): Promise<boolean>; | 判断屏幕是否锁屏，Promise方式 |
| isSecureMode(callback: AsyncCallback<boolean>): void; | 判断当前设备的屏幕锁定是否安全(安全屏幕锁定意味着解锁屏幕需要密码、图案或其他用户身份识别)，callback方式 |
| isSecureMode(): Promise<boolean>; | 判断当前设备的屏幕锁定是否安全(安全屏幕锁定意味着解锁屏幕需要密码、图案或其他用户身份识别)，Promise方式 |
| unlockScreen(callback: AsyncCallback<void>): void; | 三方应用解锁屏幕，callback方式 |
| unlockScreen(): Promise<void>; | 三方应用解锁屏幕，Promise方式 |
| on(type: 'beginWakeUp' , callback: Callback<void>): void; | 锁屏应用注册开始唤醒监听事件 |
| on(type: 'endWakeUp' , callback: Callback<void>): void; | 锁屏应用注册结束唤醒监听事件 |
| on(type: 'beginScreenOn' , callback: Callback<void>): void; | 锁屏应用注册开始亮屏监听事件 |
| on(type: 'endScreenOn' , callback: Callback<void>): void; | 锁屏应用注册结束亮屏监听事件 |
| on(type: 'beginScreenOff' , callback: Callback<void>): void; | 锁屏应用注册开始灭屏监听事件 |
| on(type: 'endScreenOff' , callback: Callback<void>): void; | 锁屏应用注册结束灭屏监听事件 |
| on(type: 'unlockScreen' , callback: Callback<void>): void; | 锁屏应用注册请求解锁监听事件 |
| on(type: 'beginExitAnimation' , callback: Callback<void>): void; | 锁屏应用注册开始退场监听事件 |
| on(type: 'systemReady' , callback: Callback<void>): void; | 锁屏应用注册锁屏管理服务系统准备完成监听事件 |
| on(type: 'beginSleep' , callback: Callback<number>): void; | 锁屏应用注册开始休眠监听事件 |
| on(type: 'endSleep' , callback: Callback<number>): void; | 锁屏应用注册结束休眠监听事件 |
| on(type: 'changeUser', callback: Callback<number>): void; | 锁屏应用注册切换用户监听事件 |
| on(type: 'screenlockEnabled', callback: Callback<boolean>): void; | 锁屏应用注册锁屏是否启用监听事件 |
| off(type: 'beginWakeUp' , callback: Callback<void>): void; | 锁屏应用取消开始唤醒监听事件 |
| off(type: 'endWakeUp' , callback: Callback<void>): void; | 锁屏应用取消结束唤醒监听事件 |
| off(type: 'beginScreenOn' , callback: Callback<void>): void; | 锁屏应用取消开始亮屏监听事件 |
| off(type: 'endScreenOn' , callback: Callback<void>): void; | 锁屏应用取消结束亮屏监听事件 |
| off(type: 'beginScreenOff' , callback: Callback<void>): void; | 锁屏应用取消开始灭屏监听事件 |
| off(type: 'endScreenOff' , callback: Callback<void>): void; | 锁屏应用取消结束灭屏监听事件 |
| off(type:  'unlockScreen' , callback: Callback<void>): void; | 锁屏应用取消请求解锁监听事件 |
| off(type:  'beginExitAnimation' , callback: Callback<void>): void; | 锁屏应用取消开始退场监听事件 |
| off(type: 'systemReady', callback: Callback<void>): void; | 锁屏应用取消锁屏管理服务系统准备完成监听事件 |
| off(type: 'beginSleep' , callback: Callback<number>): void;<boolean>): void; | 锁屏应用取消开始休眠监听事件 |
| off(type: 'endSleep' , callback: Callback<number>): void;<boolean>): void; | 锁屏应用取消结束休眠监听事件 |
| off(type: 'changeUser', callback: Callback<number>): void;<boolean>): void; | 锁屏应用取消切换用户监听事件 |
| off(type: 'screenlockEnabled', callback: Callback<boolean>): void; | 锁屏应用取消锁屏是否启用监听事件 |
| sendScreenLockEvent(event: String, parameter: number, callback: AsyncCallback<boolean>): void; | 锁屏应用给锁屏管理服务发送事件,callback方式 |
| sendScreenLockEvent(event: String, parameter: number): Promise<boolean>; | 锁屏应用给锁屏管理服务发送事件,promise方式 |

## JS 接口使用示例

```js
导入模块
import screenLock from '@ohos.screenlock';

查询屏幕状态接口说明: 三方应用向锁屏管理服务进行查询屏幕锁屏状态
//Promise方式，在异步回调里面获取锁屏状态结果
screenLock.isScreenLocked()
    .then((value) => {
        //异步回调打印查询锁屏状态的结果
        console.log(`success to screenLock.isScreenLocked: ${value}`);
    }).catch((err) => {
        //打印错误信息
        console.error(`failed to screenLock.isScreenLocked because ${err.message}`)
});
```

 ```js
 //callback方式，在异步回调里面获取锁屏状态结果
screenLock.isScreenLocked((err, value) => {
    if (err) {
        //打印错误信息
        console.error(`failed to screenLock.isScreenLocked because ${err.message}`);
        return;
        }
    //打印查询锁屏状态的结果
    console.log(`success to screenLock.isScreenLocked: ${value}`);
});
```

锁屏应用注册事件说明:锁屏应用向锁屏管理服务注册相关监听事件

事件类型beginWakeUp示例代码如下
 ```js
var eventType = "beginWakeUp";
screenLock.on(eventType, (err, value) => {
    if (err) {
        // 接口调用失败，打印错误信息
        console.error(`screenlockOn_unlockScreen_callback failed, because ${err.message}`);
        return;
    }
    // 接口调用成功，打印返回信息
    console.log(`screenlockOn_unlockScreen_callback success to ${value} `);
});
 ```

三方应用向锁屏管理服务发起解锁屏幕请求
 ```js
//三方应用callback方式调用请求解锁
screenLock.unlockScreen((err, data) => {
    console.log("Screenlock_Test_2300: send unlockScreen issue begin");
    if (err) {
    // 接口调用失败，打印错误信息
    console.log("Screenlock_Test_2300: unlockScreen fail-->"+err);
    return;
    }
    // 接口调用成功，打印返回信息
    console.log("Screenlock_Test_2300: unlockScreen success-->"+data);
});

screenLock.unlockScreen().then((data) => {
    // 接口调用成功，打印返回信息
    console.log("ScreenLock_Test_Promise_0500: unlockScreen success-->"+data);
}).catch((error) => {
    // 接口调用失败，打印错误信息
    console.error("ScreenLock_Test_Promise_0500: unlockScreen fail--> " + error);
});
```

## 相关仓

**Misc软件服务子系统**

 miscservices_screenlock
