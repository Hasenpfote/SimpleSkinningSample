/**
 * CRC-32-IEEE 802.3
 */
#pragma once

unsigned int calcCRC32(const unsigned char* buf, int len);
unsigned int calcCRC32(const unsigned char* string);

