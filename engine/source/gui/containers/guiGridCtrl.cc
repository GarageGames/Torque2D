//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "gui/containers/guiGridCtrl.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(GuiGridControl);

//------------------------------------------------------------------------------

GuiGridControl::GuiGridControl()
{
	mIsContainer = true;
}

//------------------------------------------------------------------------------

void GuiGridControl::initPersistFields()
{
	Parent::initPersistFields();

	addField("Rows",		TypeStringTableEntryVector, Offset(mGridRows, GuiGridControl), "Number of rows in the grid");
	addField("Columns",     TypeStringTableEntryVector, Offset(mGridCols, GuiGridControl), "Number of columns in the grid");
}

//------------------------------------------------------------------------------

bool GuiGridControl::onWake()
{
	if (!Parent::onWake())
        return false;

	return true;
}

//------------------------------------------------------------------------------

void GuiGridControl::onSleep()
{
	Parent::onSleep();
}

//------------------------------------------------------------------------------

void GuiGridControl::inspectPostApply()
{
    resize(getPosition(), getExtent());
}

//------------------------------------------------------------------------------

bool GuiGridControl::IsPointInGridControl(GuiControl* ctrl, const Point2I& pt)
{
	if (mRowSizes.size() > 0 && mColSizes.size() > 0)
	{
		RectI gridRect = GetGridRect(ctrl);
		RectI ctrlRect = ctrl->getBounds();

		Point2I chkPt = gridRect.point + ctrlRect.point;
		Point2I chkBound = chkPt + ctrlRect.extent;

		if (pt.x >= chkPt.x && pt.x <= chkBound.x && pt.y >= chkPt.y && pt.y <= chkBound.y)
			return true;
		else
			return false;
	}
	else
    {
		return false;
    }

}

//------------------------------------------------------------------------------

void GuiGridControl::addObject(SimObject *obj)
{
	if (mRowSizes.size() <= 0 && mRowSizes.size() <= 0)
		AdjustGrid(mBounds.extent);

	GuiControl *ctrl = static_cast<GuiControl *>(obj);
	
    if (ctrl)
	{
		RectI ctrlRect = GetGridRect(ctrl);
		if (ctrl->getExtent().isZero())
		{
			ctrl->setExtent(ctrlRect.extent);
		}
		else
		{
			if (ctrl->mBounds.extent.x > ctrlRect.extent.x)
				ctrl->mBounds.extent.x = ctrlRect.extent.x;
			if (ctrl->mBounds.extent.y > ctrlRect.extent.y)
				ctrl->mBounds.extent.y = ctrlRect.extent.y;
		}

		Point2I pt = ctrl->getPosition();
		mOrginalControlPos.push_back(pt);
		pt += ctrlRect.point;
		ctrl->setPosition(pt);
	}

	Parent::addObject(obj);
}

//------------------------------------------------------------------------------

void GuiGridControl::removeObject(SimObject *obj)
{
	for(int idx =0; idx < objectList.size();idx++)
	{
		if ( objectList[idx] == obj )
		{
			mOrginalControlPos.erase(idx);
			break;
		}
	}

	Parent::removeObject(obj);
}

//------------------------------------------------------------------------------

void GuiGridControl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
	setUpdate();

	Point2I actualNewExtent = Point2I(  getMax(mMinExtent.x, newExtent.x), getMax(mMinExtent.y, newExtent.y));
	mBounds.set(newPosition, actualNewExtent);

	bool bFirstResize = false;
	iterator i;
	Vector<Point2I> oldCtrlExtent;
	
	if (mRowSizes.size() == 0 && mColSizes.size() == 0)
	{
		bFirstResize = true;
	}
	else
	{
		for(i = begin(); i != end(); i++)
		{
			GuiControl *ctrl = static_cast<GuiControl *>(*i);
			if (ctrl)
			{
				RectI newRect = GetGridRect(ctrl);		
				oldCtrlExtent.push_back(newRect.extent);
			}
		}

	}

	AdjustGrid(mBounds.extent);

	//resize and position all child controls.
	int idx = 0;
	for(i = begin(); i != end(); i++)
	{
		GuiControl *ctrl = static_cast<GuiControl *>(*i);
		if (ctrl)
		{
			RectI newRect = GetGridRect(ctrl);		
			
			if (ctrl->getExtent().x == 0 && ctrl->getExtent().y == 0)
				ctrl->setExtent(newRect.extent);

			ctrl->setPosition(mOrginalControlPos[idx] + newRect.point);

			if (bFirstResize)
			{
				ctrl->parentResized(newRect.extent, newRect.extent);
			}
			else
			{
				ctrl->parentResized(oldCtrlExtent[idx++], newRect.extent);
			}
		}
	}

	GuiControl *parent = getParent();

	if (parent)
		parent->childResized(this);

	setUpdate();
}

//------------------------------------------------------------------------------

RectI GuiGridControl::GetGridRect(GuiControl* ctrl)
{
	S32 col = dAtoi(ctrl->getDataField( StringTable->insert("Col"), NULL));
	S32 row = dAtoi(ctrl->getDataField( StringTable->insert("Row"), NULL));
	S32 colSpan = dAtoi(ctrl->getDataField( StringTable->insert("ColSpan"), NULL));
	S32 rowSpan = dAtoi(ctrl->getDataField( StringTable->insert("RowSpan"), NULL));

	AssertFatal (col < mColSizes.size(), "Col is out of bounds");
	AssertFatal (row < mRowSizes.size(), "Row is out of bounds");

	if (colSpan < 1)
        colSpan = 1;

	if (rowSpan < 1)
        rowSpan = 1;

	RectI newRect(0,0,0,0);

	for(int i = 0; i < col; i++)
	{
		newRect.point.x += mColSizes[i];
	}

	for(int i =col; i < col+colSpan; i++)
	{
		newRect.extent.x += mColSizes[i];
	}
	
	for(int i = 0; i < row; i++)
	{
		newRect.point.y += mRowSizes[i];
	}

	for(int i =row; i < row+rowSpan; i++)
	{
		newRect.extent.y += mRowSizes[i];
	}

	return newRect;
}

//------------------------------------------------------------------------------

void GuiGridControl::AdjustGrid(const Point2I& newExtent)
{
	mColSizes.clear();
	mRowSizes.clear();
	AdjustGridItems(newExtent.x, mGridCols, mColSizes);
	AdjustGridItems(newExtent.y, mGridRows, mRowSizes);
}

//------------------------------------------------------------------------------

void GuiGridControl::AdjustGridItems(S32 size, Vector<StringTableEntry>& strItems, Vector<S32>& items)
{
	Vector<GridItem> GridItems;
    S32 bFoundStar;
	bFoundStar = false;
	S32 IndexRemaining = -1;
	S32 totalSize = 0;
	S32 idx =0;

	//First step : Convert the string based column data into a GridItem vector.
	for(Vector<char const*>::iterator col = strItems.begin(); col != strItems.end(); ++col, idx++)
	{
		StringTableEntry str = *col;

		int len = dStrlen(str);
		AssertFatal(len >= 1, "Item can not be blank.");
		
		//we support three types of values (absolute size in pixels, percentage based, and remaining size in pixels).
		if (str[0] == '*') // use the remaining space left in the columns.
		{
			AssertFatal(!bFoundStar, "Can only use one * item field");
			GridItem gi;
			gi.IsAbsolute = false;
			gi.IsPercentage = false;
			gi.IsRemaining = true;
			gi.Size = 0;
			GridItems.push_back(gi);
		}
		else if ( len > 1 && str[len-1] == '%' ) //percentage based
		{
			char* tmp = new char[len-1];
			dStrncpy(tmp, str, len-1);
			int perc = dAtoi(tmp);
			delete tmp;

			GridItem gi;
			gi.IsAbsolute = false;
			gi.IsPercentage = true;
			gi.IsRemaining = false;
			gi.Size = perc;
			GridItems.push_back(gi);

		}
		else //standard absolute pixel based
		{
			int px = dAtoi(str);

			GridItem gi;
			gi.IsAbsolute = true;
			gi.IsPercentage = false;
			gi.IsRemaining = false;
			gi.Size = px;
			GridItems.push_back(gi);

			totalSize += px;
		}
	}

    //step two: iterate the grid columns again, and fill in any percentage based sizing, and setup the correct grid array.
	int remainingSize = size - totalSize;
	int sizeForPerc = remainingSize;
	for(int i = 0; i < GridItems.size(); ++i)
	{
		GridItem gi = GridItems[i];

		if (gi.IsAbsolute)
		{
			items.push_back(gi.Size);
		}
		else if (gi.IsPercentage)
		{
			F32 perc = gi.Size / 100.0f;
			S32 realSize = sizeForPerc * (S32)perc;
			remainingSize -= realSize;
			items.push_back(realSize);
		}
		else if(gi.IsRemaining)
		{
			//place holder for the moment.
			items.push_back(0);
			IndexRemaining = i;
		}			
	}

	if (IndexRemaining >= 0)
	{
		items[IndexRemaining] = remainingSize;
		remainingSize = 0;
	}
}
