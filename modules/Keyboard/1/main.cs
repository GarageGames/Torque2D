function Keyboard::create(%this)
{
   %this.resetState();
   
}

function Keyboard::destroy(%this)
{
   
}

function Keyboard::resetState(%this)
{
   %this.state = "toLower";
   %this.dock = "bottom";
   %this.targetGui ="";
   %this.textBox = "";
}
function Keyboard::push(%this, %targetGui, %textBox, %showClose)
{
   Sandbox.add( TamlRead("./gui/keyboardGui.taml") );
   %textBox.setText("");
   
   %this.targetGui = %targetGui;
   %this.textBox = %textBox;
   Canvas.pushDialog(%targetGui);
   // resize to targetGui
   %targetExtent = %targetGui.Extent;
   KeyboardGui.resize(0, 0, %targetExtent._0, %targetExtent._1); 
   %this.targetGui.addGuiControl(KeyboardSet);
   %textBox.setFirstResponder();
   
   if (%showClose)
      keyCloseBtn.setVisible(true);
   else
      keyCloseBtn.setVisible(false);
   %this.toLower();
   
}

function Keyboard::Pop(%this)
{
   KeyboardSet.delete();
   KeyboardGui.delete();
   Canvas.popDialog(%this.targetGui);
   %this.resetState();
}

function Keyboard::toLower(%this)
{
   %this.state = "toLower";
   toLower_NumberBtn.visible = false;
   toLowerBtn.visible = false;
   toSymbolBtn.visible = false;
   toNumber_symbolBtn.visible = false;
   toLower_UpperLockBtn.visible = false;
   toUpperLockBtn.visible =false;
   
   toUpperBtn.visible = true;
   toNumberBtn.visible = true;
   keyboardSet.Image = "Keyboard:keyboardAlphaLower";
   
}

function Keyboard::toUpper(%this)
{
   %this.state = "toUpper";
   toUpperBtn.visible = false;
   toLower_NumberBtn.visible = false;
   toLowerBtn.visible = false;
   toSymbolBtn.visible = false;
   toNumber_symbolBtn.visible = false;
   toLower_UpperLockBtn.visible = false;

   toUpperLockBtn.visible =true;
   toNumberBtn.visible = true;
   keyboardSet.Image = "Keyboard:keyboardAlpha";
}

function Keyboard::toUpperLock(%this)
{
   %this.state = "toUpperLock";
   toUpperBtn.visible = false;
   toUpperLockBtn.visible = false;
   toLowerBtn.visible = false;
   toLower_NumberBtn.visible = false;
   toSymbolBtn.visible = false;
   toNumber_symbolBtn.visible = false;
   
   toLower_UpperLockBtn.visible = true;
   toNumberBtn.visible = true;
   keyboardSet.Image = "Keyboard:keyboardAlpha";
}

function Keyboard::toNumber(%this)
{
   %this.state = "toNumber";
   toNumberBtn.visible = false;
   toLowerBtn.visible = false;
   toUpperBtn.visible = false;
   toNumber_symbolBtn.visible = false;
   toLower_UpperLockBtn.visible = false;
   toUpperLockBtn.visible =false;
   
   toSymbolBtn.visible = true;
   toLower_NumberBtn.visible = true;
   keyboardSet.Image = "Keyboard:keyboardNumber";
}

function Keyboard::toSymbol(%this)
{
   %this.state = "toSymbol";
   toSymbolBtn.visible = false;
   toUpperBtn.visible = false;
   toLowerBtn.visible = false;
   toNumberBtn.visible = false;
   toLower_UpperLockBtn.visible = false;
   toUpperLockBtn.visible =false;
   
   toNumber_symbolBtn.visible = true;
   toLower_NumberBtn.visible = true;
   
   keyboardSet.Image = "Keyboard:keyboardSymbol";
}


function Keyboard::KeyPress(%this, %letter, %number, %symbol)
{
   if (%letter $= "close")
      %this.schedule(50, "Pop");
   else if (%letter $= "delete")
      %this.deleteCommand();
   else if (%letter $= "space")
      %this.insertChar(" ");
   else
      %this.insertChar(%letter, %number, %symbol);
}


function Keyboard::InsertChar(%this, %letter, %number, %symbol)
{
   if (strlen(%this.textBox.getText()) >= %this.textBox.MaxLength)
      return;
   if (%letter $= " ")
   {
      %this.textBox.setText(%this.textBox.getText() @ " ");
   }
   else
   {
      if (%this.state $= "toLower")
         %this.textBox.setText(%this.textBox.getText() @ strlwr(%letter));
      else if (%this.state $= "toUpper")
      {
         %this.textBox.setText(%this.textBox.getText() @ strupr(%letter));
         %this.toLower();
      }
      else if (%this.state $= "toUpperLock")
      {
         %this.textBox.setText(%this.textBox.getText() @ strupr(%letter));
      }
      else if (%this.state $= "toNumber")
      {
         if (%letter $= "B")
            %number = "'";
         else if (%letter $= "K")
            %number = "&";
         %this.textBox.setText(%this.textBox.getText() @ %number);
      }
      else if (%this.state $= "toSymbol")
      {
         if (%letter $= "B")
            %symbol = "'";
         %this.textBox.setText(%this.textBox.getText() @ %symbol);
      }
   }
}

function Keyboard::DeleteCommand(%this)
{
   %len = strlen(%this.textBox.getText());
   if (%len > 0)
      %this.textBox.setText(getSubStr(%this.textBox.getText(), 0, %len - 1));
}

if(!isObject(GuiKeyboardProfile)) new GuiControlProfile (GuiKeyboardProfile)
{
    tab = false;
    canKeyFocus = false;
    hasBitmapArray = false;
    mouseOverSelected = false;

    // fill color
    opaque = false;
    fillColor = "211 211 211";
    fillColorHL = "244 244 244";
    fillColorNA = "244 244 244";

    // border color
    border = 0;
    borderColor   = "100 100 100 255";
    borderColorHL = "128 128 128";
    borderColorNA = "64 64 64";

    // font
    fontType = $platformFontType;
    fontSize = $platformFontSize;

    fontColor = "0 0 0";
    fontColorHL = "32 100 100";
    fontColorNA = "0 0 0";
    fontColorSEL= "10 10 10";

    // used by guiTextControl
    modal = true;
    justify = "left";
    autoSizeWidth = false;
    autoSizeHeight = false;
    returnTab = false;
    numbersOnly = false;
    cursorColor = "0 0 0 255";

    // sounds
    soundButtonDown = "Keyboard:keypress";
    //soundButtonOver = "Sandbox:mouseOver";
};