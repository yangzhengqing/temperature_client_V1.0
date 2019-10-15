#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include "get_temp.h"
int DS_get_tem(float *temp,char *chip)
{
        DIR                *dp;
        struct dirent      *dirp;
        char               w1_path[30] = "/sys/bus/w1/devices/";
        int                found = 0;
        int                fd;
        char               *ptr;
        char               buf[100];
        if((dp = opendir(w1_path)) < 0)
        {
                printf("opendir w1_path failed: %s\n",strerror(errno));
        }

        while((dirp = readdir(dp)) != NULL)
        {
                if(strstr(dirp->d_name,"28-") > 0)
                {
                        strcpy(chip,dirp->d_name);
                        found = 1;
                        break;
                }
        }
        closedir(dp);
        strncat(w1_path,chip,sizeof(w1_path));
        strncat(w1_path,"/w1_slave",sizeof(w1_path));
   
        if(!found)
        {
                printf("readdir failed and can't find %s\n",dirp->d_name);
        } 
        
        
        if((fd=open(w1_path,O_RDONLY)) < 0)
        {
                printf("open w1_path failed: %s\n",strerror(errno));
                close(fd);
        }


        if(read(fd,buf,sizeof(buf)) < 0)
        {
                printf("read from w1_path failed: %s\n",strerror(errno));
                close(fd);
        }
       
        ptr = strstr(buf,"t=");
        if(!ptr)
                printf("ERROR:can't get temperature\n");
        ptr+=2;
        *temp = atof(ptr)/1000;

        return 0;
}


