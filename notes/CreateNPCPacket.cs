NPC Create Packet structure

pak.WriteShort((ushort)npc.ObjectID);				//0x00
pak.WriteShort((ushort)(speed));					//0x02
pak.WriteShort(npc.Heading);						//0x04
pak.WriteShort((ushort)npc.Z);						//0x06
pak.WriteInt((uint)npc.X);							//0x08
pak.WriteInt((uint)npc.Y);							//0xC
pak.WriteShort(speedZ);								//0x10
pak.WriteShort(npc.Model);							//0x12
pak.WriteByte(npc.Size);							//0x13
pak.WriteByte(level);								//0x14
pak.WriteByte(flags);								//0x15	
pak.WriteByte(0x20); // max stick					//0x16
pak.WriteByte(flags2); // flags 2					//0x17
pak.WriteByte(flags3); // new in 1.71 (region instance ID from StoC_0x20) OR f //0x18
pak.WriteShort(0x00); // new in 1.71 unknown		//0x19
pak.WritePascalString(name);						//0x1A - 20 chars?
pak.WritePascalString(guildName.Substring(0, 47));	//0x3A or //0x3B?
pak.WriteByte(0x00);								//~0x82