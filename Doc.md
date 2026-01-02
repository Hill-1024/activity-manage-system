# 题目: 社团活动管理系统

## 要求:

- 用户登陆/注册功能
- 学生报名功能
- 管理员发布/删除活动功能
- 管理员查看报名清单功能
- 具有图形化界面

## 项目结构

```FILE
|
|-Data //数据存储文件夹
| |-activity.txt
| |-student.txt
| |-trash.txt
|-frontend //前端界面文件夹
| |-admin.html
| |-index.html
| |-student.html
|-main.c //核心逻辑处理
|-mongoose.c //mongoose网络库文件
|-mongoose.h //mongoose网络库文件
```

## 总体设计

利用mongoose库提供的网络通信功能,让C/Cpp程序和前端直接通过JSON通信,让我们的逻辑处理程序充当web中的后端服务器处理前端的POST请求,让图形界面模块和逻辑处理模块解耦合,缩短开发流程

## 详细设计

### 流程图

```mermaid
graph TD
    A[程序启动]-->B[从文件夹开始load各项数据]
    B-->C[主程序开始对目标端口监听]
    C-->D[访问本地对应端口]
    D-->E[启动前端界面]
    E-->F{后端等待前端交互}
    F-->|最长轮询1000ms| F
    F-->|/api/login|login[登陆]-->|sid,password|back
    F-->|/api/register|register[注册]-->|sid,name,password,phone_number,class|back
    F-->|/api/activities|activities[获取活动列表]-->|aid|back
    F-->|/api/add|add[添加活动]-->|name,category,location,deadline,max_capacity|back
    F-->|/api/enroll|enroll[参加活动]-->|aid,sid|back
    F-->|/api/delete|delete[删除活动]-->|aid|back
    F-->|/api/details|details[获取学生详情页]-->|sid|back
    F-->|/api/status|status[获取活动绘图总览]-->back
    back[C程序后端]
```
### 前端请求:

- 登陆(/api/login):发送POST内容{sID,password}
    - 返回成功:依据账号类型跳转至目标界面
    - 返回失败:警告框提示密码错误
- 注册(/api/register):发送POST内容{sID,name,password,phone_number,class}
    - 返回成功:返回登陆界面
    - 返回失败:警告框提示相应问题
- 获取活动列表(/api/activities):无POST内容
    - 接受返回字符串,根据JSON改写内部HTML,显示活动状态
- 发布新活动(/api/add):发送POST内容{name,category,location,deadline,max_capacity}
    - 返回成功:重新获取活动列表
    - 返回失败:警告框提示相应问题
- 报名参加活动(/api/enroll):发送POST内容{aID,sID}
    - 返回成功:重新获取活动列表
    - 返回失败:警告框提示相应问题
- 删除活动(/api/delete):发送POST内容{aID}
    - 重新获取活动列表
- 获取学生详情(/api/details):发送POST内容{sID}
    - 展开表格显示详情
- 获取活动总览(/api/status):无POST内容
    - 根据返回数据绘制饼状图

## 感悟总结

本次课程设计中，我尝试跳出传统的“控制台（Console）程序”思维，挑战性地采用了**前后端分离**的开发模式。利用 C 语言配合 Mongoose 网络库作为后端服务器，与 HTML/JS 前端进行交互，这一过程让我受益匪浅。

1. **对 C 语言应用场景的拓展**：以往我认为 C 语言仅限于底层开发或算法实现，但通过这次项目，我意识到 C 语言同样可以处理 HTTP 请求，充当 Web 服务器。这让我对 socket 编程和网络协议（特别是 GET/POST 请求的处理）有了更直观的理解
2. **前后端交互与 JSON 处理**：由于 C 语言没有原生的 JSON 支持，在处理 /api/login 或 /api/enroll 等接口时，我不得不深入研究字符串的拼接与解析。这极大地锻炼了我对 char* 指针操作、内存分配（malloc/free）以及缓冲区溢出问题的把控能力
3. **系统解耦合的优势**：在开发过程中，我发现图形界面（前端）的修改完全不需要重新编译后端的 C 代码，两者的解耦显著提高了开发效率。这种架构思想比单纯的代码实现更为宝贵
4. **不足与展望**：目前数据存储仅依赖 TXT 文件，且缺乏完善的并发控制。在未来的学习中，我希望引入 SQLite 数据库来替代文件操作，并增加多线程支持，以提升系统的健壮性,同时,因引入多线程和数据库,我还将面对互锁,唯一占用等挑战
