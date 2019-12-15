static const byte SLEEPCMD[19] = {
	0xAA,	// head
	0xB4,	// command id
	0x06,	// data byte 1
	0x01,	// data byte 2 (set mode)
	0x00,	// data byte 3 (sleep)
	0x00,	// data byte 4
	0x00,	// data byte 5
	0x00,	// data byte 6
	0x00,	// data byte 7
	0x00,	// data byte 8
	0x00,	// data byte 9
	0x00,	// data byte 10
	0x00,	// data byte 11
	0x00,	// data byte 12
	0x00,	// data byte 13
	0xFF,	// data byte 14 (device id byte 1)
	0xFF,	// data byte 15 (device id byte 2)
	0x05,	// checksum
	0xAB	// tail
};

static uint8_t DUTTYCMD[19] = {
	0xAA,	// head
	0xB4,	// command id
	0x08, 
	0x01, 
	0x03,
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00,
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00,
	0xFF, 
	0xFF, 
	0x0A, 
	0xAB	// tail
};