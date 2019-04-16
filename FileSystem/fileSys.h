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
	};
public:
	fileSys();
	~fileSys();

private: 
	void readFileSys(); 
};

