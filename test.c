#include"ephoto.h"
int main()
{
	int set = 0;
	char acount[7];
	char pw[7];
	puser myuser =init_user();
	//账户信息写入链表
	user_write(myuser);
	pho mypho_dir=init_pho();

	pho_write(mypho_dir);
	showbmp("qidong.bmp",800,480,0,0,1);
	get_xy();
	
	//输入账号密码
	while(1)
	{
		set = 2;
		showbmp("main.bmp",800,480,0,0,1);
		get_num(acount,279,46);
		get_num(pw,279,124);
		get_xy();
		if(tx>665&&ty>42&&tx<765&&ty<103)
		{
			set = log_in(myuser,acount,pw);
			
			
		//进入相册
			if(set == 1)
			{
				showbmp("tu2.bmp",800,480,0,0,0);
				showbmp("tu1.bmp",800,480,0,0,1);
				showbmp("denglu.bmp",800,480,0,0,0);
				sleep(2);
				my_con(mypho_dir);
				break;	
			}
			if(set == 0)
			{
				showbmp("denglu1.bmp",800,480,0,0,0);
				sleep(1);
			}
			if(set == 2)
			{
				showbmp("denglu2.bmp",800,480,0,0,0);
				sleep(1);
			}
		}
		if(tx>663&&ty>119&&tx<767&&ty<180)
		{
			set=zhuce_user(myuser,acount,pw);
		}
	
	}
	
	cun(myuser);
	free_ph(mypho_dir);
	free_list(myuser);
	showbmp("jieshu.bmp",800,480,0,0,1);
	return 1; 
	 
}                             
