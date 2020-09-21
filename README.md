# ClassAttendanceManageSystem-RFID
基于RFID的课堂考勤管理系统
image/图片1.png
本设计以STM32F103ZET6单片机作为控制核心，对MFRC522模块进行编程实现读写卡操作，将通过串口与上位机通信，从而实现课堂考勤的读卡签到以及管理员录用信息的写卡功能等。 
本次设计主要完成以下内容：
(1)能够使用STM32单片机对按键、蜂鸣器进行驱动控制；主要用于用户端的提醒或者触发读写卡操作； 
(2)该单片机外接OLED模块，正常驱动该模块，主要用于实现显示时间或者读卡时读到内容；
(3)该单片机外接继电器模块，使用是用于模拟学生刷卡签到时完成签到的提醒；
(4)该单片机外接MFRC522读写卡模块，是本设计的重要模块。主要使用该模块完成课堂考勤签到以及读写卡操作。那么下位机编程时对模块分为两个客户端，即学生端和管理员端。学生端是教师上课时通过串口与下位机通信之后并送签到码（本设计使用了教室号作为签到码），选择签到结束时间并与下位机通过串口通信即可激活下位机进行考勤签到功能；而管理员端是管理员对S50卡通过提示并按下按键即可对卡片进行读卡以及写卡，那么管理员端读卡是读取卡号，写卡是写入学生信息（本设计只写入学号），那么在管理员端进行读写卡操作之后就可以写入数据库，即添加学生信息。
(5)上位机C#开发是使用Visual Studio 2015作为开发工具，该上位机主要对串口通信发来的数据进行处理并写入数据库，那么上位机的角色或者用户就可以对考勤信息查看等等。
(6)那么上位机C#端分为了三个角色，即学生、教师和管理员。对考勤系统管理方面也进行了各业务的实现。
