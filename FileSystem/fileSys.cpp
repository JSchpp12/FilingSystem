#include "pch.h"
#include "fileSys.h"

//#include "allocationTable.h"

fileSys::fileSys()
{
	//diskPath = "DiskFile.txt"; 
	std::ifstream inFile;
	inFile.open(diskPath); 

	if (inFile.fail() == true)
		diskStatus = false;
	else
		diskStatus = true; 


	currAllocationTable = new allocationTable;
	currDrive = new drive;
}


fileSys::~fileSys()
{

}

#pragma region Public Methods

//prints out all of the filenames in the fileSystem
void fileSys::listFileNames()
{
	block temp;
}


#pragma endregion

#pragma region Private Methods

void fileSys::readFileSys()
{

}

//initilize the disk (only run if the file does not exsist on the drive, so just initilize all data structs as empty 
void fileSys::initilizeEmptyDisk()
{
	for (int i = 0; i < 4096; i++)
	{
		currAllocationTable->tableInfo[i] = -1; //set vals in allocationTable to 'empty' 
	}
}

bool fileSys::initilizeDiskFromStorage(std::string fileName)
{
	std::ifstream fileReader;
	fileReader.open(diskPath, std::ios::binary); 
	if (!fileReader.is_open())
		return false; 

	do
	{
		char currentChar;
		fileReader.read(&currentChar, sizeof(char));

	} while (fileReader.eof());
}
#pragma endregion