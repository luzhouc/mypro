#include <stdio.h>
#include <string.h>

int main()
{
    char buf[128];
    char buf1[128];
    char buf2[128];
    strcpy(buf,"zhuce&");
    strcpy(buf1,"11111");
    strcpy(buf2,"11111");
    strcat(buf1,"@");
    strcat(buf1,buf2);
    
    strcat(buf,buf1);
    printf("%s\n",buf);
    return 0;

}