<h1>IIC,SPI通信,flash存储操作，陀螺仪姿态解算，屏幕显示以及合适的调参工具</h1>

<h2>目录</h2>

<ul>
<li>一.前言</li>
<li>二.数据和flash读写</li>
    <ul>
        <li>stm32单片机中的内存映射</li>
        <li>计算机中的内存映射</li>
    </ul>
<li>三.IIC和SPI通信</li>
<li>四.实际应用之陀螺仪和屏幕</li>
<li>五.合适的调参工具</li>
</ul>

<h3>一.前言</h3>

<p>本周主要涉及重点内容是<strong>两种常见的通信协议：IIC和SPI</strong>，需要熟悉的内容的是<strong>flash读写操作和调参工具</strong>。<br>
本周涉及的通信协议仅包含IIC和SPI两种通信协议，通信方式只讨论主从设备之间的通信，请大家不要产生误解。</p>

<h3>二.数据和flash读写</h3>

<ul>

    <li><h4>RAM和ROM：</h4>
        <ol>
            <li>在单片机中，指令和数据均以二进制的方式存储，这取决于物理电路的低电平和高电平，低电平为0，高电平为1，这样就使得一切数据都是以二进制的方式存储。</li>
            <li>在现代计算机体系中，存储系统主要分为ROM和RAM两部分，把他们代入到计算机上就是内存和硬盘，RAM具有<strong>掉电数据丢失</strong>的特点,而ROM具有<strong>掉电数据不易丢失</strong>的特点，最通俗易懂的解释就是我们每次关机都会结束目前进行的进程，再次开机所有进程都需要重新启动，而硬盘里的文件等等基本不会因为开关机而发生变化。</li>
        </ol>
    </li>
    
    <li><h4>stm32单片机中的flash映射：</h4>
        <ol>
            <li>flash基本概念：stm32中的flash，相当于对代码的储存空间,stm32的flash地址起始于0x08000000，结束地址是0x08000000加上芯片实际的flash大小，不同的芯片flash大小不同。</li>
            <li>flash存在意义：记录单片机中的代码，当单片机掉电时存储这些代码，上电时保证能执行代码，flash存储空间相当于<strong>单片机的固件</strong>。</li>
            <li>flash和RAM：RAM是上电后执行程序的内存，存储代码运行时的变量和数据等。而flash是无论掉电上电，都可以存储代码本身的存储区域。</li>
            <li>内存映射：STM32单片机将外设映射为地址，再操作地址对应的外设，这个过程可以这么解释：例如片上外设的起始地址为0x40000000，在总线ABP1上地址，范围是0x40000000至0x4000FFFF，说明总线上的外设空间大小是FFFF（十六进制），即64K。ABP1上的DAC外设，其对应的地址空间是0x40007400至0x400077FF,大小为1K,可基于外设地址0x40000000移位得到。</li>
        </ol>
    </li>

</ul>



<img title="" src="https://images2018.cnblogs.com/blog/671539/201808/671539-20180815231321785-1226074049.png" alt="Image text" data-align="right">

> 引用自[STM32学习笔记：读写内部Flash（介绍+附代码） - 竹风清 - 博客园 (cnblogs.com)](https://www.cnblogs.com/pertor/p/9484663.html)

<h3>三.IIC和SPI通信</h3>

<ul>
    <li><h4>主从设备之间通信：</h4>
    单片机除了能控制片上外设，还能对其他一系列外设进行控制，这取决于被控制设备数据手册中提供的通信手段。有些简单设备是单片机通过串口发送特定的信息序列，但智能车更多的是通过IIC和SPI通信以达到控制从设备的目的，下面将介绍这两种通信方式。
    </li>

    <li><h4>IIC通信：</h4>
    
        <ul>
        <li>概述：IIC通信是由<strong>数据线SDA</strong>和<strong>时钟线SCL</strong>构成的串行总线，用于发送和接收数据。</li>
        <li>IIC状态：
            <ol>
                <li>主设备控制的开始与终止：<br>
                    <strong>开始信号：</strong>当时钟线SCL为高期间，主设备数据线SDA由高到低的跳变；启动信号是一种电平跳变时序信号，而不是一个电平。<br>
                    <strong>停止信号：</strong>当时钟线SCL为高期间，主设备数据线SDA由低到高的跳变；停止信号也是一种电平跳变时序信号，而不是一个电平信号。<br>
                    其中，当SCL处于高期间，SDA一直为高没有变化，则为空闲状态，不进行数据的发送和接收。
                    <img src="https://img2018.cnblogs.com/blog/1477786/201905/1477786-20190522185103134-1401961545.jpg" alt="图片alt" title="IIC的起始与停止">
                </li>
                
                <li>主设备发送数据：<br>
                    <strong>SCL:</strong>SCL在开始信号触发后，开始产生脉冲，在上升沿时准备好数据，进行传送数据时，SDA拉高（发送1）或拉低（发送0），在这期间，SDA线电平保持不变。<br>
                    <strong>SDA:</strong>在SCL高电平时保持稳定，发送数据，除此之外可以变换电平，取决于下一个SCL脉冲要发送的数据。<br>
                    例如发送8个高电平，也就是发送数据11111111，开始信号产生后，SCL会正常产生8个脉冲，此时SDA可以一直处于高电平不用变化，因为数据要求他传输8个1。<br>
                    若为发送数据10101010，那么开始信号产生后，SCL会正常产生8个脉冲，此时SDA在第一个脉冲内必须是高电平，而SCL变为低电平后，SDA需要在第二个脉冲前变为低电平，并在第二个脉冲期间保持低电平不变，才能使数据正常传输。
                    <img src="https://img-blog.csdnimg.cn/20200407162837546.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2FzNDgwMTMzOTM3,size_16,color_FFFFFF,t_70" alt="图片alt" title="主设备发送数据">
    
                </li>
                
                <li>从设备的应答信号：<br>
                    当开始信号发生后，主设备开始向从设备发送信息，而从设备需要发送一个ACK信号来表示自己收到了信息。<br>
                    主设备发送信号后，需要从设备返回一个应答信号来观察从设备是否接收到数据，经过8个SCL脉冲后，<strong>在第九个SCL脉冲期间</strong>，从设备SDA拉低且不变则为接收到数据，反之则没有接收到。原因是8次脉冲代表主设备发送了一次数据，而主从设备共用一个SCL时钟线。
                    <img src="https://img2018.cnblogs.com/blog/1477786/201905/1477786-20190522185701351-301948286.png" alt="图片alt" title="从设备的应答信号">
                </li>
    
                <li>主设备接收数据：<br>
                    这一部分是单片机和从设备通信的主要目的，因为单片机想要通过从设备（例如陀螺仪）获取一些信息从而进行控制，那么就需要陀螺仪不断地向他发送数据，单片机不断接收设备。<br>
                    <strong>步骤如下：</strong>
                        <ol>
                            <li>主机首先产生START信号</li>
                            <li>然后紧跟着发送一个从机地址，注意此时该地址的第8位为0，表明是向从机写命令，</li>
                            <li>这时候主机等待从机的应答信号(ACK)</li>
                            <li>当主机收到应答信号时，发送要访问的地址，继续等待从机的应答信号，</li>
                            <li>当主机收到应答信号后，主机要改变通信模式(主机将由发送变为接收，从机将由接收变为发送)所以主机重新发送一个开始start信号，然后紧跟着发送一个从机地址，注意此时该地址的第8位为1，表明将主机设 置成接收模式开始读取数据，</li>
                            <li>这时候主机等待从机的应答信号，当主机收到应答信号时，就可以接收1个字节的数据，当接收完成后，主机发送非应答信号，表示不在接收数据</li>
                            <li>主机进而产生停止信号，结束传送过程。</li>
                        </ol>
                    <ul><li>根据数据手册，写数据和读数据时，主设备访问的地址会不同，一般写时，第八位为0，一般读时，第八位为1。</li></ul>
                </li>
            </ol>
        </li>
        </ul>
    
    </li>
    
    <li><h4>SPI通信：</h4>
    
        <ul>
        <li><strong>概述：</strong>SPI接口一般使用四条信号线通信：<strong>SDI（数据输入），SDO（数据输出），SCK（时钟），CS（片选）</strong><br>
            SPI分为主、从两种模式，一个SPI通讯系统需要包含一个（且只能是一个）主设备，一个或多个从设备。提供时钟的为主设备（Master），接收时钟的设备为从设备（Slave），SPI接口的读写操作，都是由主设备发起。当存在多个从设备时，通过各自的片选信号进行管理。<br>
            MISO： 主设备输入/从设备输出引脚。该引脚在从模式下发送数据，在主模式下接收数据。<br>
            MOSI： 主设备输出/从设备输入引脚。该引脚在主模式下发送数据，在从模式下接收数据。<br>
            SCLK：串行时钟信号，由主设备产生。<br>
            CS/SS：从设备片选信号，由主设备控制。它的功能是用来作为“片选引脚”，也就是选择指定的从设备，让主设备可以单独地与特定从设备通讯，避免数据线上的冲突。<br>
            数据线的对应时SDI对应MOSI，SDO对应MISO，以此来看主设备需要的引脚是输出还是输入。
            <img src="https://img-blog.csdnimg.cn/20200429141530267.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2FzNDgwMTMzOTM3,size_16,color_FFFFFF,t_70" alt="图片alt" title="SPI通信">
        </li>
    
        <li>
            <strong>主设备进行SPI通信的从设备选择：</strong>
            由于有片选信号的存在，只要从主从设备之间的CS片选信号线拉低，则代表主从设备通信，对于单一从设备，CS默认为低电平就可以了。
        </li>
    
        <li>
            <strong>SPI通信的四种模式：</strong>
            <stong>SPI的四种模式，简单地讲就是设置SCLK时钟信号线的那种信号为有效信号</stong><br>
    
            SPI通信有4种不同的操作模式，不同的从设备可能在出厂是就是配置为某种模式，这是不能改变的；但我们的通信双方必须是工作在同一模式下，所以我们可以对我们的主设备的SPI模式进行配置，通过CPOL（时钟极性）和CPHA（时钟相位）来控制我们主设备的通信模式，具体如下：
    
            <ol>
                <li>Mode0：CPOL=0，CPHA=0：此时空闲态时，SCLK处于低电平，数据采样是在第1个边沿，也就是SCLK由低电平到高电平的跳变，所以数据采样是在上升沿(准备数据），（发送数据）数据发送是在下降沿。</li>
    
                <li>Mode1：CPOL=0，CPHA=1：此时空闲态时，SCLK处于低电平，数据发送是在第1个边沿，也就是SCLK由低电平到高电平的跳变，所以数据采样是在下降沿，数据发送是在上升沿。</li>
    
                <li>Mode2：CPOL=1，CPHA=0：此时空闲态时，SCLK处于高电平，数据采集是在第1个边沿，也就是SCLK由高电平到低电平的跳变，所以数据采集是在下降沿，数据发送是在上升沿。</li>
    
                <li>Mode3：CPOL=1，CPHA=1：此时空闲态时，SCLK处于高电平，数据发送是在第1个边沿，也就是SCLK由高电平到低电平的跳变，所以数据采集是在上升沿，数据发送是在下降沿。</li>
    
            </ol>
    
            <img src="https://img-blog.csdnimg.cn/20200429212715554.png" alt="图片alt" title="SPI通信的四种模式">
    
        </li>
        </ul>
    </li>
</ul>






<h3>四.实际应用之陀螺仪和屏幕</h3>
<ul>
    <li><h4>陀螺仪（以ICM42605为例子）：</h4></li>
    <ul>
        <li>陀螺仪为和单片机通信的外设，这里采用SPI通信的方式来一一解释陀螺仪的工作方式。<br></li>
    </ul>
    陀螺仪的工作目的是获取当前XYZ三轴的加速度和角速度值，以此来得到相关的pitch，raw，roll角获取姿态信息。
    本教程采用硬件SPI，配置陀螺仪工作的步骤如下：
        <ol>
            <li>SPI初始化，单片机的引脚有SPI的工作脚模式，可以作为主从设备通信引脚。</li>
            <li>陀螺仪初始化，根据数据手册配置好各项陀螺仪需要的寄存器数据。</li>
            <li>通过SPI的读写操作来控制陀螺仪。</li> 
        </ol> 
</ul>

> Tri-core的TC377硬件SPI配置如下

```c

//-------------------------------------------------------------------------------------------------------------------
//  @brief      SPI初始化
//  @param      spi_n           选择SPI模块(SPI_1-SPI_4)
//  @param      cs_pin          选择SPI片选引脚
//  @param      sck_pin         选择SPI时钟引脚
//  @param      mosi_pin        选择SPI MOSI引脚
//  @param      miso_pin        选择SPI MISO引脚
//  @param      mode            SPI模式 0：CPOL=0 CPHA=0    1：CPOL=0 CPHA=1   2：CPOL=1 CPHA=0   3：CPOL=1 CPHA=1 //具体细节可自行百度
//  @param      baud            设置SPI的波特率
//  @return     void
//  Sample usage:               spi_init(SPI_2, SPI2_SCLK_P15_3, SPI2_MOSI_P15_5, SPI2_MISO_P15_4, SPI2_CS0_P15_2, 0, 1*1000*1000);//硬件SPI初始化  模式0 波特率为1Mhz
//-------------------------------------------------------------------------------------------------------------------
void spi_init(SPIN_enum spi_n, SPI_PIN_enum sck_pin, SPI_PIN_enum mosi_pin, SPI_PIN_enum miso_pin, SPI_PIN_enum cs_pin, uint8 mode, uint32 baud)
{

	IfxQspi_SpiMaster_Config MasterConfig;
	IfxQspi_SpiMaster MasterHandle;
	IfxQspi_SpiMaster_Channel MasterChHandle;
	IfxQspi_SpiMaster_Pins MasterPins;
	IfxQspi_SpiMaster_Output SlsoPin;
	volatile Ifx_QSPI *moudle;

	moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);

	spi_mux(spi_n, sck_pin, mosi_pin, miso_pin, cs_pin, &MasterPins, &SlsoPin);

	IfxQspi_SpiMaster_initModuleConfig(&MasterConfig, moudle);
	MasterConfig.base.mode = SpiIf_Mode_master;
	MasterConfig.base.maximumBaudrate = MAX_BAUD;
	MasterConfig.base.isrProvider = IfxSrc_Tos_cpu0;


	MasterConfig.pins = &MasterPins;
	IfxQspi_SpiMaster_initModule(&MasterHandle, &MasterConfig);

	IfxQspi_SpiMaster_ChannelConfig MasterChConfig;
	IfxQspi_SpiMaster_initChannelConfig(&MasterChConfig, &MasterHandle);


	MasterChConfig.base.baudrate = (float)baud;
	switch(mode)
	{
		case 0:
		{
			MasterChConfig.base.mode.clockPolarity = SpiIf_ClockPolarity_idleLow;//CPOL
			MasterChConfig.base.mode.shiftClock = SpiIf_ShiftClock_shiftTransmitDataOnTrailingEdge;//CPHA
		}break;
		case 1:
		{
			MasterChConfig.base.mode.clockPolarity = SpiIf_ClockPolarity_idleLow;
			MasterChConfig.base.mode.shiftClock = SpiIf_ShiftClock_shiftTransmitDataOnLeadingEdge;
		}break;
		case 2:
		{
			MasterChConfig.base.mode.clockPolarity = SpiIf_ClockPolarity_idleHigh;
			MasterChConfig.base.mode.shiftClock = SpiIf_ShiftClock_shiftTransmitDataOnTrailingEdge;
		}break;
		case 3:
		{
			MasterChConfig.base.mode.clockPolarity = SpiIf_ClockPolarity_idleHigh;
			MasterChConfig.base.mode.shiftClock = SpiIf_ShiftClock_shiftTransmitDataOnLeadingEdge;
		}break;

	}

	MasterChConfig.base.mode.dataHeading = SpiIf_DataHeading_msbFirst;
	MasterChConfig.base.mode.dataWidth = 8;

	MasterChConfig.base.mode.csActiveLevel = Ifx_ActiveState_low;
	MasterChConfig.sls.output = SlsoPin;
	IfxQspi_SpiMaster_initChannel(&MasterChHandle, &MasterChConfig);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      SPI发送接收函数
//  @param      spi_n           选择SPI模块   (SPI_1-SPI_4)
//  @param      cs_pin          选择SPI片选引脚
//  @param      modata          发送的数据缓冲区地址
//  @param      midata          发送数据时接收到的数据的存储地址(不需要接收则传 NULL)
//  @param      len             发送的字节数
//  @param      continuous      本次通信是CS是否持续保持有效状态 1:持续保持  0:每传输完一个字节关闭CS(一般设置为1 即可)
//  @return     void
//  @since      v2.0
//  Sample usage:       		spi_mosi(SPI_2,SPI2_CS0_P15_2,buf,buf,1,1);    //发送buff的内容，并接收到buf里，长度为1字节 通信期间CS持续拉低
//-------------------------------------------------------------------------------------------------------------------
void spi_mosi(SPIN_enum spi_n, SPI_PIN_enum cs_pin, uint8 *modata, uint8 *midata, uint32 len, uint8 continuous)
{
	uint32 i;
	Ifx_QSPI_BACON bacon;
	volatile Ifx_QSPI *moudle;

	moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);

	bacon.U = moudle->BACON.U;

	bacon.B.DL = 7;
	bacon.B.IDLE = 1;
	bacon.B.IPRE = 1;
	bacon.B.LEAD = 1;
	bacon.B.LPRE = 1;
	bacon.B.MSB = 1;
	bacon.B.PARTYP = 0;
	bacon.B.BYTE = 0;
	bacon.B.TRAIL = 1;
	bacon.B.TPRE = 1;
	bacon.B.CS = cs_pin%102/6-3;
	if(continuous)	IfxQspi_writeBasicConfigurationBeginStream(moudle, bacon.U);//发送数据后CS继续保持为低
	else			IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);	//每发送一个字节CS信号拉高一次

	if(len>1)
	{
		i = 0;
		while(i < (len-1))
		{
			while(moudle->STATUS.B.TXFIFOLEVEL != 0);
			IfxQspi_write8(moudle, IfxQspi_ChannelId_0, modata, 1);
			while(moudle->STATUS.B.RXFIFOLEVEL == 0);
			if(NULL != midata)	
			{
				IfxQspi_read8(moudle,midata,1);
				midata++;
			}
			else				(void)moudle->RXEXIT.U;
			modata++;
			
			i++;
		}
	}

	//发送最后一个数据
	if(continuous)	IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);
	IfxQspi_writeTransmitFifo(moudle, *modata);
	while(moudle->STATUS.B.TXFIFOLEVEL != 0);

	while(moudle->STATUS.B.RXFIFOLEVEL == 0);
	if(NULL != midata)	IfxQspi_read8(moudle,midata,1);
	else				(void)moudle->RXEXIT.U;
}
```

> 基于上面的SPI库，ICM42605的驱动和使用实现

```c

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ICM42606 SPI写寄存器
//  @param      cmd     寄存器地址
//  @param      val     需要写入的数据
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void icm42605_spi_w_reg_byte(uint8 cmd, uint8 val)
{
    uint8 dat[2];

    dat[0] = cmd | ICM42605_SPI_W;
    dat[1] = val;

    spi_mosi(SPI_NUM,SPI_CS_PIN,dat,dat,2,1);
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      ICM42605 SPI读寄存器
//  @param      cmd     寄存器地址
//  @param      *val    接收数据的地址
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void icm42605_spi_r_reg_byte(uint8 cmd, uint8 *val)
{
    uint8 dat[2];

    dat[0] = cmd | ICM42605_SPI_R;
    dat[1] = *val;

    spi_mosi(SPI_NUM,SPI_CS_PIN,dat,dat,2,1);

    *val = dat[1];
}


void icm42605_self3_check(void)
{
    uint8 dat=0;
    while(ICM42605_ID != dat)   //读取ICM20602 ID
    {
        icm42605_spi_r_reg_byte(who_am_i,&dat);
        printf("%02X",dat);
        systick_delay_ms(STM0, 10);
        //卡在这里原因有以下几点
        //1 ICM42605坏了，如果是新的这样的概率极低
        //2 接线错误或者没有接好
        //3 可能你需要外接上拉电阻，上拉到3.3V
    }

}



/*************************************************************************
*  函数名称：uint8_t icm42605_init(void)
*  功能说明：ICM42605初始化
*  参数说明：无
*  函数返回：0 成功；1失败
*************************************************************************/
uint8 icm42605_init(void)
{
    uint8 reg_val =0x0;
    systick_delay_ms(STM0,10);  //上电延时


    (void)spi_init(SPI_NUM, SPI_SCK_PIN, SPI_MOSI_PIN,  SPI_MISO_PIN , SPI_CS_PIN , 3, 10*1000*1000);

    icm42605_self3_check();
    reg_val = icm_spi_r_reg_bytes(who_am_i);  //who_am_i
    printf("ID:0x%02X",reg_val);

    icm42605_spi_w_reg_byte(reg_bank_sel,0x00);  //设置bank 0区域寄存器
    icm42605_spi_w_reg_byte(device_config_reg,bit_soft_reset_chip_config);  //软复位传感器
    systick_delay_ms(STM0,50);
  
    if(reg_val == 0x42) {
        icm42605_spi_w_reg_byte(reg_bank_sel,0x01);  //设置bank 1区域寄存器
        icm42605_spi_w_reg_byte(intf_config4,0x02);  //设置为4线SPI通信

        icm42605_spi_w_reg_byte(reg_bank_sel,0x00);  //设置bank 0区域寄存器
        icm42605_spi_w_reg_byte(fifo_config_reg,0x40);  //Stream-to-FIFO Mode

        reg_val = icm_spi_r_reg_bytes(int_source0_reg);  //保存寄存器的值
        icm42605_spi_w_reg_byte(int_source0_reg,0x00);  //失能中断
        icm42605_spi_w_reg_byte(fifo_config2_reg,0x00);// Lower bits of FIFO watermark
        icm42605_spi_w_reg_byte(fifo_config3_reg,0x02);// Upper bits of FIFO watermark
        icm42605_spi_w_reg_byte(int_source0_reg, reg_val);  //重新写回寄存器的值
        icm42605_spi_w_reg_byte(fifo_config1_reg,0x63);// 使能加速度和角速度的FIFO

        icm42605_spi_w_reg_byte(reg_bank_sel,0x00);
        icm42605_spi_w_reg_byte(int_config_reg,0x36);  //设置INT1和INT2为推挽锁存模式

        icm42605_spi_w_reg_byte(reg_bank_sel, 0x00);
        reg_val = (icm_spi_r_reg_bytes(int_source0_reg)|bit_int_fifo_ths_int1_en);  //EN FIFO
        icm42605_spi_w_reg_byte(int_source0_reg, reg_val);

        icm42605_spi_w_reg_byte(reg_bank_sel, 0x00);
        reg_val = ((icm_spi_r_reg_bytes(accel_config0_reg)&0x1F)|(bit_accel_ui_fs_sel_8g));//8g
        icm42605_spi_w_reg_byte(accel_config0_reg, reg_val);

        icm42605_spi_w_reg_byte(reg_bank_sel, 0x00);
        reg_val = ((icm_spi_r_reg_bytes(accel_config0_reg)&0xF0)|bit_accel_odr_50hz);  //50Hz
        icm42605_spi_w_reg_byte(accel_config0_reg, reg_val);

        icm42605_spi_w_reg_byte(reg_bank_sel, 0x00);
        reg_val = ((icm_spi_r_reg_bytes(gyro_config0_reg)&0x1F)|(bit_gyro_ui_fs_sel_2000dps));  //2000dps
        icm42605_spi_w_reg_byte(gyro_config0_reg,reg_val);

        icm42605_spi_w_reg_byte(reg_bank_sel, 0x00);
        reg_val = ((icm_spi_r_reg_bytes(gyro_config0_reg)&0xF0)|bit_gyro_odr_50hz);  //50Hz
        icm42605_spi_w_reg_byte(gyro_config0_reg, reg_val);

        icm42605_spi_w_reg_byte(reg_bank_sel, 0x00);
        reg_val = icm_spi_r_reg_bytes(pwr_mgmt0_reg)|(bit_accel_mode_ln); // Accel on in LNM
        icm42605_spi_w_reg_byte(pwr_mgmt0_reg, reg_val);
        systick_delay_ms(STM0,1);

        icm42605_spi_w_reg_byte(reg_bank_sel, 0x00);
        reg_val = icm_spi_r_reg_bytes(pwr_mgmt0_reg)|(bit_gyro_mode_ln); // Gyro on in LNM
        icm42605_spi_w_reg_byte(pwr_mgmt0_reg, reg_val);
        systick_delay_ms(STM0,1);

        printf("[Info] ICM42605.c: ICM42605 Initial is OK\n");
        return 0;
    }
    else {
        return 1;
    }
}




```

> 对ICM42605的关键数据读取

```c

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取ICM42605加速度计数据
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:               执行该函数后，直接查看对应的变量即可
//-------------------------------------------------------------------------------------------------------------------
void get_icm42605_accdata_spi(void)
{
    struct
    {
        uint8 reg;
        uint8 dat[6];
    }buf;

    buf.reg = ICM_ACCEL_XOUTH_REG | ICM42605_SPI_R;

    icm42605_spi_r_reg_bytes(&buf.reg, 7);
    icm_acc_x = (int16)(((uint16)buf.dat[0]<<8 | buf.dat[1]));
    icm_acc_y = (int16)(((uint16)buf.dat[2]<<8 | buf.dat[3]));
    icm_acc_z = (int16)(((uint16)buf.dat[4]<<8 | buf.dat[5]));
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取ICM42605陀螺仪数据
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:               执行该函数后，直接查看对应的变量即可
//-------------------------------------------------------------------------------------------------------------------
void get_icm42605_gyro_spi(void)
{
    struct
    {
        uint8 reg;
        uint8 dat[6];
    }buf;

    buf.reg = ICM_GYRO_XOUTH_REG | ICM42605_SPI_R;

    icm42605_spi_r_reg_bytes(&buf.reg, 7);
    icm_gyro_x = (int16)(((uint16)buf.dat[0]<<8 | buf.dat[1]));
    icm_gyro_y = (int16)(((uint16)buf.dat[2]<<8 | buf.dat[3]));
    icm_gyro_z = (int16)(((uint16)buf.dat[4]<<8 | buf.dat[5]));
}



```

 <h4>屏幕：</h4>
<li>屏幕的驱动已经成熟，主设备和屏幕的通信方式采用IPS，请同学下载相关例程进行学习。</li>
</ul>
</li>    



### 五.合适的调参工具

> vofa+是一个可视化的可以调参的串口数据上位机，下载地址为[VOFA+ | VOFA+](https://www.vofa.plus/)
