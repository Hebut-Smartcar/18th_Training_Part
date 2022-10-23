# PCB layout层的绘制有以下几步：

#### 网表导入→规则设置→元器件的布局→布线→检查

**1>** **网表导入**

注意掌握导入的方法及错误修正

![](https://github.com/Hebut-Smartcar/18th_Training_Part/blob/main/IMAGE/hardware_image/clip_image001.png)

**2>** **规则设置**

 

推荐使用PCB的快捷键设置：

![](https://github.com/Hebut-Smartcar/18th_Training_Part/blob/main/IMAGE/hardware_image/clip_image003.png)

​    

规则的设置关系到生产出PCB质量的好坏，主要体现在间距，过孔，走线等几个方面，同时注意必须开启全部的电气规则，这将影响你的电路基本性能。

间距规则：线距，华秋还是比较友善的，免费打板要求最小线距>5mil。由于以前用嘉立创打板子，其最小线宽要求是6mil，因此一直沿用。（同时，给同胞门提个醒，嘉立创改规则了，想要在嘉立创免费打板子，需要使用嘉立创的EDA设计，或者上个月消费超过20元）

![](https://github.com/Hebut-Smartcar/18th_Training_Part/blob/main/IMAGE/hardware_image/clip_image005.png)

​    线宽规则：华秋要求最小线宽在5mil，一般信号线的线宽比较小，这样可以更好的传输信号；电源线的线宽较大一些，以保证通过足够大的电流.

![](https://github.com/Hebut-Smartcar/18th_Training_Part/blob/main/IMAGE/hardware_image/clip_image007.png)

​    过孔规则：华秋要求孔径在0.2mm（约等于8mil）以上

**3>** **元器件的布局**

此次要求画四层的驱动板，顶层和底层设置为信号层，中间两层为电源层。

注意事项：布局的首要原则是保证布线的布通率，移动器件时注意线的连接；

​        模块化布局，即一个模块的元器件尽量放到一块；

​        数字器件和模拟器件要分开，尽量远离；

​        滤波电容尽量靠近IC器件；

​     放置器件时要考虑以后的焊接，不要太密集；

​        

**4>** **布线**

切记自动布线，自动布线出的错误千奇百怪，当你的工程量比较大的时候，找错的时间比手工绘制的时间都要长。

电源线与地线尽量加粗，具体的线宽与电流对应关系如下表

 

![](https://github.com/Hebut-Smartcar/18th_Training_Part/blob/main/IMAGE/hardware_image/clip_image009.png)

​    

零件排列时各部份电路尽可能排列在一起，走线尽可能短；

如果两条线路之间的电压差较大时需注意安全间距；

线路拐角时尽量不要有锐角、直角，对高频电路而言；

两条线路最好不要平行走太长，以减少分布电容的影响最好用钝角和圆弧；

​    高频电路须考虑地线的高频阻抗，一般采用大面积接地的方式，各点就近接地。

 

**5>** **检查**

根据DRC的检测修正电路。

 

 

**当以上步骤都完成后，你就可以打板子了，可以选择华秋免费打板，以下是打板方法：**

https://www.hqpcb.com/pcbnews/1700.html

