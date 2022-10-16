/*
daoc-logger - Copyright (c) 2022 towbes
Contact: discord afkgamer#0162
Contact: https://github.com/towbes

This program is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see < https://www.gnu.org/licenses/>.
*/

#pragma once
#include <Windows.h>
#include <atomic>
#include <mutex>


//https://stackoverflow.com/questions/70341367/using-a-c-stdvector-as-a-queue-in-a-thread

std::atomic_bool RequestPassed = false;
std::mutex RequestHandleMutex;

//void queueSendPacket(std::shared_ptr<send_packet> nextPacket) {
//    std::lock_guard<std::mutex> lg(RequestHandleMutex);
//    sendQueue.push(nextPacket);
//#ifdef _DEBUG
//    //Debug output
//    std::cout << "[Receive to Queue] Queue size: ";
//    std::cout << std::hex << (((int)nextPacket->packetHeader) & 0xff) << " ";
//    for (size_t x = 1; x < 13; ++x) {
//        std::cout << std::hex << (((int)nextPacket->packetBuffer[x]) & 0xff) << " ";
//    }
//    std::cout << std::endl << "Send queue size: " << sendQueue.size() << std::endl;
//#endif
//
//    RequestPassed.store(true);
//    RequestPassed.notify_all();
//}

/* Archived code from packet processing
* 
*       //send packet handler
*       void sendPacketHandler() {
            while (sendThreadRunning) {
                end = std::chrono::high_resolution_clock::now();
                if (sendQueue.size() > 0 && std::chrono::duration_cast<std::chrono::milliseconds>(end - begin) >= duration) {
                    //RequestPassed.wait(false);
                    if (RequestPassed.is_lock_free()) {
                        std::lock_guard<std::mutex> lg(RequestHandleMutex);
                        //copy elements 1 -> packetlen - 1 of packetBuffer into char* (element 0 is the header)
                        std::copy(&sendQueue.front()->packetBuffer[1], &sendQueue.front()->packetBuffer[sendQueue.front()->packetLen], bufferToSend);
        #ifdef _DEBUG
                        //Debug output
                        std::cout << "[Send to Send] ";
                        std::cout << std::hex << (((int)sendQueue.front()->packetHeader) & 0xff) << " ";
                        //printing copied char buffer so should be elements 0 -> 11
                        for (size_t x = 0; x < 12; ++x) {
                            std::cout << std::hex << (((int)bufferToSend[x]) & 0xff) << " ";
                        }
                        std::cout << std::endl;

        #endif
                        Send(bufferToSend, sendQueue.front()->packetHeader, sendQueue.front()->packetLen, sendQueue.front()->unknown);
                        sendQueue.pop();
        #ifdef _DEBUG
                        //Debug output
                        std::cout << std::endl << "Send queue size: " << sendQueue.size() << std::endl;

        #endif
                        RequestPassed.store(false);
                    }
                    begin = std::chrono::high_resolution_clock::now();
                }
            }
        }
* 
* 
*       UI menu case
        case SELLINV_BUTTON:
            //newBufferToSend[0] = '\x79';
            //newBufferToSend[1] = '\x0';
            //newBufferToSend[2] = '\x0';
            //newBufferToSend[3] = '\x75';
            //newBufferToSend[4] = '\x80';
            //newBufferToSend[5] = '\x0';
            //newBufferToSend[6] = '\x0';
            //newBufferToSend[7] = '\x84';
            //newBufferToSend[8] = '\x96';
            //newBufferToSend[9] = '\x0';
            //newBufferToSend[10] = '\x10';
            //newBufferToSend[11] = '\x0';
            //bufferToSend[0] = '\x79'; // packet header
            //bufferToSend[1] = '\x0';
            //bufferToSend[2] = '\x0';
            //bufferToSend[3] = '\x75';
            //bufferToSend[4] = '\x80';
            //bufferToSend[5] = '\x0';
            //bufferToSend[6] = '\x0';
            //bufferToSend[7] = '\x84';
            //bufferToSend[8] = '\x96';
            //bufferToSend[9] = '\x0';
            //bufferToSend[10] = '\x10';
            //bufferToSend[11] = '\x0';
            //bufferToSend[12] = '\x28'; // slot id
            for (int i = 40; i < 80; i++) {
                //bufferToSend[12] = i;
                //newBufferToSend[12] = i;

                //make a pointer to the buffer+1 to account for packet header
                //newBuff = bufferToSend;
                //newBuff++;
                //send first char of bufferToSend for the header
                //Subtract 1 from i to account for the packet header
                // last parameter is usually 0xffff
                //only send a packet if ther eis an item there
                if (m_readItemId(i) > 0) {
                    //Initialize an object then make it a shared pointer
                    //https://stackoverflow.com/questions/42250767/making-a-shared-pointer-to-a-new-struct-with-initialisation
                    //send_packet tmpPack = { newBuff, bufferToSend[0], 13, 0xffff };
                    //std::shared_ptr<send_packet> packetPtr(new send_packet{ newBufferToSend, (DWORD)newBufferToSend[0], 13, 0xffff });
#ifdef _DEBUG
                    //Debug output
                    //std::cout << "[Send to Queue] ";
                    //std::cout << std::hex << (((int)packetPtr->packetHeader) & 0xff) << " ";
                    //for (size_t x = 1; x < 13; ++x) {
                    //    std::cout << std::hex << (((int)packetPtr->packetBuffer[x]) & 0xff) << " ";
                    //}
                    //std::cout << std::endl;
                    std::cout << "Slot number: " << std::hex << i << std::endl;

#endif
                    SellRequest(i);
                    //Send(newBuff, bufferToSend[0], 13, 0xffff);
                }
            }
            break;



            //In main window thread
            //Start the send packet handler thread
            //std::thread sendPacketThread(sendPacketHandler);
            //sendThreadRunning = true;


            //after exit message
            //turn off the send handler
            //sendThreadRunning = false;
            //RequestPassed.store(true);
            //RequestPassed.notify_all();
            //sendPacketThread.join();




*/