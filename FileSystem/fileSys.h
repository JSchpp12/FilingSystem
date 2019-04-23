#pragma once
#include "fileSys.h"

#include <fstream>
#include <iostream>
#include <string>
#include <bitset>


class fileSys
{
	struct allocationTable
	{
		short int tableInfo[4096]; 
	};
	struct block 
	{
		//each block is 4 kilobytes 
		char block_storage[4000]; //access this in binary or something
	};
	struct drive
	{
		//4096 blocks 
		block data[4096]; 
	};
	struct directoryEntry
	{
		//32 bytes to hold filename 
		//short int indicating starting block number for the file 
		//int to indicate number of bytes in the file 
		unsigned char fileName_storage[32]; 
		short int firstblockNum = -1; 
		int fileSize = 0; //size in bytes

		directoryEntry* nextEntry = nullptr; //for creating the directory structure
	};
 
	directoryEntry* firstEntry = nullptr; //store the first entry in the file
	directoryEntry* lastEntry = nullptr; 

	std::fstream fileIO; 
	std::string diskPath = "Disk.txt"; //name for disk storage
	bool diskStatus = false; //set to false if the diskfile does not exsist on disk
	
	int numDirectoryEntry = 0; //number of entries in the directory 
	allocationTable* allTable; 
	drive* Drive; 

	int numBlocksDirectory = 0; //number of blocks the directory table will take up 

	std::bitset<16> shortIntBuilder; 
	int allTableCount = 0; 
	int shortIntBuilderCount = 15; 

	int blockStorageCount = 0; //counter used to keep track of which char is the next empty one in the block storage
	int dataCount = 0; //which block is the next empty one 

	

public:
	fileSys();
	~fileSys();
	void listFileNames();
	void writeShort(short int newInt); 
	void writeBlank(); 
private: 
	void readFileSys(); 
	void populateDirectory();
	void initilizeEmptyDisk(); 
	bool initilizeDiskFromStorage(std::string fileName);
	void addBinaryShort(bool newVal); 
	void buildBlocks(char newBlockPiece);
	char readCharFromFile(); 
	void writeToFile(); 
};

