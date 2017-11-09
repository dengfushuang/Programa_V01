//********************************************************************************************
//
#ifndef  ETHERNET_H
#define  ETHERNET_H

#define ETH_TYPE_ARP_V		0x0806
#define ETH_TYPE_ARP_H_V	0x08
#define ETH_TYPE_ARP_L_V	0x06
#define ETH_TYPE_IP_V		0x0800
#define ETH_TYPE_IP_H_V		0x08
#define ETH_TYPE_IP_L_V		0x00

#define ETH_HEADER_LEN		14

#define ETH_DST_MAC_P		0
#define ETH_SRC_MAC_P		6
#define ETH_TYPE_H_P		12
#define ETH_TYPE_L_P		13
//********************************************************************************************
//
// Prototype function
//
//********************************************************************************************
//void eth_generate_packet ( ETH_HEADER eth_header );
 WORD software_checksum( BYTE *rxtx_buffer, WORD len, DWORD sum);
 void eth_generate_header ( BYTE *rxtx_buffer, WORD type, BYTE *dest_mac );

#endif
