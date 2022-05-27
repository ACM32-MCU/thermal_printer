# 基于上海航芯 ACM32F403 系列的 MCU 的热敏打印机方案

## 1. 简介

随着电子信息化、自动化程度提高，条码识别技术的发展，热敏打印机的应用范围也在不断扩大，已从传统的办公和家庭传真文档，快速向商业零售、工业制造业、交通运输业、物流、金融、彩票、医疗 、教育 等新兴专业应用领域拓展。

## 2. 使用芯片

ACM32F403 芯片是上海爱信诺航芯电子科技有限公司(后续简称上海航芯)一系列支持多种低功耗模式的通用 MCU。包括如下硬件特性：

| 硬件       | 描述           |
| ---------- | -------------- |
| 芯片型号   | ACM32F403 系列 |
| CPU        | ARM Cortex-M33 |
| 主频       | 180MHz         |
| 片内 SRAM  | 192K           |
| 片内 Flash | 512K           |

具体型号及资源请参考上海航芯官方网站[ACM32F403](http://www.aisinochip.com/index.php/product/child1/id/219.html)。

## 3. 方案特点

⚫ 支持蓝牙、 USB、 UART 等多种通讯接口的打印方式<br>
⚫ 支持无任务时自动进入断电模式，续航时间更长<br>
⚫ 支持打印高温、缺纸和低电量报警<br>
⚫ SPI FLASH 存放字库，支持在线更新字库，可调整字体、大小、粗细等<br>
⚫ 支持 MCU 和 BLE 芯片固件在线升级<br>

## 4. 联系我们

![官方微信号](http://www.aisinochip.com/Apps/Tpl/default/Public/images/wx.png)
![官方QQ群](http://www.aisinochip.com/Apps/Tpl/default/Public/images/wb.png)
