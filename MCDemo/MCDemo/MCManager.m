//
//  MCManager.m
//  MCDemo
//
//  Created by zhangqi on 18/7/2016.
//  Copyright © 2016 zhangqi. All rights reserved.
//

#import "MCManager.h"

@implementation MCManager

- (id)init
{
    self = [super init];
    if (self) {
        _peerID = nil;
        _session = nil;
        _browser = nil;
        _advertiser = nil;
    }
    return self;
}

#pragma mark - Public method implementation
-(void)setupPeerAndSessionWithDisplayName:(NSString *)displayName
{
    _peerID = [[MCPeerID alloc] initWithDisplayName:displayName];
    _session = [[MCSession alloc] initWithPeer:_peerID];
    _session.delegate = self;
}

-(void)setupMCBrowser
{
    _browser = [[MCBrowserViewController alloc] initWithServiceType:@"chat-files" session:_session];
}

-(void)advertiseSelf:(BOOL)shouldAdvertise
{
    if (shouldAdvertise) {
        _advertiser = [[MCAdvertiserAssistant alloc] initWithServiceType:@"chat-files"
                                                           discoveryInfo:nil
                                                                 session:_session];
        [_advertiser start];
    }
    else{
        [_advertiser stop];
        _advertiser = nil;
    }

}

#pragma mark - MCSession Delegate method implementation
// Remote peer changed state.
- (void)session:(MCSession *)session peer:(MCPeerID *)peerID didChangeState:(MCSessionState)state
{
    NSDictionary *dict = @{@"peerID": peerID,
                           @"state" : [NSNumber numberWithInt:state]
                           };
    
    [[NSNotificationCenter defaultCenter] postNotificationName:@"MCDidChangeStateNotification"
                                                        object:nil
                                                      userInfo:dict];
}

// Received data from remote peer.
- (void)session:(MCSession *)session didReceiveData:(NSData *)data fromPeer:(MCPeerID *)peerID
{
    NSDictionary *dict = @{@"data": data,
                           @"peerID": peerID
                           };
    
    [[NSNotificationCenter defaultCenter] postNotificationName:@"MCDidReceiveDataNotification"
                                                        object:nil
                                                      userInfo:dict];
}

// Received a byte stream from remote peer.
- (void)    session:(MCSession *)session
   didReceiveStream:(NSInputStream *)stream
           withName:(NSString *)streamName
           fromPeer:(MCPeerID *)peerID
{

}

// Start receiving a resource from remote peer.
- (void)                    session:(MCSession *)session
  didStartReceivingResourceWithName:(NSString *)resourceName
                           fromPeer:(MCPeerID *)peerID
                       withProgress:(NSProgress *)progress
{
    NSDictionary *dict = @{@"resourceName"  :   resourceName,
                           @"peerID"        :   peerID,
                           @"progress"      :   progress
                           };
    
    [[NSNotificationCenter defaultCenter] postNotificationName:@"MCDidStartReceivingResourceNotification"
                                                        object:nil
                                                      userInfo:dict];
    
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [progress addObserver:self
                   forKeyPath:@"fractionCompleted"
                      options:NSKeyValueObservingOptionNew
                      context:nil];
    });

}

// Finished receiving a resource from remote peer and saved the content
// in a temporary location - the app is responsible for moving the file
// to a permanent location within its sandbox.
- (void)                    session:(MCSession *)session
 didFinishReceivingResourceWithName:(NSString *)resourceName
                           fromPeer:(MCPeerID *)peerID
                              atURL:(NSURL *)localURL
                          withError:(nullable NSError *)error
{
    NSDictionary *dict = @{@"resourceName"  :   resourceName,
                           @"peerID"        :   peerID,
                           @"localURL"      :   localURL
                           };
    
    [[NSNotificationCenter defaultCenter] postNotificationName:@"didFinishReceivingResourceNotification"
                                                        object:nil
                                                      userInfo:dict];
}

-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context{
    [[NSNotificationCenter defaultCenter] postNotificationName:@"MCReceivingProgressNotification"
                                                        object:nil
                                                      userInfo:@{@"progress": (NSProgress *)object}];
}


@end
