/*
 * bit_io.cpp
 *
 *  Created on: Sep 4, 2015
 *      Author: pouryas
 */
#include <assert.h>
#include "bit_io.h"

BitStorage::BitStorage() {
	this->reset();
}

BitStorage::BitStorage(const BitStorage& rhs) {
	this->copyfrom(rhs);
}

BitStorage::BitStorage(const string& str) {
	this->from_string(str);
}

BitStorage::BitStorage(const vector<u8>& bits, u32 count_bits) {
	m_bitwise_count = count_bits;
	m_bitwise_index = m_bitwise_count;
	m_storage.assign(bits.begin(), bits.end());
}

BitStorage::~BitStorage() {

}

void BitStorage::copyfrom(const BitStorage& other) {
	m_bitwise_count = other.m_bitwise_count;
	m_bitwise_index = other.m_bitwise_index;
	m_storage.assign(other.m_storage.begin(), other.m_storage.end());
}

void BitStorage::appendfrom(const BitStorage& other) {

	for(u32 i=0; i < other.count_total_bits(); i++)
		this->write_bit((u8)other.read(1));
}

void BitStorage::reset() {
	m_bitwise_index = 0;
	m_bitwise_count = 0;
	m_storage.resize(0);
	m_storage.reserve(128);
	m_storage.push_back(0);
}

bool BitStorage::is_bit_set(u8 byte, u8 index) {
	return  ((byte & (1 << index)) & 0xFF) != 0;
}

int BitStorage::read(int count_bits) const {
	assert(count_bits > 0);

	//adjust count
	count_bits = min<int>(count_bits, (m_bitwise_count - m_bitwise_index));

	//read count bits from storage and increment the index
	assert((m_bitwise_index + count_bits) <= m_bitwise_count);

	int output = 0;
	for(int i = 0; i < count_bits; i++) {
		u32 byte_index = m_bitwise_index >> 3;
		u32 bit_index = m_bitwise_index % 8;
		m_bitwise_index ++;

		u8 byte = m_storage[byte_index];
		if(is_bit_set(byte, bit_index))
			output |= (1 << i);
	}

	return output;
}

bool BitStorage::write_bit(u8 bit) {
	u8 byte = m_storage.back();
	u8 bit_index = m_bitwise_index % 8;
	if(bit == 1)
		byte |= (1 << bit_index);
	m_bitwise_count++;
	m_bitwise_index++;
	m_storage[m_storage.size() - 1] = byte;

	//push new byte
	if(bit_index == 7) {
		m_storage.push_back(0);
	}

	return true;
}


bool BitStorage::write_multiple_bits(u32 bits, u32 length) {
	if(length == 0)
		return false;

	//order is msb to lsb
	for(int i= (int)length - 1; i >= 0; i--) {
		u8 b = (bits >> i) & 0x01;
		write_bit(b);
	}

	return true;
}

bool BitStorage::write_multiple_bits(const string& str) {
	BitStorage storage;
	storage.from_string(str);
	return true;
}

bool BitStorage::is_end() const {
	return (m_bitwise_index == m_bitwise_count);
}

int BitStorage::from_string(const string& bitstring) {
	this->reset();

	//return earlyl
	if(bitstring.length() == 0)
		return 0;

	//lsb bit is on the right side
	for(int i= (int)bitstring.length() - 1; i >= 0; i--) {
		if(bitstring[i] == '0')
			write_bit(0);
		else
			write_bit(1);
	}

	return (int)m_bitwise_count;
}

string BitStorage::to_string() const {
	string str = "";
	for(u32 i=0; i < m_bitwise_count; i++) {
		u32 blk = i / 8;
		u32 rem = i % 8;

		u8 byte = m_storage[blk];
		u8 bit = (byte >> rem) & 0x01;
		if(bit == 0)
			str += "0";
		else
			str += "1";
	}

	return str;
}

