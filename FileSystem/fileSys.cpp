#include "pch.h"
#include "fileSys.h"

#include <bitset>

//#include "allocationTable.h"

fileSys::fileSys()
{
	//diskPath = "DiskFile.txt"; 
	fileIO.open(diskPath, std::ios::binary);

	if (fileIO.fail() == true)
		diskStatus = false;
	else
		diskStatus = true; 


	allTable = new allocationTable;
	Drive = new drive;

	if (diskStatus == false)
	{
		initilizeEmptyDisk(); 
	}
}


fileSys::~fileSys()
{

}

void fileSys::writeBlank()
{
	//initilizeEmptyDisk(); 
	writeToFile(); 
}

#pragma region Public Methods

//prints out all of the filenames in the fileSystem
void fileSys::listFileNames()
{
	//block temp;
}

//used for testing, write a short int into the file allocation table 
void fileSys::writeShort(short int newInt)
{

}
#pragma endregion

#pragma region Private Methods

void fileSys::readFileSys()
{

}

//initilize the disk (only run if the file does not exsist on the drive, so just initilize all data structs as empty 
void fileSys::initilizeEmptyDisk()
{
	allTable->tableInfo[0] = 1; 
	allTable->tableInfo[1] = -1; 

	allTableCount = allTableCount + 2; 
	for (int i = 2554554; i < 4096; i++)
	{
		allTable->tableInfo[i] = -9; //set vals in allocationTable to 'empty' 
	}
	//create empty directory 
	directoryEntry newDirectory; 
	firstEntry = &newDirectory;
	lastEntry = &newDirectory;
}

//read in disk from file into memory structures 
bool fileSys::initilizeDiskFromStorage(std::string fileName)
{
	bool reachedEnd = false;

	//double check if the file is open
	if (!fileIO.is_open())
		return false;

	for (int j = 0; j < 4096; j++)
	{
		//read in the allocation table first 
		char currentByte = readCharFromFile();
		if (fileIO.eof() == true)
		{
			//reached end of file 
			reachedEnd = true;
			break;
		}
		//store char into storage
		buildBlocks(currentByte); 

		std::bitset<8> storage(currentByte);

		for (int i = 7; i >= 0; i--)
		{
			if (storage.test(i) == true)
			{
				addBinaryShort(true);
			}
			else
			{
				addBinaryShort(false);
			}
		}
	}
	if ((reachedEnd == true) && (allTableCount < 4096))
	{
		//reached the end of file while reading -- set values to emptry y 
		for (int j = allTableCount; j < 4096; j++)
		{
			allTable->tableInfo[j] = -9; 
		}
	}
}

//primarly used for building shorts for the file allocation table
void fileSys::addBinaryShort(bool newVal)
{
	shortIntBuilder.set(shortIntBuilderCount, newVal); 
	shortIntBuilderCount--; 

	//add the constructed short to the table
	if (shortIntBuilderCount < 0)
	{
		unsigned short int tempContainer = (unsigned short int)shortIntBuilder.to_ulong(); 
		allTable->tableInfo[allTableCount];
		allTableCount++; 
	}
}

//builds the actual blocks for the data in the disk struct in memory 
void fileSys::buildBlocks(char newBlockPiece)
{ 
	Drive->data[dataCount].block_storage[blockStorageCount] = newBlockPiece; 
	blockStorageCount++; 

	if (blockStorageCount > 4000)
	{
		dataCount++; 
		std::cout << "block full, new block \n"; 
	}
}

char fileSys::readCharFromFile()
{
	char storage; 

	fileIO.read(&storage, sizeof(char)); 

	if (fileIO.eof() == true)
		return '-1';
	else
		return storage; 
}

//write virtual disk to file 
void fileSys::writeToFile()
{
	fileIO.seekp(std::ios::beg);

	//write the file allocation table
	for (int i = 0; i < 4000; i++)
	{
		fileIO << allTable->tableInfo[i]; 
	}

	for (int j = 0; j < numBlocksDirectory; j++) //go through each block 
	{
		//write contents of block to disk
		block* currentBlock = &Drive->data[j]; 
		for (int k = 0; k < 4000; k++)
		{
			fileIO << currentBlock->block_storage[k]; 
		}
	}
}
#pragma endregion