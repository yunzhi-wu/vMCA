
--------- GENERAL ----------

Requirement: Visual Studio Express 10 (or newer)
             Windows XP (or newer)

This directory contains several examples of plug-ins, and a separate folder for the the plug-in framework.

You are recommended NOT to modify any files in the plugin_framework folder, as these tend to change with new LogScrutinizer and 
plug-in kit versions. Later, if you don't modify the framework files, it should be a simple copy and replace action required 
when a new plug-in kit version is available/downloaded.

--------- SHORT STEP-BY-STEP TO IMPLEMENT A PLUG-IN ----------

When developing a new plug-in you do the following steps:

1. Copy and rename one of the example folders
   Note: Keep the new folder at the same level as the plugin_framework (as the examples)
   
2. Modify the files dll_plugin.cpp and dll_plugin.h with your specific plots and decoders.
   Note: These two files are the only ones you need to modify

3. Build

4. Drag-and-drop the file: dll_plugin.dll created in the build folder (debug or release) into LogScrutinizer

5. In the LogScrutinizer workspace select the plot name, right click, and select run (if it is a plug-in with plot capabilities)

--------- DEBUGGING PLUG-INS ----------

To ease the plug-in debugging each of the example plug-ins has a testApp directory. This contains a similar implementation compared
to how LogScrutinizer runs a plug-in.

Hence, by building and modifying the testApp, according to how you want to test you plug-in, you can then set break-points in you
plug-in and debug line-by-line in developer studio.

--------- EXAMPLE DESCRIPTIONS ----------

Folder: Plugin_Example_1
Description:
  This plug-in example shows the minimum implementation to create a plug-in with PLOT capabilities drawing lines.

Folder: Plugin_Example_2
Description:
  This plug-in example shows the minimum implementation to create a plug-in with DECODER capabilities.

 Folder: Plugin_Example_3
Description:
  This plug-in example shows an implementation with PLOT capabilities drawing different variant of lines and boxes in
  several sub-plots.
     subPlot_1, boxes with different colors and labels
     subPlot_2, lines with different colors and labels
     subPlot_3, graphs with overriden color, preventing LogScrutinizer to assign them colors automatically

 Folder: Plugin_Example_3_1
Description:
  This plug-in example shows an implementation with PLOT capabilities drawing non-coherent lines 

Folder: Plugin_Example_4
Description:
  This plug-in example shows an implementation using sequence diagrams  

Folder: Plugin_Example_5
Description:
  This plug-in example shows the minimum implementation to create a plug-in with PLOT capabilities drawing lines, and 
  implementing the callback function to provide additional information about the graphical objects in the plot when the
  user is hovering over it.
  
--------- HELP AND SUPPORT ----------  

Ask questions here
http://www.logscrutinizer.com/FORUM/index.php/

