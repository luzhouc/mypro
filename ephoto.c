#include"ephoto.h"
//初始化照片链表
pho init_pho()
{
	pho pos = (pho)malloc(sizeof(ph));
	if(pos == NULL)
	{
		perror("head malloc fail\n");
		return 0;
	}
	
	pos->next=pos;
	pos->pre =pos;
	return pos;
}
//照片信息读取
int pho_write(pho head)
{
	DIR *dp = opendir("photo");
	
	
	 struct dirent *p = NULL;
	  while(1)
	 {
		 p = readdir(dp);
		 
		 if(p==NULL)
		 {
			 
			 break;
		 }
		 pho q =(pho)malloc(sizeof(ph));
	
			if(dp == NULL)
			{
				printf("open dir fail\n");
				return -1;
			}
		if(p->d_name[0]!='.')
		 {
			 
			
			
			q->next=head->next;
			q->pre=head;
			head->next->pre=q;
			head->next = q;
			sprintf(q->ph_dir,"photo/%s",p->d_name); 
			 
		 }
	 }
		 return 0;
}
//将文本中用户信息写入链表
void user_write(puser head)
{
		FILE *fp =fopen("1.txt","r+");
		char acc[7];
		char pw1[7];
		while(fscanf(fp,"%s %s",acc,pw1)!=EOF)
		{
			puser q =(puser)malloc(sizeof(user));
			puser pos =(puser)malloc(sizeof(user));
			if(pos == NULL)
			{
				perror("head malloc fail\n");
				return ;
			}
			if(q == NULL)
			{
				perror("head malloc fail\n");
				return ;
			}
			strcpy(q->ac,acc);
			strcpy(q->pas,pw1);
			pos =head;
			
			while(pos->next!=head)
			{
				pos = pos->next;
			}
			pos->next = q;
			q->next = head;
		}
			fclose(fp);
}
//初始化用户链表
puser init_user()
{
	puser pos = (puser)malloc(sizeof(user));
	if(pos == NULL)
	{
		perror("head malloc fail\n");
		return 0;
	}
	pos->next=pos;
	return pos;
}
//登陆函数
int log_in(puser head, char ac[7],char pw[7])
{
	puser pos = (puser)malloc(sizeof(user));
	
	for(pos=head->next;pos!=head;pos=pos->next)
	{
		if(strcmp(pos->ac,ac)==0)
		{
			if(strcmp(pos->pas,pw)==0)
			{
				printf("登录成功\n");
				return 1;
			}
			else
			{
				printf("密码错误\n");
				return 0;
			}
		}
		
		
	}
	return 2;
}
//释放用户链表
void free_list(puser p)
{
	puser pos = p->next;
	while  (pos!=p)
	{
		pos = pos->next;
		free (p);
		p = pos;
		
	}
	
	
	
}
//获取触摸屏坐标
int get_xy()
{
	int x,y;
	int x0,y0;
	int tx_flag = 1;
	int ty_flag = 1;
    //1.打开触摸屏驱动  "/dev/input/event0"
	int ts_fd = open("/dev/input/event0",O_RDWR);
	if(ts_fd < 0)
	{
		printf("open ts fail \n");
		return -1;
	}
	// 2.定义一个输入子系统结构体  
     struct input_event ts;
	//3.循环检测触摸屏 ，判断是否有触摸动作
	while(1)
	{
		//1.把触摸信息读取到输入子系统
		read(ts_fd,&ts,sizeof(ts));
		//2.分析输入子系统获取触摸坐标
		if(ts.type == EV_ABS)
		{
			if(ts.code == ABS_X)//x轴坐标
			{
				x = ts.value*800/1024;
				printf("%d \n",x);
				if(tx_flag == 1)
				{
					x0 = x;//记录起点坐标
					tx_flag = 0;
				}
				
			}
			if(ts.code == ABS_Y)//Y轴坐标
			{
				y = ts.value*480/614;
                printf("%d \n",y);
		        if(ty_flag == 1)
				{
					y0 = y;//记录起点坐标
					ty_flag = 0;
				}
								
			}
						
		}
		//判断手指头是否抬起
		if(ts.type == EV_KEY&&ts.code == BTN_TOUCH)
		{
			if(ts.value == 0)
			{   
		        if(x - x0 > 30)//左往右
		        {
					printf("左往右\n");
					return 1;
				}
				if(x - x0 < -30)//右往左
		        {
					printf("右往左\n");
					return 2;
				}
				if(y - y0 > 30)//上往下
		        {
					printf("上往下\n");
					return 3;
				}
				if(y - y0 < -30)//下往上
		        {
					printf("下往上\n");
					return 4;
				}
		        tx= x;
				ty= y;
				printf("x %d   y %d\n",x,y);
				break;
			}
		}
		
	}
		
}
//任意位置显示任意大小的bmp
int showbmp(char *filename,int w,int h,int x0,int y0,int s)
{
    // 1.打开lcd驱动  open  "/dev/fb0"
	int lcd_fd = open("/dev/fb0",O_RDWR);
	if(lcd_fd < 0)
	{
		printf("lcd open fail\n");
	    return -1;
	}
	//2.申请虚拟共享内存
	unsigned char *lcd = mmap(NULL,800*480*4,PROT_READ| PROT_WRITE,MAP_SHARED,lcd_fd,0);
	if(lcd == MAP_FAILED)
	{
		printf("mmap fail\n");
		return -1;
	}
	printf("mmap ok \n");
	//3.打开bmp图片，读取bmp颜色数据 24位 
	int bmp_fd = open(filename,O_RDWR);
	if(bmp_fd <0)
	{
		printf("open bmp fail\n");
		return -1;
	}
	printf("open bmp ok\n");
	
	unsigned char bmp[w*h*3];
	//跳过头54个字节
	//read(bmp_fd,bmp,54);
	lseek(bmp_fd,54,SEEK_SET);
	read(bmp_fd,bmp,w*h*3);
	
	//4.把24位的bmp数据转换成32位的lcd数据放入共享内存
	//*lcd  buf[0] lcd[0]
	 int x,y;
	 //瀑布式
/* 	 for(y=y0;y<h+y0;y++)
	 {
		 for(x=x0;x<w+x0;x++)
		 {
	        lcd[0+4*x+y*800*4] = bmp[0+3*(x-x0)+(h-1-(y-y0))*w*3];//b
	        lcd[1+4*x+y*800*4] = bmp[1+3*(x-x0)+(h-1-(y-y0))*w*3];//g 
	        lcd[2+4*x+y*800*4] = bmp[2+3*(x-x0)+(h-1-(y-y0))*w*3];//r	
	        lcd[3+4*x+y*800*4] = 0;//a
		 }
		 usleep(500*100);
	 } */
     //拉幕式
	 if(s == 1)
	 {
		for(x=x0;x<w+x0;x++)
	   {
		 for(y=y0;y<h+y0;y++)
		 {
	        lcd[0+4*x+y*800*4] = bmp[0+3*(x-x0)+(h-1-(y-y0))*w*3];//b
	        lcd[1+4*x+y*800*4] = bmp[1+3*(x-x0)+(h-1-(y-y0))*w*3];//g 
	        lcd[2+4*x+y*800*4] = bmp[2+3*(x-x0)+(h-1-(y-y0))*w*3];//r	
	        lcd[3+4*x+y*800*4] = 0;//a
		 }
		 usleep(500);
	   }	 	  
		 
	 }
	 if(s == 0)
	 {
		for(x=x0;x<w+x0;x++)
	   {
		 for(y=y0;y<h+y0;y++)
		 {
	        lcd[0+4*x+y*800*4] = bmp[0+3*(x-x0)+(h-1-(y-y0))*w*3];//b
	        lcd[1+4*x+y*800*4] = bmp[1+3*(x-x0)+(h-1-(y-y0))*w*3];//g 
	        lcd[2+4*x+y*800*4] = bmp[2+3*(x-x0)+(h-1-(y-y0))*w*3];//r	
	        lcd[3+4*x+y*800*4] = 0;//a
		 }
	   }	 	  
		 
	 }
	 //5.关闭文件
	 close(lcd_fd);
	 close(bmp_fd);
	 //6.释放虚拟内存 
	 munmap(lcd,800*480*4);
	return 0; 
	
}
//获取按键信息
int get_num(char input_num[7],int x,int y)
{
	strcpy(input_num,"******");
	int i = 0;
	while(1)
	{
	get_xy();
	if(i>=0&&i<6)
	{
	if(tx>263&&tx<344&&ty>220&&ty<301)
	{
		input_num[i]='1';
		if(y == 124)
		{
		showbmp("s.bmp",52,48,x+52*i,y,1);
		}
		else
		{
		showbmp("1.bmp",52,48,x+52*i,y,1);
		}
		i++;
	}
	if(tx>340&&tx<427&&ty>220&&ty<301)
	{
		input_num[i]='2';
		if(y==124)
		{
		showbmp("s.bmp",52,48,x+52*i,y,1);	
		}
		else
		{
		showbmp("2.bmp",52,48,x+52*i,y,1);
		}
		i++;
	}
	if(tx>427&&tx<523&&ty>219&&ty<301)
	{
		input_num[i]='3';
		if(y==124)
		{
		showbmp("s.bmp",52,48,x+52*i,y,1);	
		}
		else
		{
		showbmp("3.bmp",52,48,x+52*i,y,1);
		}
		i++;
	}
	if(tx>260&&tx<344&&ty>301&&ty<387)
	{
		input_num[i]='4';
		if(y==124)
		{
		showbmp("s.bmp",52,48,x+52*i,y,1);	
		}
		else
		{
		showbmp("4.bmp",52,48,x+52*i,y,1);
		}
		i++;
	}
	if(tx>344&&tx<427&&ty>301&&ty<387)
	{
		input_num[i]='5';
		if(y==124)
		{
		showbmp("s.bmp",52,48,x+52*i,y,1);	
		}
		else
		{
		showbmp("5.bmp",52,48,x+52*i,y,1);
		}
		i++;
	}
	if(tx>427&&tx<523&&ty>301&&ty<387)
	{
		input_num[i]='6';
		if(y==124)
		{
		showbmp("s.bmp",52,48,x+52*i,y,1);	
		}
		else
		{
		showbmp("6.bmp",52,48,x+52*i,y,1);
		}
		i++;
	}
	if(tx>260&&tx<344&&ty>388&&ty<461)
	{
		input_num[i]='7';
		if(y==124)
		{
		showbmp("s.bmp",52,48,x+52*i,y,1);	
		}
		else
		{
		showbmp("7.bmp",52,48,x+52*i,y,1);
		}
		i++;
	}
	if(tx>344&&tx<425&&ty>388&&ty<461)
	{
		input_num[i]='8';
		if(y==124)
		{
		showbmp("s.bmp",52,48,x+52*i,y,1);	
		}
		else
		{
		showbmp("8.bmp",52,48,x+52*i,y,1);
		}
		i++;
	}
	if(tx>429&&tx<522&&ty>388&&ty<461)
	{
		input_num[i]='9';
		if(y==124)
		{
		showbmp("s.bmp",52,48,x+52*i,y,1);	
		}
		else
		{
		showbmp("9.bmp",52,48,x+52*i,y,1);
		}
		i++;
	}
	if(tx>522&&tx<617&&ty>301&&ty<387)
	{
		input_num[i]='0';
		if(y==124)
		{
		showbmp("s.bmp",52,48,x+52*i,y,1);	
		}
		else
		{
		showbmp("0.bmp",52,48,x+52*i,y,1);
		}
		i++;
	}
	if(tx>522&&tx<617&&ty>379&&ty<444&&i>0)
	{
		i--;
		showbmp("x.bmp",52,48,x+52*i,y,1);
		
		input_num[i]='*';
		
	}
	if(tx>522&&tx<617&&ty>221&&ty<301)
	{
		i = 0;
		printf("ok\n");
		break;
	}
	}
	else
	{
	if(tx>522&&tx<617&&ty>379&&ty<444&&i>0)
	{
		i--;
		showbmp("x.bmp",52,48,x+52*i,y,1);
		input_num[i]='*';
		
	}
	if(tx>522&&tx<617&&ty>220&&ty<301)
	{
		i = 0;
		printf("ok\n");
		break;
	}
	}
	}
	return 0;
	
}
//手动模式
int show_hand(pho head)
{
	showbmp("main2.bmp",800,480,0,0,0);
	
	pho pos = (pho)malloc(sizeof(ph));
	pos = head;
	int set;
	//开始显示
	while(1)
	{
		
		
		set =0;
		set = get_xy();
		if (set == 1)
		{
			break;
		}
		if(tx<400)
		{
			pos=pos->pre;
			printf("open ok\n");
			showbmp(pos->ph_dir,800,480,0,0,0);
			
		}
		if(tx>400)
		{
			pos=pos->next;
			printf("open ok111\n");
			showbmp(pos->ph_dir,800,480,0,0,0);
			
		}
		if(pos == head)
		{
			break;
		}
	}
}
int show_auto(pho head)
{
	pho pos = (pho)malloc(sizeof(ph));
	pos = head->next;
	while(pos!=head)
	{
		showbmp(pos->ph_dir,800,480,0,0,0);
		usleep(500*1000);
		pos = pos->next;
	}
}
//功能界面
int my_con(pho head)
{
	showbmp("main1.bmp",800,480,0,0,0);
	while(1)
	{
		get_xy();
		if(tx>569&&tx<783&&ty>8&&ty<99)
		{
			break;
		}
		if(tx>453&&tx<693&&ty>388&&ty<455)
		{   
			show_hand(head);
			tx=0;
			ty=0;
		   showbmp("main1.bmp",800,480,0,0,0);
			//手动模式
		}
		if(tx>68&&tx<269&&ty>394&&ty<457)
		{
			 show_auto(head);
			 showbmp("main1.bmp",800,480,0,0,0);
			 tx =0;
			 ty =0;
			//自动模式
		}
	
	}
}
int zhuce_user(puser head,char acount[7],char pw1[7])
{
	puser pos = (puser)malloc(sizeof(user));
	for(pos=head->next;pos!=head;pos=pos->next)
	{
		if(strcmp(pos->ac,acount)==0)
		{
			showbmp("zhuce2.bmp",800,480,0,0,0);
			sleep(1);
			return 0;
		}
		
	}
	puser q = (puser)malloc(sizeof(user));
	while(pos->next!=head)
		{
			pos = pos->next;
		}
	pos->next =q;
	q->next =head;
	strcpy(q->ac,acount);
	strcpy(q->pas,pw1);
	showbmp("zhuce.bmp",800,480,0,0,0);
	sleep(2);
	return 1;
}
void cun(puser head)
{
	FILE *fp = fopen("1.txt","w");
	puser pos = (puser)malloc(sizeof(user));
	pos =head->next;
	while(pos!=head)
	{
		fprintf(fp,"%s ",pos->ac);
		fprintf(fp,"%s ",pos->pas);
		pos=pos->next;
		printf("\n");
	}
	fclose(fp);
}
void free_ph(pho head)                                                                                                         
 {
	pho pos = head->next;
	while  (pos!=head)
	{
		head->next = pos->next;
		free (pos);
		pos=head->next;
		
	}
	free(head);
}








