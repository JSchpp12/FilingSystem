#pragma once
class allocationTable
{
	struct table
	{
		short  int tableInfo[4096]; 
	};

public:
	allocationTable();
	~allocationTable();
};

