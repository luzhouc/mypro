#include "charge.h"

//tcp初始化
int TCP_INIT(void)
{
    // 创建套接字
     tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket == -1)
    {
        perror("socket error");
        exit(0);
    }
    perror("socket ");


    
    // 设置服务器的地址信息
    struct sockaddr_in ServerAddr = {
        .sin_addr = inet_addr("192.168.2.103") , //设置对方的地址（云服务器）
        // .sin_addr = inet_addr("13.53.137.48") , //设置对方的地址（云服务器）
        .sin_port = htons(atoi("65001")) ,
        .sin_family = AF_INET
    };
    addrlen = sizeof(ServerAddr); 

    // 主动发起连接服务器的请求
    if(connect( tcp_socket  , (const struct sockaddr *)&ServerAddr, addrlen))
    {
        perror("connect error");
        exit(0);
    }
    perror("connect ");

    // 返回待连接套接字
    return 0 ;
}

//发送消息
void sendMessage(char *buf)
{
        printf("请输入数据：\n");
        sndmsg = calloc(1,512);
   
        bzero(sndmsg , 512) ;
        
        printf("请输入数据：\n");
       strcpy(sndmsg , buf);

        ssize_t ret_val = send( tcp_socket , sndmsg , strlen(sndmsg) , 0 );
        if (ret_val > 0)
        {
            printf("消息发送成功%ld...\n" , ret_val );
            if(!strcmp(sndmsg , "byebye\n"))
            {
                exit(0);
            }

        }
        else{
            perror("send error");
           
        }

    
}

//接收消息
void  RecvMsg()
{
    int connect_fd = tcp_socket;

    // 等待客户端发来消息
     rcvmsg = calloc(1,128);

    
        bzero(rcvmsg , 128 );

        int ret_val = read(connect_fd , rcvmsg , 128);
        if (ret_val > 0)
        {
            printf("recv%dbyt:%s\n" , ret_val , rcvmsg );
        }
        else if (ret_val == 0 )
        {
            printf("对方已经下线..\n");
           
        }
        else{
            perror("接受消息失败...\n");
           
        }  

}

//映射图片
//任意大小图片输入
int show(char *picture,int starx,int stary,int hight,int wide)
{
	int temp[hight*wide];
	int temp1[hight*wide];
	char temp_pic[hight*wide*3];
	int k = 0;  //访问temp数组
	bzero(temp,sizeof(temp));
	
	int fp_fb0 = open("/dev/fb0",O_RDWR);
	if(fp_fb0 == -1)
	{
		perror("open_fp_fb0");
		return -1;
	}
	
	int fp_picture = open(picture,O_RDWR);
	if(fp_picture == -1)
	{
		perror("open_picture");
		return -1;
	}
	
	int *pmap = mmap(NULL,800*480*4,PROT_READ | PROT_WRITE,MAP_SHARED,fp_fb0,0);
	if(pmap == (void*)-1)
	{
		return -1;
	}
	
	lseek(fp_picture,54,SEEK_SET);
	read(fp_picture,temp_pic,sizeof(temp_pic));

	for(int i = 0; i<hight*wide;i++)
	{
		temp[i] = temp_pic[3*i + 0] | temp_pic[3*i + 1]<<8 | temp_pic[3*i + 2]<<16 | 0x00<<24;
	}
	//翻转
	for(int i = 0;i < hight;i++)
	{
		for(int j = 0;j < wide;j++)
		{
			temp1[i*wide + j] = temp[wide*(hight - i-1) + j];
		}
	}
	
	for(int i = stary;i < (stary + hight);i++)
	{
		for(int j = starx;j < (starx + wide);j++)
		{
			pmap[i*800 + j] = temp1[k++];
		}
	}

	munmap(pmap,800*480*4);
	close(fp_fb0);
	close(fp_picture);
	return 0;
	
}
 struct LcdDevice *init_lcd(const char *device)
    {
        //申请空间
        struct LcdDevice* lcd = malloc(sizeof(struct LcdDevice));
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
        lcd->mp = mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcd->fd,0);

        return lcd;
    }

int input()
{
	while (1)
	{

		struct input_event info;
		read(ts_fd,&info,sizeof(info));

		if (info.type == EV_ABS && info.code == ABS_X)
		{
			ts_x = info.value;
		}
		if (info.type == EV_ABS && info.code == ABS_Y)
		{
			ts_y = info.value;
		}
		if (info.type == EV_KEY && info.code == BTN_TOUCH && info.value == 0)
		{
			/*
			ts_x = (ts_x*800*1.0)/1024;	
			ts_y = (ts_y*480*1.0)/600;
			*/
			printf("%d,%d\n",ts_x,ts_y);
			break;
		}		
		
	}

	return 0;
}

//写入文字	参数为 需要输入的文字 字体大小 画布长 画布宽 背景颜色 画布起始x轴 画布起始y轴
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

	return 0;
}

food * init_food_menu()
{
    printf("进入初始化界面\n");
    food * food_node = (food *)malloc(sizeof(food));
    if(food_node == NULL)
    {
        perror("malloc failure\n");
        exit(0);
    }
    
   
    strcpy(food_node->name,"first");
    food_node->num = 0;
    food_node->price = 0;
    food_node->next = NULL;
    printf("hello world\n");
    return food_node;
}

food * getfoodnode(food *node,char* name,int num,int pri)
{
    food * head ;
    head = node;
    food* data=(food *)malloc(sizeof(food));
    data->num =num;
    data->price=pri;
    strcpy(data->name,name);
    printf("菜名：%s\n",data->name);
    data->next=head->next;
    head->next =data;
    return head;

}

food * huoqucaidan(food *node)
{
    printf("获取食物节点\n");
    food* head = node;
    getfoodnode(head,"家常豆腐1",0,15);
    getfoodnode(head,"家常豆腐",0,15);
    getfoodnode(head,"宫保鸡丁",0,25);
    getfoodnode(head,"炒腐竹",0,10);
    getfoodnode(head,"红烧肉",0,45);
    getfoodnode(head,"辣椒炒肥肠",0,35);
    getfoodnode(head,"丝瓜炒蛋",0,13);
    getfoodnode(head,"凉拌藕片",0,13);
    getfoodnode(head,"凉拌三丝",0,13);
    getfoodnode(head,"凉拌皮蛋",0,13);
    getfoodnode(head,"丝拍黄瓜",0,13);
    getfoodnode(head,"苏打水",0,13);
    getfoodnode(head,"柠檬茶",0,13);
    getfoodnode(head,"可乐",0,13);
    //head=head->next;
    //printf("caiming %s\n",head->name);
    return node;
}

void changefood_list(food *node,char* name ,int num)
{
    food * temp=node;
    while(temp->next!=NULL)
    {
        temp=temp->next;
        if(strcmp(temp->name,name)==0)
        {
            temp->num=num;
            break;
        }
    }
}
food* checkfoodnum(food *node,char* name)
{
    food * temp=node;
    while(temp->next!=NULL)
    {
        temp=temp->next;
        if(strcmp(temp->name,name)==0)
        {
            return temp;
            
        }
    }
}
//登录界面
int login()
{
    
    char buf1[128];
    char buf[128];
    show("/IOT/project/pic/denglu.bmp",0,0,480,800);
    int len=0;
    int len1=0;
    while(1)
    {
        
        ts_x = 0;
	    ts_y = 0;
        input();
        
        
        if(ts_x>320 && ts_y>212 && ts_x<=796 && ts_y <= 315)
        {
            //输入账号
            printf("Input acount\n");
            write_world("登录",60,100,65,0x00ffffff,313,177);
            sleep(2);
            show("/IOT/project/pic/denglu.bmp",0,0,480,800);
            bzero(buf,128);
            scanf("%s",buf);
             len = strlen(buf);
           write_world(buf,60,25*len,65,0x00ffffff,313,177);

        }
        if(ts_x>324 && ts_y>367 && ts_x<=804 && ts_y <= 478)
        {
            //输入密码
            printf("Iput pw\n");
            write_world("输入密码",60,200,65,0x00ffffff,313,308);
            sleep(2);
            write_world(NULL,60,200,65,0x00ffffff,313,308);
            //sleep(2);
           // show("/IOT/project/pic/denglu.bmp",0,0,480,800);
            bzero(buf1,128);
            scanf("%s",buf1);
             len1 = strlen(buf1);
           write_world(buf1,60,25*len1,65,0x00ffffff,313,308);
        }
        if(ts_x>747 && ts_y>512 && ts_x<=931 && ts_y <= 596)
        {
            //输入完成
            char buf2[128];
            
            
            strcpy(buf2,"denglu&");
          // printf("buf2:%s",buf2);
            strcat(buf2,buf);
            //printf("buf2:%s",buf2);
            strcat(buf2,"@");
            strcat(buf2,buf1);
            //printf("buf2:%s",buf2);
            sendMessage(buf2);
            printf("buf2:%s",buf2);
            RecvMsg();
           //判断账号密码是否正确
            char *stat = rcvmsg;
            if(strcmp(stat,"ok")==0)
            {
                printf("ok");
                usleep(400000);
                //进入菜单
                food * head =init_food_menu();
                caidan(head);
                char *str =getmenu(head);
                char str1[300];
                strcpy(str1,"jiezhang&");
                printf("385\n");
                char str2[20];
                int size = sizeof(str);
                 snprintf(str2, size, "%d", yue);
                strcat(str1,buf);
                strcat(str1,"$");
                strcat(str1,buf1);
                strcat(str1,"$");
                strcat(str1,str2);
                
                strcat(str1,str);
                printf("开始发送\n");
                sendMessage(str1);
                write_world("结账完成！",60,180,65,0x00ffffff,350,170);
                sleep(3);
                return 0;
                

            }
            if(strcmp(stat,"noaccount")==0)
            {
                printf("noaccount\n");

            }
            if(!strcmp(stat,"passwderror")==0)
            {
                printf("passwderror\n");

            }
        }
        if(ts_x>12 && ts_y>506 && ts_x<=206 && ts_y <= 598)
        {
                return 0;
        }
        
    }
}

//注册界面
int sign_up()
{
    
    ts_x = 0;
	ts_y = 0;
    char buf1[128];
    char buf[128];
    char buf2[128];
    while(1)
    {
        show("/IOT/project/pic/zhuce.bmp",0,0,480,800);
        input();
        
        if(ts_x>320 && ts_y>212 && ts_x<=796 && ts_y <= 315)
        {
            //输入账号
            printf("Input acount\n");
            write_world("注册",60,100,65,0x0099d9ea,313,177);
            sleep(2);
            show("/IOT/project/pic/zhuce.bmp",0,0,480,800);
            bzero(buf,128);
            scanf("%s",buf);
            int len = strlen(buf);
            write_world(buf,60,25*len,65,0x0099d9ea,313,177);
        }
        
        if(ts_x>324 && ts_y>367 && ts_x<=804 && ts_y <= 478)
        {
            //输入密码
            printf("Iput pw\n");
            write_world("输入密码",60,200,65,0x0099d9ea,313,308);
            sleep(2);
            write_world(NULL,60,200,65,0x0099d9ea,313,308);
            //sleep(2);
           // show("/IOT/project/pic/denglu.bmp",0,0,480,800);
            bzero(buf1,128);
            scanf("%s",buf1);
            int len1 = strlen(buf1);
            write_world(buf1,60,25*len1,65,0x0099d9ea,313,308);
        }
        if(ts_x>747 && ts_y>512 && ts_x<=931 && ts_y <= 596)
        {
            //输入完成

            
            
            strcat(buf,"@");
            strcat(buf,buf1);
            strcpy(buf2,"zhuce&");
            strcat(buf2,buf);
            sendMessage(buf2);
            printf("input ok\n");
            RecvMsg();
           //判断账号密码是否正确
            char *stat = rcvmsg;
            if(!strcmp(stat,"noaccount"))
            {
                printf("ok\n");
                write_world("成功注册返回登录",60,300,65,0x0099d9ea,313,177);
                //return 1;
            }
            if(!strcmp(stat,"passwderror")||!strcmp(stat,"ok"))
            {
                printf("haveaccout\n");
                write_world("已有账户请修改账号或直接登录",60,300,65,0x0099d9ea,313,177);
                //return 2;
            }
            
        }
        if(ts_x>12 && ts_y>506 && ts_x<=206 && ts_y <= 598)
        {
                return 0;
        }
        
    }
}
int  choicefood(food* head,char* name,int price )
{
     //printf("%s数量为0\n",name);
     
    
    food *temp = checkfoodnum(head,name);
    
    while(1)
    {
        write_world(name,60,180,65,0x00ffffff,200,390);
        char str[10];
         int size = sizeof(str);  
         snprintf(str, size, "%d", temp->num); 
        write_world(str,60,40,65,0x00ffffff,380,390);
        ts_x = 0;
        ts_y = 0;
        input();
        printf("huoqu\n");
        if(ts_x>528 && ts_y>500 && ts_x<=647 && ts_y <= 598)//点击加减
         {
                    temp->num++; 
                    printf("%s %d,price:%d\n",temp->name,temp->num,temp->price);
                    changefood_list(head,name,temp->num);
                    price =price + temp->price;
                    printf("price:%d\n",price);
                    printf("%s数量为%d1\n",name,temp->num);
                    
        }
        else if(ts_x>656 && ts_y>500 && ts_x<=765 && ts_y <= 598)//点击减
        {
                    if(temp->num !=0)
                    {
                        temp->num --;
                        changefood_list(head,name,temp->num);
                        price = price-temp->price;
                        printf("price:%d\n",price);
                         printf("%s数量为%d1\n",name,temp->num);
                    }
                    if(temp->num==0)
                    {
                        printf("%s数量为%d2\n",name,temp->num);
                            }
        }
         else
         {
            printf("break\n");
            return price;
         } 
                   
    }
                
                

                return price;
}
//将点的菜拼接到一起
char* getmenu(food *head)
{
    printf("kaishipingjie\n");
    food* temp = head;
     char  *str= malloc(256);
    strcpy(str,"$");
    while(temp->next!=NULL)
    {

        printf("进入循环\n");
        if(temp->num!=0)
        {
            printf("name=:%s\n",temp->name);
            char str1[20];
            int size = sizeof(str1);
            snprintf(str1, size, "%d",temp->num);
            strcat(str,temp->name);
            strcat(str,"@");
            strcat(str,str1);
            
            
        }
        temp=temp->next;
    }
    printf("pingjieok\n");
    return str;
}
//进入菜单
food * caidan(food *head)
{
    food * temp = head;
    
    printf("进入菜单\n");
    show("/IOT//project/pic/caidan.bmp",0,0,480,800);
    //读取余额
    //RecvMsg();
    //开始点菜
    ts_x = 0;
	ts_y = 0;
    
    char temp1[128];
    usleep(500000);
    RecvMsg();
   
    strcpy(temp1,rcvmsg);
    printf("yue%s",temp1);
    yue=atoi(temp1);
    //创建链表将菜品放入链表
    huoqucaidan(temp);
    int price=0;
    while(1)
    {
        input();
        //food *head = init_food_menu();
        
        char str[20];
        int size = sizeof(str);
        //选择是进入什么菜种
        if(ts_x>2 && ts_y>86 && ts_x<=218 && ts_y <= 184)
        {
            //进入家常菜
            
            show("/IOT//project/pic/caidan.bmp",0,0,480,800);
            int length = snprintf(str, size, "%d", price);
            printf("%s\n",str);
           
            write_world(str,60,100,65,0x00ffffff,0,0);
            snprintf(str, size, "%d", yue);
            printf("%s\n",str);
            write_world(str,60,100,65,0x00ffffff,4,395);
            ts_x = 0;
	        ts_y = 0;
            input();
            if(ts_x>233 && ts_y>2 && ts_x<=457 && ts_y <= 145)
            {
                //选择家常豆腐
                    
                price = choicefood(head,"家常豆腐",price);
                
                
            }
            if(ts_x>503 && ts_y>2 && ts_x<=733 && ts_y <= 145)
            {
                //选择宫保鸡丁
                price = choicefood(head,"宫保鸡丁",price);
                

              
            }
            if(ts_x>760 && ts_y>2 && ts_x<=1022 && ts_y <= 145)
            {
                //选择炒腐竹
                price = choicefood(head,"炒腐竹",price);
                

               
            }
            if(ts_x>234 && ts_y>211 && ts_x<=482 && ts_y <= 363)
            {
                //选择家红烧肉
               price = choicefood(head,"红烧肉",price);
               

                 
            }
            if(ts_x>491 && ts_y>211 && ts_x<=742 && ts_y <= 363)
            {
                //选择辣椒炒肥肠
               price = choicefood(head,"辣椒炒肥肠",price);
                

                
            }
            if(ts_x>742 && ts_y>211 && ts_x<=1022 && ts_y <= 363)
            {
                //选择丝瓜炒蛋
                price = choicefood(head,"丝瓜炒蛋",price);
                

            }
            //加减如果菜品数量为0不能减
            //点击其他位置重新进入点菜
            //点菜时将菜品价格加到餐费里  

        }
        if(ts_x>2 && ts_y>184 && ts_x<=218 && ts_y <= 270)
        {
            ts_x = 0;
	        ts_y = 0;
            input();
            //进入凉拌
            show("/IOT//project/pic/liangban.bmp",0,0,480,800);
            int length = snprintf(str, size, "%d", price);
            printf("%s\n",str);
            write_world(str,60,100,65,0x00ffffff,0,0);
            snprintf(str, size, "%d", yue);
            printf("%s\n",str);
            write_world(str,60,100,65,0x00ffffff,4,395);
            //加减如果菜品数量为0不能减
            //点击其他位置重新进入点菜
            //点菜时将菜品价格加到餐费里
            if(ts_x>236 && ts_y>4 && ts_x<=573 && ts_y <= 189)
            {
                //选择凉拌藕片
                price = choicefood(head,"凉拌藕片",price);
                

            }
            if(ts_x>640 && ts_y>4 && ts_x<=981 && ts_y <= 189)
            {
                //选择凉拌藕片
                price = choicefood(head,"凉拌三丝",price);
                

            }
            if(ts_x>236 && ts_y>265 && ts_x<=544 && ts_y <= 399)
            {
                //选择凉拌藕片
                price = choicefood(head,"凉拌皮蛋",price);
                

            }
            if(ts_x>613 && ts_y>271 && ts_x<=921 && ts_y <= 396)
            {
                //选择凉拌藕片
                price = choicefood(head,"拍黄瓜",price);
                

            }

        }
        if(ts_x>2 && ts_y>270 && ts_x<=218 && ts_y <=370 )
        {
            //进入火锅
            show("/IOT//project/pic/caidan.bmp",0,0,480,800);
            int length = snprintf(str, size, "%d", price);
            printf("%s\n",str);
            write_world(str,60,100,65,0x00ffffff,0,0);
            snprintf(str, size, "%d", yue);
            printf("%s\n",str);
            write_world(str,60,100,65,0x00ffffff,4,395);
            //加减如果菜品数量为0不能减
            //点击其他位置重新进入点菜
            //点菜时将菜品价格加到餐费里  

        }
        if(ts_x>2 && ts_y>370 && ts_x<=218 && ts_y <=470 )
        {
            //进入饮料
            show("/IOT//project/pic/yinliao.bmp",0,0,480,800);
            int length = snprintf(str, size, "%d", price);
            printf("%s\n",str);
            write_world(str,60,100,65,0x00ffffff,0,0);
            snprintf(str, size, "%d", yue);
            printf("%s\n",str);
            write_world(str,60,100,65,0x00ffffff,4,395);
            ts_x = 0;
	        ts_y = 0;
            input();
             if(ts_x>262 && ts_y>9 && ts_x<=437 && ts_y <= 270)
            {
                //选择凉拌藕片
                price = choicefood(head,"苏打水",price);
                

            }
             if(ts_x>485 && ts_y>9 && ts_x<=667 && ts_y <= 267)
            {
                //选择凉拌藕片
                price = choicefood(head,"柠檬茶",price);
                

            }
             if(ts_x>781 && ts_y>2 && ts_x<=921 && ts_y <= 270)
            {
                //选择凉拌藕片
                price = choicefood(head,"可乐",price);
                

            }
            //加减如果菜品数量为0不能减
            //点击其他位置重新进入点菜
            //点菜时将菜品价格加到餐费里  

        }
        if(ts_x>803 && ts_y>470 && ts_x<=1004 && ts_y <=596 )
        {
            //进入结账
            show("/IOT//project/pic/caidan.bmp",0,0,480,800);
            int length = snprintf(str, size, "%d", price);
            printf("%s\n",str);
            write_world(str,60,100,65,0x00ffffff,0,0);
            
            if(yue >= price)
            {
                yue -= price;
                printf("yue:%d\n",yue);
                snprintf(str, size, "%d", yue);
                printf("%s\n",str);
                write_world(str,60,100,65,0x00ffffff,4,395);
                
                printf("结账完成\n");
                sleep(2);
                return NULL;
            }
            //加减如果菜品数量为0不能减
            //点击其他位置重新进入点菜
            //点菜时将菜品价格加到餐费里  

        }
        
        //点菜完成遍历链表将不为0的菜品结合在一起传送到服务器斌计算总额减去余额
        //点菜完成
    }

    
}


int login_interface()
{
    ts_x = 0;
	ts_y = 0;
    
   //注册登录选项
   while(1)
   {
    show("/IOT/project/pic/start.bmp",0,0,480,800);
    
    input();
    if(ts_x>286 && ts_y>200 && ts_x<=669 && ts_y <= 317)
    {
        printf("login\n");
        //进入登录界面
        login();
    }
     if(ts_x>84 && ts_y>358 && ts_x<=670 && ts_y <= 492)
    {
        printf("zhuce\n"); 
        //进入注册界面
        sign_up();
    }


   }
   
    
   //注册注册成功将信息加入链表
   //登录->用户或者管理员
   //用户进入用户界面
   //管理员进入管理员界面
   return 0;
}

