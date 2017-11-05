这是我的本科毕业设计，那时候主要做嵌入式设计居多。因此，毕业设计也选择了与此相关的题目。

# 整体介绍
项目使用激光发射器、专用光学透镜以及图像传感器构成一个图像位移采集系统。简单地说，就是利用了光电鼠标的工作原理设计并实现了一个高精度的光电定位仪。  

![](http://chuquan-public-r-001.oss-cn-shanghai.aliyuncs.com/github-images/photoelectriclocator001.png?x-oss-process=image/resize,w_500)

如上图所示，系统主要包含以下几个部分：  
- **高速激光传感器**：采用安华高公司的ADNS-9500芯片，该芯片提供3V和5V双重电源电压选择；可编程图像分辨率，实现采样率90~5040CPI的自由选择。具有可编程控制图像采样率，最高可达117500fps；能够通过SPI串口与外部微控制器进行通信，从而实现图像信息的连续传输。
- **高精度电子罗盘传感器**：采用Honeywell公司的三轴电子罗盘HMC5983芯片。
- **主控制器**：采用德州仪器(TI)公司的MSP430系列的F149单片机，其提供1.8V~3.6V的供电电压范围；具有五种低功耗模式；采用16位精简指令架构，指令周期125ns。
- **LCD**：采用12864液晶屏。
- **专用光学透镜组**：采用如下图所示的光学透镜组。  

![](http://chuquan-public-r-001.oss-cn-shanghai.aliyuncs.com/github-images/photoelectriclocator002.png?x-oss-process=image/resize,w_400)  

## 其他补充
系统使用IAR Embedded Workbench IDE的C语言开发环境。二进制文件下载器是SF_BSL430。  
系统电路的PCB使用Altium Designer设计实现。
