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

ConsoleMethodGroupBeginWithDocs(PNGImage, SimObject)

/*! Create the base image to merge onto 
*/
ConsoleMethodWithDocs(PNGImage, CreateBaseImage, ConsoleBool, 5, 5, (width, height, imageType))
{
    U32 width = dAtoi(argv[2]);
    U32 height = dAtoi(argv[3]);

    return object->Create(width, height, (PNGImageType)dAtoi(argv[4]));
}

/*! Add an image to the spritesheet
*/
ConsoleMethodWithDocs(PNGImage, MergeOn, ConsoleBool, 5, 5, (x, y, imageFile))
{
    U32 width = dAtoi(argv[2]);
    U32 height = dAtoi(argv[3]);

    // File name is argv[4]
    FileStream fStream;

    if(!fStream.open(argv[4], FileStream::Read))
    { 
        Con::printf("Failed to open file '%s'.", argv[4]);
        return false;
    }

    PNGImage* newImage = new PNGImage();

    bool didReadImage = newImage->Read(argv[4]);

    if(!didReadImage)
    {
        newImage->CleanMemoryUsage();

        delete newImage;
        return false;
    }

    fStream.close();

    bool didMergeOn = object->MergeOn(width, height, newImage);

    newImage->CleanMemoryUsage();
    delete newImage;

    return didMergeOn;
}

/*! Save the new spritesheet to a file
*/
ConsoleMethodWithDocs(PNGImage, SaveImage, ConsoleBool, 3, 3, (fileName))
{
    FileStream fStream;

    if(!fStream.open(argv[2], FileStream::Write))
    { 
        Con::printf("Failed to open file '%s'.", argv[2]);
        return false;
    }

    fStream.close();

    object->Write(argv[2]);

    object->CleanMemoryUsage();

    return true;
}

ConsoleMethodGroupEndWithDocs(PNGImage)
