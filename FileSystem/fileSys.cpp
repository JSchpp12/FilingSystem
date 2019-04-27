#include "pch.h"
#include "fileSys.h"

#include <bitset>

//#include "allocationTable.h"

fileSys::fileSys()
{
	//diskPath = "DiskFile.txt"; 
	fileIO.open(diskPath, std::ios::binary | std::ios::in | std::ios::out);

	if (fileIO.fail() == true)
		diskStatus = false;
	else
		diskStatus = true; 


	allTable = new allocationTable;
	Drive = new drive;

	if (diskStatus == false)
		initilizeEmptyDisk();
	else
		initilizeDiskFromStorage(diskPath); 
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

void fileSys::readInFile(std::string newFilePath)
{
	bool success; 
	success = insertFile(newFilePath); 
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
	for (int i = 2; i < 4096; i++)
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
		/*
		char currentByte = readCharFromFile();
		if (fileIO.eof() == true)
		{
			//reached end of file 
			reachedEnd = true;
			break;
		}
		//store char into storage
		buildBlocks(currentByte); 
		*/ 
		short int allInfo; 
		fileIO >> allInfo; 

		//add the int to the all table 
		allTable->tableInfo[allTableCount] = allInfo; 
		allTableCount++;

		//std::bitset<16> storage(currentByte);

		/*
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
		*/ 
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

//called to insert a file into the file system
bool fileSys::insertFile(std::string newFilePath)
{
	int file_size, free_space, firstBlock; 
	short int targetBlock, previousBlock; 
	int num_blocksNeed = 0;
	std::ifstream newFileReader;
	newFileReader.open(newFilePath, std::ios::ate | std::ios::binary);

	//check if file is open
	if (newFileReader.is_open() == false)
	{
		std::cout << "Filed to open the file\n";
		return false; //failed to open file 
	}

	//check if there is room for the file 
	file_size = newFileReader.tellg(); //number of bytes in file 
	num_blocksNeed = (file_size / 4000);
	if ((file_size % 4000) > 0)
		num_blocksNeed++;

	std::cout << "Number of blocks needed " << num_blocksNeed << "\n";
	newFileReader.seekg(0, std::ios::beg); //move file reader back to beginning of file 

	//build new directory entry for the file 
	targetBlock = findEmptyBlock;
	buildDirectoryEntry(newFilePath, file_size, targetBlock); 

	//read in the blocks that are needed 
	for (int i = 0; i < num_blocksNeed; i++)
	{
		targetBlock = findEmptyBlock();
		block* newBlock = new block;

		if (i == 0)
			firstBlock = targetBlock;
		else
		{
			//update file allocation table 
			allTable->tableInfo[previousBlock] = targetBlock; 
		}

		//read the file into the new block structure 
		for (int k = 0; k < 4000; k++)
		{
			char* tempStorage = new char;
			newFileReader >> tempStorage;

			//check if hit end of file 
			if (newFileReader.eof() == true)
				break;
			newFileReader >> Drive->data[targetBlock].block_storage[k];


		}
		previousBlock = firstBlock;
	}
	//insert directory entry
	//start inserting data into blocks and update file allocation table as needed 
}

//builds a directory entry for the new file being inserted 
void fileSys::buildDirectoryEntry(std::string newFileName, int fileSize, int startingBlock)
{
	//check if the directory table is empty 
	if (directoryTableFirst == -9)
	{
		//build directory table 
		short int directoryTableTarget = findEmptyBlock();
		directoryTableFirst = directoryTableTarget;

		//update FAT table
		allTable->tableInfo[directoryTableTarget] = -1; //mark this as eof since the directory will only take up one block at start
	}

	//build the entry and add it to the chain 
	directoryEntry newEntry;
	strcpy(newEntry.fileName_storage, newFileName.c_str());

	newEntry.fileSize = fileSize;
	newEntry.firstblockNum = startingBlock;

	//add new block to the chain
	if (firstEntry == nullptr)
	{
		firstEntry = &newEntry;
		lastEntry = &newEntry;
	}
	else
	{
		lastEntry->nextEntry = &newEntry;
		lastEntry = &newEntry;
	}

	//check how much space the chain will take up and see if a new block is needed 
	directoryEntry* entryScanner = firstEntry;
	int numEntries = 1;
	while (entryScanner->nextEntry != nullptr)
	{
		entryScanner = entryScanner->nextEntry;
		numEntries++;
	}

	if (allTable->tableInfo[2] == -9)
	{
		allTable->tableInfo[2] = -1;
	}

	int sizeofDirectory = (sizeof(directoryEntry) - sizeof(entryScanner->nextEntry)) * numEntries; //calc size of current directory ---- MIGHT BE WRONG SINCE THERE IS A POINTER IN THE STRUCTURE
	if (sizeofDirectory > (sizeof(block) * numBlocksDirectory))
	{
		short int targetBlock = findEmptyBlock(); //find next block for FAT

		//size has exceeded blocks allocated for the table -- allocate another block for the table 
		//follow the chain of the FAT table (will always start on the 3rd block of the filesystem 
		short int nextBlock; 
		nextBlock = allTable->tableInfo[2]; 
		do
		{
			nextBlock = allTable->tableInfo[nextBlock]; 
		} while (nextBlock != -1); 

		allTable->tableInfo[nextBlock] = targetBlock; //set next block in the chain 
	}
}

//returns index of empty block based on file allocation table data 
short int fileSys::findEmptyBlock()
{
	for (int i = 0; i < 4096; i++)
	{
		if (allTable->tableInfo[i] == -9)
			//this block is empty 
			return i; 
	}
}

//returns number of free blocks based on file allocation table data
int fileSys::getNumOfFreeBlocks()
{
	int numFree = 0; 
	for (int i = 0; i < 4096; i++)
	{
		if (allTable->tableInfo[i] = -9)
			numFree++; 
	}
	return numFree; 
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
	if (fileIO.is_open() == false)
	{
		fileIO.open(diskPath, std::ios::out | std::ios::binary); 
	}
	fileIO.seekp(std::ios::beg);

	//write the file allocation table
	for (int i = 0; i < 4096; i++)
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