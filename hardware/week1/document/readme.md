## 智能车常用电路模块基础知识介绍

### 电源管理相关

#### 锂电池(BAT)

锂电池是智能车硬件系统中的能源，是给小车所有功能实现提供电力的器件。电池	大家肯定都知道是个什么玩意，我这里边主要讲一讲智能车竞赛中常用的电池：

- 2S锂电池

    ![](https://gd3.alicdn.com/imgextra/i3/2364650632/O1CN016ANMaY1GXUwh1kkwY_!!2364650632.jpg)

    上图即为智能车中常用的2S锂电池，2S中的2代表的它由两3.7V锂电池组成，比如上图中的2S锂电池就是由两个18650电池构成的。一般3.7V的锂电池充电后满电电压能达到4.2V，所以买到一个7.2V的2S锂电池，在充满电后电压表测试电压为8.4V是正常现象。

    ![](https://gd4.alicdn.com/imgextra/i3/290188169/O1CN01cEx5R92ADSDeVFw6P_!!290188169.jpg)

    上图的这种也是2S锂电池，但是和之前蓝色的电池由两个18650锂电池不同，这种类型的锂电池是由两块3.7V的聚合物锂电池构成，同样在充满后能达到8.4V。

- 3S锂电池

    同理可得，3S锂电池就是2S锂电池在加上一个1S锂电池，其它的特性可以参考2S电池进行类比

#### LDO(low dropout regulator)

**LDO指的是一类器件，这种器件能将电压高过指定电压的部分进行截断，**这种器件在电路中就像在电源输入与输出之间放置了一个可调电阻，勉强承受着两个电压节点的电压降。例如使用经典的代号为LM1117的LDO(如下图)，可以简单且有效的将电压为5V的电压稳压(稳压为一种调节输出电压的概念，可以理解成调节为)3.3V(3.3V的电压在电路标识中常常使用3V3)，给下一级(这里的下一级指的值使用3.3V电压供电的电路)的电路。

![](https://www.dgzj.com/uploads/allimg/191026/1000222O6-0.jpg)

当然了，不是说只有长得像上图那样的就是LDO；同样，也不是长的和上面那张图一样的就是LDO，各种芯片的类型有各种各样，需要结合它的位置和周围的器件具体分析(目前我们只需要了解即可，不需要掌握如何分析)，就比如下面这个家伙也是LM1117。

![](https://assets.lcsc.com/images/szlcsc/900x900/C73181_front.jpg)

在原理图中，LDO的周边的器件(器件指的是电容电阻电感之类的基础元件)通常都十分简单，如下图

![](https://ts1.cn.mm.bing.net/th/id/R-C.00aee43022696cdf1333134e4e36ea86?rik=TQs4Qg2zR96t9g&riu=http%3a%2f%2fc.51hei.com%2fd%2fforum%2f201904%2f16%2f170536vlbkdllorln788pl.png&ehk=rwYVc3uRlz7%2fUXQJuOVMz8o1h%2bcwQy%2f5lAR0c9jnsxo%3d&risl=&pid=ImgRaw&r=0)

电路中具体的器件功能在大家分流后，在硬件方向重会着重讲解，这里我们只做了解即可。

**注：LDO的烧毁常常伴随着一缕青烟，随后将会飘来一股烧焦的刺鼻味道，不知道在座的各位谁有幸能亲身经历一下**

#### 开关电源(buck or boost or buck-boost)

- buck电源

    **buck电源指的是一种电源拓扑(电源拓扑指可以理解成电路的一种组成形式)，能将电源电压通过高速开关的形式以及电容电感储能元件将电压降低的电路。**区别于LDO直接阶段超过目标输出电压的降压形式，开关电源通过神奇的电子学(主要是高频电子线路！)，**魔法一般的**将高电压转变成低电压，并且保留了超过目标输出电压部分的能量(LDO则主要将这部分能量通过热能形式向空气传播)。

    其实上述知识不用太过纠结，只不过是我讲起来就停不下来了而已，具体的工作原理同样会在分流后硬件部分内容详细讲解，这里我们只做了解，**了解buck电路能降低电压，且工作原理和LDO不同**即可。

    同样，我们给大家一些经典的buck电路以及芯片的图片，如下所示：

    1. buck电路最基本拓扑

    ![](https://imgconvert.csdnimg.cn/aHR0cHM6Ly9waWM0LnpoaW1nLmNvbS84MC92Mi04NWZmZTA1ODI0ZTg1ODc0NDZjZGI3NGJiZTc3NzkxZV83MjB3LmpwZw?x-oss-process=image/format,png)

    2. 带开关电源芯片的buck电路原理图

        ![](https://pic1.zhimg.com/v2-c6185423edba996ee03483c6aa3ff36c_720w.jpg?source=172ae18b)

    3. 常见的buck芯片，LM2596

        <img src="https://p1.ssl.qhimg.com/dr/270_500_/t01374f1c7d82181a93.jpg?size=293x220" style="zoom:150%;" />

        <img src="https://ts1.cn.mm.bing.net/th/id/R-C.956c78865a17b9abfcb205f15006b84a?rik=Dqn2gOry%2bBtBzg&riu=http%3a%2f%2fpic.baike.soso.com%2fp%2f20120613%2f20120613183743-1596630954.jpg&ehk=8tZEOq8XAf8uGxqGaOSyb6Lk9BghipgsFbKlSLqLetw%3d&risl=&pid=ImgRaw&r=0&sres=1&sresct=1" style="zoom:50%;" />

- boost电源

    **boost电源指的是一种电源拓扑(电源拓扑指可以理解成电路的一种组成形式)，能将电源电压通过高速开关的形式以及电容电感储能元件将电压提高的电路。**能提高输出的电压是开关芯片的一大魔法，如果把电压降理解成水流，开关芯片所做的就是使水流逆流而上！这个比喻不太恰当，实际上boost这个拓扑不是使得“水流”逆转物理规律，而是征用一堆劳动力来搬运水，从低电势的位置将“水流”搬运到高电势的位置上

    1. boost电路基本拓扑

        ![](https://ts1.cn.mm.bing.net/th/id/R-C.0dd77e0dfb61d99f414260abf5d856c5?rik=hvmITI2D9fSPxg&riu=http%3a%2f%2ffile2.dzsc.com%2fdata%2f18%2f05%2f28%2f9207_175131560.jpg&ehk=BjiufRvQGU6%2bBmiTur%2bgqX2xeUTwcbQ9p%2bKe%2fdffvbo%3d&risl=&pid=ImgRaw&r=0&sres=1&sresct=1)

    2. 带开关芯片的boost原理图

        ![](https://file.diangongwu.com/img/20/0609/o4YBAF8VVbaAKg5qAACspesr0b0928.png)

    3. 常用的开关boost芯片MC34063

        ![](https://alimg.szlcsc.com/upload/public/product/source/20190814/CAAAC0D0687861FEC2D7CD98AD489A68.jpg)

        ![](https://alimg.szlcsc.com/upload/public/product/source/20191202/1ED11EA8D2A19983698A6C95B4779340.jpg)

- buck-boost电源

**buck-boost电源指的是一种电源拓扑(电源拓扑指可以理解成电路的一种组成形式)，能将电源电压通过高速开关的形式以及电容电感储能元件将电压提高或降低的电路。**这种电路能够自动调节自己工作在buck拓扑模式或者是boost拓扑模式，多用作于输入电压变化范围较大的稳压电路中，这里仅做了解

### 传感器相关

#### 	电感(inductor)

小车的寻迹方式大体分为两种，一种是电感寻迹，另外一种是摄像头寻迹。电感寻迹主要利用的是赛道中间的一条电磁线(如下图)，这根电磁线会通过频率为20KHz的电流信号，为电感提供电磁感应的能量。

![](https://img-blog.csdnimg.cn/20210627174256429.png#pic_center)

电磁检测传感器，这个传感器将会放大电磁感应的电压大小，使得它的大小能被单片机（MCU）检测到

![](https://gd4.alicdn.com/imgextra/i4/0/O1CN01znu0Ma1rqrPdWibe0_!!0-item_pic.jpg_400x400.jpg)

**小车通过判断左右几个电感的感应电压的大小，判断自己是否处于赛道的中央，借此数据来调整自己的位置**

#### 	摄像头(camera)

摄像头就相对来说更加符合我们的常识一点，**摄像头采集图像数据，通过判断赛道中的白色位置（赛道中心），区别于赛道边缘的蓝色位置（除赛道中心以外的位置）来判断小车的位置，**赛道如下图

![](https://pic2.zhimg.com/v2-5e2e035916127fa17145147858a05ed0_720w.jpg?source=172ae18b)

摄像头采集到的数据是一堆0与1（二进制嘛），这些0与1的数据将会传入到单片机中，单片机将这些0与1通过大家设计的算法进行解析，然后就能使得小车问问的跑在赛道中央了，如下为智能车常用的摄像头的图片

![](https://gd1.alicdn.com/imgextra/i1/2364650632/O1CN01QZhGMp1GXV1c9knPh_!!2364650632.jpg)

#### 	陀螺仪(gyro)

陀螺仪想必大家也不陌生，如今的陀螺仪可是一个手机或者手环的必需品。**它作为世界上最常用的运动传感器，可以将加速度，角加速度等物理量进行采集，然后将其数字化。**同样的，陀螺仪也是将这些数据采集到时候，转化成一堆0与1发送给单片机，我们智能车在应用陀螺仪主要是来判断小车旋转过的角度，借此来判断小车目前的姿态

陀螺仪芯片

![](https://ts1.cn.mm.bing.net/th/id/R-C.8a585cd997e4affc8bd4a8d3776a537b?rik=DtXQhiyQGQT7Zw&riu=http%3a%2f%2fwww.m-trade.cn%2fckfinder%2fuserfiles%2fimages%2fs_47840cd012ba43bb96387002367d1f7f.jpg&ehk=6wEb9OyKjZKjAgpz3RfSHEqTnPlbgflWNNBIK8HKpMc%3d&risl=&pid=ImgRaw&r=0)

智能车会用上的陀螺仪模块

![](https://gd1.alicdn.com/imgextra/i1/119255683/O1CN01oEGYNK1rqrScNV7l0_!!119255683.jpg)

#### 	编码器(encoder)

**编码器是一种速度传感器**，它通过齿轮啮合在电机齿轮上，通过计算编码器的转速和齿轮比，我们可以得到小车电机的旋转速度，借此来判断小车的运动速度。在小车速度的控制上，编码器是必不可少的一环

智能车常用的迷你微型小旋转编码器

![](https://gd1.alicdn.com/imgextra/i1/62252437/O1CN01uTt0rM1TsBq4Xs3AU_!!62252437.jpg)

### 人机交互相关

#### 	下载接口(download port)

**下载接口是我们下载单片机程序的接口，**下载的数据总会遵循某一种数据传输的格式或者说方法，常用的下载协议有JTAG、SWD、ISP等。不同型号的单片机会有不同的下载协议，不同的开发板（设计来让嵌入式工程师熟悉开发环境的电路板）根据设计者的喜好会有不同种类的下载接口。这里大家需要了解，下载程序只是对于传输的数据格式有要求，而不是对于传输的接口有要求，不同的下载接口可以是同一种下载协议，可以通过飞线，或者接线的方式实现不同接口之间的转换

如下面几个示例

![](/IMAGE/hardware_image/宏晶STC16核心板.jpg)

![](/IMAGE/hardware_image/沁恒CH32V307核心板.jpg)

![](/IMAGE/hardware_image/英飞凌TC264核心板.jpg)

#### 	串行接口(serial port)

串行接口是嵌入式系统中最常用的数据传输接口，串行接口有着数据传输的规定以及对应接口的规定，常见的接口规定有RS232，这种接口在老式PC，现在的工控机中均有大量出现，外形图如下图：

![](https://ts1.cn.mm.bing.net/th/id/R-C.2528188aee2022c35f4b76de52435b58?rik=QdIqrEaaNjckOg&riu=http%3a%2f%2fwww.lulian.cn%2fimages%2f201412%2f1417545331470045305.jpg&ehk=G9UGL9o7F8hEmB88cHE1BKbkqA9w05IeuASdwq6DJPw%3d&risl=&pid=ImgRaw&r=0)

我们智能车上的嵌入式应用主要是用来进行短距离的数据交换，所以用不上上述的RS232的规定，我们需要用到的是遵循基本TTL电平的串口传输，数据类型如下图所示

![](/IMAGE/hardware_image/UART帧格式.png)

这里我们只需要简单了解一下这种传输数据的方式就行，具体的数据格式和电平定义的方式会在后续的培训中进行详细的讲解

#### 	屏幕(screen)

屏幕，大家天天见，屏幕可以用来显示一些需要调试的数据或者监控的变量（变量指的是在运作过程中会变化的数据），智能车中也会用上不同的屏幕类型。和大家电脑或者手机的显示器的屏幕类型一样，可以分为IPS、TN等，这里不作详细介绍了。像下面的屏幕就是使用串口这种数据传输规定来驱动显示内容的屏幕

![](https://gd1.alicdn.com/imgextra/i4/2364650632/O1CN01BtcVHe1GXV4N8DtEQ_!!2364650632.jpg)

#### 	蜂鸣器(beep)

哔————

用以方便调试，毕竟小车一直在跑，不可能一直跟着跑看屏幕，这时候蜂鸣器刺耳且向量的身影往往能给调试着们带来极大的安全感（PS：或者是极大的恐惧），取决于你的代码。

![](https://ts1.cn.mm.bing.net/th/id/R-C.5c684754f4af688b09b0bc0bc5ba2027?rik=O%2f4i0HXzP3k0uw&riu=http%3a%2f%2fjkrnrwxhlkki5q.leadongcdn.cn%2fcloud%2fioBqkKqrSRrkmokmiki%2ffengmingqi.jpg&ehk=bHTG6NQvxwrbFzX8IrNhsnrJZwiaAGQAHMxBWprWMoI%3d&risl=&pid=ImgRaw&r=0)

这玩意一直叫的话能吵死人（

#### 	旋钮，又称旋转编码器(rotary encoder)

旋钮交互，日常生活中也很常见，如音响的音量旋钮。旋钮有用电位器实现的，也有用旋转编码器实现的，这里我们主要用的是旋转编码器，如下图。旋转编码器可以极大的方便各种参数的调试，是智能车人机交互方式的不二之选

![](https://ts1.cn.mm.bing.net/th/id/R-C.411a1d6a8f23333eb73f9caf7670ebbc?rik=iaueeFC7tqdIew&riu=http%3a%2f%2fwww.best-dz.com%2fUploadFiles%2f2014-11%2fadmin%2f2014112116291167393.jpg&ehk=A%2fk23wy3cMb%2bGIlI0850jjGZIp37hSY3P%2bGougwgaj0%3d&risl=&pid=ImgRaw&r=0&sres=1&sresct=1)

#### 	拨码开关(dip switch)

拨码开关是一种开关，它将使得输入给单片机的某几个信号一直处于0或者1的状态，一般用于单片机程序工作模式的选择，后续会在软件中具体讲解应用方式，实物如下图

![](https://tse4-mm.cn.bing.net/th/id/OIP-C.R28Pv_n-au69NDL9FBOapAHaHa?pid=ImgDet&rs=1)

#### 	按键(button)

按键，不用我多说吧，你们手机上的电源音量键等都是按键的一种，我就不多解释了（主要是懒

#### 	发光二极管(LED)

发光二极管通过光信号能快速的向人传递某种状态的信息，一般用以电路板整版功能验证，检测单片机程序是否正常运行。这里的LED是指的能焊接在电路板上的LED，一般情况下长下面这个样子

![](https://alimg.szlcsc.com/upload/public/product/source/20180907/377825F7866594E345B797D823A8FDE3.jpg)

### 动力相关

#### 	电机(motor)

智能车小车的肌肉，小车跑动的动力来源，电机需要通过专门的电机驱动电路来驱动才能正常的控制其正反转与速度的大小。一般使用的控制方法为PWM控制，后续在硬件和软件的培训中均会深入讲解，有兴趣的可以现在就先去看看，你会发现，智能车的大多控制就和魔法一样神奇

驱动电机的电路板

![](https://gd4.alicdn.com/imgextra/i4/2364650632/O1CN01Cmozu71GXV1mMYfSl_!!2364650632.jpg)

智能车常用的电机

![](https://gd1.alicdn.com/imgextra/i1/2364650632/TB2e.RwjtRopuFjSZFtXXcanpXa_!!2364650632.jpg)

#### 	舵机（servo）

用以四轮车模的转向控制，舵机根据不同的种类可以将PWM信号转化成为齿轮的位置或者是齿轮旋转的方向和角度。舵机是四轮车（强调四轮车是因为还有不用舵机调整拐向位置的车模，例如直立车和三轮车，这些请大家自行了解）方向控制的重要结构，类似汽车的前轮打角的功能，下图为舵机的图片

![](https://gd2.alicdn.com/imgextra/i4/2364650632/O1CN01mmpM221GXV5zpy0a5_!!2364650632.jpg)

## 这里第一周的硬件内容就暂告一段落了