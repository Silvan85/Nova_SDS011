#pragma once
#include "NovaSDS011.h"

static CommandType REPORTTYPECMD = {
	0xAA, // head
	0xB4, // command id
	0x02, // data byte 1
	0x01, // data byte 2 (0：query the current mode 1：set reporting mode)
	0x00, // data byte 3 (0：report active mode 1：Report query mode)
	0x00, // data byte 4
	0x00, // data byte 5
	0x00, // data byte 6
	0x00, // data byte 7
	0x00, // data byte 8
	0x00, // data byte 9
	0x00, // data byte 10
	0x00, // data byte 11
	0x00, // data byte 12
	0x00, // data byte 13
	0xFF, // data byte 14 (device id byte 1)
	0xFF, // data byte 15 (device id byte 2)
	0x00, // checksum
	0xAB  // tail
};

static ReplyType REPORTTYPEREPLY = {
	0xAA, // head
	0xC5, // command id
	0x02, // data byte 1
	0x01, // data byte 2 (0：query the current mode 1：set reporting mode)
	0x00, // data byte 3 (0：report active mode 1：Report query mode)
	0x00, // data byte 4
	0x00, // data byte 5 (device id byte 1)
	0x00, // data byte 6 (device id byte 2)
	0x00, // checksum
	0xAB  // tail
};

static CommandType QUERYCMD = {
	0xAA, // head
	0xB4, // command id
	0x04, // data byte 1
	0x00, // data byte 2
	0x00, // data byte 3
	0x00, // data byte 4
	0x00, // data byte 5
	0x00, // data byte 6
	0x00, // data byte 7
	0x00, // data byte 8
	0x00, // data byte 9
	0x00, // data byte 10
	0x00, // data byte 11
	0x00, // data byte 12
	0x00, // data byte 13
	0xFF, // data byte 14 (device id byte 1)
	0xFF, // data byte 15 (device id byte 2)
	0x00, // checksum
	0xAB  // tail
};

static ReplyType QUERYREPLY = {
	0xAA, // head
	0xC0, // command id
	0x00, // data byte 1 (PM2.5 low byte)
	0x00, // data byte 2 (PM2.5 high byte)
	0x00, // data byte 3 (PM10 low byte)
	0x00, // data byte 4 (PM10 high byte)
	0x00, // data byte 5 (device id byte 1)
	0x00, // data byte 6 (device id byte 2)
	0x00, // checksum
	0xAB  // tail
};

static CommandType SETIDCMD = {
	0xAA, // head
	0xB4, // command id
	0x05, // data byte 1
	0x00, // data byte 2
	0x00, // data byte 3
	0x00, // data byte 4
	0x00, // data byte 5
	0x00, // data byte 6
	0x00, // data byte 7
	0x00, // data byte 8
	0x00, // data byte 9
	0x00, // data byte 10
	0x00, // data byte 11
	0x00, // data byte 12 (new device id byte 1)
	0x00, // data byte 13 (new device id byte 2)
	0xFF, // data byte 14 (device id byte 1)
	0xFF, // data byte 15 (device id byte 2)
	0x00, // checksum
	0xAB  // tail
};

static ReplyType SETIDREPLY = {
	0xAA, // head
	0xC5, // command id
	0x05, // data byte 1 
	0x00, // data byte 2 
	0x00, // data byte 3 
	0x00, // data byte 4 
	0x00, // data byte 5 (new device id byte 1)
	0x00, // data byte 6 (new device id byte 2)
	0x00, // checksum
	0xAB  // tail
};








static const CommandType SLEEPCMD = {
	0xAA, // head
	0xB4, // command id
	0x06, // data byte 1
	0x01, // data byte 2 (set mode)
	0x00, // data byte 3 (sleep)
	0x00, // data byte 4
	0x00, // data byte 5
	0x00, // data byte 6
	0x00, // data byte 7
	0x00, // data byte 8
	0x00, // data byte 9
	0x00, // data byte 10
	0x00, // data byte 11
	0x00, // data byte 12
	0x00, // data byte 13
	0xFF, // data byte 14 (device id byte 1)
	0xFF, // data byte 15 (device id byte 2)
	0x05, // checksum
	0xAB  // tail
};

static CommandType DUTTYCMD = {
	0xAA, // head
	0xB4, // command id
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
	0xAB // tail
};

static CommandType VERSIONCMD = {
	0xAA,
	0xB4,
	0x07,
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
	0x00,
	0x00,
	0xFF,
	0xFF,
	0x05,
	0xAB // tail
};