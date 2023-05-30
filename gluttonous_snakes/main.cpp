#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <vector>

using namespace std;
extern int show_anybmp(int w,int h,int x,int y,const char *bmpname);
struct pos
{
	int x; //一节蛇身左上角x坐标
	int y; //一节蛇身左上角y坐标
};

int ts_x; //保存触摸屏x坐标
int ts_y; //保存触摸屏y坐标
int tsfd; //保存触摸屏的文件描述符
vector<struct pos> snakevector; //保存整个蛇每一节蛇身坐标位置

//游戏最开始有三节蛇身
struct pos pos1={0,0};  //最开始蛇尾位置
struct pos pos2={10,0}; //中间的蛇身
struct pos pos3={20,0}; //最开始蛇头位置

void *readts(void *arg)
{
	//定义结构体存放坐标值
	struct input_event myevent;
	while(1)
	{
		//读取触摸屏坐标
		read(tsfd,&myevent,sizeof(myevent));
		//判断x,y打印出来
		if(myevent.type==EV_ABS) //触摸屏
		{
			if(myevent.code==ABS_X) //X坐标
			{
				ts_x=(800*myevent.value)/1024;
			}
			if(myevent.code==ABS_Y) //Y坐标
			{
				ts_y=(480*myevent.value)/600;	
			}
		}
	}
}

void *snakemove(void *arg)
{
	while(1)
	{
		//保存新的蛇头到容器中
		pos3.x+=10;
		//判断x是否超标
		if(pos3.x>630)
		{
			cout<<"对不起，蛇身移动超范围，游戏结束"<<endl;
			pthread_exit(NULL);
		}
		snakevector.push_back(pos3);
		//把蛇尾填充成白色图片，从容器中删除蛇尾
		struct pos &temppos=snakevector.front();
		show_anybmp(32,32,temppos.x,temppos.y,"/IOT/project1/pic/kongbai.bmp");
		snakevector.erase(snakevector.begin());
		//遍历容器snakelist刷图，就可以让蛇身动起来
		for(auto curpos:snakevector)
		{
			show_anybmp(32,32,curpos.x,curpos.y,"/IOT/project1/pic/tou.bmp");
			usleep(50000);
		}
	}
}
int main()
{
	pthread_t tsid;
	pthread_t snakeid;
	
	//保存三节蛇身(0,0)(10,0)(20,0)
	snakevector.push_back(pos1);
	snakevector.push_back(pos2);
	snakevector.push_back(pos3);
	
	//打开触摸屏的驱动
	tsfd=open("/dev/input/event0",O_RDWR);
	if(tsfd==-1)
	{
		printf("打开触摸屏失败!\n");
		return -1;
	}
	
	//显示游戏背景
	show_anybmp(800,480,0,0,"/IOT/project1/pic/bizhi.bmp");
	
	//创建线程读取触摸屏坐标
	pthread_create(&tsid,NULL,readts,NULL);
	
	//进入死循环判断触摸屏点击坐标位置
	while(1)
	{
		if(ts_x>655&&ts_x<795&&ts_y>8&&ts_y<41) //开始游戏
		{
			cout<<"游戏开始"<<endl;
			//在(0,0)位置绘制3节蛇身，并且默认自动往右开始移动(后面由用户来控制方向)
			pthread_create(&snakeid,NULL,snakemove,NULL);
			ts_x=0;
			ts_y=0;
		}	
	}
	return 0;
}