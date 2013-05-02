#ifndef _GUIGRIDCTRL_H_
#define _GUIGRIDCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#include "graphics/dgl.h"
#include "console/console.h"
#include "console/consoleTypes.h"

class GuiGridControl : public GuiControl
{
private:

    struct GridItem
	{
		int Size;
		bool IsPercentage;
		bool IsRemaining;
		bool IsAbsolute;
	};

private:
	typedef GuiControl Parent;

	Vector<StringTableEntry> mGridRows;
	Vector<StringTableEntry> mGridCols;

	Vector<S32> mRowSizes;
	Vector<S32> mColSizes;
	Vector<Point2I> mOrginalControlPos;

	void AdjustGrid(const Point2I& newExtent);
	void AdjustGridItems(S32 size, Vector<StringTableEntry>& strItems, Vector<S32>& items);
	RectI GetGridRect(GuiControl* ctrl);
	bool IsPointInGridControl(GuiControl* ctrl, const Point2I& pt);


public:
	GuiGridControl();

	void resize(const Point2I &newPosition, const Point2I &newExtent);
	void inspectPostApply();

	void addObject(SimObject *obj);
	void removeObject(SimObject *obj);
	bool onWake();
	void onSleep();

	static void initPersistFields();
	DECLARE_CONOBJECT(GuiGridControl);
};

#endif // _GUIGRIDCTRL_H_