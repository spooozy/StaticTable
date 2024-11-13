#pragma once
#include "Cell.h"
#include "Header.h"
#include <vector>


class Table {
public:
	HWND parentWindow;	HDC hdc;
	int tableWidth, tableHeight;

	const int CLICK_TIMER_ID = 1;
	const int RCLICK_TIMER_ID = 2;
	bool singleClickPending = false;
	bool singleRClickPending = false;

	Table(HWND parentWindow, int tableWidth, int tableHeight, int rows, int cols);
	~Table();

	void drawTable(HDC hdc, int scrollPos);
	int rowsLeft();
	void handleLeftClick(int clickPosX, int clickPosY, const std::wstring& time);
	void handleDblLeftClick(int clickPosX, int clickPosY, const std::wstring& time);
	void handleRightClick(int clickPosX, int clickPosY, const std::wstring& time);
	void handleDblRightClick(int clickPosX, int clickPosY, const std::wstring& time);
	void updateTableParams(int newTableWidth, int newTableHeight);
	void startClickTimer();
	void stopClickTimer();
	void startRClickTimer();
	void stopRClickTimer();
	void changeCellType();

private:
	std::vector<std::vector<Cell>>cells;
	Header* header;
	int rows, cols, cellWidth, cellHeight, scrollPos = 0, visibleRows, visibleCols, minCellWidth = 10, minCellHeight = 15,
		activeCellI = 0, activeCellJ, headerHeight = 40;
	bool type = true;
	void createCells();
	void initHeaderItems();
	void setVisibleCells();
	void updateCellsParams(int cols, int rows);
	void drawTableBorders(HDC hdc);
	void changeActiveCell(int row, int col, COLORREF color);
};
