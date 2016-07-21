//
//  PIDGenerater.cpp
//  TVUTransport
//
//  Created by shawnsong on 11-12-13.
//  Copyright 2011年 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "PIDGenerater.h"
//#include "RManagerDefine.h"
#include <sys/socket.h>
#include <unistd.h>
#include "IPAddress.h"
uint64 g_peerid = 0;
//#define DEFPEERIDADD      "ec2-50-18-77-155.us-west-1.compute.amazonaws.com"
//#define DEFPEERIDADD      "ec2-122-248-196-136.ap-southeast-1.compute.amazonaws.com"
#define DEFPEERIDADD      "anywhere.tvupack.com"
//#define DEFPEERIDADD      "192.168.1.100"
//#define DEFPEERIDADD      "10.12.23.101"
//#define DEFPEERIDPORT      8992
//v2.5.0 port
//#define DEFPEERIDPORT      8995
//v 2.5.2 port
//#define DEFPEERIDPORT      8996
//v 2.6.0
//#define DEFPEERIDPORT      8998
//v 3.0.0
//#define DEFPEERIDPORT      8999
//v 3.0.1
#define DEFPEERIDPORT      9001

#define TRANSPORT_MSG_GET_DEF_PEERID                0x24


#define mix64(a,b,c) \
{ \
a -= b; a -= c; a ^= (c>>43); \
b -= c; b -= a; b ^= (a<<9); \
c -= a; c -= b; c ^= (b>>8); \
a -= b; a -= c; a ^= (c>>38); \
b -= c; b -= a; b ^= (a<<23); \
c -= a; c -= b; c ^= (b>>5); \
a -= b; a -= c; a ^= (c>>35); \
b -= c; b -= a; b ^= (a<<49); \
c -= a; c -= b; c ^= (b>>11); \
a -= b; a -= c; a ^= (c>>12); \
b -= c; b -= a; b ^= (a<<18); \
c -= a; c -= b; c ^= (b>>22); \
}

CPIDGenerater * CPIDGenerater::m_instance = NULL;


CPIDGenerater::CPIDGenerater()
{
    m_peerid = 0;
    m_cansupportDefault = false;
    m_defaultPeerid = 0;
    InitAddresses();
    GetIPAddresses();
//    GetHWAddresses();
}
CPIDGenerater::~CPIDGenerater()
{
    FreeAddresses();
}

uint64 CPIDGenerater::GetLocalPeerID(bool &bgenerate)
{
    if(m_peerid == 0)
    {
        bgenerate = false;
        return 0;
    }
    bgenerate = true;
    //printf("%llx peerid \n",m_peerid);
    return m_peerid;
}

char* CPIDGenerater::Get_cm_ipaddress()
{
    return getCM_ipaddress();
}

CPIDGenerater * CPIDGenerater::GetInstance()
{
    if(m_instance == NULL)
    {
        m_instance = new CPIDGenerater();
        
    }
    return m_instance;
}


void CPIDGenerater::SetHardWareID(string hid)
{
    m_hardwareID = hid;
    
    char hashstring[100];
// changed for appstore not allow use udid
    //sprintf(hashstring, "IOST%s",m_hardwareID.c_str());
    //m_peerid = Hash64((uint8 *)(hid.c_str()), hid.length());
    if(hw_addrs[0] != 0 && strcmp(hw_addrs[0], "02:00:00:00:00:00") != 0)
    {
        sprintf(hashstring, "IOST%s",hw_addrs[0]);
        
    }
    else
    {
        sprintf(hashstring, "IOST%s",hid.c_str());
    }
    m_peerid = Hash64((uint8 *)hashstring, strlen(hashstring));
    g_peerid = m_peerid;    

}

void CPIDGenerater::EnterDemoMode()
{
    m_peerid = m_defaultPeerid;
    g_peerid = m_peerid; 
};

uint64 CPIDGenerater::Hash64(uint8 *k, uint32 length, uint64 level)
{
    uint64 a,b,c;
    uint32 len;
    
    /* Set up the internal state */
    len = length;
    a = b = level;                         /* the previous hash value */
    c = 0x9e3779b97f4a7c13LL; /* the golden ratio; an arbitrary value */
    
    /*---------------------------------------- handle most of the key */
    while (len >= 24)
    {
        a += (k[0]              +((uint64)k[ 1]<< 8)+((uint64)k[ 2]<<16)+((uint64)k[ 3]<<24)
              +((uint64)k[4 ]<<32)+((uint64)k[ 5]<<40)+((uint64)k[ 6]<<48)+((uint64)k[ 7]<<56));
        b += (k[8]              +((uint64)k[ 9]<< 8)+((uint64)k[10]<<16)+((uint64)k[11]<<24)
              +((uint64)k[12]<<32)+((uint64)k[13]<<40)+((uint64)k[14]<<48)+((uint64)k[15]<<56));
        c += (k[16]             +((uint64)k[17]<< 8)+((uint64)k[18]<<16)+((uint64)k[19]<<24)
              +((uint64)k[20]<<32)+((uint64)k[21]<<40)+((uint64)k[22]<<48)+((uint64)k[23]<<56));
        mix64(a,b,c);
        k += 24; len -= 24;
    }
    
    /*------------------------------------- handle the last 23 bytes */
    c += length;
    switch(len)              /* all the case statements fall through */
    {
        case 23: c+=((uint64)k[22]<<56);
        case 22: c+=((uint64)k[21]<<48);
        case 21: c+=((uint64)k[20]<<40);
        case 20: c+=((uint64)k[19]<<32);
        case 19: c+=((uint64)k[18]<<24);
        case 18: c+=((uint64)k[17]<<16);
        case 17: c+=((uint64)k[16]<<8);
            /* the first byte of c is reserved for the length */
        case 16: b+=((uint64)k[15]<<56);
        case 15: b+=((uint64)k[14]<<48);
        case 14: b+=((uint64)k[13]<<40);
        case 13: b+=((uint64)k[12]<<32);
        case 12: b+=((uint64)k[11]<<24);
        case 11: b+=((uint64)k[10]<<16);
        case 10: b+=((uint64)k[ 9]<<8);
        case  9: b+=((uint64)k[ 8]);
        case  8: a+=((uint64)k[ 7]<<56);
        case  7: a+=((uint64)k[ 6]<<48);
        case  6: a+=((uint64)k[ 5]<<40);
        case  5: a+=((uint64)k[ 4]<<32);
        case  4: a+=((uint64)k[ 3]<<24);
        case  3: a+=((uint64)k[ 2]<<16);
        case  2: a+=((uint64)k[ 1]<<8);
        case  1: a+=((uint64)k[ 0]);
            /* case 0: nothing left to add */
    }
    mix64(a,b,c);
    /*-------------------------------------------- report the result */
    return c;
    
}


//void CPIDGenerater::GetDefaultPID()
//{
//    
//    static int i = 0;
//    if(i> 10)
//        return ;
//    i++;
//	char *    dir [1024];
//	int	sd;
//	struct sockaddr_in pin;
//	struct hostent *hp;
//    
//	/* go find out about the desired host machine */
//	if ((hp = gethostbyname(DEFPEERIDADD)) == 0) {
//		return;
//	}
//    
//	/* fill in the socket structure with host information */
//	memset(&pin, 0, sizeof(pin));
//	pin.sin_family = AF_INET;
//	pin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
//	pin.sin_port = htons(DEFPEERIDPORT);
//    
//	/* grab an Internet domain socket */
//	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
//		return;
//	}
//    //printf("%s create socket %d \n",__FUNCTION__,sd);
//	/* connect to PORT on HOST */
//	if (connect(sd,(struct sockaddr *)  &pin, sizeof(pin)) == -1) {
//        //printf("error during connect %s\n", strerror(errno));
//        close(sd);
//		return;
//	}
//    
//    struct timeval tv; 
//    tv.tv_sec = 3;
//    tv.tv_usec = 0;
//    if (setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,  sizeof tv))
//    {
//        close(sd);
//        return;
//    }
//    
//    int msglen = sizeof(msg_header);
//    uint8 *pBuf = (uint8 *)malloc(sizeof(msg_header));
//    memset(pBuf, 0, msglen);
//    msg_header *pMsg = (msg_header *)pBuf;
//    pMsg->len = htons(msglen);
//    pMsg->type = TRANSPORT_MSG_GET_DEF_PEERID;
//    
//	/* send a message to the server PORT on machine HOST */
//	if (send(sd, pBuf, msglen,0) == -1) {
//		close(sd);
//        free(pBuf);
//        
//        return;
//	}
//    /* wait for a message to come back from the server */
//    
//    
//    if (recv(sd, dir, 1024, 0) == -1) {
//        close(sd);
//        free(pBuf);
//        return;
//    }
//    
//    close(sd);
//    msg_getPeerID* prespMsg = (msg_getPeerID*)dir;
//    if(ntohs(prespMsg->hdr.len) != sizeof(msg_getPeerID))
//    {
//        
//        close(sd);
//        return;
//    }
//    free(pBuf);
//    
//    if(prespMsg->using_lite == 1)
//    {
//        m_cansupportDefault = true;
//        m_defaultPeerid = ntoh64(prespMsg->peerid);
//    }
//#ifdef DEBUG_PS_M
//    printf("get default using_lite=%d   0x%llx\n",prespMsg->using_lite,m_defaultPeerid);
//#endif
//    return;
//    
//}

bool CPIDGenerater::IsSupportDemo()
{
    return m_cansupportDefault;
}

void CPIDGenerater::refreshAddresses()
{
    FreeAddresses();
    GetIPAddresses();
//    GetHWAddresses();
}