#include<stdio.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<getopt.h>
#include<stdlib.h>
#include<sys/types.h>
#include<dirent.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<syslog.h>
#include<time.h>
#include<stdarg.h>
#include<signal.h>
#include<libgen.h>
#include<netdb.h>
#include<netinet/in.h>
#include "get_temp.h"


void sig_quit(int);

void print_usage(char *order)
{
        printf("%s usages:\n",order);
        printf("-I(IP):server IP\n");
        printf("-p(port):server port\n");
        printf("-h(help):help information\n");

        exit(0);
}

int get_stop = 0;

int main(int argc,char **argv)
{
        char                   *serv_IP = NULL;
        int                    serv_port = 0;
        int                    opt;
        struct sockaddr_in     serv_addr;
        char                   buf[100];
        char                   buf1[100];
        char                   chip[20];
        int                    sockfd;
        int                    rv = -1;
        int                    on = 1;
        float                  temp;
        int                    line;
        struct timespec        tout;
        struct tm              *tmp;
        struct hostent         *hptr;
        struct option long_options[]=
        {
                
                {"IP",1,NULL,'i'},
                {"port",1,NULL,'p'},
                {"help",0,NULL,'h'},
                {0,0,0,0}
        
        };

        hptr = gethostbyname(argv[1]);
        if(hptr != NULL)
        {
                printf("domian name visit.\n");
                           
                serv_IP = hptr->h_addr_list[1];

                printf("IP address2:%s\n",inet_ntoa(*(struct in_addr *)serv_IP));

        }
        else if(hptr == NULL)
        {
                while((opt=getopt_long(argc,argv,"i:p:h",long_options,NULL)) > 0)
                {

                        switch(opt)
                        {


                         case 'i':
                                serv_IP = optarg;
                                break;
                         case 'p':
                                serv_port = atoi(optarg);
                                break;
                         case 'h':
                                print_usage(argv[0]);
                                break;
                         default:
                                break;

                       }
      
                }

                if((!serv_IP)|(!serv_port))
                {
                         print_usage(argv[0]);
                }
       }

       
        if(signal(SIGQUIT,sig_quit) < 0)
        {
                printf("signal SIGKILL failed:%s\n",strerror(errno));
        }

        if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        {
                printf("socket failed: %s\n",strerror(errno));
          //      mylog(__FUNCTION__,__LINE__,ERROR,"socket failed:%s\n",strerror(errno));
                return -1;
        }

        setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

        memset(&serv_addr,0,sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(serv_port);
        inet_aton(serv_IP,&serv_addr.sin_addr);
         
        memset(buf,0,sizeof(buf));

        printf("client is connecting serve...\n");

        if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        {
                printf("connect failed: %s\n",strerror(errno));
          //      mylog(__FUNCTION__,__LINE__,ERROR,"connect failed:%s\n",strerror(errno));
        }
        else
        {

                printf("Connect successfully...\n");
        }


        while(!get_stop)
        {
                 
                DS_get_tem(&temp,chip);
                tmp = localtime(&tout.tv_sec);
                strftime(buf1,sizeof(buf1),"%r",tmp);

                sprintf(buf,"%s %.3f℃ %s",chip,temp,buf1);

                if(write(sockfd,buf,strlen(buf)) <0 )
                {

                        printf("write to serve unsuccessfully: %s\n",strerror(errno));
            //            mylog(__FUNCTION__,__LINE__,ERROR,"write to serve failed:%s\n",strerror(errno));
                        goto cleanup;
                }

		printf("temprature:%s\n",buf);

                memset(buf,0,sizeof(buf));
            

                if((rv = read(sockfd,buf,sizeof(buf))) < 0)
                {   
                        printf("read from serve unsuccessfully: %s\n",strerror(errno));
              //          mylog(__FUNCTION__,__LINE__,ERROR,"read from serve failed:%s\n",strerror(errno));
                        goto cleanup;
                }

                if(rv == 0)
                { 
                        printf("disconnect serve\n");
                //        mylog(__FUNCTION__,__LINE__,ERROR,"disconnect failed:%s\n",strerror(errno));
                        goto cleanup;
                }

                printf(" %s\n",buf);

                memset(buf,0,sizeof(buf));

                sleep(10);          
           }

        close(sockfd);

cleanup:
        close(sockfd);
        return 0;

}


void sig_quit(int signo)
{
    get_stop = 1;
}





