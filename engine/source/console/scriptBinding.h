// Warning!  This file is not real C++ code!  It is designed to provide types and operators, etc.
// that are needed by Doxygen to create a TorqueScript reference.  As long as the
// #define PSEUDOCODE_FOR_DOXYGEN is not set while compiling, there will be no issues.

#ifdef PSEUDOCODE_FOR_DOXYGEN

// Define a doxygen group for all TorqueScript functions to live within
// Clusters of functions -- such as vector functions or event schedule functions --
// should be kept within a subgroup of the TorqueScriptFunction umbrella group
/*!
   @defgroup TorqueScriptFunctions Function Categories
*/

//-----------------------------------------------
// TorqueScript Data Types
//-----------------------------------------------

/*!
   @defgroup TorqueScriptTypes Data Types
      @{
*/

/*! Vector2 is a string of two numbers separated by a space.

    For example, "50 30.5" is a 2-vector with an x value of 50 and a y value of 30.5.
	You can create a vector from scalars (single numbers) by using the SPC operator.
	~~~
	50 SPC 30.5
	~~~
*/
typedef type Vector2;

/*! Vector is a string of any number of values separated by spaces between each.

    For example, "1 2 3 4" is a 4-vector, as is "a b c d".
	You can create a vector from scalars (single numbers) by using the @ref SPC operator.
	~~~
	"50" SPC "30.5"
	~~~
	You can also combine smaller Vectors with the SPC operator:
	~~~
	"a b" SPC "c d"
	~~~
*/
typedef type Vector;

/*! string is a series of characters of arbitrary length.

	You represent a string by surrounding it with double quotes, such as "some string".
	For example, "this is a string" but so is "this" or even "".
*/
typedef type string;

/*! @} */
//-----------------------------------------------
// End TorqueScript Data Types
//-----------------------------------------------

//-----------------------------------------------
// TorqueScript Operators
//-----------------------------------------------

/*!
   @defgroup TorqueScriptOps Operators
      @{
*/

/*! SPC concatenates two values into a string with a space between them

	~~~
	"50" SPC "30.5"
	~~~

	produces "50 30.5"
*/
typedef type SPC;

/*! @} */
//-----------------------------------------------
// End TorqueScript Operators
//-----------------------------------------------

#endif // PSEUDOCODE_FOR_DOXYGEN