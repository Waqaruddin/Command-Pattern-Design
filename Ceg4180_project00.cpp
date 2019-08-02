////Syed Waqar Uddin
//CEG 4180
////Project00 
////Due Date June 11th, 2019

#include <iostream>
#include <vector>
#include <memory>
#include <cassert>
#include <string>

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////
/// This global variable exists solely to provide some state that is modified by commands.
/// In a real application, teh application state would not be global and would not be so 
/// simple.
string str = "Hello World";

// Foreward declaration
class UndoManager;

//////////////////////////////////////////////////////////////////////////////////////////
/// This abstract base class defined the interface that all "commands" must implement.
class Command
{
public:
	///////////////////////////////////////////////////////////////////////////////////////
	/// Call this method to invoke a command that modifies teh application state
	virtual void exec() = 0;

	///////////////////////////////////////////////////////////////////////////////////////
	/// \return a pointer to a command that is the opposite e.g. if the command is "delete"
	/// then an opposite might be "insert". If there is no opposite command, return 
	/// nullptr and override isUndoable() to return false.
	virtual std::shared_ptr<Command> getOppositeCommand() = 0;

	///////////////////////////////////////////////////////////////////////////////////////
	/// \return true if the command has an opposite and false otherwise.
	virtual bool isUndoable() { return true; }
};

//////////////////////////////////////////////////////////////////////////////////////////
/// This class encapsulates undo and redo stacks to enable unlimited undo and redo of
/// commands. Commands are instances of concrete subclasses of the Command class.
class UndoManager
{
	std::vector<std::shared_ptr<Command> > m_undoStack;
	std::vector<std::shared_ptr<Command> > m_redoStack;
	std::vector<std::shared_ptr<Command> > *m_currentStack_p = &m_undoStack;

public:
	///////////////////////////////////////////////////////////////////////////////////////
	/// An application may have more than one undo manager, but it is assumed there will
	/// be at least one, so here it is.
	static UndoManager sharedUndo;

	///////////////////////////////////////////////////////////////////////////////////////
	/// Call this method to invoke the command, c, in a manner that makes later undo of the
	/// command possible.
	///
	///\param c a pointer to a command to invoke
	void doCommand(std::shared_ptr<Command>c)
	{
		if (c->isUndoable())
		{
			m_currentStack_p->push_back(c->getOppositeCommand());
		}
		c->exec();
	}

	///////////////////////////////////////////////////////////////////////////////////////
	/// Invoke the command at the top of the undo stack and pop it off. The opposite of
	/// the invoked command is pushed onto the redo stack
	void undo()
	{
		assert(0 < m_undoStack.size());
		std::shared_ptr<Command>c = m_undoStack.back();
		m_currentStack_p = &m_redoStack;
		this->doCommand(c);
		m_currentStack_p = &m_undoStack;
		m_undoStack.pop_back();
	}

	///////////////////////////////////////////////////////////////////////////////////////
	/// Invoke the command at the top of the redo stack and pop it off. The opposite of
	/// the invoked command is pushed onto the undo stack
	void redo()
	{
		assert(0 < m_redoStack.size());
		std::shared_ptr<Command> c = m_redoStack.back();
		m_currentStack_p = &m_undoStack;
		this->doCommand(c);
		m_redoStack.pop_back();
	}
};

//////////////////////////////////////////////////////////////////////////////////////////
/// This class encapsulates a single menu item. This class exists to show how an object
/// may "have a" command to execute. A button or script might similarly "have" commands
/// to be executed. In fact, a script might be nothing more than a list of commands to
/// execute.
class MenuItem
{
	///////////////////////////////////////////////////////////////////////////////////////
	/// The command to execute when onPress() is called.
	std::shared_ptr<Command>m_command;

public:

	MenuItem(std::shared_ptr<Command>c) : m_command(c)
	{
		// Intentionally empty
	}

	///////////////////////////////////////////////////////////////////////////////////////
	/// Change the command associated with this menu item at run time.
	///
	/// \param c the command to invoke when onPress() is called
	void setCommand(std::shared_ptr<Command>c)
	{
		m_command = c;
	}

	///////////////////////////////////////////////////////////////////////////////////////
	/// Invoke the command associated with the pressed menu item.
	///
	/// This implementation asks the shared undo manager to invoke the command so that 
	/// the command is automatically recorded for later undo.
	void onPress()
	{
		UndoManager::sharedUndo.doCommand(m_command);
	}

};


//This class is an example of a command that appends a letter at 
//the end of the string.

class AppendEndLetterCommand : public Command
{
	string m_letter;
	
public:
 AppendEndLetterCommand(string letter)
	{
	 m_letter = letter;
	}
	void exec() { 
		str += m_letter;
	cout << str << endl;
	}
	std::shared_ptr<Command> getOppositeCommand();
};

//This class is an example of a command that deletes a letter from 
//the end of the string.
class DeleteEndLetterCommand : public Command
{
	string endLetter;
public:
	DeleteEndLetterCommand()
	{
		endLetter = str[str.size() - 1];
	}

	void exec() { str.pop_back(); cout << str << "\n"; }
	std::shared_ptr<Command> getOppositeCommand()
	{
		return std::shared_ptr<Command>(new AppendEndLetterCommand(endLetter));

	}

	
};

//This class is an example of a command that appends a letter at 
//the start of the string.

class AppendStartLetterCommand : public Command
{
	string l;
public:
	AppendStartLetterCommand(string s)
	{
		l = s;
	}

	void exec() { str.insert(0, l); cout << str << "\n"; }

	std::shared_ptr<Command> getOppositeCommand();
	

};

//This class is an example of a command that deletes a letter from
//the beginning of the string.
class DeleteStartLetterCommand : public Command
{
	string l;
public:
	DeleteStartLetterCommand()
	{
		l = str[0];
	}
	void exec() {
		str.erase(str.begin());
		cout << str << endl;
	}
	std::shared_ptr<Command> getOppositeCommand()
	{
		return std::shared_ptr<Command>(new AppendStartLetterCommand(l));

	}
};

//This class is an example of the command that capitalizes 
//a letter at any index within a string
class CapitalStringCommand : public Command
{
	int m_index;
public:
	CapitalStringCommand(int index)
	{
		m_index = index;

	}
	void exec() {

		str[m_index] = toupper(str[m_index]);
		cout << str << endl;
	}
	std::shared_ptr<Command> getOppositeCommand();
};
//This class is an example of the command that lower
//cases a letter at any index within a string
class LowerStringCommand : public Command
{
	int m_index;
public:
	LowerStringCommand(int index)
	{
		m_index = index;
	}
	void exec() {
		str[m_index] = tolower(str[m_index]);
		cout << str << endl;
	}
	std::shared_ptr<Command> getOppositeCommand()
	{
		return std::shared_ptr<Command>(new CapitalStringCommand(m_index));

	}
};

//This class is an example of the command that lower
//cases the first letter of the string
class LowerFirstCommand : public Command
{
	void exec() {

		str[0] = tolower(str[0]);

		for (int i = 1; i < str.size(); i++)
		{
			if (str[i - 1] == ' ')
				str[i] = tolower(str[i]);
			else
				str[i] = tolower(str[i]);
		}

		std::cout << str << std::endl;
	}

	std::shared_ptr<Command> getOppositeCommand();
};

//This class is an example of the command that capitalizes
//the first letter of every word within a string
class TitleCaseCommand : public Command
{
	void exec(){
		/*const int size = 80;

		char sentence[size];

		std::cout << "Please enter a sentence.\n";
		std::cin.getline(sentence, size);*/

		str[0] = toupper(str[0]);

		for (int i = 1; i < str.size() ; i++)
		{
			if (str[i - 1] == ' ')
				str[i] = toupper(str[i]);
			else
				str[i] = tolower(str[i]);
		}

		std::cout << str << std::endl;
	}

	std::shared_ptr<Command> getOppositeCommand()
	{
		return std::shared_ptr<Command>(new LowerFirstCommand());

	}
};



//////////////////////////////////////////////////////////////////////////////////////////
/// This class provides and example of a command that invokes undo. This command has no 
/// opposite because the opposite operation is redo, but redo is built-in to the 
/// UndoManager class. It doesn't make sunse to undo an undo - instead, use redo.
class UndoCommandCommand : public Command
{
	void exec() { UndoManager::sharedUndo.undo(); }
	std::shared_ptr<Command> getOppositeCommand();

	///////////////////////////////////////////////////////////////////////////////////////
	/// \return false because undoing an undo is a redo.
	virtual bool isUndoable() { return false; }
};


//////////////////////////////////////////////////////////////////////////////////////////
/// This class provides and example of a command that invokes redo. This command has no 
/// opposite because the opposite operation is undo, but undo is built-in to the 
/// UndoManager class. It doesn't make sense to redo a redo.
class RedoCommandCommand : public Command
{
	void exec() { UndoManager::sharedUndo.redo(); }
	std::shared_ptr<Command> getOppositeCommand()
	{
		return std::shared_ptr<Command>(new UndoCommandCommand());
	}

	///////////////////////////////////////////////////////////////////////////////////////
	/// \return false because undoing a redo is an undo.
	virtual bool isUndoable() { return false; }
};

//////////////////////////////////////////////////////////////////////////////////////////
/// This test program creates instances of MenuItem initialized with different commands.
/// The program then calls onPress() for teh various menu item to simulate a user pressing
/// graphical menu items in a user interface.
int main(int argc, const char * argv[])
{
	MenuItem menuUndo(std::shared_ptr<Command>(new UndoCommandCommand()));
	MenuItem menuRedo(std::shared_ptr<Command>(new RedoCommandCommand()));
	MenuItem menuA(std::shared_ptr<Command>(new AppendEndLetterCommand("h")));
	MenuItem menuB(std::shared_ptr<Command>(new DeleteEndLetterCommand()));
	MenuItem menuD(std::shared_ptr<Command>(new DeleteStartLetterCommand()));
	MenuItem menuE(std::shared_ptr<Command>(new CapitalStringCommand(3)));
	MenuItem menuF(std::shared_ptr<Command>(new LowerStringCommand(0)));
	MenuItem menuG(std::shared_ptr<Command>(new TitleCaseCommand()));
	MenuItem menuH(std::shared_ptr<Command>(new LowerFirstCommand()));

	menuA.onPress();
	UndoManager::sharedUndo.undo();
	UndoManager::sharedUndo.redo();

	menuB.onPress();
	UndoManager::sharedUndo.undo();
	UndoManager::sharedUndo.redo();


	menuD.onPress();
	UndoManager::sharedUndo.undo();
	UndoManager::sharedUndo.redo();

	menuE.onPress();
	UndoManager::sharedUndo.undo();
	UndoManager::sharedUndo.redo();


	menuF.onPress();
	UndoManager::sharedUndo.undo();
	UndoManager::sharedUndo.redo();

	menuG.onPress();
	UndoManager::sharedUndo.undo();
	UndoManager::sharedUndo.redo();

	

	return 0;
}


UndoManager UndoManager::sharedUndo = UndoManager();


std::shared_ptr<Command> AppendEndLetterCommand::getOppositeCommand()
{
	return std::shared_ptr<Command>(new DeleteEndLetterCommand());
}

std::shared_ptr<Command> AppendStartLetterCommand::getOppositeCommand()
{
	return std::shared_ptr<Command>(new DeleteStartLetterCommand());
}

std::shared_ptr<Command> CapitalStringCommand::getOppositeCommand()
{
	return std::shared_ptr<Command>(new LowerStringCommand(m_index));
}

std::shared_ptr<Command> LowerFirstCommand::getOppositeCommand()
{
	return std::shared_ptr<Command>(new TitleCaseCommand());
}
std::shared_ptr<Command> UndoCommandCommand::getOppositeCommand()
{
	return std::shared_ptr<Command>(new RedoCommandCommand());
}
