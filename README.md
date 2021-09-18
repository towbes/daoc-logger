# daoc-logger
Logging DLL for DAOC

Huge thanks to erarnitox for the great walkthrough posted on GuidedHacking.com! https://guidedhacking.com/threads/how-to-make-an-mmo-bot-mmorpg-bot-automation.15173/

You can see their repository for Tera Proxy on github here: https://github.com/Erarnitox/TeraProxy

This is a work in progress.  Currently just have a skeleton built that is using the Microsoft Detours framework to hook the ws2_32.send and ws2_32.wsasend API calls.

It captures packets that match the packets seen in Wireshark, which from what I have read about DAOC means those packets are encrypted inside of game.dll before being sent to the send function

Still working on trying to reverse that part

Action shot of send logging:

![Send Log](https://github.com/towbes/daoc-logger/blob/master/Images/sendlogging.JPG?raw=true)

The Hook on ws2_32.send:

![send hook](https://github.com/towbes/daoc-logger/blob/master/Images/ws2_32sendjump.JPG?raw=true)

And the mysend function in memory:

![Send Log](https://github.com/towbes/daoc-logger/blob/master/Images/mysend.JPG?raw=true)