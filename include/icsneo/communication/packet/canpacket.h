#ifndef __CANPACKET_H__
#define __CANPACKET_H__

#ifdef __cplusplus

#include "icsneo/communication/message/canmessage.h"
#include "icsneo/api/eventmanager.h"
#include <cstdint>
#include <memory>

namespace icsneo {

typedef uint16_t icscm_bitfield;

struct HardwareCANPacket {
	static std::shared_ptr<CANMessage> DecodeToMessage(const std::vector<uint8_t>& bytestream);
	static bool EncodeFromMessage(const CANMessage& message, std::vector<uint8_t>& bytestream, const device_eventhandler_t& report);

	struct {
		icscm_bitfield IDE : 1;
		icscm_bitfield SRR : 1;
		icscm_bitfield SID : 11;
		icscm_bitfield EDL : 1;
		icscm_bitfield BRS : 1;
		icscm_bitfield ESI : 1;
	} header;
	struct {
		icscm_bitfield EID : 12;
		icscm_bitfield TXMSG : 1;
		icscm_bitfield TXAborted : 1;
		icscm_bitfield TXLostArb : 1;
		icscm_bitfield TXError : 1;
	} eid;
	struct {
		icscm_bitfield DLC : 4;
		icscm_bitfield RB0 : 1;
		icscm_bitfield IVRIF : 1;
		icscm_bitfield HVEnable : 1;// must be cleared before passing into CAN driver
		icscm_bitfield ExtendedNetworkIndexBit : 1;//DO NOT CLOBBER THIS
		icscm_bitfield RB1 : 1;
		icscm_bitfield RTR : 1;
		icscm_bitfield EID2 : 6;
	} dlc;
	unsigned char data[8];
	uint16_t stats;
	struct {
		uint64_t TS : 60;
		uint64_t : 3; // Reserved for future status bits
		uint64_t IsExtended : 1;
	} timestamp;
};

#define DATA_ON_WIRE_DECODER_DEVELOPMENT
#ifdef DATA_ON_WIRE_DECODER_DEVELOPMENT
#pragma pack(push, 1)
// From std::vector<uint8_t> EthernetPacketizer::EthernetPacket::getBytestream() const
struct EthHeader
{
	uint8_t destMAC[6];		// 0..5
	uint8_t srcMAC[6];		// 6..11
	uint16_t Protocol;		// Should be 0xcab1	12..13  // Big endian
	uint32_t icsEthernetHeader; // 0xaaaa5555 OK	14..17	// Big endian
};

struct IcsCan11BitArb
{
	uint8_t ArbID_3_11;	// 0
	uint8_t ArbID_0_2; // 1
	int GetArbID() { return (ArbID_3_11 << 3) + (ArbID_0_2 >> 5); }
};

struct IcsCanPacket11 // 15 bytes
{
	uint8_t NetworkID : 4;		// Network::NetID::Main51  whatever that means  18
	uint8_t Size: 4;			// DLC
	uint16_t DescriptionID;		// big endian 19..20
	IcsCan11BitArb ArbID;		// 21..22
	uint8_t LengthNibble : 4;	// 23
	uint8_t statusNibble : 4;
	uint8_t data[8];			// 24..32
};

struct IcsCanPacket11Fd
{
	uint8_t NetworkID : 4;		// Network::NetID::Main51  whatever that means  18
	uint8_t Size: 4;
	uint16_t DescriptionID;		// big endian 19..20

	IcsCan11BitArb ArbID;		// 21..22
	uint8_t FDFrame = 0xF;		// 23
	uint8_t LengthNibble : 4;	// 24
	uint8_t statusNibble : 4;
	uint8_t data[64];			// 25..89
};

struct IcsCan29BitArb
{
	uint8_t ArbID_28_21;		// byte 0
	uint8_t ArbID_16_17:2;		// byte 1
	uint8_t :1;
	uint8_t b29:1;				// if 1, then extended
	uint8_t :1;
	uint8_t ArbID_18_20:3;
	uint8_t ArbID_8_15;			// byte 2
	uint8_t ArbID_0_7;			// byte 3

	int GetArbID() {
		return ArbID_0_7 + (ArbID_8_15 << 8) + (ArbID_28_21 << 21) + (ArbID_16_17 << 16) + (ArbID_18_20 << 18);
	}
};

struct IcsCanPacket29 // 29 bit
{
	uint8_t NetworkID : 4;		// byte 18.0..4 with Network::NetID::Main51  whatever that means		| 0
	uint8_t Size: 4;
	uint16_t DescriptionID;		// 19..20 big endian													| 1..2
	IcsCan29BitArb ArbID;		// 21..24																| 3..6
	uint8_t LengthNibble : 4;	// 25
	uint8_t statusNibble : 4;
	uint8_t data[8];			// 26
};

struct IcsCanPacket29Fd // 29 bit
{
	uint8_t NetworkID : 4;		// Network::NetID::Main51  whatever that means  18
	uint8_t Size: 4;
	uint16_t DescriptionID;		// 19..20 big endian 
	IcsCan29BitArb ArbID;		// 21..24
	uint8_t FDFrame = 0xF;		// 25
	uint8_t LengthNibble : 7;	// 26.0..6
	uint8_t BaudRateSwitch : 1; // 26.7
	uint8_t data[64];			// 27..91
};

#endif DATA_ON_WIRE_DECODER_DEVELOPMENT
#pragma pack(pop)

}

#endif // __cplusplus

#endif