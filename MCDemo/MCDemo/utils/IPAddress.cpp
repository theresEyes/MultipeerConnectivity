/*
 *  IPAddress.c
 *  PersonalProxy
 *
 *  Created by Chris Whiteford on 2009-02-20.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <errno.h>
#include <net/if_dl.h>
//#include <netinet/ether.h>
#include <net/ethernet.h>
#include "IPAddress.h"
extern FILE * g_logfile ;

char g_3GAddress[40]="";
//#define __DEBUG_IPADDRESS__

#define BUFFERSIZE	4000

char *if_names[MAXADDRS];
char *ip_names[MAXADDRS];
char *hw_addrs[MAXADDRS];

char *cm_ipaddress[MAXADDRS];
unsigned long ip_addrs[MAXADDRS];

static int   nextAddr = 0;

void InitAddresses()
{
	int i;
	for (i=0; i<MAXADDRS; ++i)
	{
		if_names[i] = ip_names[i] = hw_addrs[i] = NULL;
		ip_addrs[i] = 0;
	}
}

void FreeAddresses()
{
	int i;
	for (i=0; i<MAXADDRS; ++i)
	{
		if (if_names[i] != 0) free(if_names[i]);
		if (ip_names[i] != 0) free(ip_names[i]);
		if (hw_addrs[i] != 0) free(hw_addrs[i]);
		ip_addrs[i] = 0;
	}
	InitAddresses();
}

void GetIPAddresses()
{
	int                 i, len, flags;
	char                buffer[BUFFERSIZE], *ptr, lastname[IFNAMSIZ], *cptr;
	struct ifconf       ifc;
	struct ifreq        *ifr, ifrcopy;
	struct sockaddr_in	*sin;

	char temp[80];

	int sockfd;

	for (i=0; i<MAXADDRS; ++i)
	{
		if_names[i] = ip_names[i] = NULL;
		ip_addrs[i] = 0;
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		perror("socket failed");
		return;
	}
	
	ifc.ifc_len = BUFFERSIZE;
	ifc.ifc_buf = buffer;
	
	if (ioctl(sockfd, SIOCGIFCONF, &ifc) < 0)
	{
		perror("ioctl error");
		return;
	}
	
	lastname[0] = 0;
	
	for (ptr = buffer; ptr < buffer + ifc.ifc_len; )
	{
		ifr = (struct ifreq *)ptr;
		len = max(sizeof(struct sockaddr), ifr->ifr_addr.sa_len);
		ptr += sizeof(ifr->ifr_name) + len;	// for next one in buffer
        
#ifdef __DEBUG_IPADDRESS__
        if(g_logfile != NULL)
        {
            fprintf(g_logfile,"get slot %s   family = %d \n",ifr->ifr_name,ifr->ifr_addr.sa_family);
        }
#endif
        
		if (ifr->ifr_addr.sa_family != AF_INET || strcmp(ifr->ifr_name,"lo0") == 0)
		{
#ifdef __DEBUG_IPADDRESS__
            if(g_logfile != NULL)
            {
                fprintf(g_logfile,"-- Fail get slot %s   family = %d \n",ifr->ifr_name,ifr->ifr_addr.sa_family);
            }
#endif
			continue;	// ignore if not desired address family
		}
        
		if ((cptr = (char *)strchr(ifr->ifr_name, ':')) != NULL)
		{
			*cptr = 0;		// replace colon will null
		}
	
		if (strncmp(lastname, ifr->ifr_name, IFNAMSIZ) == 0)
		{
#ifdef __DEBUG_IPADDRESS__
            if(g_logfile != NULL)
            {
                fprintf(g_logfile,"-- Fail get slot %s   name length = 0 \n",ifr->ifr_name);
            }
#endif
			continue;	/* already processed this interface */
		}
	
		memcpy(lastname, ifr->ifr_name, IFNAMSIZ);
	
		ifrcopy = *ifr;
		ioctl(sockfd, SIOCGIFFLAGS, &ifrcopy);
		flags = ifrcopy.ifr_flags;
		if ((flags & IFF_UP) == 0)
		{
#ifdef __DEBUG_IPADDRESS__
            if(g_logfile != NULL)
            {
                fprintf(g_logfile,"-- Fail get slot %s   IFF_UP \n",ifr->ifr_name);
            }
#endif
			continue;	// ignore if interface not up
		}
	
		if_names[nextAddr] = (char *)malloc(strlen(ifr->ifr_name)+1);
		if (if_names[nextAddr] == NULL)
		{
			return;
		}
		strcpy(if_names[nextAddr], ifr->ifr_name);
	
		sin = (struct sockaddr_in *)&ifr->ifr_addr;
		strcpy(temp, inet_ntoa(sin->sin_addr));
	
		ip_names[nextAddr] = (char *)malloc(strlen(temp)+1);
		if (ip_names[nextAddr] == NULL)
		{
			return;
		}
		strcpy(ip_names[nextAddr], temp);
        if(strcmp(ifr->ifr_name, "pdp_ip0") == 0)
        {
            strcpy(g_3GAddress, ip_names[nextAddr]);
        }
        
		ip_addrs[nextAddr] = sin->sin_addr.s_addr;
//        fprintf(stderr,"%d  get family %s   %s   %0lxd\n",nextAddr,if_names[nextAddr],ip_names[nextAddr],ip_addrs[nextAddr]);
//#ifdef __DEBUG_IPADDRESS__
//        if(g_logfile != NULL)
//        {
//            printf("%d  get family %s   %s   %0lxd\n",nextAddr,if_names[nextAddr],ip_names[nextAddr],ip_addrs[nextAddr]);
////            NSLog(@"111111111111111111111");
////            NSLog(@"%d  get family %s   %s   %0lxd\n",nextAddr,if_names[nextAddr],ip_names[nextAddr],ip_addrs[nextAddr]);
//        }
//        else
//        {
            printf("%d  get family %s   %s   %0lxd\n",nextAddr,if_names[nextAddr],ip_names[nextAddr],ip_addrs[nextAddr]);
        
        if (
            strcmp(if_names[nextAddr], "pdp_ip0") == 0) {
            cm_ipaddress[MAXADDRS] = ip_names[nextAddr];
        }
        
//            NSLog(@"2222222222222222222222");
//            NSLog(@"%d  get family %s   %s   %0lxd\n",nextAddr,if_names[nextAddr],ip_names[nextAddr],ip_addrs[nextAddr]);
//        }
//#endif
		++nextAddr;
	}
	
	close(sockfd);
}

char* getCM_ipaddress()
{
    return cm_ipaddress[MAXADDRS];
}

void GetHWAddresses()
{
    struct ifconf ifc;
    struct ifreq *ifr;
    int i, sockfd;
    char buffer[BUFFERSIZE], *cp, *cplim;
    char temp[80];
    
    for (i=0; i<MAXADDRS; ++i)
    {
        hw_addrs[i] = NULL;
    }
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket failed");
        return;
    }
    
    ifc.ifc_len = BUFFERSIZE;
    ifc.ifc_buf = buffer;
    
    if (ioctl(sockfd, SIOCGIFCONF, (char *)&ifc) < 0)
    {
        perror("ioctl error");
        close(sockfd);
        return;
    }
    
    ifr = ifc.ifc_req;
    
    cplim = buffer + ifc.ifc_len;
    
    for (cp=buffer; cp < cplim; )
    {
        ifr = (struct ifreq *)cp;
        if (ifr->ifr_addr.sa_family == AF_LINK)
        {
            struct sockaddr_dl *sdl = (struct sockaddr_dl *)&ifr->ifr_addr;
            int a,b,c,d,e,f;
            int i;
            strcpy(temp, (char *)ether_ntoa((const struct ether_addr *)LLADDR(sdl)));
            sscanf(temp, "%x:%x:%x:%x:%x:%x", &a, &b, &c, &d, &e, &f);
            sprintf(temp, "%02X:%02X:%02X:%02X:%02X:%02X",a,b,c,d,e,f);
            //printf("name = %s  %d   %s \n",ifr->ifr_name,ifr->ifr_addr.sa_family,temp);
            if(strcmp(ifr->ifr_name, "en0") == 0)
            {
                if (hw_addrs[i] == NULL)
                {
                    //printf("name = %s \n",ifr->ifr_name);
                    hw_addrs[i] = (char *)malloc(strlen(temp)+1);
                    strcpy(hw_addrs[0], temp);
                }
            }
            
            
            
//            for (i=0; i<MAXADDRS; ++i)
//            {
//                if ((if_names[i] != NULL) && (strcmp(ifr->ifr_name, if_names[i]) == 0))
//                {
//                    if (hw_addrs[i] == NULL)
//                    {
//                        printf("name = %s \n",ifr->ifr_name);
//                        hw_addrs[i] = (char *)malloc(strlen(temp)+1);
//                        strcpy(hw_addrs[1], temp);
//                        break;
//                    }
//                }
//            }
        }
        cp += sizeof(ifr->ifr_name) + max(sizeof(ifr->ifr_addr), ifr->ifr_addr.sa_len);
    }
    
#ifdef __DEBUG_IPADDRESS__
    if(g_logfile != NULL)
    {
        if(hw_addrs[0] != NULL)
            fprintf(g_logfile,"-----------mac address = %s \n",hw_addrs[0]);
        else
            fprintf(g_logfile,"-----------mac address not get  \n");
    }
    else
    {
        if(hw_addrs[0] != NULL)
            fprintf(stderr,"-----------mac address = %s \n",hw_addrs[0]);
        else
            fprintf(stderr,"-----------mac address not get  \n");
    }
#endif

    
    
    
//    for (i=0; i<MAXADDRS; ++i)
//    {
//        printf("%s ",hw_addrs[i]);
//    }
//    printf("\n");
    close(sockfd);

}
