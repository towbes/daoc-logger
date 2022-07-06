#pragma once
#include <Windows.h>
#include <atomic>
#include <mutex>
//Include the game header to get packet struct and queue
#include "Daoc.h"

//https://stackoverflow.com/questions/70341367/using-a-c-stdvector-as-a-queue-in-a-thread

std::atomic_bool RequestPassed = false;
std::mutex RequestHandleMutex;

void queueSendPacket(std::shared_ptr<send_packet> nextPacket) {
    std::lock_guard<std::mutex> lg(RequestHandleMutex);
    sendQueue.push(nextPacket);
#ifdef _DEBUG
    //Debug output
    std::cout << "[Receive to Queue] Queue size: ";
    std::cout << std::hex << (((int)nextPacket->packetHeader) & 0xff) << " ";
    for (size_t x = 1; x < 13; ++x) {
        std::cout << std::hex << (((int)nextPacket->packetBuffer[x]) & 0xff) << " ";
    }
    std::cout << std::endl << "Send queue size: " << sendQueue.size() << std::endl;
#endif

    RequestPassed.store(true);
    RequestPassed.notify_all();
}