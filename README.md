# 社团活动管理系统

中文 | [English](./README.en.md) | [日本語](./README.ja.md)

社团活动管理系统是一个 C 语言课程设计项目。它使用 Mongoose 嵌入式 HTTP 服务器提供后端 API，用 HTML/CSS/JavaScript 构建前端页面，并通过本地文本文件保存学生账号、活动信息和删除记录。

项目重点是用较少依赖完成一个可运行的 B/S 风格管理系统：学生可以注册、登录、浏览活动并报名；管理员可以发布活动、修改活动、删除活动、查看报名名单和统计信息。

## 功能概览

### 学生端

- 学号注册与登录。
- 浏览活动列表。
- 按活动状态或类别查看活动。
- 报名参加活动。
- 查看报名状态。

### 管理员端

- 管理员登录。
- 发布活动，包含名称、类别、地点、容量和截止时间。
- 修改活动信息。
- 删除活动并写入回收记录。
- 查看活动报名学生。
- 查看活动统计图表。

### 后端校验

- 学号格式校验。
- 管理员账号保留。
- 活动日期格式校验。
- 报名人数限制。
- 重复报名校验。
- 活动过期状态更新。

## 技术栈

- C
- Mongoose HTTP library
- HTML / CSS / JavaScript
- Fetch API
- 本地 TXT 文件持久化

## 快速开始

### 编译

```bash
gcc main.c mongoose.c -o activity-system
```

如遇到平台网络库链接问题，请按本机编译环境补充对应链接参数。

### 运行

```bash
./activity-system
```

服务启动后访问：

```text
http://localhost:8000
```

默认管理员账号在 `main.c` 中定义：

```text
username: admin
password: admin123
```

## 项目结构

```text
.
├── Data/
│   ├── activity.txt
│   ├── student.txt
│   └── trash.txt
├── frontend/
│   ├── admin.html
│   ├── index.html
│   └── student.html
├── Doc.md
├── main.c
├── mongoose.c
└── mongoose.h
```

| 路径 | 说明 |
| --- | --- |
| `main.c` | HTTP 路由、业务逻辑、链表数据结构和文件读写 |
| `mongoose.c` / `mongoose.h` | 嵌入式 HTTP 服务器库 |
| `frontend/index.html` | 登录和注册入口 |
| `frontend/student.html` | 学生活动浏览与报名页面 |
| `frontend/admin.html` | 管理员后台页面 |
| `Data/activity.txt` | 活动信息与报名学生 |
| `Data/student.txt` | 学生账号信息 |
| `Data/trash.txt` | 删除活动备份记录 |
| `Doc.md` | 课程设计报告和模块说明 |

## 数据格式

活动数据保存在 `Data/activity.txt`。每行是一条活动记录，基本格式为：

```text
ID 名称 类别 地点 容量 状态 截止时间 报名学号1|报名学号2|...
```

学生数据保存在 `Data/student.txt`。每行包含学号、姓名、密码、手机号和班级。

## API 设计

后端通过 Mongoose 监听 `http://localhost:8000`，根据 URI 分发请求。主要接口包括：

| 接口 | 说明 |
| --- | --- |
| `/api/login` | 登录 |
| `/api/register` | 注册 |
| `/api/add` | 管理员发布活动 |
| `/api/change` | 管理员修改活动 |
| `/api/delete` | 管理员删除活动 |
| `/api/enroll` | 学生报名活动 |
| `/api/list` | 获取活动列表 |

未匹配 API 的请求会回退到 `frontend/` 静态文件服务。

## 设计说明

项目使用链表在内存中维护学生和活动数据，启动时从 `Data/` 目录加载，操作后再写回文本文件。这样实现简单、可读性高，适合课程设计展示数据结构、文件持久化和 HTTP 交互流程。

这个项目没有引入数据库、用户会话系统或生产级权限模型。它更适合作为 C 语言 Web 化实践、Mongoose 使用示例和课程设计材料。

## 许可证

当前仓库尚未声明开源许可证。复用、分发或作为课程提交材料二次使用前，请先确认授权要求。
