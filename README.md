ArmyLister is WIP. The code is mostly void of any comments. Currently 40k is in a bit of a hiatus, waiting for more 9.ed. codexes to come out, hopefully making the game at least somewhat balanced again. For now, developement is directed at 9th Age armies.

Currently building armydata files is done with a text input, with some information on text commands.
Many functions may not work or exist, if I haven't needed them yet.
The goal of my project is to create a listing tool able to stand up with the other tools currently available, though I don't expect to ever actually meet this goal. Just having a nice proper working program of my own making is a goal good enough.

The code is written with Qt 5.14 C++, and requires no additional libraries.

To use the program, it's first neccessary to create the armydata files. Each is made with a different editor under the "Create" menu. With the open nature of 9th Age, its armylists should be fine to be shared openly, I shall see how to approach this. 40k lists will likely remain unshared, as GW is quite infamously protective of its assets. 

Using the main program should be easy enough. Select an armylist, define max points and start selecting on the list on right. Click on plusses to expand items, click on items to select them, double-click on units to create another copy of them. Each selection will show points counting to category limits in the respective slot on left.


TODO:
- Editor outlook
- Add selectable common tags to editor
- Saving selections to file
- Loading selections from file
- Tooltip information on list items, espec. 9A special items
- ...
- 40k lists

POSSIBILITIES:
- Rich text/PDF printing, with more graphical elements
- Basic statlines for units, maybe even included with prints
- ...
