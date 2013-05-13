# Documenting TorqueScript

notes
+ TDN docs expounded
+ dox must have comments
	+ cannot be md in the open
+ md is not comments
	+ but can have "@" codes
+ pages
	+ can be made with markdown naturally
	+ or with a comment block starting with @@page

## Introduction

This references describes how the TorqueScript manual is created.  It covers the tool used, the location of the docs source, and
the process of adding or editing all the documentation types.  The end goal is a single manual that can be referenced on-line or downloaded.
The manual created contains information about classes and functions, and also information about operators, sub-systems,
general information, etc.

As a bit of background, TorqueScript is a scripting language written specifically for Torque.  The compiling and running
of the script code is a part of the engine itself.  Our goal here is to extract just the internal documentation that refers
to TorqueScript functions and classes (which are really "bindings" to C++ functions and classes), etc.

The manual is created using [Doxygen](doxygen.org), so its helpful to get some experience with that tool and
 also to refer to its manual for documenting features.  This reference only covers Doxygen lightly but probably
enough to get started.  It also covers our use of its high-level features to get what we want.  For now, we'll just
say that Doxygen is a feature-rich tool to extract documents out of a code base.  Doxygen recognized specialed comment
sections designed for this purse.  These sections are explicity about return types from functions, code examples, and
other features.  The end result is an organized series of pages, like an index, but with descriptions and concepts
explained and with links to each other as a cross-reference.

> Normally, Doxygen is used to create a cross-referenced document for a C++ code base.  Note that our desire here is
to document a class and function binding library, along with the syntax, operators, types, etc. of TorqueScript.
But Doxygen is so useful and capable that it was worth working out a way to use it for our purposes.  As a matter of fact,
we can also produce a Torque2D engine document from the same code base!

The docs can be output in an HTML form, among many others including pdf or chm.

## Documentation As an Ongoing Project

First, thanks in advance for any contribution to our collective knowledge to the TorqueScript docs!
Besides our collective wisdom, there are also fertile bits of information over at the
[Torque Developer Network](http://tdn.garagegames.com/wiki/TDN_Home) (free login required) that could be yanked
over.

While many of the basics are done, there is always more to cover.  Whether you know of tricks for creating better
game loops with less, gotchas to avoid, or mistake in the docs that should be corrected, we could use your help.  The goal
of this project is to collect great information about Torque2D and get it into the source itself,
where it will live with the code.  The docs should include classes, functions, and even architectural design.

## How to "Compile" the Torque2D Manual

If we want to see our docs as you go, the first thing you'll need is the ability to see the output from Doxygen as
you make edits.  Once you know how, this can be done in less than 10 seconds.  Here are the steps:

1. Go to the directory `Torque2D/tools/documentation`
     You should see, among files and directories
     + `config` - a directory containing a few files that are used to tell Doxygen how to proceed.
     + `output` - a directory where the results will go.  If it doesn't exist for you, simple make a new `output` directory.
	 (If you run `doxygen` without this directory, it will fail.)
     + `doxygen.exe` - currently Torque2D comes with a Windows copy of Doxygen.  But you can download a newer version or a
	 version for almost any platform, which you may need to do now.  It's free!
2. Start a command line tool in the `documentation` directory.
    I use Windows PowerShell (which I open by clicking the Window's Start menu and typing "powershell" into the search box).
3. Run Doxgyen from a command line feeding it the `config\torqueScriptReference.cfg` file.  I also like to pipe the output
to a file, as you see below.  Doxygen is very verbose!
	@code
	> .\doxygen.exe .\config\torqueScriptReference.cfg > .\output\info-ts.log
	@endcode
4. Afterwards, the directory `output/torqueScriptDocs` should have been produced, and inside of it, you should
have both `html` and `xml` output.  To see the results, click `html/index.html` to open it in a browser.
Click around and get accusomted to the manual if you haven't already.  As you make changes, "recompile" the manual
and simply refresh in your browser to see the results.

## About Doxygen "Compounds"

There are three Doxygen concepts -- called "compounds" in Doxygen vernacular -- that we use to document TorqueScript.
These are symbols, modules, and pages.  For us, "symbol" documents are materials attached directly to classes, member functions,
and global functions.  Here is SimSet as an example of documented class methods that roll-up into a class page.  The
[list of classes](classes.html) is built by Doxygen as well.
This is the most common type of documentation by far.  You write them in command-laced comments directly above the symbol.

Then we use "module" documents -- interchangeably called "group" documents in Doxygen vernacular -- to keep together clusters
of related global function, like vector math, string functions, or even string operators.  Within each grouping, the bulk
of the actual function documentation is also symbol documentation.  If you go to the [modules](modules.html) page, you will
see all of the groups listed.  Note that they mostly cover global functions, but also operators, data types, etc.

Module documents are nice because they are a bit more free-form.  You can also add any amount of extra documentation for
the entire group.  For instance, in the engine manual (not the TorqueScript one in this case), all the classes than form
the %Taml subsystem are kept in a group.  But more importantly, a lengthy white paper on using the %Taml module of Torque2D
is kept within the group document as the "detailed description".  The point is that module/group docs
can have lots of connective information about a sub-sytem and still cross-reference with any symbols in your code.

Finally, most rare of all we use Doxygen "pages" to write up information.  Pages are the best bet when their content isn't really
connected to the software architecture or any particular class or functions.  This very document is an example.  All the "loose"
pages are kept under the ["related pages"](pages.html) tab by Doxygen.
Even though the pages aren't related to any one symbol of grouping, mentioning particular functions and other symbols will
create cross references to those symbols.

## About ScriptBinding and ScriptPage Files

In order to have Doxygen create two kinds of documents with only one code base, some cleverness was required.  Without
getting too deep here, documenting TorqueScript "the language binding" isn't the natural purpose of Doxygen!
However, we were able to achieve a pretty strong manual just by using a few techniques.  To keep you from getting
confused -- we hope it's not a problem, but better safe than sorry -- this short section is a quick-and-dirty
view of some of those techniques used.  If something doesn't seem to work like you expected, this section may help.

The main rule to follow here is that most TorqueScript documentation is in the ScriptBinding.h files.  For instance, comments
in `SimObject_ScriptBinding.h` are gleened by Doxygen, 
but not comments in `SimObject.h` or `SimObject.cpp`.  Other docs may
be in ScriptBinding.* or ScripPage.* files.  
Because the ScriptBinding.h files contain the TorqueScript binding functions and not the engine code, this is an ideal
place to focus Doxygen to get only TorqueScript information.  (Otherwise, it would try to add reference docs to engine-specific
code as well.  We show Doxygen the opposite docs when building a manual for the engine.)

You may notice a few binding functions (ConsoleMethod(), ConsoleFunction(), etc.) not in the ScriptBindings.  You may also
notice that these don't produce docs yet.  This is for the reason stated above.  (The code is actively being re-arranged and is
almost complete.  For what it's worth, the idea of the ScriptBbinding files preceeds the new documentation.)

The next rule is that .dox files are not read by C++ compilers but are read as if code files by Doxygen.  The extension is
specific to Doxygen as a matter of fact.  Code and comments in these files are read (by Doxygen) as if live code, however.
Hence they are a great place to put module/group documents that don't fit neatly in any one class or function group.
The .dox files are often kept in the directory with the code they relate to.  They will be found and read, wherever they are, by Doxgyen.
If the .dox file is for TorqueScript it should contain `_ScriptPage.` in its name.  Otherwise, it will be assumed to be a page
for the engine instead.


A second use of the .dox file was to create pseudo-code for TorqueScript -- code that isn't really part of building the 
TorqueScript interpreter nor the engine but that Doxygen picks up and parses as if real code.  Look at the file
`Torque2D\engine\source\console\core_scriptBinding.dox` for a great place to create a list of
TorqueScript operators (new, SPC, TAB, etc) and TorqueScript types (Integer, Boolean, String, Vector) that aren't real
code but that help create a complete manual for TorqueScript.


Finally, the "page" files are often kept as markdown (extension .md).  Because a page file isn't related to any classes,
functions, or whatever, its text does not need to be wrapped into a comment.  Put differently, there is no need for doxygen
commands (@@param, @@see, @@defgroup, etc.)  The text is pure markdown with a few exceptions which are probably Doxygen bugs, and some
of those are listed at the end of this doc.  By being pure markdown -- that is without requiring a surrounding comment block --
they are a little easier to manipulate.
These pages can only show as single pages in the "Related Pages" tab of a Doxygen manual, so they are limited.  And
even though they aren't connected to any code symbols, they will link (automatically usually) to any symbols mentions.

## Documenting Classes and Methods

As mentioned above, documenting symbols is by far the bulk of the good documentation. Each class, method, or function can have a 
description as complex as you want.  (Also as stated above, the global functions will ultimately end up in a "module" section of
the manual but the functions themselves are documented as symbols.)  For methods and functions, 
you can (and should) explicitly state the parameters and return type.  You can put example code with each symbol if it helps.  
Finally, we should probably keep a "see also" section for related methods and functions.

Here is an edited version of comments for SimObject::getId() as an example of how to accomplish several of these practices:

@verbatim
/*! get the unique numeric ID -- or "handle" -- of this object.

	@param param1 a pretend param for this call since there wasn't one :)
	@return Returns the numeric ID.

	The id is provided for you by the simulator upon object creation.  You can not change it
	and it likely will not be reused by any other object after this object is deleted.

	@par Example
	@code
	new SimObject(example);
	echo(example.getId());
	> 1752
	@endcode

	@par Caveat
	You can not access the id directly.  That is, you can not access `%%object.id`.
	If you do set `%%object.id` you will only succeed in creating a dynamic field named
	`id` -- an unrelated field to the actual object's id.

	@par Example
	@code
	%example = SimObject();
	echo(%example.getId());
	> 1753

	... 

	I'm pretending I need one more paragraph here to show off lists.  I can use
	+ a list symbol such as "+" here on each line of a list
	+ a second element in my list

	@see getName, setName
*/
ConsoleMethodWithDocs(SimObject, getId, ConsoleInt, 2, 2, ())
{
   return object->getId();
}
@endverbatim

Knowing a bit about Doxygen does help, but you can get pretty far just trying the ideas above and experimenting.  The following
are several notes about the above example:

+ The entire doxygen comment section is surrounded by `/*!` and `*/` comment enclosings.  There are
other options, but this author prefers to set these at beginning and end and then not have to decorate every line.
+ We have a `@param`, `@return`, and `@see` (for "see also" sections). Those are easy.
+ Any other text is just part of the detailed description.
+ We can create "sections" just like the "param" and "return" sections by using `@par <section>` like we did with "@par Caveat".  Just remember to start the paragraph immediately after that line.
+ We also wanted to create a couple of examples so we start each one with "@par Example" and follow that with a block of code between "@code" and "@endcode".
+ We can also use backticks "`" around a phrase to make it represent in-line code.
+ Now in *real* code sections between "@code" and "@endcode" any text goes.  But in regular text
or even in in-line code sections, we have to use two percent signs to create one.
This is because Doxygen sees percent as a "meta-character" and swallows it.  To show a single percent,
"escape" percent with two in a row.
+ Note that I'm using "+" to start a list.  You can use "*" or "-" just like in markdown.  However, because
some people like to have a "*" at the beginning of every line of comments, Doxygen likes to eat those up, so
it's best to just avoid them.  This author just prefers "+" to "-" for lists also.
+ Obvious function names and class names will be automatically cross-referenced.  They are "obvious"
when they have a camel case letter in them somewhere.  If your function does not, try putting "::" in front of it
or "()" after it.  To reference something in another class, you have to use className::methodName.  Doxygen explains
all this in its docs.  One last point though: let's suppose you want to avoid a link.  Use "%" in front of the camel
cased word that happens to be another class or function.

## Documenting Global Function Groups

All of the global functions in TorqueScript are being documented into "groups." 
We also take advantage of Doxygen's ability to have subgroups here.  All of the function groups are
a subgroup of the TorqueScriptFunctions main group.  (There are other main groups not related to global functions.)

To document a group of functions, let's suppose they are in the same file together.  If they are not, we can
modify the following formulas.  Go to that file, say the console output functions in `output_ScriptBinding.h`.
Near the top of the file, before any functions, tell Doxygen this will be a group as follows:

@verbatim
/*! @defgroup ConsoleOutputFunctions Console Output
	@ingroup TorqueScriptFunctions
	@{
*/
@endverbatim

What this does is start a new group `ConsoleOutputFunctions` and also gives it the more human-readable name "Console Output".
Then it puts this group within the `TorqueScriptFunctions` main group.  And finally it starts the grou with `@{`.

Now go to the bottom of the file, somewhere after all the functions and put the following:

@verbatim
/*! @} */ // group ConsoleOutputFunctions
@endverbatim

This line ends the group with "@}".  The comment after that is just to be friendly.

This should effectively put all the documented functions between "@{" and "@}" into a group together.

## Documenting Operations, Types, etc.

Another type of module documentation is for non-functions.  This includes TorqueScript types (String, Boolean, etc.) and
operators (SPC, new, etc.).

To do this, we are playing a bit of a trick.  Remember that Doxygen was not created to document your language that you created
(such as TorqueScript).  What we will do is make a psuedo-code file full of pretend C++ constructs, called
`Torque2D\engine\source\console\core_ScriptBinding.dox`  The C++ compiler won't be interested, but when Doxygen sees
the file it will parse it as real code.

The solution we have employed is to have one special file located in the same directory as the TorqueScript interpreter code.
C++ compilers will not bother to compile a .dox file but doxygen will read it.  For all practical purposes, though,
this is a C++ file in form.

We will not go into the details here, but read that file and add to it to create lists of operators or
types in groups.

## Documenting Sub-Systems

Our final module type is sub-systems of the engine, such as %Taml or the Module Manager, etc.  Because these
are not related to one specific class, they were placed in individual files with the .dox extension.

While there are no examples of TorqueScript sub-systems documented yet, we can see an example of Torque2D engine
docs for a sub-system.  Check out `Torque2D\engine\source\persistence\taml\taml.dox` as an example.

Note that the docs are currently kept close to the classes that form the system, if possible.  Specifically,
they are put in the same directory as the classes.  The compiler will ignore files ending in .dox
but Doxygen won't.  In the case of the taml file, it is a mostly markdown formatted file.  Section headers,
URLs and other markdown data is present in standard form.  However, the entire file (of markdown) is placed
in a Doxygen comment block with `/*!` at the top and `*/` at the bottom.  Then the block was marked as
creating a "group" known as "tamlGroup" like so:

@code
/*! @defgroup tamlGroup TAML Reference
  ... the entire contents in markdown
*/
@endcode

For a little cross-referencing sweetness, classes related to this sub-system were marked with the following
comment

@code
/// @ingroup tamlGroup
/// @see tamlGroup
@endcode

This will bring a reference to the marked class into the module document.

Again, this was done for an engine subsytem, but the same *could* be done for TorqueScript systems or modules.

## Documenting General Information

Our final category of document is the "page" as described above.  Two great examples page documentation are the Syntax Guide
and the "how to" you are currently reading.  To create these, simply create a markdown file (extension .md).  The level-1 header of the file
(the section header that starts with a single hash #), will be the name of the page in the final document.

This markdown file should be suffixed with `_ScriptPage.*`, for instance "syntax_ScriptPage.md"

We try to keep the pages close to the code it relates to, but rememer "pages" tend to be unrelated to particular classes,
modules, etc.  Therefore the pages have been kept in the `Torque2D\engine\source\console` directory since it most
represents the entire TorqueScript interpreter in general.  For example this document is `documenting_ScriptPage.md`
within the `console` directory.

The content of the file requires no doxygen commands such as @@defgroup or @@param (although @@code has come in useful as will be
described in the appendix).  It is read as pure markdown (with a few
exceptions).  Doxygen will successfully create links for any symbols is recongizes however.  (To force the link off, put a
percent % in front of the text.

## Converting "Old" Documents to "New"

*If you read the appendix on how documenting TorqueScript is done with Doxygen, this may make more sense to you.*

You may find, from time to time, snippets of an older form of documentation for TorqueScript in the code base.  One of the
Appendices explains in more detail, but basically, documents used to be a "usage" string as part of a macro call like this:

@code
ConsoleFunction(myFunction, void, 4, 4, "(id) very long string of documents..."
						"@param including parameters and other doxygen commands!")
@endcode

These macro calls appeared with the source code in the same file, for instance in `SimObject.cc`.  For readability reasons,
these functions were mostly pulled to their own files, such as `SimObject_ScriptBinding.h`, but not all of them have made it.
(We could use help in moving the stragglers as well!)

The new documentation process takes advantage of these binding files being separate.  Additionally it needs a new macro that
does not take the usage string, called ConsoleFunctionWithDocs, etc.  For instance this is the new form:

@code
/*! very long string of documents
    @param which is a good thing now
*/
ConsoleFunctionWithDocs(myFunction, void, 3, 3, (id))
@endcode

Ultimately, when all the functions and methods are complete, the macro name may change back to not include the "WithDocs" ending.
For now, this is what you are seeing!

## Appendix A: Doxygen Command and Markup Caveats

It will probably be necessary to limit yourself some when trying to grab a markdown page and move it to the source base for
Doxygen's use.  While Doxygen claims to handle markdown, including GitHub flavored markdown, in practice we have run into a
few edge cases.  Below are a few of them.

+ lists starting with `*` may not work.  This is because it seems like the line is part of a comment surrounded with *.
If you use `+` or `-` it will work.

+ doxygen understands "fencepost" code blocks (~~~) or (~~~{.cpp}) but we have seen some issues.  Specifically, doxygen
will eat double-slash `//` comments within them!  This is probably a bug.  Luckily Doxygen understands its own commands
embedded in markdown, so you can use @@code and @@endcode around code sections.  This will preserver comments inside of those sections.

+ starting a word with % such as %%myVar is very common in TorqueScript, but Doxygen reserves % to mean "do not create a link here".
For instance, if you don't want SimObject to become a link, use `%%SimObject`.  For TorqueScript variables use two percents before your
variables to see one.  This is not necessary in code sections!  But it is necessary in in-line code and regular text.  :)

+ GitHub flavored markdown provides for easier numbered lists.  Simply start each item with `1.` and they will be re-numbered.
However this does not work in Doxygen.  Number each item correctly.
 
## Appendix B.  How Documenting TorqueScript Works

For several years, Torque2D produced a TorqueScript manual in a different way than described in this reference.

As a bit of back story, don't forget that we want to be able to produce a manual for Torque2D's C++ engine as well
as produce a manual for TorqueScript.  So running Doxygen on the code produced only the Torque2D manual.  This is "as it should be."

Back then, to create a TorqueScript manual one would compile Torque2D, and run it.  From within the program one would call
dumpConsoleFunctions() and other functions to produce a "psuedo-code" output file.  The file was suspiciously like a program itself,
complete with documentation of the "functions" and "classes".  This file would then be run through Doxygen and viola!, a
second reference manual was produced.

The problem with using the above method had two specific drawbacks worth removing if possible.  First, it required several steps -- 
compiling the code, running it against some script calls, running Doxygen on the output of the dump -- and so it was impractical to 
add and modify new docs as easily as Doxygen is designed to do.

Second, the script documents had to be hidden from Doxygen on the normal 
pass lest it include confusing bits of information in the engine docs!  To do so, the docs appeared in a (large) string for each TorqueScript
call or class.  This was harder to work with than simple comments.  A less important issue, but still a real one, was the possibility that
all these strings, compiled into the engine, could be expensive in space.  (Though the strings could be skipped at compile time if
TORQUE_SHIPPING was defined.)

The new solution employed a different clever solution.  Now, the TorqueScript function bindings are documented just like any other code
-- in comment sections with the code.  And "compiling" the documents is no harder than running Doxygen on the code base.
The way we get Doxygen to pick between documenting the engine or  
TorqueScript is that TorqueScript functions are in special files ending with `_ScriptBinding.*` or `_ScriptPage.*`.  These files are skipped when 
creating docs for the engine.  Likewise, they are the only files used when creating the script reference.
