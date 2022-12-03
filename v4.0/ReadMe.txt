------------------------
ReadMe.txt for
Col's SuDoku Solver v4.0
------------------------

Hello, and congratulations for downloading Col's SuDoku Solver v4.0!
Welcome to one of the most powerful SuDoku tools on the web.  You are
ready to ally yourself with a flexible suite of problem-solving tools,
and actually learn HOW the computer solves these fiendishly addictive
problems.  What's more, you can follow the thinking process yourself
and step through it one stage at a time.  Thanks to an advanced array
of analytical techniques, you can now solve nearly all SuDoku puzzles
you will ever come across, and you can do this using logic alone!
There's no guesswork involved, folks!

This ReadMe file is designed to introduce you to how Col's SuDoku
solver works, and how it will help you to improve your problem-solving
skills.  It contains three sections:

(1) How to use Col's SuDoku Solver
(2) The Algorithms
(3) What's Coming Up in Version v4.0?

If you have any comments on Col's SuDoku solver, or if you have
suggestions for the next version, then please feel free to email me
at colin@frayn.net.

Happy solving!
Col

---------------
Dr. Colin Frayn
CERCIA,
School of Computer Science
University of Birmingham,
Edgbaston, Birmingham, B15 2TT, UK
8th August, 2005
colin@frayn.net
---------------



----------------------------------
(1) How to use Col's SuDoku Solver
----------------------------------

Col's SuDoku solver doesn't need any special installation - it's deliberately
as simple as possible to use.  All you need to do is to unzip the distribution
files into a directory somewhere on your hard drive, and then you can start
playing! If you need a copy of WinZip then you can get an evaluation version
from the following website: http://www.winzip.com/

To start the solver, run the SuDoku.exe file.

You will initially be presented with two windows (you can move them around
so that they fit nicely into your monitor, if you wish.)  One screen is the
SuDoku grid itself, and the other is the control panel.  The SuDoku grid can
be re-sized by using the drag-and-resize control at the corners of the window.
The fonts automatically re-size.  In order to set up a new puzzle, either load
one of the bundled data files (see the list below) using the File->load menu
option, or else make your own.  You can click on any of the grid squares and
input a number, either using the keypad, or by scrolling with your mouse wheel.
You can also generate random puzzles from the File->New-> menu. This may take
some time for the more difficult puzzles, as they are much harder to generate.
If it is taking too long, then you can cancel the generation procedure at any
time. All puzzles can be saved using the File->save menu option, so that
you can come back to them at a later date.

Contained within the .zip file are:
- SuDoku.exe  (The main executable)
- ReadMe.txt  (This file)
- Progress.txt  (Indicates changes in this version)
- Simple.dat   (A simple SuDoku puzzle)
- Complex1.dat, Complex2.dat, Complex3.dat
  These are unsolvable using simple methods.
- Unsolved1.dat, Unsolved2.dat
  These are unsolvable without using guesswork methods (Nishio pruning)!

In terms of solving problems, there are two linked methods.  The first one is the
basic set of techniques that were included in versions v1.0 and v2.0. That is to say:
#1 'fill in any cells which only have one single permission.' and #2 'if there is
only one cell in any row/column/box that could have a certain value, then it MUST
have that value'.  Or, put simply, these two correspond to "Which cells could only
be one possible number?" and "which numbers could only be in one possible cell?"

These two simple rules are available near the top of the control pad, named
"Only one option", and then three boxes named "Box", "Row" and "Col", filling in
values that could only occur in one possible place in every box, row or column.
The "Do All" button repeatedly applies these rules until it can get no further.
And the "Clear" option clears the potential solutions.  Potential solutions can
be accepted using the "Update Board" button.  At this stage, the solutions become
fixed on the board, though they can still be undone.  The Undo/Redo options are
in the 'Edit' menu in the main board window.

Also in the main window menus are options to display the current code version, 
to reset the current solution and to load/save a problem file. You can also start
a new puzzle, or exit the programme. In the Help menu, you have the option of opening
an "Explanation Window" which explains in words exactly what the solver has done.
You can get extended information by clicking the "Details" button after highlighting
any particular line. In the "Edit" menu, you can automatically solve, or rate any 
SuDoku puzzle, though this solver won't give any feedback - it purely uses a blind
search technique (Knuth's "Dancing Links" algorithm).

Returning to the control panel, you have a second set of buttons towards the
middle of the panel. These deal with the "permission matrix". That is to say, the
possible values available for every cell.  By pressing 'Show Permissions' you can
view those values.  Pressing the same button again hides them.  'Reset Permissions'
resets the values to their default state after being altered.  The default state
is to calculate which numbers are possible by eliminating those numbers which already
exist in the same box/row/column.  You can toggle permissions on or off in each
cell by clicking them with the right mouse button.

Moving down the panel, we reach the numbers 1-9, plus a blank button.  Pressing any
of these greys out the cells in the board which could not possibly contain that
value. Pressing the blank button resets this to the normal view.

Finally, at the bottom of the dialog box are eight buttons which alter the permission
matrix directly. They are another four logical rules which can be used to eliminate
potential values from the permission matrix using logic alone. Explanations for
these algorithms are given in section (2) below. The last two ("Nishio" and "Forcing
Chains") aren't strictly logic rules, but they are included here for completeness.

------------------
(2) The Algorithms
------------------

Versions 1.0 and 2.0 contained simple solving algorithms, which could be
used to complete most of the simpler SuDoku puzzles. However, the more complex
ones require a substantially more advanced set of methods.  In version v3.0 I
introduced some advanced methods for reducing the permission matrix. In v4.0
I have introduced some more complex ones. All permission reduction methods
are listed here;

- Manual permission toggling : You can now manually select/remove permission
values in the permission matrix.  To do this, click a permission value with
the right mouse button.  With a permission value changed, you can then re-run
the solving algorithms from that stage onwards.

- Undo / redo options : Just in case you get stuck! Note that these reset any
permission values that you have changed.

- I have implemented seven 'reducing candidates' rules.  The following web page 
gives an excellent explanation of these:

http://www.simes.clara.co.uk/programs/sudokutechniques.htm)

- I have also implemented the 'Forcing Chains' rule, which helps to 

For further explanations, try using these methods and examine the help given
in the 'Explanations' window. You can switch this on or off via the 'Help' menu.
Full textual explanations of the major algorithms are given there whenever they
are used.

---------------------------------------
(3) What's Coming Up in Future Versions
---------------------------------------

I have absolutely no idea. Any suggestions?

Your feedback is valuable, and I greatly appreciate it. Thanks for helping
me build Col's SuDoku Solver into (soon, I hope) the web's #1 SuDoku solver
resource!

Cheers,
Col


