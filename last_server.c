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


#define USERFILE "idpassword.txt"
#define URLFILE "url_file.txt"

struct INFO_URL{
    char host_name[1000];
    char resource_name[1000];
    char file_name[1000];
    int no_port;
};
//ID, password
void create_user(int sid){
    char sbuf[1024], namebuf[1024], passwordbuf[1024], lbuf[10240];
    int fd;
    ssize_t numw;

    fd = open(USERFILE, O_WRONLY|O_APPEND|O_CREAT, 0666);
    if(fd < 0){
        perror("Error: opne()");
        exit(-1);
    }

    ssize_t nums, numr;

    //creating user
    while(1){
        char buf[10240], rbuf[1024];
        sprintf(sbuf, "ユーザー名を入力してください。\n");
        nums = send(sid, sbuf, strlen(sbuf), 0);
        if(nums == -1){perror("Error:send()");exit(-1);}
        numr = recv(sid, rbuf, sizeof(rbuf), 0);
        if(numr == -1){perror("ERROR:recv");exit(-1);}
        rbuf[numr] = '\0';
        strcpy(namebuf, rbuf);

        sprintf(buf, "これでいいですか？\nNAME: %s\n 良ければ 'DONE'を入力してください\n", rbuf);
        nums = send(sid, buf, strlen(buf), 0);
        if(nums == -1){perror("Error:send()");exit(-1);}
        memset(rbuf, '\0', sizeof(rbuf));
        numr = recv(sid, rbuf, sizeof(rbuf), 0);
        if(numr == -1){perror("ERROR:recv");exit(-1);}
        if(strcmp(rbuf, "DONE") == 0){
            break;
        }

    }
    //creating password 
    while(1){
        char buf[10240], rbuf[1024];
        sprintf(sbuf, "passwordを作成する。入力してください。\n");
        nums = send(sid, sbuf, strlen(sbuf), 0);
        if(nums == -1){perror("Error:send()");exit(-1);}
        numr = recv(sid, rbuf, sizeof(rbuf), 0);
        if(numr == -1){perror("ERROR:recv");exit(-1);}
        rbuf[numr] = '\0';
        strcpy(passwordbuf, rbuf);

        sprintf(buf, "これでいいですか？\nNAME: %s\n 良ければDONEを入力してください\n", passwordbuf);
        nums = send(sid, buf, strlen(buf), 0);
        if(nums == -1){perror("Error:send()");exit(-1);}

        memset(rbuf, '\0', sizeof(rbuf));
        numr = recv(sid, rbuf, sizeof(rbuf), 0);
        printf("%s\n", rbuf);
        if(numr == -1){perror("ERROR:recv");exit(-1);}
        if(strcmp(rbuf, "DONE") == 0){
            break;
        }
    }

    sprintf(lbuf, "%s::%s\n", namebuf,passwordbuf);
    numw = write(fd, lbuf, strlen(lbuf));
    if(numw < 0){
        perror("Error:write()");
        exit(-1);
    }

    close(fd);
    
}
//ID, password
int idchecker(int sid, char *buf, char *pbuf){
    int fd, numr, cnt = 0; 
    ssize_t nums;
    char d_filebuf[102400], idbuf[1024], passbuf[1024], stbuf[10240];
    char *mid, *end;
    printf("%s, %s\n", buf, pbuf);

    fd = open(USERFILE, O_RDONLY, 0666);
    if(fd < 0){
        perror("Error: NO file()");
        exit(-1);
    }

    numr = read(fd, d_filebuf, sizeof(d_filebuf));
    if(numr < 0){
        perror("Error: read()");
        exit(-1);
    }

    char *start = d_filebuf;

    /*char *start = strstr(d_filebuf, "\n");
    if(start = NULL){
        perror("ERROR: STRSTR()");
        exit(-1);
    }*/

    while (1)
    {
        
        /*start = strstr(start, "\n");
        if(start = NULL){
            perror("ERROR: STRSTR()");
            exit(-1);
        }*/
        //printf("%s\n", start);

        mid = strstr(start + 1, "::");
        //printf("%s\n", mid);
        end = strstr(mid, "\n");
        
        printf("let's go3\n");
        
        memset(idbuf, '\0', sizeof(idbuf));
        memset(passbuf, '\0', sizeof(passbuf));
        strncpy(idbuf, start, mid - start);
        strncpy(passbuf, mid + 2, end - mid - 2);
        printf("%s\n", idbuf);
        printf("%s\n", passbuf);
        

        if(strcmp(buf, idbuf) == 0){
            sprintf(stbuf, "ID PASS\n");
            //printf("%s", stbuf);
            nums = send(sid, stbuf, strlen(stbuf), 0);
            if(nums == -1){perror("Error:send()");exit(-1);}

            
        }
        
        if(strcmp(pbuf, passbuf) == 0){
            sprintf(stbuf, "PASSWORD PASS\n");
            //printf("%s", stbuf);
            nums = send(sid, stbuf, strlen(stbuf), 0);
            if(nums == -1){perror("Error:send()");exit(-1);}


                
            break;
        }

        
        start = end + 1;
    }
    


close(fd);
return 1;
}



//URL
void lineread(int sid, char *buf){
    ssize_t numr;
    //int cnt = 0;
    //char *s = buf;

    while(1){
        numr = recv(sid, buf, 1, 0);
        if(numr < 0){perror("ERROR:recv()");exit(-1);}

        if(numr == 0){break;}


        if(*buf == '\n'){
            *(buf + 1) = '\0';
            break; 
        }
        buf++;
    }
   
}
//URL
void SSL_lineread(SSL *ssl, char *buf){
    ssize_t numr;
   // int cnt = 0;
    //char *s = buf;
    while(1){
        numr = SSL_read(ssl, buf, 1);
        if(numr < 0){perror("ERROR:recv()");exit(-1);}

        if(numr == 0){break;}


        if(*buf == '\n'){
            *(buf + 1) = '\0';
            break; 
        }
        buf++;
    }
    
}
//URL
struct INFO_URL url_editer(char *url){
    struct INFO_URL info_url = {};
    char http[1024];



    char *start = strstr(url, "//");
    if(start == NULL){
        printf("URL???\n");
        return info_url;
    }
    char *end = strstr(start + 2, "/");
    strncpy(info_url.host_name, start + 2, end - start - 2);

    strcpy(info_url.resource_name, end);

    end = strstr(url, ":");
    strncpy(http, url, end - url);
    if(strcmp(http, "https") == 0){
        info_url.no_port = 443;
    }else if(strcmp(http, "http") == 0){
        info_url.no_port = 80;
    }

    //printf("%d\n", info_url.no_port);
    //endで行ける気がする無理なら要変更

    start = strrchr(url, '/');
    strcpy(info_url.file_name, start + 1);
    

    return info_url;
}
//URL
void http_exchanger(int sid, struct INFO_URL info_url){
    int fd,code;
    char s_buf[1024];
    char rbuf[1024];
    char buf[1024];
    ssize_t nums, numr, num;


    fd = open(info_url.file_name, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    if(fd < 0){
        perror("Error:open()");
        exit(-1);
    }


    sprintf(s_buf, "GET %s HTTP/1.0\r\n\r\n", info_url.resource_name);
    //printf("%s\n", s_buf);
    nums = send(sid, s_buf, strlen(s_buf), 0);
    if(nums < 0){
        perror("Error:send()");
        exit(-1);
    }

    lineread(sid, rbuf);
    sscanf(rbuf, "%*s %d %*s", &code);
    printf("%d\n", code);


    
    while(1){
        lineread(sid, rbuf);
        if(strcmp(rbuf, "\r\n") == 0)break;
    }

    
    while(1){
        numr = recv(sid, buf, sizeof(buf), 0);
        if(numr < 0){
            perror("Error: recv()");
            exit(-1);
        }
        if(numr == 0){break;}
        
        num = write(fd, buf, numr);
        if(num < 0){
            perror("Error:write()");
            exit(-1);
        }

    }
    close(fd);
}
//url
void https_exchanger(int sid, struct INFO_URL info_url){
    ssize_t numr, nums, num;
    int fd, code;
    char s_buf[10240], r_buf[2048], buf[2048];
    
    
    SSL *ssl;
    SSL_CTX *ctx;
    int err;

    SSL_load_error_strings();
    SSL_library_init();
    ctx = SSL_CTX_new(SSLv23_client_method());

    ssl = SSL_new(ctx);
    err = SSL_set_fd(ssl, sid);
    SSL_connect(ssl);


    
    fd = open(info_url.file_name, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    if(fd < 0){
        perror("Error:open()");
        exit(-1);
    }

    sprintf(s_buf, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", info_url.resource_name, info_url.host_name);
    //printf("%s", s_buf);
    nums = SSL_write(ssl, s_buf, strlen(s_buf));
    if(nums < 0){
        perror("Error:send()");
        exit(-1);
    }

    SSL_lineread(ssl, r_buf);
    //printf("%s\n", r_buf);
    sscanf(r_buf, "%*s %d %*s", &code);
    printf("%d\n", code);

    while(1){
        char buf[1000];
        SSL_lineread(ssl, buf);
        if(strcmp(buf, "\r\n") == 0)break;
    }

    while(1){
        numr = SSL_read(ssl, buf, sizeof(buf));
        if(numr < 0){
            perror("Error: recv()");
            exit(-1);
        }
        if(numr == 0){break;}
        
        num = write(fd, buf, numr);
        if(num < 0){
            perror("Error:write()");
            exit(-1);
        }

    }
    close(fd);

}

//URL
void url_connecter(struct INFO_URL info_url){
    int sid;
    struct hostent *host;
    struct sockaddr_in addr;
    

    host = gethostbyname(info_url.host_name);
    if(host == NULL){
        printf("gethostbyname() failed\n");
        exit(-1);
    }

    if((sid = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Error:socket()");
        exit(-1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(info_url.no_port);
    addr.sin_addr = (*(struct in_addr *)host->h_addr_list[0]);

    if(connect(sid, (struct sockaddr *)&addr, sizeof(addr)) != 0){
        perror("error:connect()");
        exit(-1);
    }


    if(info_url.no_port == 80){
        http_exchanger(sid, info_url);
    }else if(info_url.no_port == 443){
        https_exchanger(sid, info_url);
    }else{printf("違うかもね%d\n", info_url.no_port);}

    close(sid);

}

void url_data_send(int sid, struct INFO_URL info_url){
    int fd, numr, d_cnt = 0;
    char read_buf[1024], send_buf[1024];
    ssize_t nums;
    //機能注意、無理かも
    fd = open(info_url.file_name, O_RDONLY, 0666);
    if(fd < 0){
        perror("Error:open(URL_DATA_SEND)");
        exit(-1);
    }

    

    
    while(1){
        numr = read(fd, read_buf, sizeof(read_buf));
        if(numr == -1){
            perror("Error:read()");
            exit(-1);
        }
        if(numr == 0){
            break;
        }

        nums = send(sid, read_buf, numr, 0);
        if(nums < 0){
            perror("ERROR:send(URL_DATA_send)");
            exit(-1);
        }

        d_cnt += numr;

    }

    printf("read data %d\n", d_cnt);

}




void command(int sid, char *buf){
    char send_buf[10240], recv_buf[10240], cbuf[10240];
    ssize_t nums, numr;
    struct INFO_URL info_url = {};
    strcpy(cbuf, buf);
    
    
    
    

    if(strncmp(cbuf, "!HTTP", 5) == 0){
        sprintf(send_buf, "Enable HTTP Mode\n");
        nums = send(sid, send_buf, sizeof(send_buf), 0);
        if(nums == -1){perror("ERROR:send(com1)");exit(-1);}

        
        numr = recv(sid, recv_buf, sizeof(recv_buf), 0);
        if(numr == -1){perror("error:recv(com2)");exit(-1);}
        if(strncmp(recv_buf, "http", 4) == 0){
            printf("OK\n");
            info_url = url_editer(recv_buf);

            url_connecter(info_url);

            //sprintf(send_buf, "URLのデータ送信を開始します.\n");
            //nums = send(sid, send_buf, sizeof(send_buf), 0);
            //if(nums == -1){perror("ERROR:send(com3)");exit(-1);}
            url_data_send(sid, info_url);

        }else{
            return;
        }
        
        

        

    }else if(strncmp(buf, "!help", 5) == 0){
        
        sprintf(send_buf, "!HTTP !QUIT  !help.\n");
        nums = send(sid, send_buf, sizeof(send_buf), 0);
        if(nums == -1){perror("ERROR:send(com3)");exit(-1);}
    }else if(strncmp(buf, "!QUIT", 5) == 0){
        sprintf(send_buf, "disconnect\n");
        nums = send(sid, send_buf, sizeof(send_buf), 0);
        if(nums == -1){perror("ERROR:send(com3)");exit(-1);}
        close(sid);exit(-1);
    }else{
        sprintf(send_buf, "!helpでコマンドを確認\n");
        nums = send(sid, send_buf, sizeof(send_buf), 0);
        if(nums == -1){perror("ERROR:send(L177)");exit(-1);}
    }


}


int main(int argc, char *argv[]){
    int sid, newsid, maxfd, retval, cnt = 0;
    int port = atoi(argv[1]);
    char in_passwordbuf[1024], in_idbuf[1024], decipherment[512];
    ssize_t numr, nums;
    fd_set rdfs;
    struct sockaddr_in addr;
	struct sockaddr_in client_addr;

    char sbuf[10240], hako1[128];
    int hako2 = 0;


    socklen_t client_len = sizeof(client_addr);
	
	if((sid = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Error:socket()");
		exit(-1);		
	}

    memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if(bind(sid, (struct sockaddr *)&addr, sizeof(addr)) != 0){
		perror("Error:bind()");exit(-1);
	}
	printf("Server started: port(%d)\n", port);


    if(listen(sid, 10) < 0){
		perror("Error:listen()");exit(-1);
	}

    if((newsid = accept(sid, (struct sockaddr *)&client_addr, &client_len)) < 0){
		perror("Error:accept()");exit(-1);
	}

    
    
    while(1){
        FD_ZERO(&rdfs);
        FD_SET(0, &rdfs);
        FD_SET(newsid, &rdfs);
        maxfd = newsid;
        retval = select(maxfd + 1, &rdfs, NULL, NULL, NULL);
        if(retval < 0){
            perror("select()");
            exit(-1);
        }

        if(retval > 0){
            if(FD_ISSET(newsid, &rdfs)){
                char rbuf[1000];
                ssize_t numr;
                

                

                if(hako2 == 0){

                    sprintf(sbuf, "Connect\n");
                    nums = send(newsid, sbuf, strlen(sbuf), 0);
                    if(nums == -1){perror("Error:recv()");exit(-1);}
                    numr = recv(newsid, decipherment, sizeof(decipherment), 0);
                    if(numr == -1){perror("Error:recv()");exit(-1);}

                    while(1){        
                        
                        sprintf(sbuf, "新規ユーザーの場合, CREATE, 登録済みの場合は, NOを入力してください\n");
                        nums = send(newsid, sbuf, strlen(sbuf), 0);
                        if(nums == -1){perror("Error:recv()");exit(-1);}

                        memset(decipherment, '\0', sizeof(decipherment));
                        numr = recv(newsid, decipherment, sizeof(decipherment), 0);
                        if(numr == -1){perror("Error:recv()");exit(-1);}
            
            
                        if(strcmp(decipherment, "NO") == 0){
                            printf("PASS\n");
                            break;
                        }else if(strcmp(decipherment, "CREATE") == 0) {
                            printf("creating user\n");
                            create_user(newsid);
                            break;
                        }
                        sprintf(sbuf, "無効な操作\n");
                        nums = send(newsid, sbuf, strlen(sbuf), 0);
                        if(nums == -1){perror("Error:recv()");exit(-1);}
            
        
                    }

                    while(1){
                        

                        sprintf(sbuf, "IDを入力してください。\n");
                        nums = send(newsid, sbuf, strlen(sbuf), 0);
                        if(nums == -1){perror("Error:recv()");exit(-1);}

                        numr = recv(newsid, rbuf, sizeof(rbuf), 0);
                        if(numr < 0){
                        perror("Error: recv()");
                        exit(-1);
                        }
                        rbuf[numr] = '\0';
                        strcpy(in_idbuf, rbuf);

                        sprintf(sbuf, "これでいいです？%s いいならDONEを入力してください\n", in_idbuf);
                        nums = send(newsid, sbuf, strlen(sbuf), 0);
                        if(nums == -1){perror("Error:send()");exit(-1);}

                        numr = recv(newsid, hako1, sizeof(hako1), 0);
                        if(numr < 0){
                            perror("Error: recv()");
                            exit(-1);
                        }
                        if(strcmp(hako1, "DONE") == 0){
                            printf("ok\n");
                            break;
                        }

                    }

                    while(1){        
                        sprintf(sbuf, "passwordを入力してください。\n");
                        nums = send(newsid, sbuf, strlen(sbuf), 0);
                        if(nums == -1){perror("Error:recv()");exit(-1);}

                        memset(rbuf, '\0', sizeof(rbuf));
                        numr = recv(newsid, rbuf, sizeof(rbuf), 0);
                        if(numr < 0){
                            perror("Error: recv()");
                            exit(-1);
                        }
                        rbuf[numr] = '\0';
                        strcpy(in_passwordbuf, rbuf);
                        
                        

                        sprintf(sbuf, "これでいいです？%s でいいならDONEを入力してください\n", in_passwordbuf);
                        nums = send(newsid, sbuf, strlen(sbuf), 0);
                        if(nums == -1){perror("Error:send-L253()");exit(-1);}
            
                        memset(hako1, '\0', sizeof(hako1));
                        numr = recv(newsid, hako1, sizeof(hako1), 0);
                        if(numr < 0){
                            perror("Error: recv()");
                            exit(-1);
                        }
                        if(strcmp(hako1, "DONE") == 0){       
                            hako2 = idchecker(newsid, in_idbuf, in_passwordbuf);                
                        }
                        if(hako2 == 1){
                            break;
                        }
                    }

                }
                
    
        

    
    





                if(cnt == 0){
                    sprintf(sbuf, "'!'　を先頭に入れることでコマンドが使用できます。\n");
                    nums = send(newsid, sbuf, strlen(sbuf), 0);
                    if(nums == -1){perror("Error:recv()");exit(-1);}
                    
                }


                memset(rbuf, '\0', sizeof(rbuf));
                numr = recv(newsid, rbuf, sizeof(rbuf)-1, 0);
                if(numr == -1){perror("ERROR:recv() L359");exit(-1);}
                if(numr == 0){
                    printf("接続先から切断されていますL693\n");
                    printf("終了します。");
                    close(sid);
                    exit(-1);
                    //cl   ose(fd);
                }


                rbuf[numr] = '\0';

                printf("%s\n", rbuf);


                if(rbuf[0] == '!'){
                    printf("go command\n");
                    command(newsid, rbuf);
                }
                cnt = 1;
            }
            if(FD_ISSET(0, &rdfs)){
                char buf[100];
                char allbufw[1024];
                scanf("%s", buf);
                
               
                
                sprintf(allbufw, "%s", buf);
                nums = send(newsid, allbufw, strlen(allbufw), 0);
                if(nums == -1){perror("Error:send()");exit(-1);}
            }

        }

    }

    close(sid);
    
    return 0;
}