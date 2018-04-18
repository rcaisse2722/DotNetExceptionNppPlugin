//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//#include <stdio.h>

#include "PluginDefinition.h"
#include "menuCmdID.h"

#define DEBUG

using namespace std;
//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;

#ifdef DEBUG
// 
// Logger
// 
FILE* logFile;
#endif

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE hModule)
{
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{

    //--------------------------------------------//
    //-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
    //--------------------------------------------//
    // with function :
    // setCommand(int index,                      // zero based number to indicate the order of command
    //            TCHAR *commandName,             // the command name that you want to see in plugin menu
    //            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
    //            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
    //            bool check0nInit                // optional. Make this menu item be checked visually
    //            );
    setCommand(0, TEXT("Format"), formatException, NULL, false);
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here
}


//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//
void formatException()
{
#ifdef DEBUG
	logFile = fopen("C:\\temp\\out.log", "w+");
#endif
	// Get the current scintilla
	int which = -1;
	::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
	if (which == -1)
	{
#ifdef DEBUG
		fprintf(logFile, "Error getting current scintilla.");
		fclose(logFile);
#endif
		return;
	}
	HWND curScintilla = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

	::SendMessage(curScintilla, SCI_SELECTALL, 0, NULL);

	int bufferLength = ::SendMessage(curScintilla, SCI_GETTEXTLENGTH, 0, NULL);
	if (bufferLength > 0)
	{
		char* buffer = new char[bufferLength];
#ifdef DEBUG
		fprintf(logFile, "Buffer length %d\n", bufferLength);
#endif
		::SendMessageA(curScintilla, SCI_GETSELTEXT, bufferLength + 1, (LPARAM)buffer);
#ifdef DEBUG
		fprintf(logFile, "Content:\n %s\n", buffer);
#endif
		std::string input(buffer);
		formatString(input);

		::SendMessage(curScintilla, SCI_REPLACESEL, 0, (LPARAM)input.c_str());
		delete buffer;	
	}
#ifdef DEBUG
	fclose(logFile);
#endif	
}

//
// This function does the actual formatting of the input string
// Used the following Git repo for reference:
// https://github.com/staxmanade/ExceptionMessageBeautifier
//
void formatString(std::string& input)
{
#ifdef DEBUG
	fprintf(logFile, "Testing:\n %s\n", input.c_str());
#endif

	replaceAll(input, " at ", "\r\n   at ");
	replaceAll(input, " --- End of inner exception stack trace --- ", "\r\n--- End of inner exception stack trace--- ");

#ifdef DEBUG
	fprintf(logFile, "Output:\n %s\n", input.c_str());
#endif
}

//
// Found here:
// http://bits.mdminhazulhaque.io/cpp/find-and-replace-all-occurrences-in-cpp-string.html
//
void replaceAll(string& source, string const& find, string const& replace)
{
	for (string::size_type i = 0; (i = source.find(find, i)) != string::npos;)
	{
		source.replace(i, find.length(), replace);
		i += replace.length();
	}
}