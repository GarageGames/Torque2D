# TorqueScript Syntax {#TorqueScriptSyntax}

## Main Rules
Like other languages, TorqueScript has certain syntactical rules you need to follow. The language is very forgiving, easy to debug, and is not as strict as a low level language like C++. Observe the following line in a script:

~~~{.cpp}
// Create test variable with a temporary variable
%testVariable = 3;
~~~
The three most simple rules obeyed in the above code are:

1. Ending a line with a semi-colon (;)
2. Proper use of white space.
3. Commenting

The engine will parse code line by line, stopping whenever it reaches a semi-colon. This is referred to as a statement termination, common to other programming languages such as C++, Javascript, etc. The following code will produce an error that may cause your entire script to fail:

~~~{.cpp}
%testVariable = 3
%anotherVariable = 4;
~~~

To the human eye, you are able to discern two separate lines of code with different actions. Here is how the script compiler will read it:

~~~{.cpp}
%testVariable = 3%anotherVariable = 4;
~~~

This is obviously not what the original code was meant to do. There are exemptions to this rule, but they come into play when multiple lines of code are supposed to work together for a single action:

~~~{.cpp}
if(%testVariable == 4)
  echo("Variable equals 4");
~~~

We have not covered conditional operators or echo commands yet, but you should notice that the first line does not have a semi-colon. The easiest explanation is that the code is telling the compiler: "Read the first line, do the second line if we meet the requirements." In other words, perform operations between semi-colons. Complex operations require multiple lines of code working together.

The second rule, proper use of whitespace, is just as easy to remember. Whitespace refers to how your script code is separated between operations. Let's look at the first example again:

~~~{.cpp}
%testVariable = 3;
~~~

The code is storing a value (3) in a local variable (`%%testVariable`). It is doing so by using a common mathematical operator, the equal sign. TorqueScript recognizes the equal sign and performs the action just as expected. It does not care if there are spaces in the operation:

~~~{.cpp}
%testVariable=3;
~~~

The above code works just as well, even without the spaces between the variable, the equal sign, and the 3. The whitespace rule makes a lot more sense when combined with the semi-colon rule and multiple lines of code working together. The following will compile and run without error:

~~~{.cpp}
if(%testVariable == 4) echo("Variable equals 4");
~~~

## Comments
The last rule is optional, but should be used as often as possible if you want to create clean code. Whenever you write code, you should try to use comments. Comments are a way for you to leave notes in code which are not compiled into the game. The compiler will essentially skip over these lines.

There are two different comment syntax styles. The first one uses the two slashes, `//`. This is used for single line comments:

Example:
~~~{.cpp}
// This comment line will be ignored
// This second line will also be ignored
%testVariable = 3;
// This third line will also be ignored
~~~

In the last example, the only line of code that will be executed has to do with `%%testVariable`. If you need to comment large chunks of code, or leave a very detailed message, you can use the `/*comment*/` syntax. The `/*` starts the commenting, the `*/` ends the commenting, and anything in between will be considered a comment.

Example:

~~~{.cpp}
/*
While attending school, an instructor taught a mantra I still use:

"Read. Read Code. Code."

Applying this to Torque 2D development is easy: 

READ the documentation first. 

READ CODE written by other Torque developers.

CODE your own prototypes based on what you have learned.
*/
~~~

As you can see, the comment makes full use of whitespace and multiple lines. While it is important to comment what the code does, you can also use this to temporarily remove unwanted code until a better solution is found:

Example:

~~~{.cpp}
// Why are you using multiple if statements. Why not use a switch$?
/*
if(%testVariable == "Mich")
  echo("User name: ", %testVariable);

if(%testVariable == "Heather")
  echo("User Name: ", %testVariable);

if(%testVariable == "Nikki")
  echo("User Name: ", %testVariable);
*/
~~~

# Variables

## Usage
Now that you know the two most basic rules of writing code in TorqueScript, this is the best time to learn about variables. A variable is a letter, word, or phrase linked to a value stored in your game's memory and used during operations. Creating a variable is a one line process. The following code creates a variable by naming it and assigning a value:

~~~{.cpp}
%localVariable = 3;
~~~

You can assign any type value to the variable you want. This is referred to as a language being type-insensitive. TorqueScript does not care (insensitive) what you put in a variable, even after you have created it. The following code is completely valid:

~~~{.cpp}
%localVariable = 27;
%localVariable = "Heather";
%localVariable = "7 7 7";
~~~

The main purpose of the code is to show that TorqueScript treats all data types the same way. It will interpret and convert the values internally, so you do not have to worry about typecasting. That may seem a little confusing. After all, when would you want a variable that can store a number, a string, or a vector?

You will rarely need to, which is why you want to start practicing good programming habits. An important practice is proper variable naming. The following code will make a lot more sense, considering how the variables are named:

~~~{.cpp}
%userName = "Heather";
%userAge = 27;
%userScores = "7 7 7";
~~~

Earlier, I mentioned that TorqueScript is more forgiving than low level programming languages. While it expects you to obey the basic syntax rules, it will allow you to get away with small mistakes or inconsistency. The best example is variable case sensitivity. At some point in school you learned the difference between upper case and lower case letters.

With variables, TorqueScript is not case sensitive. You can create a variable and refer to it during operations without adhering to case rules:

~~~{.cpp}
%userName = "Heather";
echo(%Username);
~~~

In the above code, `%%userName` and `%%Username` are the same variable, even though they are using different capitalization. You should still try to remain consistent in your variable naming and usage, but you will not be punished if you slip up occasionally.

## Variable Types
There are two types of variables you can declare and use in TorqueScript: local and global. Both are created and referenced similarly:

~~~{.cpp}
%localVariable = 1;
$globalVariable = 2;
~~~

As you can see, local variable names are preceded by the percent sign `(%%)`. Global variables are preceded by the dollar sign `($)`. Both types can be used in the same manner: operations, functions, equations, etc. The main difference has to do with how they are scoped.

In programming, scoping refers to where in memory a variable exists and its life. A local variable is meant to only exist in specific blocks of code, and its value is discarded when you leave that block. Global variables are meant to exist and hold their value during your entire programs execution. Look at the following code to see an example of a local variable:

~~~{.cpp}
function test()
{
   %userName = "Heather";
   echo(%userName);
}
~~~

We will cover functions a little later, but you should know that functions are blocks of code that only execute when you call them by name. This means the variable, `%%userName`, does not exist until the test() function is called. When the function has finished all of its logic, the `%%userName` variable will no longer exist. If you were to try to access the %%userName variable outside of the function, you will get nothing.

Most variables you will work with are local, but you will eventually want a variables that last for your entire game. These are extremely important values used throughout the project. This is when global variables become useful. For the most part, you can declare global variables whenever you want:

~~~{.cpp}
$PlayerName = "Heather";

function printPlayerName()
{
   echo($PlayerName);
}

function setPlayerName()
{
   $PlayerName = "Nikki";
}

~~~

The above code makes full use of a global variable that holds a player's name. The first declaration of the variable happens outside of the functions, written anywhere in your script. Because it is global, you can reference it in other locations, including separate script files. Once declared, your game will hold on to the variable until shutdown.

## Data Types
TorqueScript implicitly supports several variable data-types: numbers, strings, booleans, and arrays and vectors. If you wish to test the various data types, you can use the echo(...) command. For example:

~~~{.cpp}
%meaningOfLife = 42;
echo(%meaningOfLife);

$name = "Heather";
echo($name);
~~~

The echo will post the results in the console, which can be accessed by pressing the ctrl+tilde key (`~`) while in game.

## Numbers
TorqueScript handles standard numeric types

~~~{.cpp}
123     (Integer)
1.234   (floating point)
1234e-3 (scientific notation)
0xc001  (hexadecimal)
~~~

## Strings
Text, such as names or phrases, are supported as strings. Numbers can also be stored in string format. Standard strings are stored in double-quotes.

~~~{.cpp}
"abcd"
~~~

Example:

~~~{.cpp}
$UserName = "Heather";
~~~

Strings with single quotes are called "tagged strings."

~~~{.cpp}
'abcd'  (tagged string)
~~~

Tagged strings are special in that they contain string data, but also have a special numeric tag associated with them. Tagged strings are used for sending string data across a network. The value of a tagged string is only sent once, regardless of how many times you actually do the sending.

On subsequent sends, only the tag value is sent. Tagged values must be de-tagged when printing. You will not need to use a tagged string often unless you are in need of sending strings across a network often, like a chat system.

Example:

~~~{.cpp}
$a = 'This is a tagged string';
echo("  Tagged string: ", $a);
echo("Detagged string: ", detag('$a'));
~~~

The output will be similar to this:

~~~{.cpp}
24
___
~~~

The second echo will be blank unless the string has been passed to you over a network.

## String Operators
There are special values you can use to concatenate strings and variables. Concatenation refers to the joining of multiple values into a single variable. The following is the basic syntax:

~~~{.cpp}
"string 1" operation "string 2"
~~~

You can use string operators similarly to how you use mathematical operators (=, +, -, *). You have four operators at your disposal:

**String Operators**

| Operator | Name | Example | Explanation |
|-------------|:-------------:|:-------------:|------------|
| @ | String concatenation | `$c @ $d` | Concatenates strings $c and $d into a single string. Numeric literals/variables convert to strings. |
| NL | New Line | `$c NL $d` | Concatenates strings $c and $d into a single string separated by new-line. |
| TAB | Tab | `$c TAB $d` | Concatenates strings $c and $d into a single string separated by tab. |
| SPC | Space | `$c SPC $d` | Concatenates strings $c and $d into a single string separated by space. Note: such a string can be decomposed with getWord() |

The `@` symbol will concatenate two strings together exactly how you specify, without adding any additional whitespace:

Note: Do not type in OUTPUT: ___. This is placed in the sample code to show you what the console would display

Example:
~~~{.cpp}
%newString = "Hello" @ "World";
echo(%newString);

OUTPUT: HelloWorld
~~~

Notice how the two strings are joined without any spaces. If you include whitespace, it will be concatenated along with the values:

Example:
~~~{.cpp}
%newString = "Hello " @ "World";
echo(%newString);

OUTPUT: Hello World
~~~

String operators work with variables as well:

Example:
~~~{.cpp}
%hello = "Hello ";
%world = "World";

echo(%hello @ %world);

OUTPUT: Hello World
~~~

The rest of the operators will apply whitespace for you, so you do not have to include it in your values:

Example:
~~~{.cpp}
echo("Hello" @ "World");
echo("Hello" TAB "World");
echo("Hello" SPC "World");
echo("Hello" NL "World");

OUTPUT:
HelloWorld
Hello   World
Hello World
Hello
World
~~~

## Booleans
Like most programming languages, TorqueScript also supports Booleans. Boolean numbers have only two values- true or false.

<pre>
true    (1)
false   (0)
</pre>

Again, as in many programming languages the constant "true" evaluates to the number 1 in TorqueScript, and the constant "false" evaluates to the number 0. However, non-zero values are also considered true. Think of booleans as "on/off" switches, often used in conditional statements.

Example:
~~~{.cpp}
$lightsOn = true;

if($lightsOn)
  echo("Lights are turned on");
~~~

## Arrays
Arrays are data structures used to store consecutive values of the same data type.

~~~{.cpp}
$TestArray[n]   (Single-dimension)
$TestArray[m,n] (Multidimensional)
$TestArray[m_n] (Multidimensional)
~~~

If you have a list of similar variables you wish to store together, try using an array to save time and create cleaner code. The syntax displayed above uses the letters 'n' and 'm' to represent where you will input the number of elements in an array. The following example shows code that could benefit from an array:

Example:
~~~{.cpp}
$firstUser = "Heather";
$secondUser = "Nikki";
$thirdUser = "Mich";

echo($firstUser);
echo($secondUser);
echo($thirdUser);
~~~

Instead of using a global variable for each user name, we can put those values into a single array:

Example:
~~~{.cpp}
$userNames[0] = "Heather";
$userNames[1] = "Nikki";
$userNames[2] = "Mich";

echo($userNames[0]);
echo($userNames[1]);
echo($userNames[2]);
~~~

Now, let's break the code down. Like any other variable declaration, you can create an array by giving it a name and value:

~~~{.cpp}
$userNames[0] = "Heather";
~~~

What separates an array declaration from a standard variable is the use of brackets []. The number you put between the brackets is called the index. The index will access a specific element in an array, allowing you to view or manipulate the data. All the array values are stored in consecutive order.

If you were able to see an array on paper, it would look something like this:

~~~{.cpp}
[0] [1] [2]
~~~

In our example, the data looks like this:

~~~{.cpp}
["Heather"] ["Nikki"] ["Mich"]
~~~

Like other programming languages, the index is always a numerical value and the starting index is always 0. Just remember, index 0 is always the first element in an array. As you can see in the above example, we create the array by assigning the first index (0) a string value ("Heather").

The next two lines continue filling out the array, progressing through the index consecutively.

~~~{.cpp}
$userNames[1] = "Nikki";
$userNames[2] = "Mich";
~~~

The second array element (index 1) is assigned a different string value ("Nikki"), as is the third (index 2). At this point, we still have a single array structure, but it is holding three separate values we can access. Excellent for organization.


The last section of code shows how you can access the data that has been stored in the array. Again, you use a numerical index to point to an element in the array. If you want to access the first element, use 0:

~~~{.cpp}
echo($userNames[0]);
~~~

In a later section, you will learn about looping structures that make using arrays a lot simpler. Before moving on, you should know that an array does not have to be a single, ordered list. TorqueScript also supports multidimensional arrays.

An single-dimensional array contains a single row of values. A multidimensional array is essentially an array of arrays, which introduces columns as well. The following is a visual of what a multidimensional looks like with three rows and three columns:

~~~{.cpp}
[x] [x] [x]
[x] [x] [x]
[x] [x] [x]
~~~

Defining this kind of array in TorqueScript is simple. The following creates an array with 3 rows and 3 columns.

Example:
~~~{.cpp}
$testArray[0,0] = "a";
$testArray[0,1] = "b";
$testArray[0,2] = "c";

$testArray[1,0] = "d";
$testArray[1,1] = "e";
$testArray[1,2] = "f";

$testArray[2,0] = "g";
$testArray[2,1] = "h";
$testArray[2,2] = "i";
~~~

Notice that we are are now using two indices, both starting at 0 and stopping at 2. We can use these as coordinates to determine which array element we are accessing:

~~~{.cpp}
[0,0] [0,1] [0,2]
[1,0] [1,1] [1,2]
[2,0] [2,1] [2,2]
~~~

In our example, which progresses through the alphabet, you can visualize the data in the same way:

~~~{.cpp}
[a] [b] [c]
[d] [e] [f]
[g] [h] [i]
~~~

The first element `[0,0]` points to the letter 'a'. The last element `[2,2]` points to the letter 'i'.

## Vectors
"Vectors" are a helpful data-type which are used throughout Torque 2D. For example, many fields on SceneObjects take numeric values in sets of 2 or 3. These are stored as strings and interpreted as "vectors".

~~~{.cpp}
"1.0 1.0"  (2 element vector)
~~~

The most common example of a vector would be a world position. Like most 2D coordinate systems, an object's position is stored as `(X Y)`. You can use a two element vector to hold this data:

Example:
~~~{.cpp}
%position = "25 32";
~~~

You can separate the values using spaces or tabs (both are acceptable whitespace). Another example is storing color data in a four element vector. The values that make up a color are "Red Blue Green Alpha," which are all numbers. You can create a vector for color using hard numbers, or variables:

Example:
~~~{.cpp}
%firstColor = "100 100 100 1.0";
echo(%firstColor);

%red = 128;
%blue = 255;
%green = 64;
%alpha = 1.0;

%secondColor = %red SPC %blue SPC %green SPC %alpha;
echo(%secondColor);
~~~

# Operators
Operators in TorqueScript behave very similarly to operators in real world math and other programming languages. You should recognize quite a few of these from math classes you took in school, but with small syntactical changes. The rest of this section will explain the syntax and show a brief example, but we will cover these in depth in later guides.

**Arithmetic Operators**

| Operator | Name | Example | Explanation |
|:-------------:|:-------------:|:-------------:|------------|
| * | multiplication | `$a * $b` | Multiply $a and $b. |
| / | division | `$a / $b` | Divide $a by $b. |
| % | modulo | `$a % $b` | Remainder of $a divided by $b. |
| + | addition | `$a + $b` | Add $a and $b. |
| - | subtraction | `$a - $b` | Subtract $b from $a. |
| ++ | auto-increment (post-fix only) | $a++ | Increment $a. The value of $a++ is that of the incremented variable: auto-increment is post-fix in syntax, but pre-increment in sematics (the variable is incremented, before the return value is calculated). This behavior is unlike that of C and C++. |
| \-- | auto-decrement (post-fix only) | $b\-- | Decrement $b. The value of $a\-- is that of the decremented variable: auto-decrement is post-fix in syntax, but pre-decrement in sematics (the variable is decremented, before the return value is calculated). This behavior is unlike that of C and C++. |

**Relational Operators**

Used in comparing values and variables against each other. Relations can be arithmetic, logical, and string:

| Operator | Name | Example | Explanation |
|:-------------:|:-------------:|:-------------:|------------|
| `<` | Less than | `$a < $b` | 1 if $a is less than $b (0 otherwise.) |
| `>` | More than | `$a > $b` | 1 if $a is greater than $b (0 otherwise.) |
| `<=` | Less than or Equal to | `$a <= $b` | 1 if $a is less than or equal to $b (0 otherwise.) |
| `>=` | More than or Equal to | `$a >= $b` | 1 if $a is greater than or equal to $b (0 otherwise.) |
| `==` | Equal to | `$a == $b` | 1 if $a is equal to $b (0 otherwise.) |
| `!=` | Not equal to | `$a != $b` | 1 if $a is not equal to % b (0 otherwise.) |
| `!` | Logical NOT | `!$a` | 1 if $a is 0 (0 otherwise.) |
| `&&` | Logical AND | `$a && $b` | 1 if $a and $b are both non-zero (0 otherwise.) |
| `$=` | String equal to | `$c $= $d` | 1 if $c equal to $d . |
| `!$=` | String not equal to | `$c !$= $d` | 1 if $c not equal to $d. |

One additional operator is the logical OR, represented by `||`. Example:

~~~{.cpp}
$a || $b
~~~
OR will return 1 if either `$a` or `$b` is non-zero (0 otherwise.)

**Bitwise Operators**

Used for comparing and shifting bits

| Operator | Name | Example | Explanation |
|:-------------:|:-------------:|:-------------:|------------|
| ~ | Bitwise complement | ~$a | flip bits 1 to 0 and 0 to 1. (i.e. ~10b == 01b) |
| & | Bitwise AND | $a & $b | composite of elements where bits in same position are 1. (i.e. 1b & 1b == 1b) |
| ^ | Bitwise XOR | $a ^ $b | composite of elements where bits in same position are opposite. (i.e. 100b & 101b == 001b) |
| << | Left Shift | $a << 3 | element shifted left by 3 and padded with zeros. (i.e. 11b << 3d == 11000b) |
| >> | Right Shift | $a >> 3 | element shifted right by 3 and padded with zeros. (i.e. 11010b >> 3d == 00011b) |

One additional operator is the bitwise OR, represented by `|`. Example:

~~~{.cpp}
$a | $b
~~~
This will return composite of elements where bits 1 in either of the two elements. (i.e. `100b & 001b == 101b`).

**Assignment Operators**

Used for setting the value of variables.

| Operator | Name | Example | Explanation |
|:-------------:|:-------------:|:-------------:|------------|
| Assignment | $a = $b; | Assign value of $b to $a. | Note: the value of an assignment is the value being assigned, so $a = $b = $c is legal. |
| op= | Assignment Operators | $a op= $b; | Equivalent to $a = $a op $b, where op can be any of: `*` `/` `%` `+` `-` `&` `^` `<<` `>>` \| |

# Control Statements
TorqueScript provides basic branching structures that will be familiar to programmers that have used other languages. If you are completely new to programming, you use branching structures to control your game's flow and logic. This section builds on everything you have learned about TorqueScript so far.

## if, then, else
This type of structure is used to test a condition, then perform certain actions if the condition passes or fails. You do not always have to use the full structure, but the following syntax shows the extent of the conditional:

~~~{.cpp}
if(<boolean expression>) 
{
   pass logic
}
else 
{
   alternative logic
}
~~~

Remember how boolean values work? Essentially, a bool can either be true (1) or false (0). The condition (boolean) is always typed into the parenthesis after the "if" syntax. Your logic will be typed within the brackets {}. The following example uses specific variable names and conditions to show how this can be used:

Example:
~~~{.cpp}
// Global variable that controls lighting
$lightsShouldBeOn = true;

// Check to see if lights should be on or off
if($lightsShouldBeOn)
{
   // True. Call turn on lights function
   turnOnLights();

   echo("Lights have been turned on");
}
else
{
   // False. Turn off the lights
   turnOffLights();

   echo("Lights have been turned off");
}
~~~

Brackets for single line statements are optional. If you are thinking about adding additional logic to the code, then you should use the brackets anyway. If you know you will only use one logic statement, you can use the following syntax:

Example:
~~~{.cpp}
// Global variable that controls lighting
$lightsShouldBeOn = true;

// Check to see if lights should be on or off
if($lightsShouldBeOn)
  turnOnLights();   // True. Call turn on lights function
else
  turnOffLights(); // False. Turn off the lights
~~~

## switch and switch$
If your code is using several cascading if-then-else statements based on a single value, you might want to use a switch statement instead. Switch statements are easier to manage and read. There are two types of switch statements, based on data type: numeric (`switch`) and string (`switch$`).

Switch Syntax:
~~~{.cpp}
switch(<numeric expression>) 
{
   case value0:
       statements;
   case value1:
       statements;
   case value3:
       statements;
   default:
       statements;
}
~~~

As the above code demonstrates, start by declaring the switch statement by passing in a value to the `switch(...)` line. Inside of the brackets `{}`, you will list out all the possible cases that will execute based on what value being tested. It is wise to always use the default case, anticipating rogue values being passed in.

Example:
~~~{.cpp}
switch($ammoCount)
{
   case 0:
      echo("Out of ammo, time to reload");
      reloadWeapon();
   case 1:
      echo("Almost out of ammo, warn user");
      lowAmmoWarning();
   case 100:
      echo("Full ammo count");
      playFullAmmoSound();
   default:
      doNothing();
}
~~~

`switch` only properly evaluates numerical values. If you need a switch statement to handle a string value, you will want to use `switch$`. The `switch$` syntax is similar to what you just learned:

Switch$ Syntax:
~~~{.cpp}
switch$ (<string expression>) 
{
   case "string value 0":
       statements;

   case "string value 1":
       statements;
...
   case "string value N":
       statements;

   default:
       statements;
}
~~~

Appending the `$` sign to switch will immediately cause the parameter passed in to be parsed as a string. The following code applies this logic:

Example:
~~~{.cpp}
// Print out specialties
switch($userName)
{
   case "Heather":
      echo("Sniper");
   case "Nikki":
      echo("Demolition");
   case "Mich":
      echo("Meat shield");
   default:
      echo("Unknown user");
}
~~~

# Loops
As the name implies, this structure type is used to repeat logic in a loop based on an expression. The expression is usually a set of variables that increase by count, or a constant variable changed once a loop has hit a specific point.

## For Loop
~~~{.cpp}
for(expression0; expression1; expression2) 
{
    statement(s);
}
~~~

One way to label the expressions in this syntax are `(startExpression; testExpression; countExpression)`. Each expression is separated by a semi-colon.

Example:
~~~{.cpp}
for(%count = 0; %count < 3; %count++) 
{
    echo(%count);
}

OUTPUT:
0
1
2
~~~

The first expression creates the local variable `%count` and initializing it to 0. In the second expression determines when to stop looping, which is when the `%count` is no longer less than 3. Finally, the third expression increases the count the loop relies on.

## While Loop
A while loop is a much simpler looping structure compared to a for loop.

~~~{.cpp}
while(expression) 
{
    statements;
}
~~~

As soon as the expression is met, the while loop will terminate:

Example:
~~~{.cpp}
%countLimit = 0;

while(%countLimit <= 5)
{
   echo("Still in loop");
   %count++;
}
echo("Loop was terminated");
~~~

# Functions
Much of your TorqueScript experience will come down to calling existing functions and writing your own. Functions are a blocks of code that only execute when you call them by name. Basic functions in TorqueScript are defined as follows:

~~~{.cpp}
// function - Is a keyword telling TorqueScript we are defining a new function.
// function_name - Is the name of the function we are creating.
// ... - Is any number of additional arguments.
// statements - Your custom logic executed when function is called
// return val - The value the function will give back after it has completed. Optional.

function function_name([arg0],...,[argn]) 
{
    statements;
    [return val;]
}
~~~

The `function` keyword, like other TorqueScript keywords, is case sensitive. You must type it exactly as shown above. The following is an example of a custom function that takes in two parameters, then executes code based on those arguments.

TorqueScript can take any number of arguments, as long as they are comma separated. If you call a function and pass fewer parameters than the function's definition specifies, the un-passed parameters will be given an empty string as their default value.

Example:
~~~{.cpp}
function echoRepeat (%echoString, %repeatCount) 
{
   for (%count = 0; %count < %repeatCount; %count++)
   {
      echo(%echoString);
   }
}
~~~
You can cause this function to execute by calling it in the console, or in another function:

~~~{.cpp}
echoRepeat("hello!", 5);

OUTPUT:
"hello!"
"hello!"
"hello!"
"hello!"
"hello!"
~~~

If you define a function and give it the same name as a previously defined function, TorqueScript will completely override the old function. This still applies even if you change the number of parameters used; the older function will still be overridden.

**Console Methods**
Console Methods are C++ functions that have been exposed to TorqueScript, which are attached to specific objects.

**Console Functions**
Console Functions are written in C++, then exposed to TorqueScript. These are global functions you can call at any time, and are usually very helpful or important. Throughout this document, I have been using a ConsoleFunction: `echo(...)`. The `echo` function definition exists in C++:


C++ echo:

~~~{.cpp}
ConsoleFunction(echo, void, 2, 0, "echo(text [, ... ])")
{
   U32 len = 0;
   S32 i;
   for(i = 1; i < argc; i++)
      len += dStrlen(argv[i]);

   char *ret = Con::getReturnBuffer(len + 1);
   ret[0] = 0;
   for(i = 1; i < argc; i++)
      dStrcat(ret, argv[i]);

   Con::printf("%s", ret);
   ret[0] = 0;
}
~~~

Instead of having to write that out every time, or create a TorqueScript equivalent, the ConsoleFunction macro in C++ exposes the command for you. This is much cleaner, and more convenient. We will cover all the ConsoleFunctions later.

**Objects**
The most complex aspect of TorqueScript involves dealing with game objects. One thing to remember is that everything in TorqueScript is a string. However, when accessing a sprite, sceneObject, or any other object, a string is converted to an object ID under the hood.

*Syntax*
Even though objects are originally created in C++, they are exposed to script in a way that allows them to be declared using the following syntax:

Object Definition:
~~~{.cpp}
// In TorqueScript
%objectID = new ObjectType(Name) 
{   
   [existing_field0 = InitialValue0;]
   ...
   [existing_fieldN = InitialValueN;]

   [dynamic_field0 = InitialValue0;]
   ...
   [dynamic_fieldN = InitialValueN;]
};
~~~

Syntax Breakdown:

**%objectID** - Is the variable where the object's handle will be stored.
new - Is a key word telling the engine to create an instance of the following ObjectType.

**ObjectType** - Is any class declared in the engine or in script that has been derived from SimObject or a subclass of SimObject. SimObject-derived objects are what we were calling "game world objects" above.

**Name (optional)** - Is any expression evaluating to a string, which will be used as the object's name.

**existing_fieldN** - You may initialize existing class members (fields) here. Note: In order to modify a member of a C++-defined class, the member must be exposed to the Console. This concept is discussed in detail later.

**dynamic_fieldN** - Lastly, you may create new fields (which will exist only in Script) for your new object. These are unique to the instance of the object you are creating.

**Handles vs Names**
Every game object added to a level can be accessed by two parameters:

**Handle** - A unique numeric ID generated when the object is created

**Name** - This is an optional parameter given to an object when it is created.

Example:

~~~{.cpp}
// In this example, Truck is the name of the object
new SceneObject(Truck) 
{
   position = "0 0";
   size = "5 5";
};
~~~

**Object Fields**
Objects instantiated via script may have data members (referred to as Fields)

**Methods**
In addition to the creation of stand-alone functions, TorqueScript allows you to create and call methods attached to objects. Some of the more important ConsoleMethods are already written in C++, then exposed to script. You can call these methods by using the dot (.) notation.

Syntax:
~~~{.cpp}
objHandle.function_name();

objName.function_name();
~~~

Example:
~~~{.cpp}
new SceneObject(Truck) 
{
   position = "0 0";
   size = "5 5";
};

// Write all the objects methods to the console log
Truck.dump();

// Get the ID of an object, using the object's name
$objID = Truck.getID();

// Print the ID to the console
echo("Object ID: ", $objID);

// Get the object's position, using the object's handle
%position = $objID.getPosition();

// Print the position to the console
echo("Object Position: ", %position);
~~~

The above example shows how you can call an object's method by using its name or a variable containing its handle (unique ID number). Additionally, TorqueScript supports the creation of methods that have no associated C++ counterpart.

Syntax:

~~~{.cpp}
// function - Is a keyword telling TorqueScript we are defining a new function.
// ClassName::- Is the class type this function is supposed to work with.
// function_name - Is the name of the function we are creating.
// ... - Is any number of additional arguments.
// statements - Your custom logic executed when function is called
// %this- Is a variable that will contain the handle of the 'calling object'.
// return val - The value the function will give back after it has completed. Optional.
function Classname::func_name(%this, [arg0],...,[argn]) 
{
   statements;
   [return val;]
}
~~~

At a minimum, Console Methods require that you pass them an object handle. You will often see the first argument named %this. People use this as a hint, but you can name it anything you want. As with Console functions any number of additional arguments can be specified separated by commas.

As a simple example, let's say there is an object called Samurai, derived from the Player class. It is likely that a specific appearance and play style will be given to the samurai, so custom ConsoleMethods can be written. Here is a sample:

Example:
~~~{.cpp}
function Samurai::sheatheSword(%this)
{
    echo("Katana sheathed");
}
~~~

When you create a Samurai object, it will be given an ID. Let's pretend the handle (ID number) is `1042`. We can call its ConsoleMethod once it is defined, using the period syntax:

Example:
~~~{.cpp}
1042.sheatheSword();

OUTPUT: "Katana sheathed"
~~~

Notice that no parameters were passed into the function. The `%this` parameter is inherent, and the original function did not require any other parameters.

# Conclusion
This guide covered the basics of TorqueScript syntax. Compared to other languages, such as C++, it is easier to learn and work with. However, no one is expected to become a TorqueScript master over night, or even in a week. You will most likely need to refer back to this documentation several times for reminders.
