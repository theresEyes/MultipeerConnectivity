//
//  PIDGenerater.h
//  TVUTransport
//
//  Created by shawnsong on 11-12-12.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//

#ifndef TVUTransport_PIDGenerater_h
#define TVUTransport_PIDGenerater_h
#include "datatype.h"
#define DEFAULT_LEVEL1 0xb814f4ad1eabee1dULL


class CPIDGenerater
{
public:
    uint64  GetLocalPeerID(bool &bgenerate);
    void SetHardWareID(string hid);
    
    char* Get_cm_ipaddress();
    static CPIDGenerater * GetInstance();
    void GetDefaultPID();
    bool IsSupportDemo();
    void EnterDemoMode();
    bool IsInDemoMode()
    {
        if(m_peerid == m_defaultPeerid)
            return true;
        else
            return false;
    };
    
    
    //added by ash
    void refreshAddresses();
private:
    uint64 Hash64(uint8 *k, uint32 length, uint64 level = DEFAULT_LEVEL1);
    string  m_hardwareID;
    uint64  m_peerid;
    bool m_cansupportDefault;
    uint64  m_defaultPeerid;
    
    static CPIDGenerater * m_instance;
    
    CPIDGenerater();
    ~CPIDGenerater();
    
};


#endif
