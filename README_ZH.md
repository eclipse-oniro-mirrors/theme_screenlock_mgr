# 锁屏组件

-   [简介](#section11660541593)
-   [目录](#section161941989596)
-   [说明](#section38521239153117)
    -   [js接口说明](#section11908203714422)
    -   [js接口使用说明](#section9938411124317)
-   [编译调试](#section38521239153118)
-   [相关仓](#section1371113476307)
-   [参与贡献](#section1371113476308)

## 简介<a name="section11660541593"></a>

向三方应用提供请求解锁，查询锁屏状态，查询是否设置锁屏密码的能力。向应运行管理提供亮屏回调，灭屏回调，屏保进入退出回调，用户切换回调,锁屏管理服务运行状态回调

**图 1** 子系统架构图  
![](figures/subsystem_architecture_zh.png "子系统架构图")


## 目录<a name="section161941989596"></a>

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

## 说明<a name="section38521239153117"></a>

1.  js 接口使用说明
<a name="table033515471012"></a>

<table><thead align="left"><tr id="row143351854201012"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p103351154121010"><a name="p103351154121010"></a><a name="p103351154121010"></a>接口名</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p1033585416105"><a name="p1033585416105"></a><a name="p1033585416105"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row204321219393"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1893413268144"><a name="p1893413268144"></a><a name="p1893413268144"></a>function isScreenLocked(callback: AsyncCallback<boolean>): void;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p18761104812149"><a name="p18761104812149"></a><a name="p18761104812149"></a>判断屏幕是否锁屏，callback方式</p>
</td>
</tr>
<tr id="row13335054111018"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p12832214151418"><a name="p12832214151418"></a><a name="p12832214151418"></a>function isScreenLocked(): Promise<boolean>;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p3335145451011"><a name="p3335145451011"></a><a name="p3335145451011"></a>判断屏幕是否锁屏，Promise方式</p>
</td>
</tr>
<tr id="row204321219393"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1893413268144"><a name="p1893413268144"></a><a name="p1893413268144"></a>function isSecureMode(callback: AsyncCallback<boolean>): void;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p18761104812149"><a name="p18761104812149"></a><a name="p18761104812149"></a>判断当前设备的屏幕锁定是否安全(安全屏幕锁定意味着解锁屏幕需要密码、图案或其他用户身份识别)，callback方式</p>
</td>
</tr>
<tr id="row204321219393"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1893413268144"><a name="p1893413268144"></a><a name="p1893413268144"></a>function isSecureMode(): Promise<boolean>;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p18761104812149"><a name="p18761104812149"></a><a name="p18761104812149"></a>判断当前设备的屏幕锁定是否安全(安全屏幕锁定意味着解锁屏幕需要密码、图案或其他用户身份识别)，Promise方式</p>
</td>
</tr>
<tr id="row204321219393"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1893413268144"><a name="p1893413268144"></a><a name="p1893413268144"></a>function unlockScreen(callback: AsyncCallback<void>): void;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p18761104812149"><a name="p18761104812149"></a><a name="p18761104812149"></a>解锁屏幕，callback方式</p>
</td>
</tr>
<tr id="row204321219393"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1893413268144"><a name="p1893413268144"></a><a name="p1893413268144"></a>function unlockScreen(): Promise<void>;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p18761104812149"><a name="p18761104812149"></a><a name="p18761104812149"></a>解锁屏幕，Promise方式</p>
</td>
</tr>
<tr id="row204321219393"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1893413268144"><a name="p1893413268144"></a><a name="p1893413268144"></a>function on(type: 'beginWakeUp' | 'endWakeUp' | 'beginScreenOn' | 'endScreenOn' | 'beginScreenOff' | 'endScreenOff' | 'unlockScreen' | 'beginExitAnimation' | 'systemReady', callback: Callback<void>): void;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p18761104812149"><a name="p18761104812149"></a><a name="p18761104812149"></a>注册系系统事件</p>
</td>
</tr>
<tr id="row204321219393"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1893413268144"><a name="p1893413268144"></a><a name="p1893413268144"></a>function on(type: 'beginSleep' | 'endSleep' | 'changeUser', callback: Callback<number>): void;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p18761104812149"><a name="p18761104812149"></a><a name="p18761104812149"></a>注册系系统事件</p>
</td>
</tr>
<tr id="row204321219393"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1893413268144"><a name="p1893413268144"></a><a name="p1893413268144"></a>function on(type: 'screenlockEnabled', callback: Callback<boolean>): void;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p18761104812149"><a name="p18761104812149"></a><a name="p18761104812149"></a>注册系系统事件</p>
</td>
</tr>
<tr id="row204321219393"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1893413268144"><a name="p1893413268144"></a><a name="p1893413268144"></a>function off(type: 'beginWakeUp' | 'endWakeUp' | 'beginScreenOn' | 'endScreenOn' | 'beginScreenOff' | 'endScreenOff' | 'unlockScreen' | 'beginExitAnimation' | 'systemReady', callback: Callback<void>): void;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p18761104812149"><a name="p18761104812149"></a><a name="p18761104812149"></a>取消注册系系统事件</p>
</td>
</tr>
<tr id="row204321219393"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1893413268144"><a name="p1893413268144"></a><a name="p1893413268144"></a>function off(type: 'beginSleep' | 'endSleep' | 'changeUser', callback: Callback<number>): void;<boolean>): void;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p18761104812149"><a name="p18761104812149"></a><a name="p18761104812149"></a>取消注册系系统事件</p>
</td>
</tr>
<tr id="row204321219393"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1893413268144"><a name="p1893413268144"></a><a name="p1893413268144"></a>function off(type: 'screenlockEnabled', callback: Callback<boolean>): void;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p18761104812149"><a name="p18761104812149"></a><a name="p18761104812149"></a>取消注册系系统事件</p>
</td>
</tr>
<tr id="row204321219393"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1893413268144"><a name="p1893413268144"></a><a name="p1893413268144"></a>function sendScreenLockEvent(event: String, parameter: number, callback: AsyncCallback<boolean>): void;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p18761104812149"><a name="p18761104812149"></a><a name="p18761104812149"></a>锁屏应用给锁屏服务发送事件</p>
</td>
</tr>
<tr id="row204321219393"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p1893413268144"><a name="p1893413268144"></a><a name="p1893413268144"></a>function sendScreenLockEvent(event: String, parameter: number): Promise<boolean>;</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p18761104812149"><a name="p18761104812149"></a><a name="p18761104812149"></a>锁屏应用给锁屏服务发送事件</p>
</td>
</tr>
</tbody>
</table>
2.  js 接口使用说明

```
// 导入模块
import screenLock from '@ohos.screenlock';

// Promise方式的异步方法查询屏幕锁屏状态
screenLock.isScreenLocked()
    .then((value) => {
        console.log(`success to screenLock.isScreenLocked: ${value}`);
    }).catch((err) => {
        console.error(`failed to screenLock.isScreenLocked because ${err.message}`)
});


// callback方式的异步方法设置时间
screenLock.isScreenLocked((err, value) => {
    if (err) {
        console.error(`failed to screenLock.isScreenLocked because ${err.message}`);
        return;
        }
    console.log(`success to screenLock.isScreenLocked: ${value}`);
});

//注册系统事件

//'beginWakeUp' | 'endWakeUp' | 'beginScreenOn' | 'endScreenOn' | 'beginScreenOff' | 'endScreenOff' | 
//'unlockScreen' | 'beginExitAnimation' | 'beginSleep' | 'endSleep' | 'changeUser' | 'screenlockEnabled'
var eventType = "beginWakeUp";
screenLock.on(eventType, (err, value) => {
    if (err) {
        // 接口调用失败，err非空
        console.error(`screenlockOn_unlockScreen_callback failed, because ${err.message}`);
        return;
    }
    // 接口调用成功，err为空
    console.log(`screenlockOn_unlockScreen_callback success to ${value} `);
});

//解锁屏幕
screenLock.unlockScreen((err, data) => {
    console.log("Screenlock_Test_2300: send unlockScreen issue success");
    if (err) {
    console.log("Screenlock_Test_2300: unlockScreen fail-->"+err);
    return;
    }
    console.log("Screenlock_Test_2300: unlockScreen success-->"+data);
});

screenLock.unlockScreen().then((data) => {
    console.log("ScreenLock_Test_Promise_0500: unlockScreen success-->"+data);
}).catch((error) => {
    console.error("ScreenLock_Test_Promise_0500: unlockScreen fail--> " + error);
});
```

## 编译调试<a name="section38521239153118"></a>

1.  配置编译参数
```
1. 仓库代码下载下来，修改工程名为screenlock，放在源码  \base\miscservices 目录下
```
```
2. OpenHarmony/productdefine/common/products/Hi3516DV300.json中在文件末尾添加  "miscservices:screenlock":{}
```

![](figures/step1.png "step 1")

```
3. 在foundation/distributedschedule/samgr/interfaces/innerkits/samgr_proxy/include 的system_ability_definition.h中添加自己服务的id:3704。 
信息：SCREENLOCK_SERVICE_ID = 3704,
```

![](figures/step2.png "step 2")

```

2.  编译命令

```
./build.sh --product-name (填写具体的产品名，如：Hi3516DV300) --build-target screenlock_native
```

3.  推送 so 文件

```
将工程目录下out\ohos-arm-release\miscservices\screenlock_native下的libscreenlock_server.z.so libscreenlock_client.z.so
libscreenlock_utils.z.so三大so推送到system/lib，

将libscreenlockability.z.so推送到system/lib/module/app下，并确保四个so至少为可读状态。
```

4.  重启设备
```
## 相关仓<a name="section1371113476307"></a>

**Misc软件服务子系统**

/base/miscservices/screenlock

## 参与贡献<a name="section1371113476308"></a>

1.  Fork 本仓库
2.  提交代码
3.  新建 Pull Request
4.  commit 完成即可
