
#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <random>
#include <unordered_set>
#include <algorithm>
#include "font.h"
extern"C"
using namespace std;
extern int show_anybmp(int w,int h,int x,int y,const char *bmpname);

int flag1,flag2;
int speed;
int score;
int start_flag;
typedef struct zuobiao{
    int x;
    int y;
}zb;
unordered_set<int> hashnum;
zb a={64,0};
zb a1={32,0};
zb a2={0,0};
vector<zb> old_zb;
vector<zb> new_zb;
vector<zb> gift_zb;
vector<zb> wall_zb;
int ts_x; //保存触摸屏x坐标
int ts_y; //保存触摸屏y坐标
int tsfd; //保存触摸屏的文件描述符

    bool operator==(zb a,zb b)
    {
        if(a.x==b.x&&a.y==b.y)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

struct LcdDevice *init_lcd(const char *device)
    {
        //申请空间
        struct LcdDevice* lcd = (struct LcdDevice*)malloc(sizeof(struct LcdDevice));
        if(lcd == NULL)
        {
            return NULL;
        } 

        //1打开设备
        lcd->fd = open(device, O_RDWR);
        if(lcd->fd < 0)
        {
            perror("open lcd fail");
            free(lcd);
            return NULL;
        }
        
        //映射
        lcd->mp =(unsigned int *) mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcd->fd,0);

        return lcd;
    }

int write_world(char * buf,int size,int length,int width,unsigned int bg_str,int x,int y)
{   
        //初始化Lcd
   
	//初始化Lcd
     struct LcdDevice* lcd = init_lcd("/dev/fb0");
	//打开字体	
	font *f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");
	  
	//字体大小的设置
	fontSetSize(f,size);

	//创建一个画板（点阵图）
	bitmap *bm = createBitmapWithInit(length,width,4,bg_str<<8);
	
	//将字体写到点阵图上
	if(buf != NULL)
	fontPrint(f,bm,0,0,buf,getColor(0,0,0,0),0);
		
	//把字体框输出到LCD屏幕上
	show_font_to_lcd(lcd->mp,x,y,bm);
	
	//关闭字体，关闭画板
	fontUnload(f);
	destroyBitmap(bm);
     close(lcd->fd);
     munmap(lcd->mp,800*480*4);
	return 0;
}





void *readts(void *arg)
{
    printf("获取坐标\n");
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
                printf("tx=%d",ts_x);
			}
			if(myevent.code==ABS_Y) //Y坐标
			{
				ts_y=(480*myevent.value)/600;	
                printf("tY=%d",ts_y);
			}
		}
	}
}
void getwall()
{
    wall_zb.clear();
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> xdist(0,19);
    uniform_int_distribution<int> ydist(5,14);
    const int num = 15;
    
    zb mywall[num];
    for(int i = 0 ;i < num;i++)
    {
        zb newwall;
        do{
            newwall.x = xdist(gen);
            newwall.y = ydist(gen);
        }while(hashnum.count(newwall.x*100+newwall.y)>0);
        hashnum.insert(newwall.x*100+newwall.y);
        mywall[i] = newwall;
    }
    for(int i = 0 ;i < num;i++)
    {
        mywall[i].x=mywall[i].x*32;
        mywall[i].y=mywall[i].y*32;
        wall_zb.push_back(mywall[i]);
        show_anybmp(32,32,mywall[i].x,mywall[i].y,"/IOT/project1/pic/wall.bmp");
    }
}
void getgift()
{
    hashnum.clear();
    gift_zb.clear();
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> xdist(0,19);
    uniform_int_distribution<int> ydist(0,14);
    const int num = 15;
    
    zb mygift[num];
    for(int i = 0 ;i < num;i++)
    {
        zb newgift;
        do{
            newgift.x = xdist(gen);
            newgift.y = ydist(gen);
        }while(hashnum.count(newgift.x*100+newgift.y)>0);
        hashnum.insert(newgift.x*100+newgift.y);
        mygift[i] = newgift;
    }
    for(int i = 0 ;i < num;i++)
    {
        mygift[i].x=mygift[i].x*32;
        mygift[i].y=mygift[i].y*32;
        gift_zb.push_back(mygift[i]);
        show_anybmp(32,32,mygift[i].x,mygift[i].y,"/IOT/project1/pic/liwu.bmp");
    }

}
//控制移动
void getflag()
{   
    
    //上
    if(ts_x>690&&ts_y>321&&ts_x<=748&&ts_y<=372)
        { 
            if(flag2==0)
            {
                flag1=0;
                flag2=-1;
            } 
        }
    //左
    if(ts_x>642&&ts_y>373&&ts_x<=689&&ts_y<=431)
        {
            if(flag1==0)
            {
                flag1=-1;
                flag2=0;
            }
           
            
        }
    //右  
    if(ts_x>749&&ts_y>372&&ts_x<=798&&ts_y<=431)
        {
            if(flag1==0)
            {
                flag1=1;
                flag2=0;
            }
            
            
        }
    //下
    if(ts_x>692&&ts_y>432&&ts_x<=748&&ts_y<=479)
        {
            if(flag2==0)
            {
                flag2=1;
                flag1=0;
            }            
        }
}

int checkgift(const zb temp)
{
    
    for(auto it:gift_zb)
    {
        if(it.x == temp.x&&it.y == temp.y)
        {
            gift_zb.erase(remove(gift_zb.begin(),gift_zb.end(),temp),gift_zb.end());
            return 1;      
        }
    }
    for(auto it:new_zb)
    {
        if(it.x == temp.x&&it.y == temp.y)
        {
            //gift_zb.erase(remove(gift_zb.begin(),gift_zb.end(),temp),gift_zb.end());
            return 2;      
        }
    }
    for(auto it:wall_zb)
    {
        if(it.x == temp.x&&it.y == temp.y)
        {
            //gift_zb.erase(remove(gift_zb.begin(),gift_zb.end(),temp),gift_zb.end());
            return 2;      
        }
    }
    return 0;
}
zb changebody(zb &a)
{
    a.x = a.x+flag1*32;
         a.y = a.y+flag2*32;
         if(a.y>448)
            {
                a.y=0;
                a.x=a.x+32;
                if(a.x>608)
                {
                    a.x=0;
                }
            }
        if(a.x>608)
            {
                a.x=0;
                a.y=a.y+32;
                if(a.y>448)
                {
                    a.y=0;
                }
            }
        if(a.x<0)
            {
                a.x=608;
                a.y-=32;
                if(a.y<0)
                {
                    a.y=448;
                }
            }

            if(a.y<0)
            {
                a.y=448;
                a.x-=32;
                if(a.x<0)
                {
                    a.x=608;
                }
            }
            return a;
}
void checkspeed()
{
    if(ts_x>645&&ts_y>59&&ts_x<=715&&ts_y<=105)
    {
        if(speed>100000)
        {
            speed -=100000;
             ts_x = 0;
            ts_y = 0;
        }
        printf("speed:%d",speed);
    }
     if(ts_x>715&&ts_y>57&&ts_x<=798&&ts_y<=105)
    {
        if(speed<1000000)
        {
            speed +=100000;
            ts_x = 0;
            ts_y = 0;
        }
        printf("speed:%d",speed);
        
    }

}
void *showsnake(void *arg)
{
    
   
    
    old_zb.push_back(a2);
    new_zb.push_back(a);
    new_zb.push_back(a1);
    new_zb.push_back(a2);
   // body_zb.push_back(a);
    //new_zb.push_back(a2);
    while(1)
    {
    usleep(10000);   
    flag1=1;
    flag2=0;
    write_world("点击按钮开始游戏",60,480,50,0xffffff,100,200);
    
   // show_anybmp(800,480,0,0,"/IOT/project1/pic/bizhi.bmp");
    
    if(ts_x>642&&ts_y>4&&ts_x<=721&&ts_y<=54)
    {
        write_world("",60,480,50,0xffffff,100,200);
        start_flag = 1;
        score = 0;
        write_world("分数:",50,100,50,0xffffff,645,150);
        getgift();
        getwall();
    }
    speed=500000;
    if(start_flag==1)
    {
       
   
    while(1)
    {
        checkspeed();
        a=new_zb.front();
        printf("ax:%d\n",a.x);
        a1=old_zb.back();
       // printf("a1x:%d\n",a1.x);
        
        getflag();
        //更改a的值
        a=changebody(a);
        //判断奖励和障碍物;
        int fa1;
        fa1 = checkgift(a);
        if(fa1==2)
        {
            start_flag = 0;
            a={64,0};
            a1={32,0};
            a2={0,0};
            old_zb.erase(old_zb.begin(),old_zb.end());
            new_zb.erase(new_zb.begin(),new_zb.end());
            old_zb.push_back(a2);
            new_zb.push_back(a);
            new_zb.push_back(a1);
            new_zb.push_back(a2);
            start_flag=0;
            ts_x=0;
            ts_y=0;
            show_anybmp(800,480,0,0,"/IOT/project1/pic/bizhi.bmp");
            write_world("游戏结束",60,300,50,0xffffff,200,200);
            sleep(2);
            break;
        }
        if(fa1==1)
        {
           new_zb.push_back(new_zb.back()); 
           score +=10;  
           
           string str =to_string(score);
            char* sscore =(char*)str.c_str();
           write_world(sscore,50,60,50,0xffffff,730,150);
        }
        if(ts_x>642&&ts_y>105&&ts_x<=798&&ts_y<=152||score==150)
        {
            start_flag = 0;
            a={64,0};
            a1={32,0};
            a2={0,0};
            old_zb.erase(old_zb.begin(),old_zb.end());
            new_zb.erase(new_zb.begin(),new_zb.end());
            old_zb.push_back(a2);
            new_zb.push_back(a);
            new_zb.push_back(a1);
            new_zb.push_back(a2);
            start_flag=0;
            ts_x=0;
            ts_y=0;
            show_anybmp(800,480,0,0,"/IOT/project1/pic/bizhi.bmp");
            write_world("游戏结束",60,300,50,0xffffff,200,200);
            sleep(2);
            break;
        }
        if(ts_x>723&&ts_y>6&&ts_x<=798&&ts_y<=57)
        {
            while(1)
            {
                usleep(10000); 
                if(ts_x>642&&ts_y>4&&ts_x<=721&&ts_y<=54)
                {
                    break;
                }
            }
        
        }
        //显示头部
        new_zb.insert(new_zb.begin(),a);
        zb w;
        show_anybmp(32,32,a.x,a.y,"/IOT/project1/pic/tou.bmp");
        //显示身体
        for( auto w:new_zb)
        {
          
            if( w.x != new_zb.front().x|| w.y != new_zb.front().y)
            {
               // printf("w.x:%d\n",w.x);
                show_anybmp(32,32,w.x,w.y,"/IOT/project1/pic/shenti.bmp");
            }
            
        }
        zb temp =new_zb.back();
        old_zb.insert(old_zb.begin(),temp);
        //去掉尾巴
        new_zb.pop_back();
        show_anybmp(32,32,a1.x,a1.y,"/IOT/project1/pic/kongbai.bmp");
        old_zb.pop_back();
        usleep(speed);
    }
    }
}
}
int main()
{
    pthread_t tsid;
    pthread_t ssnid;
    tsfd=open("/dev/input/event0",O_RDWR);
	if(tsfd==-1)
	{
		printf("打开触摸屏失败!\n");
		return -1;
	} 
    pthread_create(&tsid,NULL,readts,NULL);
    pthread_create(&ssnid,NULL,showsnake,NULL);
    while(1)
    {

    }
    return 0;
  
}


