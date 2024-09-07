#include <Arduino.h>

#include <CANRxBuffer.h>

#include <Console.h>

extern Console console;

CANRxBuffer::CANRxBuffer() {
  queue = xQueueCreateStatic(
    CAN_RX_BUFFER_SIZE, // uxQueueLength
    sizeof(CANPacket),  // uxItemSize
    buffer,             // pucQueueStorage
    &overhead           // pxQueueBuffer
  );
}

void CANRxBuffer::push(const CANPacket& packet) {
  CANPacket trashcan;  // using nullptr for receive causes segfault
  if(xPortInIsrContext()){
    if (uxQueueMessagesWaiting(queue) >= CAN_RX_BUFFER_SIZE) {
      xQueueReceiveFromISR(queue, &trashcan, 0); // discard item 
    }
    xQueueSendFromISR(queue, &packet, 0);
    // if (!xQueueSendFromISR(queue, &packet, 0)) { // try push
    //   // -> push failed (queue full)
    //   xQueueReceiveFromISR(queue, &trashcan, 0); // discard item 
    //   xQueueSendFromISR(queue, &packet, 0); // retry push
    //   console << "drop pkg" << NL; 
    //   /*
    //    * overwriting items is desired, but not natively possible with freeRTOS.
    //    * messages support this, but only if the length is = 1 ("mailbox")
    //    */
    // }
  } 
  else {
    // the same in non-ISR
    if (!xQueueSend(queue, &packet, 0)) {
      xQueueReceive(queue, &trashcan, 0);
      xQueueSend(queue, &packet, 0);
    }
  }
}

CANPacket CANRxBuffer::pop() {
  CANPacket packet;
  bool success;

  if(xPortInIsrContext()){
    success = xQueueReceiveFromISR(queue, &packet, 0);
  }
  else {
    success = xQueueReceive(queue, &packet, 0);
  }
  if (!success) {
    packet.setId(0);
    packet.setData(static_cast<uint64_t>(0));
  }

  return packet;
}

bool CANRxBuffer::isAvailable() { return (uxQueueMessagesWaiting(queue) > 0); }

uint16_t CANRxBuffer::getSize() { return uxQueueMessagesWaiting(queue); }