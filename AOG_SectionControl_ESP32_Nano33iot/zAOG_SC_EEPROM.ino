
//--------------------------------------------------------------
//  EEPROM Data Handling 11. M�rz 2020
//--------------------------------------------------------------
#define EEPROM_SIZE 512
#define EE_ident1 0xED  // Marker Byte 0 + 1
#define EE_ident2 0x41


//--------------------------------------------------------------
//  Restore EEprom Data
//--------------------------------------------------------------
void restoreEEprom() {
	Serial.println("read values from EEPROM");
	byte ECheck = EEprom_empty_check();
	if (ECheck == 1 || EEPROM_clear) { //first start?
		EEprom_write_all();     //write default data
	}
	if (ECheck == 2) { //data available
		EEprom_read_all();
	}
	if (SCSet.debugmode) { EEprom_show_memory(); }
}

//--------------------------------------------------------------
byte EEprom_empty_check() {
#if HardwarePlatform  == 1
	if (!EEPROM.isValid()) //low on first run, high, if data is there
	{
		Serial.println("no EEPROM data"); delay(1000);
		return 1;//0
	}
#endif
#if HardwarePlatform  == 0
	if (!EEPROM.begin(EEPROM_SIZE))
	{
		Serial.println("init EEPROM failed"); delay(1000);
		return 0;
	}
#endif

	if ((EEPROM.read(0) != EE_ident1) || (EEPROM.read(1) != EE_ident2))
	{
		Serial.println("no valid EEPROM data");
		return 1;  // 1 = is empty
	}
	if ((EEPROM.read(0) == EE_ident1) && (EEPROM.read(1) == EE_ident2))
	{
		Serial.println("EEPROM data found");
		return 2;     // data available
	}
}
//--------------------------------------------------------------
void EEprom_write_all() {
	byte leng = sizeof(SCSet);
	byte tempbyt = EEprom_empty_check();
	if ((tempbyt == 0) || (tempbyt == 1) || (EEPROM_clear)) {
		//EEPROM.put((4 + sizeof(SCSet)), SCSet); 
		Serial.print("rewriting EEPROM + write 2. set at #"); Serial.println(4 + leng);
		//write 2. time with defaults to be able to reload them  
		for (byte n = 0; n < leng; n++) {
			EEPROM.write(n + 4 + leng, ((unsigned char*)(&SCSet))[n]);
			delay(2);
		}
		delay(50);
		EEPROM.commit();
		delay(50);
		EEPROM.write(0, EE_ident1);	delay(2);
		EEPROM.write(1, EE_ident2); delay(2);
	}
	for (byte n = 0; n < leng; n++) {
		EEPROM.write(n + 3, ((unsigned char*)(&SCSet))[n]);
		delay(2);
	}
	//EEPROM.put(3, SCSet);
	delay(50);
	EEPROM.commit();
	delay(50);
}
//--------------------------------------------------------------
void EEprom_read_all() {
	byte leng =  sizeof(SCSet);
	Serial.print(leng);
	Serial.println(" Bytes reading from EEPROM ");
	for (byte n = 0; n < leng; n++) {
		((unsigned char*)(&SCSet))[n] = EEPROM.read(n + 3);
	}
	//	EEPROM.get(3, SCSet);

}
//--------------------------------------------------------------
void EEprom_read_default() {
	byte leng = sizeof(SCSet);
	for (byte n = 0; n < leng; n++) {
		((unsigned char*)(&SCSet))[n] = EEPROM.read(n + 4 + leng);
	}
	//EEPROM.get(4 + sizeof(SCSet), SCSet);
	Serial.print("load default value from EEPROM at #"); Serial.println(4 + sizeof(SCSet));
}

//--------------------------------------------------------------

void EEprom_block_restart() {
	if (EEPROM.read(2) == 0) {//prevents from restarting, when webpage is reloaded. Is set to 0, when other ACTION than restart is called
		EEPROM.write(2, 1);
		delay(2);
		EEPROM.commit();
		delay(50);
	}
}

//--------------------------------------------------------------

void EEprom_unblock_restart() {
	if (EEPROM.read(2) != 0) {
		EEPROM.write(2, 0); // reset Restart blocker
		delay(2);
		EEPROM.commit();
		delay(50);
	}
}

//--------------------------------------------------------------

void EEprom_show_memory() {
	byte c2 = 0, data_;
	Serial.print(EEPROM_SIZE, 1);
	Serial.println(" bytes read from Flash. Values are:");
	for (int i = 0; i < EEPROM_SIZE; i++)
	{
		data_ = byte(EEPROM.read(i));
		if (data_ < 0x10) Serial.print("0");
		Serial.print(data_, HEX);
		if (c2 == 15) {
			Serial.print(" ");
		}
		else if (c2 >= 31) {
			Serial.println(); //NL
			c2 = -1;
		}
		else Serial.print(" ");
		c2++;
	}
}

