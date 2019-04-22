#pragma once
class fileSys
{
	struct allocationTable
	{
		short int tableInfo[4096]; 
	};
	struct block 
	{
		//each block is 4 kilobytes 
		unsigned char block_storage[4000]; //access this in binary or something
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
		short int blockNum = -1; 
		int fileSize = 0; //size in bytes

		directoryEntry* nextEntry = nullptr; //for creating the directory structure
	};
public:
	fileSys();
	~fileSys();
	void listFileNames();

private: 
	void readFileSys(); 
};

