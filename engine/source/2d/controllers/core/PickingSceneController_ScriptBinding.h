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

ConsoleMethodGroupBeginWithDocs(PickingSceneController, SimObject)

/*! Sets the control group(s).
    @param groups A list of groups that are affected by the controller.
    @return No return value.
*/
ConsoleMethodWithDocs(PickingSceneController, setControlGroups, ConsoleVoid, 3, 2 + MASK_BITCOUNT, (groups$))
{
    // The mask.
    U32 mask = 0;

    // Grab the element count of the first parameter.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // Make sure we get at least one number.
    if (elementCount < 1)
    {
        object->setControlGroupMask(MASK_ALL);
        return;
    }

    // Space separated list.
    if (argc == 3)
    {
        // Convert the string to a mask.
        for (U32 i = 0; i < elementCount; i++)
        {
            S32 bit = dAtoi(Utility::mGetStringElement(argv[2], i));
         
            // Make sure the group is valid.
            if ((bit < 0) || (bit >= MASK_BITCOUNT))
            {
            Con::warnf("PickingSceneController::setControlGroups() - Invalid group specified (%d); skipped!", bit);
            continue;
            }
         
            mask |= (1 << bit);
        }
    }

    // Comma separated list.
    else
    {
        // Convert the list to a mask.
        for (U32 i = 2; i < (U32)argc; i++)
        {
            S32 bit = dAtoi(argv[i]);
         
            // Make sure the group is valid.
            if ((bit < 0) || (bit >= MASK_BITCOUNT))
            {
            Con::warnf("PickingSceneController::setControlGroups() - Invalid group specified (%d); skipped!", bit);
            continue;
            }

            mask |= (1 << bit);
        }
    }
    // Set control groups.
    object->setControlGroupMask(mask);
}

//-----------------------------------------------------------------------------

/*! Gets the control groups.
    @return (controlGroups) A list of groups that are affected by the controller.
*/
ConsoleMethodWithDocs(PickingSceneController, getControlGroups, ConsoleString, 2, 2, ())
{
    U32 mask = object->getControlGroupMask();

    bool first = true;
    char* bits = Con::getReturnBuffer(128);
    bits[0] = '\0';
    for (S32 i = 0; i < MASK_BITCOUNT; i++)
    {
        if (mask & BIT(i))
        {
            char bit[4];
            dSprintf(bit, 4, "%s%d", first ? "" : " ", i);
            first = false;
            dStrcat(bits, bit);
        }
    }

    return bits;
}

//-----------------------------------------------------------------------------

/*! Sets the control layers(s).
    @param layers A list of layers that are affected by the controller.
    @return No return value.
*/
ConsoleMethodWithDocs(PickingSceneController, setControlLayers, ConsoleVoid, 3, 2 + MASK_BITCOUNT, (layers$))
{
    // The mask.
    U32 mask = 0;

    // Grab the element count of the first parameter.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // Make sure we get at least one number.
    if (elementCount < 1)
    {
        object->setControlLayerMask(MASK_ALL);
        return;
    }

    // Space separated list.
    if (argc == 3)
    {
        // Convert the string to a mask.
        for (U32 i = 0; i < elementCount; i++)
        {
            S32 bit = dAtoi(Utility::mGetStringElement(argv[2], i));
         
            // Make sure the group is valid.
            if ((bit < 0) || (bit >= MASK_BITCOUNT))
            {
            Con::warnf("PickingSceneController::setControlLayers() - Invalid layer specified (%d); skipped!", bit);
            continue;
            }
         
            mask |= (1 << bit);
        }
    }

    // Comma separated list.
    else
    {
        // Convert the list to a mask.
        for (U32 i = 2; i < (U32)argc; i++)
        {
            S32 bit = dAtoi(argv[i]);
         
            // Make sure the group is valid.
            if ((bit < 0) || (bit >= MASK_BITCOUNT))
            {
            Con::warnf("PickingSceneController::setControlLayers() - Invalid layer specified (%d); skipped!", bit);
            continue;
            }

            mask |= (1 << bit);
        }
    }
    // Set control Layers
    object->setControlLayerMask(mask);
}

//-----------------------------------------------------------------------------

/*! Gets the control layers.
    @return (controlLayers) A list of layers that are affected by the controller.
*/
ConsoleMethodWithDocs(PickingSceneController, getControlLayers, ConsoleString, 2, 2, ())
{
    U32 mask = object->getControlLayerMask();

    bool first = true;
    char* bits = Con::getReturnBuffer(128);
    bits[0] = '\0';
    for (S32 i = 0; i < MASK_BITCOUNT; i++)
    {
        if (mask & BIT(i))
        {
            char bit[4];
            dSprintf(bit, 4, "%s%d", first ? "" : " ", i);
            first = false;
            dStrcat(bits, bit);
        }
    }

    return bits;
}

ConsoleMethodGroupEndWithDocs(PickingSceneController)
