# Pixel_City
# Pixel City - Game Hall

## 1. Introduction
Pixel City is a game hall project that integrates multiple game zones. Users can select different games to play after logging in or registering. The highlight is the Gobang (Five-in-a-Row) zone, where users can engage in online battles by choosing rooms or play against AI with托管 (hosting) feature. Each game round will adjust scores based on the game situation and rank players accordingly.

## 2. Technology Stack
- **Client**: Developed with QT on Windows. For the Gobang module, it uses array logic to judge piece placement and game results, and adopts frame synchronization for unified game management. The AI algorithm records game records and calculates the corresponding weight of each grid on the chessboard when placing pieces each time.
- **Server**: The code is in the `PlayServer` branch. It runs on Ubuntu 16 environment. It uses MySQL for data storage, defines corresponding structure nodes for each player, and uses hash tables and skip lists for score ranking. MD5 is used to simulate encryption for some content.

## 3. development environment
### 3.1 Client (Windows)
1. Ensure QT development environment is set up on Windows.
2. Clone the project repository.
3. Open the client project in QT Creator.
4. Build and run the client application.

### 3.2 Server (Ubuntu 16)
1. Switch to the `PlayServer` branch: `git checkout PlayServer`.
2. Install necessary dependencies, including MySQL.
3. Compile the server code.
4. Start the server application.

## 4. Features
- **User Management**: Support user registration and login.
- **Gobang Zone**:
  - Online battle between two players via room selection.
  - Battle against AI with hosting.
  - Score calculation and ranking based on game results.
- **Data Security**: Use MD5 to simulate encryption for partial content.


---

# Pixel City - 游戏大厅

## 1. 介绍
像素之城是一个集成了多种游戏专区的游戏大厅项目。用户登录或注册后，可选择不同游戏进行游玩。其中重点是五子棋专区，在该专区用户可通过选择房间进行双人在线对战，也可进行AI托管对战。每局游戏会根据对局情况调整分数并对玩家进行排名。

## 2. 技术栈
- **客户端**：在Windows系统上使用QT开发。五子棋模块通过数组逻辑判断落子和胜负情况，采用帧同步方式进行统一对局管理。人机算法通过记录棋谱，每次落子时计算棋盘格子的相应权值。
- **服务端**：代码位于`PlayServer`分支，运行在Ubuntu 16环境下。使用MySQL进行数据存储，为每个玩家定义相应的结构体节点，并利用哈希表和跳表进行分数排行。使用MD5对部分内容进行模拟加密。

## 3. 开发环境
### 3.1 客户端（Windows）
1. 确保Windows上已搭建QT开发环境。
2. 克隆项目仓库。
3. 在QT Creator中打开客户端项目。
4. 构建并运行客户端应用程序。

### 3.2 服务端（Ubuntu 16）
1. 切换到`PlayServer`分支：`git checkout PlayServer`。
2. 安装必要的依赖，包括MySQL。
3. 编译服务端代码。
4. 启动服务端应用程序。

## 4. 功能
- **用户管理**：支持用户注册和登录。
- **五子棋专区**：
  - 通过选择房间进行双人在线对战。
  - 进行AI托管对战。
  - 根据对局结果进行分数计算和排名。
- **数据安全**：使用MD5对部分内容进行模拟加密。
