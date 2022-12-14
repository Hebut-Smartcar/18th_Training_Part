# Week3

**大家好**，相信经过了前两周的学习，大家已经对**Altium Designer**的原理图库和**PCB封装库**有了一定的了解，而本周我们将学习驱动电路的**原理**和原理图的**绘制**。



## **全H桥电路驱动电机原理**

### 1、驱动电路的核心----H桥

![H桥](https://github.com/X-iye/Textnoob/blob/master/Image/H%E6%A1%A5.png?raw=true)

 从上图可以看出，驱动电路是由四个N[MOS管](https://so.csdn.net/so/search?q=MOS管&spm=1001.2101.3001.7020)，一个motor，以及VCC，GND所构成的。驱动芯片可以控制栅极的电平高低，来控制NMOS管的开通与关闭，所以可以通过控制四个栅极的状态来控制MOS管的开通与关断，从而达到控制电机正反转的效果。通过调控不同PWM占空比，达到控制电机转速快慢的效果。这种驱动电路就叫做**H桥**

### 2、H桥的工作模式

电机正转时，芯片会控制Q1和Q4两个MOS管导通，Q2和Q3两个MOS管截止，这样电流就要通过Q1，经过电机正侧M-A，M-B，再经过Q4回到GND

![](https://img-blog.csdnimg.cn/20200415125237908.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQ0ODk3MTk0,size_16,color_FFFFFF,t_70)

电机的反转，芯片会控制Q2和Q3两个MOS管导通，Q1和Q4两个MOS管截止，这样电流就要通过Q2，经过电机负侧M-B，M-A，在经过Q3回到GND

![](https://img-blog.csdnimg.cn/20200415125111761.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQ0ODk3MTk0,size_16,color_FFFFFF,t_70)



听起来很简单，但是在实际应用中，这样的电机驱动电路是不行的，电机是感性负载，在电路中电流不会发生突变。如果在断开电机两端所加的电压时，电机产生的**反向电动势很有可能损坏FET**。因此想让电机停下，除了断开供电，还要形成**一个续流的回路**，释放掉电机上的能量。会在MOS管的源极和漏极上并联一个寄生二极管，寄生二极管的方向和栅极方向相同，如下图：

![](https://img-blog.csdnimg.cn/20200415223448950.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQ0ODk3MTk0,size_16,color_FFFFFF,t_70)



### **3、死区控制**

**H桥中绝对不能出现同侧（左侧/右侧）的FET同时导通的情况**，因为这样会导致电流不经过电机直接到地，形成短路！因此在状态切换时需要一步一步来，而集成H桥的芯片一般会在内部自动解决这个问题（利用**死区控制**），如下图所示：在正转和制动之间切换时，会有一个过渡状态（OFF）。

![](https://img-blog.csdnimg.cn/20200417092217383.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQ0ODk3MTk0,size_16,color_FFFFFF,t_70)

此处还需补充一个知识：MOS管的**高端与低端驱动**。简单来说，高端驱动即MOS管在负载的高电位一端；相反低端驱动即MOS管在负载的低电位一端。如上图所示：Q1、Q3为高端驱动，Q2、Q4为低端驱动。在H桥中也常常被称为上臂和下臂。

此外，如果对MOS管原理有所了解，则可看出，打开高端NMOS所需的栅极电压会比打开低端NMOS所需的栅极电压大很多(要高于驱动电源电压）。(因为开启需要条件Vgs>Vth，而高端MOS导通后的源极电位较高，几乎接近电源电压，此时如果栅极电压仍为电源电压，则又关断)

**驱动电压越大，转速越快；电流越大，扭矩越大**

当扭矩<负载时，电机转速会下降，电流上升从而增大扭矩。当负载非常大，电机带不动从而停止转动时（堵转），电流达到最大值，此时需特别注意，很有可能烧坏电机驱动。

### **4、驱动芯片控制**

由于驱动直流电机需要的电流很大，单片机I/O的**驱动能力**是远远达不到的，因此需要使用专用的电机驱动芯片，下面我所引用的资料在Document文件夹中的数据手册里都有

#### **DRV8701E**

##### 1.引脚图

![](https://raw.githubusercontent.com/Hebut-Smartcar/18th_Training_Part/main/IMAGE/hardware_image/Week3/%E5%BC%95%E8%84%9A.png)

| 引脚名称 | 引脚标号 | 引脚功能&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; |
| :------: | :------: | :----------------------------------------------------------: |
|    VM    |    1     | 连接电机驱动电压5.9V-45V，使用0.1-µF陶瓷和额定VM的10-µF最小电容器旁路接地；根据驱动电流，可能需要额外的电容 |
|   GND    | 5 6 PPAD |                          与GND连接                           |
|   VCP    |    2     |                 将16V 1µF陶瓷电容器连接至VM                  |
|   CPH    |    3     |              在CPH和CPL之间接一个0.1uF的X7R电容              |
|   CPL    |    4     |              在CPH和CPL之间接一个0.1uF的X7R电容              |
|   DVDD   |    8     |       3.3V逻辑电源调节器；使用6.3V，1µF的陶瓷电容接地        |
|   AVDD   |    7     |       4.8V逻辑电源调节器；使用6.3V，1µF的陶瓷电容接地        |
|  nSleep  |    13    |  将逻辑拉低，使器件进入低功耗休眠模式，使用高阻态，内部下拉  |
|  Indive  |    12    |          施加在该引脚上的电阻值或电压设置门驱动电流          |
|   VREF   |    6     |             控制当前规律；施加0.3 V到之间的电压              |
|  nFAULT  |    9     |           故障条件下拉低逻辑；开漏输出需要外部上拉           |
|  SNSOUT  |    10    | 当驱动电流达到电流斩波阈值时，拉低逻辑；开漏输出需要外部上拉 |
|    SO    |    11    | 该引脚上的电压等于SP电压乘以AV加上偏移量；在该引脚上放置不超过1 nF的电容 |
|    SN    |    20    |               通过电流检测电阻器连接至SP并接地               |
|    SP    |    21    |            通过电流检测电阻器连接到低侧FET源和SN             |
|  GH1、2  |  17、24  |                     连接到高侧MOS管栅极                      |
|  GL1、2  |  19、22  |                     连接到低侧MOS管栅极                      |
|  SH1、2  |  18、23  |                   连接到高侧源极和低侧漏级                   |
|  IN1、2  |  15 14   |                        输入PWM控制H桥                        |



PS：由于MOS管导通后会产生一定的**饱和压降**(Vsat，不同芯片有较大差异，具体看手册)，因此在选择驱动电压VM时，可以接近或比所用电机额定电压稍高。

##### 2、芯片内部功能框图及外部电路设计

![](https://raw.githubusercontent.com/Hebut-Smartcar/18th_Training_Part/main/IMAGE/hardware_image/Week3/DRV8701E典型电路.png))

##### 3、PWM栅极驱动模式

![](https://raw.githubusercontent.com/Hebut-Smartcar/18th_Training_Part/main/IMAGE/hardware_image/Week3/PWM%E8%B0%83%E6%8E%A7.png)

DRV8701中的栅极驱动器直接驱动N沟道MOSFET，从而驱动**电机电流**。高压侧栅极驱动由电荷泵提供，低压侧栅极驱动器电压由内部

栅极驱动器的监管。峰值驱动电流可通过IDRIVE引脚调节。

**✔简单的来说**

通过**EN**和**PH**输入的DIR和PWM控制信号，分别控制电机M旋转的方向和旋转的速度

通过开关控制**nSleep**引脚，进而控制芯片的打开与关闭

**GH1GH2和GL1GL2**,控制4个MOS管的栅极，**SH1和SH2**控制源级和漏级同时连接电机(见上图)

至于详细的地方，如内部的逻辑控制或者详细的电路设计，都可以在**数据手册**里找到



#### **IR2104**

##### 1.引脚图

![](https://raw.githubusercontent.com/Hebut-Smartcar/18th_Training_Part/main/IMAGE/hardware_image/Week3/IR2104%E5%BC%95%E8%84%9A.png)

##### 2.具体讲解

相应讲解可以看下面博客

[IR2104驱动讲解](https://blog.csdn.net/zhuimeng_ruili/article/details/104378131)



## 原理图的绘制

在开始绘制原理图之前，我们必须要集齐以下两个条件

**1、封装库**

**2、装配上封装库的原理图库**

这方面的知识我们可以在**凡亿教育的AD20 19讲解视频**的P5~P15学到，其中包含了基本元器件和IC类器件的**原理图库绘制方法**（P5-P8），以及相应的**封装库的制作与寻找**(P14-P19)，在我们拥有了属于自己的一套原理图库和封装库后，我们才能真正开始绘制我们的原理图

[第6课 IC类元件模型的创建.mp4_哔哩哔哩_bilibili](https://www.bilibili.com/video/BV16t411N7RD?p=6&vd_source=8e15744aeb0c3fa9be9a9b3516128fb5)

PS：老师讲的是真的很详细，听完他的课后，足以让一个从来没接触过Altium Designer的人掌握原理图和PCB绘制的能力，智能车硬件必看！

拥有了完整的库，我们开始着手准备原理图的绘制

所谓原理图，就是表示电路板上各器件之间连接原理的图表，是不同完整电路的组合，一个完善没有缺漏的原理图是画好PCB的基础。文字还是不如视频，建议大家直接观看视频的**P9-P13**完成学习



**本周任务为绘制IR2104驱动电路，相应的原理图在Task文件夹下，请大家按时完成**



## 焊接基础

实验室为大家准备好了焊接练习板和电阻电容，周末可以来体育中心B109上手电烙铁实操练习，使用时请务必注意安全



