# QT网络跳棋设计文档

李骋昊 2021010826  2022-08-26T11:58:12+08:00 





## 工作流程

### 创建连接

#### 客户端

用户点击菜单 $\rightarrow$ 弹出窗口，用户输入IP  $\rightarrow$ 验证输入格式合法性 $\rightarrow$ 尝试建立TCP连接 $\rightarrow$ 收到测试报文后在底部状态栏显示connected

#### 服务器端

用户点击菜单 $\rightarrow$ 弹出窗口，显示本机IP

 建立TCP连接后$\rightarrow$ 在底部状态栏显示connected$\rightarrow$向客户端发送测试报文

### 开始游戏（客户端发起）

#### 客户端

用户点击菜单 $\rightarrow$ 底部状态栏显示invitation sent $\rightarrow$ 向服务器端发送邀请报文

若收到发送游戏开始，并等待的报文，更新本地界面显示

若受到发送请先手走棋的报文，调用走棋

#### 服务器端

收到邀请报文 $\rightarrow$ 底部状态栏提示用户点击play->start接受邀请 $\rightarrow$ 随机指定先手

若服务器端先手，调用走棋，并向客户端发送游戏开始，并等待的报文

若服务器端后手，向客户端发送请先手走棋的报文，并更新本地界面显示



### 开始游戏（服务器发起）

#### 客户端

收到邀请报文$\rightarrow $ 底部状态栏提示用户点击play->start接受邀请 $\rightarrow$ 向服务器发送接受邀请报文

若收到发送游戏开始，并等待的报文，更新本地界面显示

若受到发送请先手走棋的报文，调用走棋

#### 服务器端

用户点击菜单 $\rightarrow$ 底部状态栏显示invitation sent $\rightarrow$ 向客户端发送邀请报文

收到接受邀请报文  $\rightarrow$ 随机指定先手

若服务器端先手，调用走棋，并向客户端发送游戏开始，并等待的报文

若服务器端后手，向客户端发送请先手走棋的报文，并更新本地界面显示



### 走棋

收到先手（后手）走棋的报文$\rightarrow$ 更新棋盘信息$\rightarrow$ 设定计时器$\rightarrow$ 允许用户移动棋子

用户成功下棋  或者 超时 $\rightarrow$  判断是否分出输赢 $\rightarrow$  若分出输赢，调用游戏结束，否则向对方发送含有当前信息的后手（先手）走棋报文 $\rightarrow$ 设定计时器

### 游戏结束

本地发现分出胜负 $\rightarrow$ 本地提示 $\rightarrow$ 向对方发送对局结束信息

收到对局结束信息  $\rightarrow$  提示对局结束



## 通信协议

共288个bit。每4个bit转成一位16进制数，加上40后作为一个字节发送，共72个字节。

第0~3位：提示信息。
	0000 ：空。
	0001 ：我方本局超时，将行动权交出。
	0010 ：我方本局超时3次，因此失败。
	0011 ：我方认输。
	0100 ：我方没有达到特定局数移开初始营地的棋子的数量要求，因此失败。
	1000 ：我方棋子全部到达目的地，因此获胜。
	1001 ：我方发起邀请。
	1011 ：我方同意邀请。
	1100 ：游戏开始。

第4到第245位：表示当前棋盘状态。
	每2位表示一格，格的顺序是从上到下，从左到右，编号为0~120.
	用01表示先手棋子。用10表示后手棋子，00表示空。
第246~247位：表示对对方的要求。
	00 ：请对方执先手走一步棋。
	01 ：请对方执后手走一步棋。
	10 ：暂时无动作。（在初始分配先后手时或者请求开始新一局时）
	11 ：胜负已分，停止下棋。

第248~263位：以int16表示的当前局数。252为最高位，267为最低位。
第264~279位：对上一次行动的棋子的描述。
	264~271：8位整数，表示移动的棋子的初始位置。271为最低位。127表示不存在上一次移动或者上一次是超时。
	271~279：8位整数，表示移动的棋子的结束位置。279为最低位。127表示不存在上一次移动或者上一次是超时。
	
第280~283位：双方当前超时计数：
	280~281：先手
	282~283：后手
	00, 01, 10, 11分别表示0,1,2,3次。

第284~287位 : 为0000



## 网络编程框架

### 建立连接

#### 客户端

调用void GameManager::connectHost()，创建一个readWriteSocket。调用其connectHost方法，根据用户输入的IP地址尝试建立连接。

将这个readWroteSocket 的 readyRead() signal 与 recvMessage()连接。

#### 服务器端

调用GameManager::initServer()，初始化一个listenSocket。调用其listen方法。并将listenSocket的newConnection signal和acceptConnection()方法连接。

在acceptConenction()方法被触发后，将listenSocket的nextPendingConnection 方法赋值给readWriteSocket。将这个readWroteSocket 的 readyRead() signal 与 recvMessage()连接。

### 接受报文

recvMessage()方法调用readWriteSocket的readall方法，存在名为info_got的缓冲区。如果累计读取的字节数是72，则调用handle_receive()方法处理接受到的报文（此为上文提到的“收到”报文），并清零接收字节计数。否则，更新接收字节计数。

### 发送报文

send()方法调用readWriteSocket的write方法，发送格式如前述的72个字节的QByteArray数据。

## 信号与槽机制设计

### 通信方面

如前述。

### 本地

Board 类继承自 QWidget，实现了一个棋盘。提供了click_chess(int)信号指出点击了的棋子编号。提供successfully_moved(int a, int b)信号，表明成功将棋子从a位置移动到b位置。提供了move_permitted(), move_banned(), set_id(), set_last_move(),unset_last_move等用于更新其显示状态，以及可交互性的槽。

SideBar 类继承自 QWidget，实现了一个显示信息用的侧栏。提供了time_out()信号提示时间到。提供了set_id() set_round_count(),set_your_id()等用于更新其显示状态的槽。

GameManager类用于控制流程。用step_handler 接受successfully_moved(int a, int b)信号。用time_out_handler接受time_out信号。对于介绍到的报文中的四种请求，用receive_move_first与move_first, receive_move_second与move_second,  receive_wait和 do_wait, receive_stop 和 do_step四组信号和槽来处理。

MainWindow类对于其中的各个菜单， 都提供接受triggered信号的用于处理的函数on_create_the_connection_triggered()， on_connect_to_server_triggered()， on_Start_triggered() 和on_admit_defeat_triggered()。



## GUI页面设计

所有需要显示的内容都在Mainwindow中显示。分为四个区域：

顶部： 菜单栏。提供connect菜单（包括create connection, connect to server 两个action) 和 play菜单（包括start 和 admit defeat 两个action）。

中间区域左侧：Board。显示棋盘。其中包含的元素的具体大小根据窗口的长宽之间的较小值确定。空格用黑色的小圆圈表示。两种棋子分别用红色和绿色的较大圆圈表示。点击前，若存在对方上一次走的子，将在对方上一次走的子出发的位置绘制金色十字，在对方上一次走的子的到达位置绘制金色小圆圈。在点击己方的棋子后，将在被点击的己放棋子上面绘制金色十字，在所有可能到达的位置绘制金色小圆圈。红色一方为先手，出发位置在上方。绿色一方为后手，出发位置在下方。

中间右侧区域： Sidebar。靠上区域为执红色棋子的一方的信息。包括棋子颜色（"you"标识，如果是本方），超时数量，倒计时（如果为当前走棋方）。每一行的行高和字号根据界面的总高度确定，信息通常为黑色。走棋方的棋子颜色标识将被渲染成金色。当超时数量为2，或者剩余时间为1~3秒时，对应文字将为红色。当超时数量为3，或者剩余时间0秒时，将用深红色表示 。

底部：状态栏，左侧显示连接状态等，右侧显示身份(server/client)。
