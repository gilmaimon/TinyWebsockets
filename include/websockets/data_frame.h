#pragma once

#include "common.h"
#include <iostream> // for loggin
#include <bitset> // for logging

struct WebsocketsFrame {
	uint8_t fin : 1;
	uint8_t opcode : 4;
	uint8_t mask : 1;
	uint8_t mask_buf[4];
	uint64_t payload_length;
	String payload;

	void log() {
		std::cout << "Frame:" << std::endl;
		std::cout << "-> fin: " << (int)fin << std::endl;
		std::cout << "-> opcode: " << std::hex << (int)opcode << std::dec << " (" << std::bitset<4>(opcode) << ")" << std::endl;
		std::cout << "-> len: " << payload_length << " (" << std::bitset<64>(payload_length) << ")" << std::endl;
		std::cout << "-> mask: " << (int)mask << std::endl;
		std::cout << "-> payload: " << payload << std::endl;
	}
};