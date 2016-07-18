//
//  ConnectionsViewController.m
//  MCDemo
//
//  Created by zhangqi on 18/7/2016.
//  Copyright © 2016 zhangqi. All rights reserved.
//

#import "ConnectionsViewController.h"
#import "MCManager.h"
@interface ConnectionsViewController()<MCBrowserViewControllerDelegate,UITableViewDataSource,UITableViewDelegate,UITextFieldDelegate>
@property (nonatomic, strong) NSMutableArray *arrConnectedDevices;
@property (weak, nonatomic) IBOutlet UITableView *tblConnectedDevices;

@end

@implementation ConnectionsViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    [[MCManager getInstance] setupPeerAndSessionWithDisplayName:[UIDevice currentDevice].name];
    [[MCManager getInstance] advertiseSelf:_swVisible.isOn];
    
    [_txtName setDelegate:self];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(peerDidChangeStateWithNotification:)
                                                 name:@"MCDidChangeStateNotification"
                                               object:nil];
    
    _arrConnectedDevices = [[NSMutableArray alloc] init];
    
    [_tblConnectedDevices setDelegate:self];
    [_tblConnectedDevices setDataSource:self];

}

#pragma mark - UITextField Delegate method implementation

-(BOOL)textFieldShouldReturn:(UITextField *)textField{
    [_txtName resignFirstResponder];
    
    [MCManager getInstance] .peerID = nil;
    [MCManager getInstance] .session = nil;
    [MCManager getInstance] .browser = nil;
    
    if ([_swVisible isOn]) {
        [[MCManager getInstance].advertiser stop];
    }
    [MCManager getInstance] .advertiser = nil;
    
    [[MCManager getInstance]  setupPeerAndSessionWithDisplayName:_txtName.text];
    [[MCManager getInstance] setupMCBrowser];
    [[MCManager getInstance]  advertiseSelf:_swVisible.isOn];
    
    return YES;
}


- (IBAction)browseForDevices:(id)sender {
    [[MCManager getInstance]  setupMCBrowser];
    [[[MCManager getInstance]  browser] setDelegate:self];
    [self presentViewController:[[MCManager getInstance]  browser] animated:YES completion:nil];
}

- (IBAction)toggleVisibility:(id)sender {
    [[MCManager getInstance]  advertiseSelf:_swVisible.isOn];
}

- (IBAction)disconnect:(id)sender {
    [[MCManager getInstance] .session disconnect];
    
    _txtName.enabled = YES;
    
    [_arrConnectedDevices removeAllObjects];
    [_tblConnectedDevices reloadData];
}


#pragma mark - MCBrowserViewControllerDelegate method implementation

-(void)browserViewControllerDidFinish:(MCBrowserViewController *)browserViewController{
    [[MCManager getInstance] .browser dismissViewControllerAnimated:YES completion:nil];
}

-(void)browserViewControllerWasCancelled:(MCBrowserViewController *)browserViewController{
    [[MCManager getInstance] .browser dismissViewControllerAnimated:YES completion:nil];
}

#pragma mark - Private method implementation

-(void)peerDidChangeStateWithNotification:(NSNotification *)notification{
    MCPeerID *peerID = [[notification userInfo] objectForKey:@"peerID"];
    NSString *peerDisplayName = peerID.displayName;
    MCSessionState state = [[[notification userInfo] objectForKey:@"state"] intValue];
    
    if (state != MCSessionStateConnecting) {
        if (state == MCSessionStateConnected) {
            [_arrConnectedDevices addObject:peerDisplayName];
        }
        else if (state == MCSessionStateNotConnected){
            if ([_arrConnectedDevices count] > 0) {
                int indexOfPeer = [_arrConnectedDevices indexOfObject:peerDisplayName];
                [_arrConnectedDevices removeObjectAtIndex:indexOfPeer];
            }
        }
        [_tblConnectedDevices reloadData];
        
        BOOL peersExist = ([[[MCManager getInstance] .session connectedPeers] count] == 0);
        [_btnDisconnect setEnabled:!peersExist];
        [_txtName setEnabled:peersExist];
    }
}

#pragma mark - UITableView Delegate and Datasource method implementation
-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView{
    return 1;
}

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return [_arrConnectedDevices count];
}

-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"CellIdentifier"];
    
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"CellIdentifier"];
    }
    
    cell.textLabel.text = [_arrConnectedDevices objectAtIndex:indexPath.row];
    
    return cell;
}

-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 60.0;
}
@end
