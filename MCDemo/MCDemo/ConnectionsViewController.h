//
//  ConnectionsViewController.h
//  MCDemo
//
//  Created by zhangqi on 18/7/2016.
//  Copyright © 2016 zhangqi. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ConnectionsViewController : UIViewController
@property (weak, nonatomic) IBOutlet UISwitch *swVisible;
@property (weak, nonatomic) IBOutlet UITextField *txtName;
@property (weak, nonatomic) IBOutlet UIButton *btnDisconnect;

@end
