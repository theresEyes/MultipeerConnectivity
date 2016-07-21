//
//  ChatViewController.m
//  MCDemo
//
//  Created by zhangqi on 18/7/2016.
//  Copyright © 2016 zhangqi. All rights reserved.
//

#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold

#import "ChatViewController.h"
#import "MCManager.h"
#import "PIDGenerater.h"
#import "FastSocket.h"
@interface ChatViewController()<UITextFieldDelegate>
@property (nonatomic,strong) NSString *mc_ipaddress;
@end

@implementation ChatViewController

- (NSString *)mc_ipaddress
{
    if (!_mc_ipaddress) {
        CPIDGenerater::GetInstance()->refreshAddresses();
        _mc_ipaddress = [NSString stringWithUTF8String:CPIDGenerater::GetInstance()->Get_cm_ipaddress()];
    }
    return _mc_ipaddress;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    _txtMessage.delegate = self;
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(didReceiveDataWithNotification:)
                                                 name:@"MCDidReceiveDataNotification"
                                               object:nil];
}


- (IBAction)sendMessage:(id)sender {
    [self sendMyMessage];
}

- (IBAction)cancelMessage:(id)sender {
    [_txtMessage resignFirstResponder];
}

- (IBAction)onpressedButtonSocketSendMessage:(id)sender {
    // open a connection
//    SKTCPSocket *socket = []
    
    startServer();
    
    
}

- (IBAction)onpressedButtonStartClient:(id)sender {
    startClient();
}


int startServer()
{
    int ret = 0;
    struct sockaddr_in server_addr;
    server_addr.sin_len = sizeof(struct sockaddr_in);
    server_addr.sin_family = AF_INET;//Address families AF_INET互联网地址簇
    server_addr.sin_port = htons(12345);
    server_addr.sin_addr.s_addr =  INADDR_ANY;
    bzero(&(server_addr.sin_zero),8);
    
    //创建socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);//SOCK_STREAM 有连接
    if (server_socket == -1) {
        perror("socket error");
        return 1;
    }else{
        printf("socket success\n");
    }
    
    //绑定socket：将创建的socket绑定到本地的IP地址和端口，此socket是半相关的，只是负责侦听客户端的连接请求，并不能用于和客户端通信
    ret = ::bind(server_socket,(struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret != 0) {
        printf("bind error, %s\n", strerror(errno));
        return 1;
    }else{
        printf("bind success\n");
    }
    
    //listen侦听 第一个参数是套接字，第二个参数为等待接受的连接的队列的大小，在connect请求过来的时候,完成三次握手后先将连接放到这个队列中，直到被accept处理。如果这个队列满了，且有新的连接的时候，对方可能会收到出错信息。
    if (listen(server_socket, 5) == -1) {
        printf("listen error, %s\n", strerror(errno));
        return 1;
    }else{
        printf("listen success\n");
    }
    
    struct sockaddr_in client_address;
    socklen_t address_len;
    int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &address_len);
    //返回的client_socket为一个全相关的socket，其中包含client的地址和端口信息，通过client_socket可以和客户端进行通信。
    if (client_socket == -1) {
        printf("accept error, %s\n", strerror(errno));
        return 1;
    }else{
        printf("accept success\n");
    }
    
    char recv_msg[1024];
    char reply_msg[1024] = "abcd";
    
    while (1) {
        bzero(recv_msg, 1024);
        bzero(reply_msg, 1024);
        
        send(client_socket, reply_msg, 1024, 0);
        
        long byte_num = recv(client_socket,recv_msg,1024,0);
        recv_msg[byte_num] = '\0';
        printf("client said:%s\n",recv_msg);
        sleep(1);
        
    }
    
    return 0;
}


int startClient()
{
    struct sockaddr_in server_addr;
    server_addr.sin_len = sizeof(struct sockaddr_in);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    server_addr.sin_addr.s_addr = inet_addr("10.0.221.23");
    bzero(&(server_addr.sin_zero),8);
    
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        printf("socket error\n");
        return 1;
    }
    char recv_msg[1024];
    char reply_msg[1024] = "xxxxxyyyy";
    
    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in))==0)     {
        //connect 成功之后，其实系统将你创建的socket绑定到一个系统分配的端口上，且其为全相关，包含服务器端的信息，可以用来和服务器端进行通信。
        printf("connected to server\n");
        while (1) {
            bzero(recv_msg, 1024);
            bzero(reply_msg, 1024);
            long byte_num = recv(server_socket,recv_msg,1024,0);
            recv_msg[byte_num] = '\0';
            printf("server said:%s\n",recv_msg);
            
            printf("send message:%s\n", reply_msg);
            if (send(server_socket, reply_msg, 1024, 0) == -1) {
                printf("send error:%s\n", strerror((errno)));
            }
            
            sleep(1);
        }
        
    }else{
        printf("connect to server failed\n");
    }
    
    // insert code here...
    printf("Hello, World!\n");
    return 0;
}


#pragma mark - UITextField Delegate method implementation
-(BOOL)textFieldShouldReturn:(UITextField *)textField{
    [self sendMyMessage];
    return YES;
}

#pragma mark - Private method implementation

-(void)sendMyMessage{
    
    NSString *messageContent = [NSString stringWithFormat:@"%@-%@",self.mc_ipaddress,_txtMessage.text];
    
    NSData *dataToSend = [messageContent dataUsingEncoding:NSUTF8StringEncoding];
    NSArray *allPeers = [MCManager getInstance].session.connectedPeers;
    NSError *error;
    
    [[MCManager getInstance].session sendData:dataToSend
                                     toPeers:allPeers
                                    withMode:MCSessionSendDataReliable
                                       error:&error];
    
    if (error) {
        NSLog(@"%@", [error localizedDescription]);
    }
    
    [_tvChat setText:[_tvChat.text stringByAppendingString:[NSString stringWithFormat:@"I wrote:\n%@\n\n", _txtMessage.text]]];
    [_txtMessage setText:@""];
    [_txtMessage resignFirstResponder];
    
    
    
    
    
}


-(void)didReceiveDataWithNotification:(NSNotification *)notification{
    MCPeerID *peerID = [[notification userInfo] objectForKey:@"peerID"];
    NSString *peerDisplayName = peerID.displayName;
    
    NSData *receivedData = [[notification userInfo] objectForKey:@"data"];
    NSString *receivedText = [[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding];
    NSArray *array = [receivedText componentsSeparatedByString:@"-"];
    NSString *message = array[1];
    NSString *ipaddress = array[0];
    NSLog(@"qizhang----ip--->%@",ipaddress);
    
    [_tvChat performSelectorOnMainThread:@selector(setText:) withObject:[_tvChat.text stringByAppendingString:[NSString stringWithFormat:@"%@ wrote:\n%@\n\n", peerDisplayName, message]] waitUntilDone:NO];
}
@end
