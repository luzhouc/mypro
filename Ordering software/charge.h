#ifndef CHARGE_H
#define CHARGE_H


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <linux/input.h>
#include <stdbool.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 

#include "font.h"


int lcd_fd;//打开液晶屏

int *lcd_fp;//将设备文件映射到虚拟内存
int ts_fd;//打开触摸屏
int ts_x;//记录触摸屏的x轴
int ts_y;//记录触摸屏的y轴
//发送消息变量
char * rcvmsg;
char * sndmsg;
socklen_t addrlen;
int tcp_socket ;
int yue;
char buf1[128];
    char buf[128];
    char buf2[128];



char register_name[256];
char register_pwd[256];



char login_acc[256];//存储登录时键盘输入的账号缓冲区
char login_pwd[256];//存储登录时键盘输入的密码缓冲区

int show(char *picture,int starx,int stary,int hight,int wide);//映射图片

int input();//检测触摸屏事件，通过坐标x，y和长宽判断并返回整型数据
//写入文字	参数为 需要输入的文字 字体大小 画布长 画布宽 背景颜色 画布起始x轴 画布起始y轴
int write_world(char * buf,int size,int length,int width,unsigned int bg_str,int x,int y);

//登录界面函数
int login();

//判断登录界面的触屏数据
int login_interface();

int TCP_INIT(void);//tcp初始化

void sendMessage(char *buf);//发送消息

void  RecvMsg();//接收消息

int keyboard_acc();//实现输入账号功能

int keyboard_pwd();//实现输入密码功能



//菜品结构体
typedef struct food_menu
{
    char name[256];
    int num;
    int price;
     struct food_menu *next;
}food;
food * caidan(food *head);

food * init_food_menu();


char* getmenu(food *head);
//存储用户信息的结构体
typedef struct user_data
{
	char us_acc[256];//账号
	char us_pwd[256];//密码
	
	int cash;//余额
	
}us_data;


typedef struct _LinkNode
{
    food food_data;
    //us_data data;//数据域
    struct _LinkNode* next;//指针域，用于保存下一结点的地址
    //struct _LinkNode* prev;//指针域，用于保存上一结点的地址

}LinkNode;


//定义一个链表 ---包含头结点
typedef struct _CDlinkList
{

    LinkNode* head;//头结点
    int size;//链表的长度，容量
}CDlinkList;


//创建链表结点，并初始化
//LinkNode* new_node(CDlinkList* list,us_data inputData);

//创建新的链表头结点，并初始化
//CDlinkList* head();

//从文本读取用户数据并放入新的链表，最后将管理节点传递出去
CDlinkList* checkout();

//实现管理员界面触屏功能
int keyboard_suuser();

//实现账号注册功能
int register_user();

//判断注册界面的登录数据
int register_face();

//实现账号删除功能
int delete_user();
//实现整型转为字符串功能
char * int_to_char(int a);

//实现账号显示功能
int show_user();

//实现充电管理功能，并将数据写入文本
int charging_management();

//实现用户界面触屏功能
int keyboard_user();

//实现显示个人信息功能
int my_information();

LinkNode* p;



#endif