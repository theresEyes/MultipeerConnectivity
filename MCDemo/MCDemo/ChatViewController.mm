//
//  ChatViewController.m
//  MCDemo
//
//  Created by zhangqi on 18/7/2016.
//  Copyright © 2016 zhangqi. All rights reserved.
//

#import "ChatViewController.h"
#import "MCManager.h"
#import "PIDGenerater.h"
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
