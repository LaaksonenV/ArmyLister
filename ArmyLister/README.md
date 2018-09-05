ArmyLister is WIP, built for my own use first, and public use second. The code is quite totally void of any comments.

As now, building armydata files is done with a text input, with limited and possibly dated information.
Many functions may not work, if I haven't needed them yet.
The goal of my project is to create a listing tool able to stand up with the oter tools currently available, though I don't expect to ever meet this goal.

The code is written with Qt 5.9, and requires no additional libraries.

To use the program, it's first neccessary to create the armydata files. Each is made with a different editor under the "Create" menu.
Editors should contain information for items with special limitations and such. All editors create a treelist.
	Wargear and Unit point list refer to the tables at the end of codexes
	Wargear list refer to the possible lists of wargear immediately after the armywide abilities (eg. Sergeant weapons)
	Army list refers to the main listing of the codex, including all the options. Creating this will take time, and be tgenerally a pain in the ass,
		as you have to some understanding of how the selection model work.
		Make the previous lists first, as you will be asked to include those for the army list, and are the able to use autocompleter function when writing the list.
	Detachment means all the possible detachments an battleforged army may have (in the Rulebook). Basic detachments could be ok to be shared publicly,
		So I may do so at some point. Then this allows to create additional detachments (in the beginning of 8.ed I thought armies might have their own additional detachments,
		so this editor would be needed)

After all data files are ready, you will be able to start building your army, hopefully with the rules and limitations that you made to the army list.
You can save and load your selections to a file, and print it as a text file.


TODO:
-Make the program work properly
-look at editors
-...

POSSIBILITIES:
-Rich text/PDF printing, with more graphical elements
-Basic statlines for units, included with prints
-...