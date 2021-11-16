#include "icsneo/device/device.h"
#include "icsneo/communication/message/callback/messagecallback.h"
#include "icsneo/api/eventmanager.h"
#include "icsneo/communication/command.h"
#include "icsneo/device/extensions/deviceextension.h"
#include <string.h>
#include <iostream>
#include <sstream>
#include <chrono>

using namespace icsneo;

static const uint8_t fromBase36Table[256] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12,
	13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35 };

static const char toBase36Table[36] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E',
	'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };

static const uint32_t toBase36Powers[7] = { 1, 36, 1296, 46656, 1679616, 60466176, 2176782336 };

#define MIN_BASE36_SERIAL (16796160)
#define MAX_SERIAL (2176782335)

std::string Device::SerialNumToString(uint32_t serial) {
	if(serial == 0 || serial > MAX_SERIAL)
		return "0";

	std::stringstream ss;
	if(serial >= MIN_BASE36_SERIAL) {
		for (auto i = 5; i >= 0; i--) {
			ss << toBase36Table[serial / toBase36Powers[i]];
			serial = serial % toBase36Powers[i];
		}
	} else {
		ss << serial;
	}
	return ss.str();
}

uint32_t Device::SerialStringToNum(const std::string& serial) {
	if(Device::SerialStringIsNumeric(serial)) {
		try {
			return std::stoi(serial);
		} catch(...) {
			return 0;
		}
	}

	if(serial.length() != 6)
		return 0; // Non-numeric serial numbers should be 6 characters

	uint32_t ret = 0;
	for (auto i = 0; i < 6; i++) {
		ret *= 36;
		ret += fromBase36Table[(unsigned char)serial[i]];
	}
	return ret;
}

bool Device::SerialStringIsNumeric(const std::string& serial) {
	if(serial.length() == 0)
		return false;

	if(serial.length() == 1)
		return isdigit(serial[0]);

	// Check the first two characters, at least one should be a character if we need to do a base36 conversion
	return isdigit(serial[0]) && isdigit(serial[1]);
}

Device::~Device() {
	if(isMessagePollingEnabled())
		disableMessagePolling();
	close();
}

uint16_t Device::getTimestampResolution() const {
	return com->decoder->timestampResolution;
}

std::string Device::describe() const {
	std::stringstream ss;
	ss << getProductName() << ' ' << getSerial();
	return ss.str();
}

bool Device::enableMessagePolling() {
	if(isMessagePollingEnabled()) {// We are already polling
		report(APIEvent::Type::DeviceCurrentlyPolling, APIEvent::Severity::Error);
		return false;
	}
	messagePollingCallbackID = com->addMessageCallback(MessageCallback([this](std::shared_ptr<Message> message) {
		pollingContainer.enqueue(message);
		enforcePollingMessageLimit();
	}));
	return true;
}

bool Device::disableMessagePolling() {
	if(!isMessagePollingEnabled()) {
		report(APIEvent::Type::DeviceNotCurrentlyPolling, APIEvent::Severity::Error);
		return false; // Not currently polling
	}
	auto ret = com->removeMessageCallback(messagePollingCallbackID);
	getMessages(); // Flush any messages still in the container
	messagePollingCallbackID = 0;
	return ret;
}

// Returns a pair of {vector, bool}, where the vector contains shared_ptrs to the returned msgs and the bool is whether or not the call was successful.
std::pair<std::vector<std::shared_ptr<Message>>, bool> Device::getMessages() {
	std::vector<std::shared_ptr<Message>> ret;
	bool retBool = getMessages(ret);
	return std::make_pair(ret, retBool);
}

bool Device::getMessages(std::vector<std::shared_ptr<Message>>& container, size_t limit, std::chrono::milliseconds timeout) {
	// not open
	if(!isOpen()) {
		report(APIEvent::Type::DeviceCurrentlyClosed, APIEvent::Severity::Error);
		return false;
	}

	// not online
	if(!isOnline()) {
		report(APIEvent::Type::DeviceCurrentlyOffline, APIEvent::Severity::Error);
		return false;
	}

	// not currently polling, throw error
	if(!isMessagePollingEnabled()) {
		report(APIEvent::Type::DeviceNotCurrentlyPolling, APIEvent::Severity::Error);
		return false;
	}

	// A limit of zero indicates no limit
	if(limit == 0)
		limit = (size_t)-1;

	if(limit > (pollingContainer.size_approx() + 4))
		limit = (pollingContainer.size_approx() + 4);

	if(container.size() < limit)
		container.resize(limit);

	size_t actuallyRead;
	if(timeout != std::chrono::milliseconds(0))
		actuallyRead = pollingContainer.wait_dequeue_bulk_timed(container.data(), limit, timeout);
	else
		actuallyRead = pollingContainer.try_dequeue_bulk(container.data(), limit);

	if(container.size() > actuallyRead)
		container.resize(actuallyRead);

	return true;
}

void Device::enforcePollingMessageLimit() {
	while(pollingContainer.size_approx() > pollingMessageLimit) {
		std::shared_ptr<Message> throwAway;
		pollingContainer.try_dequeue(throwAway);
		report(APIEvent::Type::PollingMessageOverflow, APIEvent::Severity::EventWarning);
	}
}

bool Device::open(OpenFlags flags, OpenStatusHandler handler) {
	if(!com) {
		report(APIEvent::Type::Unknown, APIEvent::Severity::Error);
		return false;
	}

	if(!com->open()) {
		return false;
	}

	APIEvent::Type attemptErr = attemptToBeginCommunication();
	if(attemptErr != APIEvent::Type::NoErrorFound) {
		// We could not communicate with the device, let's see if an extension can
		bool tryAgain = false;
		forEachExtension([&tryAgain, &flags, &handler](const std::shared_ptr<DeviceExtension>& ext) -> bool {
			if(ext->onDeviceCommunicationDead(flags, handler))
				tryAgain = true;
			return true;
		});
		if(!tryAgain) {
			com->close();
			report(attemptErr, APIEvent::Severity::Error);
			return false; // Extensions couldn't save us
		}
		attemptErr = attemptToBeginCommunication();
		if(attemptErr != APIEvent::Type::NoErrorFound) {
			com->close();
			report(attemptErr, APIEvent::Severity::Error);
			return false;
		}
	}

	bool block = false;
	forEachExtension([&block, &flags, &handler](const std::shared_ptr<DeviceExtension>& ext) {
		if(ext->onDeviceOpen(flags, handler))
			return true;
		block = true;
		return false;
	});
	if(block) // Extensions say no
		return false;

	if(!settings->disabled) {
		// Since we will not fail the open if a settings read fails,
		// downgrade any errors to warnings. Otherwise the error will
		// go unnoticed in the opening thread's getLastError buffer.
		const bool downgrading = EventManager::GetInstance().isDowngradingErrorsOnCurrentThread();
		if(!downgrading)
			EventManager::GetInstance().downgradeErrorsOnCurrentThread();
		settings->refresh();
		if(!downgrading)
			EventManager::GetInstance().cancelErrorDowngradingOnCurrentThread();
	}

	MessageFilter filter;
	filter.includeInternalInAny = true;
	internalHandlerCallbackID = com->addMessageCallback(MessageCallback(filter, [this](std::shared_ptr<Message> message) {
		handleInternalMessage(message);
	}));

	heartbeatThread = std::thread([this]() {
		EventManager::GetInstance().downgradeErrorsOnCurrentThread();

		MessageFilter filter;
		filter.includeInternalInAny = true;
		std::atomic<bool> receivedMessage{false};
		auto messageReceivedCallbackID = com->addMessageCallback(MessageCallback(filter, [&receivedMessage](std::shared_ptr<Message> message) {
			receivedMessage = true;
		}));

		// Give the device time to get situated
		auto i = 150;
		while(!stopHeartbeatThread && i != 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			i--;
		}

		while(!stopHeartbeatThread) {
			// Wait for 110ms for a possible heartbeat
			std::this_thread::sleep_for(std::chrono::milliseconds(110));
			if(!receivedMessage && !heartbeatSuppressed()) {
				// No heartbeat received, request a status
				com->sendCommand(Command::RequestStatusUpdate);
				// The response should come back quickly if the com is quiet
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				// Check if we got a message, and if not, if settings are being applied
				if(!receivedMessage && !heartbeatSuppressed()) {
					if(!stopHeartbeatThread && !isDisconnected())
						report(APIEvent::Type::DeviceDisconnected, APIEvent::Severity::Error);
					break;
				}
			}
			receivedMessage = false;
		}

		com->removeMessageCallback(messageReceivedCallbackID);
	});

	return true;
}

APIEvent::Type Device::attemptToBeginCommunication() {
	versions.clear();

	if(!afterCommunicationOpen()) {
		// Very unlikely, at the time of writing this only fails if rawWrite does.
		// If you're looking for this error, you're probably looking for if(!serial) below.
		// "Communication could not be established with the device. Perhaps it is not powered with 12 volts?"
		return getCommunicationNotEstablishedError();
	}

	auto serial = com->getSerialNumberSync();
	int i = 0;
	while(!serial) {
		serial = com->getSerialNumberSync();
		if(i++ > 5)
			break;
	}
	if(!serial) // "Communication could not be established with the device. Perhaps it is not powered with 12 volts?"
		return getCommunicationNotEstablishedError();
	
	std::string currentSerial = getNeoDevice().serial;
	if(currentSerial != serial->deviceSerial)
		return APIEvent::Type::IncorrectSerialNumber;

	auto maybeVersions = com->getVersionsSync();
	if(!maybeVersions)
		return getCommunicationNotEstablishedError();
	else
		versions = std::move(*maybeVersions);

	return APIEvent::Type::NoErrorFound;
}

bool Device::close() {
	if(!com) {
		report(APIEvent::Type::Unknown, APIEvent::Severity::Error);
		return false;
	}

	stopHeartbeatThread = true;

	if(isOnline())
		goOffline();
	
	if(internalHandlerCallbackID)
		com->removeMessageCallback(internalHandlerCallbackID);

	internalHandlerCallbackID = 0;

	if(heartbeatThread.joinable())
		heartbeatThread.join();
	stopHeartbeatThread = false;

	forEachExtension([](const std::shared_ptr<DeviceExtension>& ext) { ext->onDeviceClose(); return true; });
	return com->close();
}

bool Device::goOnline() {
	if(!com->sendCommand(Command::EnableNetworkCommunication, true))
		return false;

	auto startTime = std::chrono::system_clock::now();

	ledState = LEDState::Online;

	updateLEDState();

	MessageFilter filter(Network::NetID::Reset_Status);
	filter.includeInternalInAny = true;

	// Wait until communication is enabled or 5 seconds, whichever comes first
	while((std::chrono::system_clock::now() - startTime) < std::chrono::seconds(5)) {
		if(latestResetStatus && latestResetStatus->comEnabled)
			break;

		bool failOut = false;
		com->waitForMessageSync([this, &failOut]() {
			if(!com->sendCommand(Command::RequestStatusUpdate)) {
				failOut = true;
				return false;
			}
			return true;
		}, filter, std::chrono::milliseconds(100));
		if(failOut)
			return false;
	}
	
	online = true;

	forEachExtension([](const std::shared_ptr<DeviceExtension>& ext) { ext->onGoOnline(); return true; });
	return true;
}

bool Device::goOffline() {
	forEachExtension([](const std::shared_ptr<DeviceExtension>& ext) { ext->onGoOffline(); return true; });

	if(isDisconnected()) {
		online = false;
		return true;
	}

	if(!com->sendCommand(Command::EnableNetworkCommunication, false))
		return false;

	auto startTime = std::chrono::system_clock::now();

	ledState = (latestResetStatus && latestResetStatus->cmRunning) ? LEDState::CoreMiniRunning : LEDState::Offline;
	
	updateLEDState();
	
	MessageFilter filter(Network::NetID::Reset_Status);
	filter.includeInternalInAny = true;

	// Wait until communication is disabled or 5 seconds, whichever comes first
	while((std::chrono::system_clock::now() - startTime) < std::chrono::seconds(5)) {
		if(latestResetStatus && !latestResetStatus->comEnabled)
			break;
		
		if(!com->sendCommand(Command::RequestStatusUpdate))
			return false;

		com->waitForMessageSync(filter, std::chrono::milliseconds(100));
	}
	
	online = false;

	return true;
}

#pragma pack(push, 1)
// From std::vector<uint8_t> EthernetPacketizer::EthernetPacket::getBytestream() const {
struct IcsMessageOnEthernetPacket
{
	uint8_t destMAC[6]; // 0..5
	uint8_t srcMAC[6];	// 6..11
	uint16_t Protocol; // Should be 0xcab1	12..13  // Big endian
	uint32_t icsEthernetHeader; // 0xaaaa5555 OK	14..17	// Big endian
	uint16_t payloadSize;	// 18..19	little endian
	uint16_t packetNumber; // 20..21	little endian

						   // packetInfo Big endian
	uint8_t	reserved;			// 22
	uint8_t firstPiece : 1;		// 23.0
	uint8_t lastPiece : 1;		// 23.1
	uint8_t bufferHalfFull : 1;	// 23.2
	uint8_t padding : 4;		// 23.3..6
	uint8_t ProtocolVersion1 : 1;// 23.7

	uint8_t AA; // 24..27			AA for some reason
	uint8_t B1; // 24..27			((1 << 4) | (uint8_t)Network::NetID::Main51), // Packet size of 1 on NETID_MAIN51
	uint8_t Command; // 24..27		(uint8_t)Command::RequestSerialNumber a1
	uint8_t B3; // 24..27			Packetizer::ICSChecksum(requestPacket.payload)
};
#pragma pack(pop)

bool Device::transmit(std::shared_ptr<Message> message) {
	// not open
	if(!isOpen()) {
		report(APIEvent::Type::DeviceCurrentlyClosed, APIEvent::Severity::Error);
		return false;
	}

	// not online
	if(!isOnline()) {
		report(APIEvent::Type::DeviceCurrentlyOffline, APIEvent::Severity::Error);
		return false;
	}

	if(!isSupportedTXNetwork(message->network)) {
		report(APIEvent::Type::UnsupportedTXNetwork, APIEvent::Severity::Error);
		return false;
	}

	bool extensionHookedTransmit = false;
	bool transmitStatusFromExtension = false;
	forEachExtension([&](const std::shared_ptr<DeviceExtension>& ext) {
		if(!ext->transmitHook(message, transmitStatusFromExtension))
			extensionHookedTransmit = true;
		return !extensionHookedTransmit; // false breaks out of the loop early
	});
	if(extensionHookedTransmit)
		return transmitStatusFromExtension;

	std::vector<uint8_t> packet;
	if(!com->encoder->encode(*com->packetizer, packet, message))
		return false;
	
	// Size on wire is 20 bytes
	
	uint8_t buffer[1000];
	int i;
	for (i = 0; i < packet.size(); i++)
		buffer[i] = packet.at(i);

	IcsMessageOnEthernetPacket *pMOE = (IcsMessageOnEthernetPacket *)buffer;

	return com->sendPacket(packet);
}

bool Device::transmit(std::vector<std::shared_ptr<Message>> messages) {
	for(auto& message : messages) {
		if(!transmit(message))
			return false;
	}
	return true;
}

void Device::setWriteBlocks(bool blocks) {
	com->setWriteBlocks(blocks);
}

size_t Device::getNetworkCountByType(Network::Type type) const {
	size_t count = 0;
	for(const auto& net : getSupportedRXNetworks())
		if(net.getType() == type)
			count++;
	return count;
}

// Indexed starting at one
Network Device::getNetworkByNumber(Network::Type type, size_t index) const {
	size_t count = 0;
	for(const auto& net : getSupportedRXNetworks()) {
		if(net.getType() == type) {
			count++;
			if(count == index)
				return net;
		}
	}
	return Network::NetID::Invalid;
}

optional<bool> Device::getDigitalIO(IO type, size_t number /* = 1 */) {
	if(number == 0) { // Start counting from 1
		report(APIEvent::Type::ParameterOutOfRange, APIEvent::Severity::Error);
		return false;
	}

	std::lock_guard<std::mutex> lk(ioMutex);
	switch(type) {
	case IO::EthernetActivation:
		if(getEthernetActivationLineCount() < number)
			break; // ParameterOutOfRange
		assert(number == 1); // If you implement a device with more, you'll need to modify the accessor

		if(!ethActivationStatus.has_value())
			report(APIEvent::Type::ValueNotYetPresent, APIEvent::Severity::Error);

		return ethActivationStatus;
	case IO::USBHostPower:
		if(getUSBHostPowerCount() < number)
			break; // ParameterOutOfRange
		assert(number == 1); // If you implement a device with more, you'll need to modify the accessor

		if(!usbHostPowerStatus.has_value())
			report(APIEvent::Type::ValueNotYetPresent, APIEvent::Severity::Error);

		return usbHostPowerStatus;
	case IO::BackupPowerEnabled:
		if(!getBackupPowerSupported())
			break; // ParameterOutOfRange
		assert(number == 1); // If you implement a device with more, you'll need to modify the accessor

		if(!backupPowerEnabled.has_value())
			report(APIEvent::Type::ValueNotYetPresent, APIEvent::Severity::Error);

		return backupPowerEnabled;
	case IO::BackupPowerGood:
		if(!getBackupPowerSupported())
			break; // ParameterOutOfRange
		assert(number == 1); // If you implement a device with more, you'll need to modify the accessor

		if(!backupPowerGood.has_value())
			report(APIEvent::Type::ValueNotYetPresent, APIEvent::Severity::Error);

		return backupPowerGood;
	case IO::Misc: {
		bool found = false;
		for(const auto& misc : getMiscIO()) {
			if(misc.number == number) {
				found = misc.supportsDigitalIn;
				break;
			}
		}
		if(!found)
			break; // ParameterOutOfRange

		if(number > miscDigital.size())
			break; // ParameterOutOfRange

		if(!miscDigital[number - 1].has_value())
			report(APIEvent::Type::ValueNotYetPresent, APIEvent::Severity::Error);

		return miscDigital[number - 1];
	}
	case IO::EMisc: {
		bool found = false;
		for(const auto& misc : getEMiscIO()) {
			if(misc.number == number) {
				found = misc.supportsDigitalIn;
				break;
			}
		}
		if(!found)
			break; // ParameterOutOfRange

		if(number > miscDigital.size())
			break; // ParameterOutOfRange

		// If there is ever a device with overlapping misc IOs and emisc IOs,
		// you will need to make a new member variable for the emisc IOs.
		if(!miscDigital[number - 1].has_value())
			report(APIEvent::Type::ValueNotYetPresent, APIEvent::Severity::Error);

		return miscDigital[number - 1];
	}
	};

	report(APIEvent::Type::ParameterOutOfRange, APIEvent::Severity::Error);
	return nullopt;
}

bool Device::setDigitalIO(IO type, size_t number, bool value) {
	if(number == 0) { // Start counting from 1
		report(APIEvent::Type::ParameterOutOfRange, APIEvent::Severity::Error);
		return false;
	}

	std::lock_guard<std::mutex> lk(ioMutex);
	switch(type) {
	case IO::EthernetActivation:
		if(getEthernetActivationLineCount() < number)
			break; // ParameterOutOfRange
		assert(number == 1); // If you implement a device with more, you'll need to modify the accessor

		ethActivationStatus = value;

		return com->sendCommand(Command::MiscControl, {
			uint8_t(1), uint8_t(value ? 1 : 0), // enetActivateSet, enetActivateValue
			uint8_t(0), uint8_t(0), // usbHostPowerSet, usbHostPowerValue
			uint8_t(0), uint8_t(0) // backupPowerSet, backupPowerValue
		});
	case IO::USBHostPower:
		if(getUSBHostPowerCount() < number)
			break; // ParameterOutOfRange
		assert(number == 1); // If you implement a device with more, you'll need to modify the accessor

		usbHostPowerStatus = value;

		return com->sendCommand(Command::MiscControl, {
			uint8_t(0), uint8_t(0), // enetActivateSet, enetActivateValue
			uint8_t(1), uint8_t(value ? 1 : 0), // usbHostPowerSet, usbHostPowerValue
			uint8_t(0), uint8_t(0) // backupPowerSet, backupPowerValue
		});
	case IO::BackupPowerEnabled:
		if(!getBackupPowerSupported())
			break; // ParameterOutOfRange
		assert(number == 1); // If you implement a device with more, you'll need to modify the accessor

		backupPowerEnabled = value;

		return com->sendCommand(Command::MiscControl, {
			uint8_t(0), uint8_t(0), // enetActivateSet, enetActivateValue
			uint8_t(0), uint8_t(value ? 1 : 0), // usbHostPowerSet, usbHostPowerValue (set to work around firmware bug)
			uint8_t(1), uint8_t(value ? 1 : 0) // backupPowerSet, backupPowerValue
		});
	case IO::BackupPowerGood:
		break; // Read-only, return ParameterOutOfRange
	case IO::Misc:
	case IO::EMisc:
		break; // Read-only for the moment, return ParameterOutOfRange
	};

	report(APIEvent::Type::ParameterOutOfRange, APIEvent::Severity::Error);
	return false;
}

optional<double> Device::getAnalogIO(IO type, size_t number /* = 1 */) {
	if(number == 0) { // Start counting from 1
		report(APIEvent::Type::ParameterOutOfRange, APIEvent::Severity::Error);
		return false;
	}

	std::lock_guard<std::mutex> lk(ioMutex);
	switch(type) {
	case IO::EthernetActivation:
	case IO::USBHostPower:
	case IO::BackupPowerEnabled:
	case IO::BackupPowerGood:
		break;
	case IO::Misc: {
		bool found = false;
		for(const auto& misc : getMiscIO()) {
			if(misc.number == number) {
				found = misc.supportsAnalogIn;
				break;
			}
		}
		if(!found)
			break; // ParameterOutOfRange

		if(number > miscAnalog.size())
			break; // ParameterOutOfRange

		if(!miscAnalog[number - 1].has_value())
			report(APIEvent::Type::ValueNotYetPresent, APIEvent::Severity::Error);

		return miscAnalog[number - 1];
	}
	case IO::EMisc: {
		bool found = false;
		for(const auto& misc : getEMiscIO()) {
			if(misc.number == number) {
				found = misc.supportsAnalogIn;
				break;
			}
		}
		if(!found)
			break; // ParameterOutOfRange

		if(number > miscAnalog.size())
			break; // ParameterOutOfRange

		// If there is ever a device with overlapping misc IOs and emisc IOs,
		// you will need to make a new member variable for the emisc IOs.
		if(!miscAnalog[number - 1].has_value())
			report(APIEvent::Type::ValueNotYetPresent, APIEvent::Severity::Error);

		return miscAnalog[number - 1];
	}
	};

	report(APIEvent::Type::ParameterOutOfRange, APIEvent::Severity::Error);
	return nullopt;
}

Lifetime Device::suppressDisconnects() {
	heartbeatSuppressedByUser++;
	return Lifetime([this] { heartbeatSuppressedByUser--; });
}

void Device::addExtension(std::shared_ptr<DeviceExtension>&& extension) {
	std::lock_guard<std::mutex> lk(extensionsLock);
	extensions.push_back(extension);
}

void Device::forEachExtension(std::function<bool(const std::shared_ptr<DeviceExtension>&)> fn) {
	std::vector<std::shared_ptr<DeviceExtension>> extensionsCopy;

	{
		std::lock_guard<std::mutex> lk(extensionsLock);
		extensionsCopy = extensions;
	}

	for(const auto& ext : extensionsCopy) {
		if(!fn(ext))
			break;
	}
}

void Device::handleInternalMessage(std::shared_ptr<Message> message) {
	switch(message->network.getNetID()) {
		case Network::NetID::Reset_Status:
			latestResetStatus = std::dynamic_pointer_cast<ResetStatusMessage>(message);
			break;
		case Network::NetID::Device: {
			auto canmsg = std::dynamic_pointer_cast<CANMessage>(message);
			if(canmsg)
				handleNeoVIMessage(std::move(canmsg));
			break;
		}
		case Network::NetID::DeviceStatus:
			// Device Status format is unique per device, so the devices need to decode it themselves
			handleDeviceStatus(message);
			break;
		default:
			break; //std::cout << "HandleInternalMessage got a message from " << message->network << " and it was unhandled!" << std::endl;
	}
	forEachExtension([&](const std::shared_ptr<DeviceExtension>& ext) {
		ext->handleMessage(message);
		return true; // false breaks out early
	});
}

void Device::handleNeoVIMessage(std::shared_ptr<CANMessage> message) {
	switch(message->arbid) {
		case 0x103: { // Report Message (neoVI FIRE 2)
			if(message->data.size() < 34) {
				report(APIEvent::Type::PacketDecodingError, APIEvent::Severity::EventWarning);
				return;
			}

			uint16_t emisc[2];
			memcpy(emisc, message->data.data() + 24, sizeof(emisc));
			std::lock_guard<std::mutex> lk(ioMutex);
			miscAnalog[0] = (message->data[24] | (uint16_t(message->data[25]) << 8)) * 0.01015511; // In volts now
			miscAnalog[1] = (message->data[26] | (uint16_t(message->data[27]) << 8)) * 0.01015511;
			miscDigital[0] = message->data[28] & 0x01;
			miscDigital[1] = message->data[29] & 0x01;
			miscDigital[4] = message->data[30] & 0x01;
			miscDigital[5] = message->data[31] & 0x01;
		}
	}
}

bool Device::firmwareUpdateSupported() {
	bool ret = false;
	forEachExtension([&ret](const std::shared_ptr<DeviceExtension>& ext) {
		if(ext->providesFirmware()) {
			ret = true;
			return false;
		}
		return true; // false breaks out early
	});
	return ret;
}

APIEvent::Type Device::getCommunicationNotEstablishedError() {
	if(firmwareUpdateSupported()) {
		if(requiresVehiclePower())
			return APIEvent::Type::NoSerialNumberFW12V;
		else
			return APIEvent::Type::NoSerialNumberFW;
	} else {
		if(requiresVehiclePower())
			return APIEvent::Type::NoSerialNumber12V;
		else
			return APIEvent::Type::NoSerialNumber;
	}
}

void Device::updateLEDState() {
	std::vector<uint8_t> args {(uint8_t) ledState};
	com->sendCommand(Command::UpdateLEDState, args);
}