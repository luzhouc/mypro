#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */

#define MAX_LINE_LENGTH 256
char yue[128];

int connect_fd;
char msg[512];
socklen_t addrlen;

int TCP_INIT(int port )
{
    // 创建套接字
    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket == -1)
    {
        perror("socket error");
        exit(0);
    }
    perror("socket ");
    

    // 设置服务器的地址信息
    struct sockaddr_in ServerAddr = {
        .sin_addr = INADDR_ANY ,
        .sin_port = htons(port) ,
        .sin_family = AF_INET
    };
    addrlen = sizeof(ServerAddr); 

    // 绑定地址信息
    if(bind(tcp_socket , (const struct sockaddr *)&ServerAddr, addrlen))
    {
        perror("bind error");
        exit(0);
    }
    perror("bind ");


    // 把待链接套接字设置为监听套接字
    if(listen(tcp_socket , 6 ))
    {
        perror("listen error");
        exit(0);
    }
    perror("listen ");


    // 返回监听套接字
    return tcp_socket ;
}


void * Recv(void *arg)
{
    int connect_fd = *(int *)arg;
    

    while(1)
    {
        bzero(msg , 512 );

        int ret_val = read(connect_fd , msg , 512);
        if (ret_val > 0)
        {
            printf("成功收到%d字节：%s\n" , ret_val , msg );
            sleep(1);
        }
        else if (ret_val == 0 )
        {
            printf("对方已经下线..\n");
            break ;
        }
        else{
            perror("接受消息失败...\n");
            continue;
        }
        
        
    }
}

void sendmsg1(char *smsg)
{
    char * sndmsg = calloc(1,128);
    strcpy(sndmsg,smsg);
    
       
        
       
        

        ssize_t ret_val = send( connect_fd , sndmsg , strlen(sndmsg) , 0 );
        if (ret_val > 0)
        {
            printf("消息发送成功%ld...%s\n" , ret_val,sndmsg );
        }
        else{
            perror("send error");
           
        }
    


}

char* checkaccount(char* ac ,char * pw)
{
    printf("jinrumimayanzheng\n");
    FILE *fp=fopen("coustomer.txt","r");
    char line[256];
    if(fp == NULL)
    {
        printf("opentxt fail\n");
        return NULL;
    }

    while(fgets(line,sizeof(line),fp) != NULL)
    {
        char* account  = strtok(line,"@");
        printf("account:%s\n",account);
        char* password = strtok(NULL,"@");
        printf("passworld:%s\n",password);
        strcpy(yue,strtok(NULL,"@"));
        printf("yue:%s\n",yue);
        if(strcmp(account , ac)==0)
        {
            if(strcmp(password,pw)==0)
            {
                printf("ok\n");
                return "ok";
            }
            else
            {
                return "passwderror";
            }
        }
        
    }
    fclose(fp);
    printf("148\n");
    return "noaccount";
}
int zhuceaccount(char *ac,char *pw)
{
    FILE *file;
    char line[256];
    FILE *fp=fopen("coustomer.txt","a");
    if (fp == NULL) {
        printf("无法打开文件。\n");
        return -1;
    }
    char temp[256];
        strcpy(temp,ac);
        printf("162\n");
            strcat(temp,"@");
           printf("163\n"); 
            strcat(temp,pw);
            printf("164\n");
            strcat(temp,"@");
            printf("165\n");
            strcat(temp,"1000");
            printf("166\n");
     fprintf(fp, "%s\n", temp);
     printf("167\n");
     fclose(fp);
     return 0;
}
int writeaccount(char *ac,char *pw,char *ye)
{
    //FILE *fp;
    printf("xieru:%s\n",ac);
    char line[256];
    FILE *fp1=fopen("coustomer.txt","r+");
    if (fp1 == NULL) {
        printf("无法打开文件。\n");
        return -1;
    }
    long previousLinePos = 0;  // 上一行的位置
    long currentLinePos = 0; 
    while (fgets(line, sizeof(line), fp1) != NULL) 
    {   
        previousLinePos = currentLinePos;  // 保存上一行的位置
        currentLinePos = ftell(fp1);    
        char* account  = strtok(line,"@");
        char temp[256];
        if(strcmp(account,ac)==0)
        {
            strcpy(temp,ac);

            strcat(temp,"@");
            strcat(temp,pw);
            strcat(temp,"@");
            strcat(temp,ye);
            fseek(fp1, previousLinePos, SEEK_SET);
            fprintf(fp1, "%s\r\n", temp);
            printf("write:%s",temp);
            break;
        }


    }
    fclose(fp1);
    return 0;
}
int writefoodmenu(char* str)
{
     //FILE *fp;
    char line[256];
    FILE *fp=fopen("food.txt","a");
    if (fp == NULL) {
        printf("无法打开文件。\n");
        return -1;
    }
    fprintf(fp, "%s\n", str);
    fclose(fp);
    return 0;
}
int main(int argc, char const *argv[])
{

    // 初始化TCP服务器
    int tcp_socket = TCP_INIT(65001);

    // 等待客户端连接
    struct sockaddr_in FromAddr ;
    connect_fd =  accept(tcp_socket , (struct sockaddr *)&FromAddr,&addrlen);
    if (connect_fd == -1)
    {
        perror("accept error");
        exit(0);
    }
    printf("有新的客户端连接成功【%s:%d】\n" , 
            inet_ntoa(FromAddr.sin_addr) , ntohs(FromAddr.sin_port) );
    
    pthread_t a;
	pthread_create(&a, NULL, Recv,(void*)&connect_fd );
    printf("打开线程\n");
    while(1)
    {
        char *rmsg =calloc(1,128);
        printf("进入循环\n");
        if(strlen(msg))
        {
            printf("传送消息\n");
            strcpy(rmsg,msg);
            printf("%s",rmsg);
             char* temp=strtok(rmsg,"&");
            if( strcmp(temp,"denglu")==0)
            {
               
                temp=strtok(NULL,"&");
                 printf("temp:%s\n",temp);
                char* account=strtok(temp,"@");
                printf("ac:%s\n",account);
                char* pw =strtok(NULL,"@");
                printf("pw:%s\n",account);
                //从文本中读取账号密码余额并把余额发送回去进入读取函数
                printf("jijiangjirumima\n");
                char* revalue = checkaccount(account, pw);
                printf("190\n");
                sendmsg1(revalue);
                printf("re:%s",revalue);
                printf("192\n");
                usleep(500000);
                if(strcmp(revalue,"ok")==0)
                {
                    sendmsg1(yue);
                }
                
            
            }
            if(strcmp(temp,"zhuce")==0)
            {
                temp=strtok(NULL,"&");
                 printf("temp:%s\n",temp);
                char* account=strtok(temp,"@");
                printf("ac:%s\n",account);
                char* pw =strtok(NULL,"@");
                printf("pw:%s\n",account);
                char* revalue = checkaccount(account, pw);
                if(strcmp(revalue,"noaccount")==0)
                {
                    zhuceaccount(account,pw);
                }
                sendmsg1(revalue);
            } 
            if(strcmp(temp,"jiezhang")==0)
            {
                 temp=strtok(NULL,"&");
                writefoodmenu(temp);
                //strtok(temp,"$");
                 char* gtname=malloc(20);
                 char* gtpw=malloc(20);
                 char* gtyue=malloc(20);
                 printf("297\n");
                 strcpy(gtname,strtok(temp,"$"));
                 printf("299\n");
                 strcpy(gtpw,strtok(NULL,"$"));
                 printf("301\n");
                 strcpy(gtyue,strtok(NULL,"$"));
                 
                 printf("303\n");
                writeaccount(gtname,gtpw,gtyue);
                free(gtname);
                free(gtpw);
                free(gtyue);
                printf("305\n");


            }
        }
        //printf("1");
       sleep(1); 
       
        
        
    }



    // 关闭套接字
    close(connect_fd); 
    close(tcp_socket);
    
     
    return 0;
}
