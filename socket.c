/* socket.c */

#include "common.h"
#include "socket.h"

static uint8 socket_recv_buf[MAXDATASIZE] = {0};
static uint8 socket_send_buf[MAXDATASIZE] = {0};
static uint8 socket_header[2] = {SOCKET_HEAD_0xFE,  SOCKET_HEAD_0xFE};

static pthread_mutex_t socket_status_mutex	= PTHREAD_MUTEX_INITIALIZER;

extern   int ugw_get_same_type_devs(uint8 dev);
extern void Risco_alarm_to_cloud(char alarm_value1,char alarm_value2,char* attrvalue);//MEI2115.12.9
extern void USDK_unpacket(void *data, uint8 len);
extern void iAudio_unpack(void* msg,int msg_len);
static void scocket_once_send(void *msg, int len);

static void socket_unpackage(void *data, int len){
	MSG socket_msg ={0} ;
	uint8 buf[MAXDATASIZE] = {0};
    Socket_data Socket = {{0},};
	int buf_len = len;
	memcpy(&buf , data, buf_len);

	if((buf[24] == 1)||(buf[24] == 0))
		memcpy(&Socket , buf, buf_len);

    switch(Socket.devType){
		case DEV_TYPE_UGW:                                
            socket_msg.type = MSG_USDK;
            buf[1] = buf_len ;
                            
            if(Socket.devAttr[0] < 0x02){
                printf("socket wifi cmd err!\n");
            }
            //必须改成函数调用方式了
            memcpy(&socket_msg.msg, buf, buf_len);
            USDK_unpacket(&socket_msg.msg,buf_len);
			break;				

        case DEV_TYPE_IAUDIO:
            socket_msg.type = MSG_USDK;
            buf[1] = buf_len ;
                            
            if(Socket.devAttr[0] < 0x02){
                printf("socket iAudio devAttr error!\n");
            }
            memcpy(&socket_msg.msg, buf, buf_len);
            iAudio_unpack(&socket_msg.msg,buf_len);

			break;				

		default: // no dev
				printf("Socket no devType conform \n");
			break;
	}
}

/*socket_recv_thread*/
void socket_recv_pthread(void){
	struct sockaddr_in server_sockaddr,client_sockaddr;
	int recvbytes;
    socklen_t sin_size;
	fd_set readfd;
	int sockfd,client_fd;
    sockfd=0;
    client_fd=0;
	int reuse=1;
	uint8 buf[MAXDATASIZE] = {0};
	
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))== -1){
		perror("recv:socket ");
		return;
	}
	printf("recv:socket success!,sockfd=%d\n",sockfd);
	server_sockaddr.sin_family=AF_INET;
	server_sockaddr.sin_port=htons(SERVPORT);
	//server_sockaddr.sin_addr.s_addr=INADDR_ANY;
	server_sockaddr.sin_addr.s_addr=inet_addr(SOCKET_ADDR);
	bzero(&(server_sockaddr.sin_zero),8);
	
	//reuse port
	
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0){
        perror("recv:setsockopet error\n");
        return;
    }
        
	if(bind(sockfd,(struct sockaddr *)&server_sockaddr,sizeof(struct sockaddr))== -1){
		perror("recv:bind ");
		return;
	}
	printf("recv: bind success!\n");
	if(listen(sockfd,BACKLOG)== -1){
		perror("recv:listen ");
		return;
	}
	printf("recv:listening ....\n");
	/*set socket as a file descriptor*/
	FD_ZERO(&readfd);
	FD_SET(sockfd,&readfd);
	
	while(1){
		
		sin_size=sizeof(struct sockaddr_in);

		/*select*/
		if(select(sockfd+1,&readfd,NULL,NULL,(struct timeval *)0)>0){
	
			if(FD_ISSET(sockfd,&readfd)>0){
		
				if((client_fd=accept(sockfd,(struct sockaddr *)&client_sockaddr,&sin_size))== -1){
				    perror("recv:accept ");
					return;
				}
					
				memset(buf, 0, MAXDATASIZE);
				if((recvbytes=recv(client_fd,buf,MAXDATASIZE,0))== -1){
					perror("recv: recv");
					return;
				}
				
                printf("===== 9769:recvbytes=%d, buf: ", recvbytes);
                for(int i=0; i < recvbytes; i++){
                    printf("0x%x, ", buf[i]);		
                }
                printf("\n");

				#ifdef LOG_FILE
				{
				   	FILE *fp;
					time_t timep;
					time(&timep);
					int i;
					if((fp=fopen(LOG_FILE_PATH, "a+")) == NULL){
						printf("open LOG_FILE_PATH failed!\n ");
					}

					fprintf(fp, "%d:====9769=:recvbytes=%d,buf: ", (int)timep, recvbytes);
					for(i=0; i < recvbytes; i++)
						fprintf(fp, "0x%x,", buf[i]);		
					fprintf(fp,"\n");
					
					fclose(fp);
				}
				#endif
				
				if((memcmp(buf,socket_header,2)==0)&&(buf[recvbytes-1] == SOCKET_END_0xFD)){
					memset(socket_recv_buf, 0, MAXDATASIZE);
					memcpy(socket_recv_buf, buf, recvbytes);
					socket_unpackage(&socket_recv_buf, recvbytes);
				}
				else{
					printf("Socket recvbytes err:header or end\n");
				}
			}/*if*/
			close(client_fd);
		}/*select*/
		sleep(1);
	}
	pthread_exit(0);
}

static void scocket_once_send(void *msg, int len){

	int sockfd,sendbytes;
	struct hostent *host;
	struct sockaddr_in serv_addr;
	//int reuse=1;
	{
        int i;
		printf("scocket_once_send: \n");
		for(i=0; i < len; i++)
			printf("0x%x, ", socket_send_buf[i]);			
		    printf("\n");
	}

	#ifdef LOG_FILE
	{
	   	FILE *fp;
		time_t timep;
		time(&timep);
		int i;
		if((fp=fopen(LOG_FILE_PATH, "a+")) == NULL){
			printf("open LOG_FILE_PATH failed!\n ");
		}

		fprintf(fp, "%d:scocket_once_send: ",(int)timep);
		for(i=0; i < len; i++)
			fprintf(fp, "0x%x,", socket_send_buf[i]);		
		fprintf(fp,"\n");
		
		fclose(fp);
	}
	#endif
		/*Address Resolution, get server addr*/
		if((host=gethostbyname(SOCKET_ADDR))==NULL){
			perror("send:gethostbyname ");
			return;
		}

		/*socket*/
		if((sockfd=socket(AF_INET,SOCK_STREAM,0))== -1){
			perror("send: socket");
			return;
		}
		/*setting sockaddr_in*/
		serv_addr.sin_family=AF_INET;
		serv_addr.sin_port=htons(CLIENTPORT);
		serv_addr.sin_addr=*((struct in_addr *)host->h_addr);
		bzero(&(serv_addr.sin_zero),8);
		//reuse port
		/*
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
	       	{
	                perror("recv:setsockopet error\n");
	                return;
	        }
	        */

		/*connect to server */
		if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr))== -1){
			perror("send:connect ");
			return;
		}

			/*send message to server*/
		if((sendbytes=send(sockfd, msg, len, 0))== -1){
			perror("send: send");
			return;
		}
								
		close(sockfd);

}

/*socket_send_thread*/
void socket_send2(void* buf,int len){
	MSG *socket_msg=(MSG*)buf;
	printf("socket_msg.type =%ld \n" ,socket_msg->type);

    #if 0 //def LOG_FILE
    {
        FILE *fp;
        time_t timep;
        time(&timep);
        int i;
        if((fp=fopen(LOG_FILE_PATH, "a+")) == NULL){
            printf("open LOG_FILE_PATH failed!\n ");
        }

        fprintf(fp, "%d:socket_msg.type =%ld\n ", timep, socket_msg.type);
        
        fclose(fp);
    }
    #endif
			
    pthread_mutex_lock(&socket_status_mutex);

    switch(socket_msg->type){
        case MSG_SOCKET:
        {
            memcpy(socket_send_buf, socket_msg->msg, len);
            socket_send_buf[0] = SOCKET_HEAD_0xFE;
            socket_send_buf[1] = SOCKET_HEAD_0xFE;
            socket_send_buf[2] = 0x01;
            socket_send_buf[len-1] = SOCKET_END_0xFD;

             scocket_once_send( socket_send_buf, len);
        }
             break;
             
        case MSG_USDK:
        {
            uint8 len = socket_msg->msg[1];
            
            memcpy(socket_send_buf, socket_msg->msg, len);
            socket_send_buf[0] = SOCKET_HEAD_0xFE;
            socket_send_buf[1] = SOCKET_HEAD_0xFE;
            socket_send_buf[4] = DEV_TYPE_UGW; //devtype
            socket_send_buf[2] = 0x01;

            socket_send_buf[len-1] = SOCKET_END_0xFD;
            
            scocket_once_send( socket_send_buf, len);
        }
            break;
        default:
            break;
		}

		pthread_mutex_unlock(&socket_status_mutex);

		sleep(1);

		pthread_exit(0);
}
