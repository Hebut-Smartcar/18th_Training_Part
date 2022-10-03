---
layout: post
title: "keil开发环境搭建教程"
excerpt: 
date: 2022-10-3
categories: [嵌入式]
tags: [嵌入式,软件,智能车]
author: BriMonzZY
index_img: 
banner_img: 
---



# keil开发环境搭建教程



keil分为MDK-ARM、C51、C251等不同的版本，其中MDK-ARM用于开发ARM内核单片机如STM32、MM32等，C51用于开发51内核单片机比如STC89C52等，C251用于开发STC16F等。不同版本对应的编译链也不同，支持的组件不同。

下面会给出keil的安装及破解方法。



如果需要同时安装不同版本的，只需要**安装在同一目录**下即可。



## keil C51

下载链接：https://pan.baidu.com/s/1LyFuZEtnkxJh_I3b-8EVfw  提取码：6666 

阿里云盘：https://www.aliyundrive.com/s/DbuZbRVY9Qx

keil注册机：https://pan.baidu.com/s/1vBLR2LMT1FQ_N72Ys8jkHQ  提取码：6666 

安装完C51后，**以管理员模式同时启动keilC51和注册机**(建议关闭声音启动kegen.exe)，注意注册机可能会被电脑的杀毒软件误删。

打开File→LicenseLicense Management，复制右上角的CID。

在kegen的CID框中输入刚才复制的CID，Target栏选择C51，点击generate。

![](https://raw.githubusercontent.com/BriMonzZY/blog-image/main/img/202210031510998.png)

将生成的license复制到keil的New License ID code中，点击ADD LIC。

破解完成后是这样的：

![image-20221003150820997](https://raw.githubusercontent.com/BriMonzZY/blog-image/main/img/202210031508069.png)

因为我同时安装了C51、C251、MDK所以有3个LIC



## keil MDK

下载链接：https://pan.baidu.com/s/1M3Hk7mw8Sn7cxS536hilNg  提取码：6666 

阿里云盘：https://www.aliyundrive.com/s/7CQU2dBZMjE

破解的方法同上，唯一不同的是kegen的target需要改成ARM



## pack安装

### C51安装STC

打开STC-ISP：

STC-ISP链接：https://pan.baidu.com/s/1hXaSQljjbggzh_QYcH2Hrg   提取码：6666 


![image-20221003151724525](C:/Users/z'ZY/AppData/Roaming/Typora/typora-user-images/image-20221003151724525.png)

根据提示添加即可。



### MDK添加STM32系列：

F1和F4的pack：链接：https://pan.baidu.com/s/1_79js9om9Fy0Jk8dXKDMGw  提取码：6666 

直接选择用keil打开即可。

其他的pack同理，可以到官网上下载。

