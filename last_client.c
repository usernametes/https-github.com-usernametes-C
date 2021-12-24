#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/select.h>
#include<sys/time.h>
#include<netdb.h>
#include<openssl/ssl.h>
#include<openssl/err.h>



void http_mode(int sid){
    ssize_t nums, numr, numw;
    char send_buf[10240], recv_buf[1024], file_buf[1024], fbuf[2024];
    int fd, retval, maxfd;
    fd_set rdfs;
    struct timeval time_out;
    time_out.tv_usec = 100000;
    time_out.tv_sec = 0;

    

    

    printf("URLを入力してください。\n");
    while(1){
        memset(send_buf, '\0', sizeof(send_buf));
        scanf("%s", send_buf);
        if(strncmp(send_buf, "http", 4) == 0){
            break;
        }
        printf("無効な形式です\n");
    }
    

    nums = send(sid, send_buf, strlen(send_buf), 0);
    if(nums < 0){
        perror("ERROR:send(HTTP_MODE)");
        exit(-1);
    }

    char  *start = strrchr(send_buf, '/');
    strcpy(file_buf, start + 1);
   

    sprintf(fbuf, "s%s", file_buf);

    fd = open(fbuf, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    if(fd < 0){
        perror("Error:open()");
        exit(-1);
    }

    FD_ZERO(&rdfs);
    FD_SET(sid, &rdfs);
    maxfd = sid;
    printf("受信中\n");
    while(1){
        retval = select(maxfd + 1, &rdfs, NULL, NULL, &time_out);
        if(retval < 0){
            perror("retval()");
            exit(-1);
        }
        if(retval == 0){
            printf("実行完了！\n");
            close(fd);
            return;
        }


        numr = recv(sid, recv_buf, sizeof(recv_buf), 0);
        if(numr < 0){
            perror("Error: recv()");
            exit(-1);
        }

        numw = write(fd, recv_buf, numr);
        if(numw < 0){
            perror("Error:write()");
            exit(-1);
        }

    }
    printf("実行完了！\n");
    close(fd);
    return;
    
}

int main(int argc, char *argv[]){
    int sid, retval, maxfd;
    int port = atoi(argv[2]);
    fd_set rdfs;
    ssize_t numr, nums, numf;
    struct sockaddr_in addr;
    
    


   

    if((sid = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Error:socket()");
        exit(-1);
    }


    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(port);


    if(connect(sid, (struct sockaddr *)&addr, sizeof(addr)) != 0){
        perror("Error:connect()");exit(-1);
    }

    


    while(1){
        FD_ZERO(&rdfs);
        FD_SET(0, &rdfs);
        FD_SET(sid, &rdfs);
        

        maxfd = sid;
        retval = select(maxfd + 1, &rdfs, NULL, NULL, NULL);
        if(retval < 0){
            perror("select()");
            exit(-1);
        }


        if(retval > 0){
            if(FD_ISSET(sid, &rdfs)){
                char recv_buf[100], allbufr[1000];
                memset(recv_buf, '\0', sizeof(recv_buf));
                numr = recv(sid, recv_buf, sizeof(recv_buf)-1, 0);
                if(numr == -1){perror("Error:recv()");exit(-1);}
                
                /*if(numr == 0){
                    printf("接続先から切断されています\n");
                    printf("終了します。");
                    close(sid);
                    close(fd);
                    exit(-1);
                    
                }*/
                recv_buf[numr] = '\0';

                //sprintf(allbufr, "R %s", recv_buf);
                printf("%s", recv_buf);

                if(strncmp(recv_buf, "Enable HTTP Mode", 16) == 0){
                    printf("http mode\n");
                    http_mode(sid);
                }
                if(strncmp(recv_buf, "disconnect", 10) == 0){
                    close(sid);
                    exit(-1);
                }

                
            }
            if(FD_ISSET(0, &rdfs)){
                char buf[100];
                memset(buf, '\0', sizeof(buf));
                scanf("%s", buf);
                
               
                
                //sprintf(allbufw, "%s", buf);
                
                nums = send(sid, buf, strlen(buf), 0);
                if(nums == -1){perror("Error:send()");exit(-1);}

                
                //printf("%s", allbufw);
                   
                

                
                
                
                
            }
        }
    }
    close(sid);
    return 0;
}