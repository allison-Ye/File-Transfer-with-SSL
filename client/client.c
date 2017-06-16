#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include <sys/stat.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>

#define port 3333

char ipaddr[15];
int sockfd;
struct sockaddr_in sockaddr;
void linkS()
{
	//����socket
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		perror("socket");
		_exit(0);
	}
	//����
	memset(&sockaddr,0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.s_addr = inet_addr(ipaddr);
	if(connect(sockfd,(struct sockaddr *)&sockaddr,sizeof(sockaddr)) == -1)
	{
		perror("connect");
		_exit(0);
	}
}

void upload_file(char *filename)
{
	int fd;
	char cmd = 'U';
	int FileNameSize = strlen(filename);
	char buf[1024];
	int count=0;
	struct stat fstat;
	
	//���ļ�
	fd = open(filename,O_RDONLY);
	//��������
	write(sockfd,&cmd,1);
	
	//�����ļ���
	write(sockfd,(void *)&FileNameSize,4);
	write(sockfd,filename, FileNameSize);
	//�����ļ�����
	if((stat(filename,&fstat)) == -1)
		return;
	write(sockfd,(void *)&fstat.st_size,4);
	
	//�����ļ�����
	while((count = read(fd,(void *)buf,1024)) > 0)
	{
		write(sockfd,buf,count);	
	}
	//�ر��ļ�
	close(fd);
}

void download_file(char *filename)
{
	int fd;
	char cmd = 'D';
	char buf[1024];
	int FileNameSize = strlen(filename);
	int filesize=0,count=0,totalrecv=0;
	
	//��������
	write(sockfd,&cmd,1);
	
	//�����ļ���
	write(sockfd,(void *)&FileNameSize,4);
	write(sockfd,filename,FileNameSize);
	
	//�򿪲������ļ�
	if((fd = open(filename,O_RDWR|O_CREAT)) == -1)
	{
		perror("open:");
		_exit(0);	
	}
	
	//��������
	read(sockfd,&filesize,4);
	while((count = read(sockfd,(void *)buf,1024)) > 0)
	{
		write(fd,buf,count);
		totalrecv += count;
		if(totalrecv == filesize)
			break;	
	}
	
	//�ر��ļ�
	close(fd);
}

void quit()
{
	char cmd = 'Q';
	//��������
	write(sockfd,(void *)&cmd,1);
	//����
	system("clear");
	//�˳�
	_exit(0);
}
void menu()
{
	char cmd;
	char c;
	char file_u[30];
	char file_d[30];
	while(1)
	{
		printf("\n------------------------------  1.Upload Files  ------------------------------\n");
		printf("------------------------------  2.Download Files  ------------------------------\n");
		printf("------------------------------      3.Exit   ------------------------------------\n");
		printf("Please input the Client command:");
		cmd = getchar();	
		
		switch(cmd)
		{
			case '1':
			{
				printf("Upload Files:");
				//�����ļ���
				while((c = getchar()) != '\n' && c != EOF);
				fgets(file_u, 30, stdin);
				file_u[strlen(file_u)-1] = '\0';
				//�ϴ��ļ�
				upload_file(file_u);
			}
			break;	
			case '2':
			{
				printf("Download Files:");
				//�����ļ���
				while((c = getchar()) != '\n' && c != EOF);
				fgets(file_d, 30, stdin);
				file_d[strlen(file_d)-1] = '\0';
				//�����ļ�
				download_file(file_d);
			}
			break;	
			case '3':
			{
				//�˳�
				quit();
				break;
			}
			break;	
			default:
			{
				printf("Please input right command!");	
			}
			break;	
		}
	}	
}
int main(int argc, char *args[])
{
	if(argc != 2)
	{
		printf("format error: you mast enter ipaddr like this : client 192.168.0.6\n");
	    	_exit(0);	
	}
	strcpy(ipaddr,args[1]);
	//��������
	linkS();
	//��ӡ�˵�
	menu();
	//��β����
	close(sockfd);
	return 0;	
}