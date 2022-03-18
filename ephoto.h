#ifndef __FONT__
#define __FONT__

#include <stdio.h>
#include<stdlib.h>
#include<libio.h>
#include<errno.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>//open 头文件
#include <unistd.h>
#include <sys/mman.h>
#include<string.h>
#include <dirent.h>
#include <linux/input.h>//输入子系统

typedef struct user_link
{
	char ac[7];
	char pas[7];
	struct user_link *next;
}user,*puser;
typedef struct photo_link
{
	char ph_dir[20];
	
	struct photo_link *next;
	struct photo_link *pre;
}ph,*pho;
int tx;
int ty;
pho init_pho();
int pho_write(pho head);
void user_write(puser head);
puser init_user();
int log_in(puser head, char ac[7],char pw[7]);
void free_list(puser p);
int get_xy();
int showbmp(char *filename,int w,int h,int x0,int y0,int s);
int get_num(char input_num[7],int x,int y);
int show_hand(pho head);
int my_con(pho head);
void free_ph(pho head);
void cun(puser head);
int zhuce_user(puser head,char acount[7],char pw1[7]);
#endif
