/*
 * recv_remote_task.c
 *
 * Created: 28.10.2020 09:55:43
 *  Author: Franz Forster
 */ 
#include <asf.h>
#include "core/recv_remote_task.h"
#include "core/taskhost.h"
#include "core/LT89xx.h"
#include "dmx/dmx_output_task.h"
#include "common/dbg_tools.h"

// -------------------------------------------------------------------------------------------------
// Constants (local for access control)
// -------------------------------------------------------------------------------------------------

#define REMOTE_CMD_BUFFER_SIZE		32

typedef enum {
	MESSAGE_OK,
	MESSAGE_REPEAT,
	MESSAGE_INVALID
} eMessageValid;


typedef union {
	struct {
		uint8_t		_leader;
		uint8_t		_counter;
		uint8_t		protocolType;	// is always 0x80
		uint8_t		remoteType;		// 0x31 = 1Ch remote, 0x34 = 4Ch remote
		uint8_t		_dummy1;
		uint16_t	remoteAddress;
		uint16_t	_dummy2;
		uint8_t		zoneCode;		// bitfield 
		uint8_t		_dummy3;
		uint16_t	cmdCode;
		uint8_t		cmdData[];
	} __attribute__ ((packed));
	uint8_t bytes[REMOTE_CMD_BUFFER_SIZE];
} tuRemoteCmdPacket;

// -------------------------------------------------------------------------------------------------
// Variables (local for access control)
// -------------------------------------------------------------------------------------------------

volatile uint8_t remoteDataAvailable = 0;

// -------------------------------------------------------------------------------------------------
// Prototypes (local for access control)
// -------------------------------------------------------------------------------------------------

void	_RemoteSetup(void);
void	_RemoteDataAvailableCallback(void);
void	_PrintReceivedCmd(const uint8_t* buffer, const uint8_t len);
eMessageValid _IsMessageValid(uint8_t* message, uint8_t length);
eMessageValid _MatchAddress(tuRemoteCmdPacket* msgPacket);

// -------------------------------------------------------------------------------------------------
/*
	Description: 

*/
void Recv_Remote_Task(void* param)
{
	QueueHandle_t* pRemoteCommandQueue = (QueueHandle_t*)param;
	tuRemoteCmdPacket remoteCmdBuffer;
	uint8_t remoteCmdLength;
	
	uint8_t result = 0;
	
	// ptr for queue
	tRemoteCmd* newRemoteCmd;

	
	_RemoteSetup();
	
	while (1)
	{
		// Suspend on entry
		// vTaskSuspend(NULL);
		vTaskDelay(pdMS_TO_TICKS(5000));
		// -> Task resumed from _RemoteDataAvailableCallback ISR
		if (remoteDataAvailable) {
			vTaskSuspendAll();
			remoteDataAvailable = 0;
			LT89XX_Read(remoteCmdBuffer.bytes, REMOTE_CMD_BUFFER_SIZE, &remoteCmdLength);
			LT89XX_StartListening();
			xTaskResumeAll();
			result = _IsMessageValid(remoteCmdBuffer.bytes, remoteCmdLength);
			if (result == MESSAGE_OK) {
				_PrintReceivedCmd(remoteCmdBuffer.bytes, remoteCmdLength);
				_MatchAddress(&remoteCmdBuffer);
				if (_MatchAddress(&remoteCmdBuffer) == MESSAGE_OK && uxQueueSpacesAvailable(*pRemoteCommandQueue) > 0) {
					if (remoteCmdBuffer.cmdCode == 0x0701) {
						// Color Packet
						newRemoteCmd = pvPortMalloc(sizeof (tRemoteCmd));
						newRemoteCmd->cmdCode = CMD_STATIC_COLOR;
						newRemoteCmd->color.r = remoteCmdBuffer.cmdData[1];
						newRemoteCmd->color.g = remoteCmdBuffer.cmdData[2];
						newRemoteCmd->color.b = remoteCmdBuffer.cmdData[3];
						newRemoteCmd->color.w = 0;
						
						xQueueSend(*pRemoteCommandQueue, &newRemoteCmd, 0);
					}
					else if (remoteCmdBuffer.cmdCode == 0x0308) {
						// White Key
						newRemoteCmd = pvPortMalloc(sizeof (tRemoteCmd));
						newRemoteCmd->cmdCode = CMD_TOGGLE_WHITE;
						xQueueSend(*pRemoteCommandQueue, &newRemoteCmd, 0);
					}
					else if (remoteCmdBuffer.cmdCode == 0x0303) {
						// Change Color Mode
						newRemoteCmd = pvPortMalloc(sizeof (tRemoteCmd));
						newRemoteCmd->cmdCode = CMD_DYNAMIC_PROG;
						newRemoteCmd->dynamicProgNr = remoteCmdBuffer.cmdData[1];
						if (newRemoteCmd->dynamicProgNr > 0x0a) newRemoteCmd->dynamicProgNr = 0x0a; // Limit

						xQueueSend(*pRemoteCommandQueue, &newRemoteCmd, 0);
					}
					else if (remoteCmdBuffer.cmdCode == 0x0206) {
						// Change speed
						newRemoteCmd = pvPortMalloc(sizeof (tRemoteCmd));
						newRemoteCmd->cmdCode = CMD_SPEED;
						newRemoteCmd->speed = (remoteCmdBuffer.cmdData[2]) * 6;
						
						xQueueSend(*pRemoteCommandQueue, &newRemoteCmd, 0);
						DBG_INFO("Speed change: %u", newRemoteCmd->speed);
					}
					else if (remoteCmdBuffer.cmdCode == 0x010a) {
						// Turn off/on
						newRemoteCmd = pvPortMalloc(sizeof (tRemoteCmd));
						if (remoteCmdBuffer.cmdData[1] == 0x00) {
							newRemoteCmd->cmdCode = CMD_OFF;
						} else {
							newRemoteCmd->cmdCode = CMD_ON;
						}
						xQueueSend(*pRemoteCommandQueue, &newRemoteCmd, 0);
						DBG_INFO("Free Heap: %u", xPortGetFreeHeapSize());
					}
				}
			} else {
				DBG_TRACE("Remote Message discarded, code: %u", result);
			}
		}
		else {
			// restart listener
			vTaskSuspendAll();
			LT89XX_StartListening();
			xTaskResumeAll();
		}
	}
}


void _RemoteSetup(void)
{
	LT89XX_Init();
	LT89XX_Register_PKT_Callback(_RemoteDataAvailableCallback);
	LT89XX_StartListening();
	
}



void _RemoteDataAvailableCallback(void)
{
	remoteDataAvailable = 1;
	xTaskAbortDelay(RecvRemoteTaskHandle);
}



eMessageValid _IsMessageValid(uint8_t* message, uint8_t length)
{
	static uint8_t lastCount = 0;
	uint8_t checksumValue, i;

	if (message[0] != 0x7e || message[length-1] != 0x7e) { // leader and trailer
		return MESSAGE_INVALID;
	}
	
	checksumValue = message[1]; 
	for (i = 2; i < (length-2); i++) {
		checksumValue ^= message[i];
	}
	
	if (checksumValue != message[length-2]) { // checksum error
		return MESSAGE_INVALID;
	}
	
	if (message[1] == lastCount) {
		return MESSAGE_REPEAT;
	} else {
		lastCount = message[1];
	}
	
	return MESSAGE_OK;
}

void _PrintReceivedCmd(const uint8_t* buffer, const uint8_t len)
{
	uint8_t i;
	DBG_OUT_PRINTF("Received CMD: ");
	for (i = 0; i < len; ++i) {
		DBG_OUT_PRINTF("%02x ", buffer[i]);
	}
	DBG_OUT_PRINTF(" | Length: %u \r", len);
}

eMessageValid _MatchAddress(tuRemoteCmdPacket* msgPacket)
{
	if (msgPacket->protocolType != 0x80) {
		return MESSAGE_INVALID;
	}
	if (msgPacket->remoteType != 0x31 && msgPacket->remoteType != 0x34) {
		return MESSAGE_INVALID;
	}
	return MESSAGE_OK;
}