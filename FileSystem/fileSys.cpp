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

//read the file in the file system out to console
void fileSys::readFile(std::string targetFile)
{
	/*
	std::string junk1 = "wow";
	char junk2[]
	{
		'w', 'o', 'w', 0
	};
	bool test = compStringChar(junk1, junk2); 
	if (strcmp(junk1.c_str(), junk2) == 0)
		std::cout << "Test Passed\n";
	*/ 
	
	directoryEntry* scanner = nullptr; 
	short int targetBlock;

	//get first block of file from directory
	short int firstBlock; 
	
	if (firstEntryExsist)
	{
		scanner = &firstEntry;
		do
		{
			if (compStringChar(targetFile, scanner->fileName_storage))
			{
				firstBlock = scanner->firstblockNum;
				break;
			}
			scanner = scanner->nextEntry;
		} while (scanner->nextEntry != nullptr);
	}

	if (scanner != nullptr)
	{
		targetBlock = scanner->firstblockNum; 
		while (targetBlock != -1)
		{
			//read the data from the target block
			for (int i = 0; i < 4000; i++)
			{
				char tempContainer = Drive->data[targetBlock].block_storage[i];
				if (tempContainer != -51)
					std::cout << tempContainer;
				else
					break; 
			}
			//get next block from FAT
			targetBlock = allTable->tableInfo[targetBlock]; 
		}
	}
	else
	{
		std::cout << "Specified file does not exsist\n";
		return; 
	}
}

void fileSys::readInFile(std::string newFileName, std::string poemContents)
{
	bool success; 
	//success = insertFile(newFilePath); 
	success = insertFile(newFileName, poemContents); 

	if (success == false)
	{
		std::cout << "Write Failed\n"; 
	}
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

	//buildDirectoryTable(); 

	/*
	//create empty directory 
	directoryEntry newDirectory; 
	firstEntry = &newDirectory;
	lastEntry = &newDirectory;
	*/ 
}

//build the directory table, called if one does not exsist
void fileSys::buildDirectoryTable()
{
	//FAT has to begin in the 3rd block of data 
	allTable->tableInfo[2] = -1; 
	directoryTableFirst = 2; 
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
		//std::cout << "block full, new block \n"; 
	}
}

//build file from map and insert it into the fileSystem
bool fileSys::insertFile(std::string fileName, std::string poemContents)
{
	int file_size, free_space; 
	int poemContentsCount = 0; 
	short int firstBlock, targetBlock, previousBlock; 
	int num_blocksNeeded = 0; 
	bool directoryEntryExsist = false; 

	//check if the poemName is already in the system 
	directoryEntry* scanner = nullptr; 
	int scannerTracker = 0; 

	if (firstEntryExsist)
	{	
		scanner = &firstEntry; 
		while (scanner->nextEntry != nullptr)
		{
			 //compare looking for the correct filename
			if (compStringChar(fileName, scanner->fileName_storage))
			{
				directoryEntryExsist = true; 
				std::cout << "File already exsists, overwriting...\n"; 
				break; 
			}
			scanner = scanner->nextEntry; 
			scannerTracker++; 
		} 
		directoryEntryExsist = false; 
	}
	else
	{
		directoryEntryExsist = false;
		allTable->tableInfo[2] = -1; 
		//directory table has not been built yet
	}
		
	//if file exsists, overwright else make new 
	if (directoryEntryExsist)
		targetBlock = scanner->firstblockNum; 
	else
		targetBlock = findEmptyBlock(); 

	//check if there is room in the system for the new information 

	file_size = sizeof(poemContents);

	num_blocksNeeded = (file_size / 4000);
	if ((file_size % 4000) > 0)
		num_blocksNeeded++;

	//targetBlock = findEmptyBlock();

	//read in the blocks that are needed 
	for (int i = 0; i < num_blocksNeeded; i++)	
	{
		//block will be set for start, else need a new block
		/*
		if (i == 0)
			if (!directoryEntryExsist)
				targetBlock = findEmptyBlock();
			else
				//directory exsists need to follow path 
				targetBlock = allTable->tableInfo[previousBlock]; 
		*/ 

		//update FAT table as needed
		if (i != 0)
			allTable->tableInfo[previousBlock] = targetBlock;
		else
			//just set initilly as eof, will update if more blocks are needed
			allTable->tableInfo[targetBlock] = -1; 

		int poemLength = poemContents.length(); 

		//start copying data into the blocks
		for (int k = 0; k < 4000; k++)
		{
			//std::strcpy(&Drive->data[targetBlock].block_storage[k], &poemContents[poemContentsCount]);
			if (poemContentsCount != poemLength)
			{
				Drive->data[targetBlock].block_storage[k] = poemContents[poemContentsCount];
				poemContentsCount++;
				//poemContentsCount++; 
			}
			else
				break;
		}
		previousBlock = targetBlock; 
	}
	buildDirectoryEntry(fileName, file_size, targetBlock);
	return true; 
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
	strcpy_s(newEntry.fileName_storage, newFileName.c_str());

	newEntry.fileSize = fileSize;
	newEntry.firstblockNum = startingBlock;

	//add new block to the chain
	if (!firstEntryExsist)
	{
		firstEntry = newEntry;
		lastEntry = &newEntry;
		firstEntryExsist = true; 
	}
	else
	{
		lastEntry->nextEntry = &newEntry;
		lastEntry = &newEntry;
	}

	//check how much space the chain will take up and see if a new block is needed 
	directoryEntry* entryScanner = &firstEntry;
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
	short int returningNum; 
	for (int i = 0; i < 4096; i++)
	{
		if (allTable->tableInfo[i] == -9)
		{
			//this block is empty 
			returningNum = i;
			break; 
		}
	}

	//update FAT to indicate block is taken now
	allTable->tableInfo[returningNum] = -1; 
	return returningNum; 
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

	for (int k = 0; k < numBlocksDirectory; k++)
	{

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

//returns true if the string and character[] are the same
bool fileSys::compStringChar(std::string string, char character[])
{
	int lengthOfString = string.length(); 
	char tempHolder; 
	int numSame = 0; 

	for (int i = 0; i < lengthOfString; i++)
	{
		std::cout << character[i]; 
		tempHolder = string.at(i); 
		std::cout << tempHolder; 
		if (tempHolder == character[i])
			numSame++; 
		else
			break; 
	}

	if (numSame == lengthOfString)
		return true;
	else
		return false; 
}
#pragma endregion