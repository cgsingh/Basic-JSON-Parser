/*
Name: Christopher Singh
Course: OOP345A
Date: October 10, 2014
*/

#include "JSONReader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

namespace json
{
	int read(const std::string& filename, std::string* buffer)
	{
		int i = 0;
		std::ifstream file(filename);
		std::string line;

		if (file.is_open())
		{
			while (!file.eof())
			{
				std::getline(file, line);
				buffer[i] = line;
				i++;
				if (i >= 50)
				{
					break;
				}
			}
			file.close();
			
			//there is a logic error in main in a1.cpp (not by me but whoever made main)
			//if number of lines is 1, then make it 2 - otherwise, the loop in main will be false and extract will never be called
			if (i == 1)
			{
				buffer[i] = "";
				i++;
			}
			
			//for whatever reason, matrix reads file differently so I created this algorithm to set line where buffer at line is not an empty line ("").
			//unfortunately, the sample file you provided, collection.json still gives me a segment error on MATRIX, but NOT on Windows.
			int tI = i;
			for (int j = 1; tI >= 0 && i != 2; j++)
			{
				if (buffer[i - j] == "")
				{
					tI--;
				}
				else
				{
					break;
				}
			}
			return tI;
		}
		return -1;
	}

	int extract(const std::string* buffer, int no_lines, int& lN, Collection& c)
	{
		int ptr = 0;
		std::string tName = "";
		std::string tValue = "";
		int nvpNext = 0;
		int totalCollection = 0;
		bool pass = false;
		int numPairs = 0;
		bool isObject = false;
		int tLine = lN;
		static int z = 0;
		int tLn = lN - 1;
		int tPtr = ptr;
		bool onLine = false;
		static int firstTime = true;
		static int memPtr = 0;
		static int incr = 2;
		bool remPtr = false;

		if (memPtr != 0)
		{
			ptr = memPtr;
			memPtr = 0;
		}

		//if line number is 1, then set it to 0 to start from the first line
		if (firstTime)
		{
			lN--;
		}

		//if this is the first time the function is viewing the buffer, then skip empty lines and spaces
		if (firstTime)
		{
			while (lN != no_lines && (buffer[lN] == "" || isspace(buffer[lN].at(ptr))))
			{
				ptr++;
				if (ptr >= buffer[lN].length())
				{
					lN++;
					//if line number goes out of bounds, revert ptr back to the previous alue
					if (lN == no_lines)
					{
						ptr--;
					}
					else
					{
						ptr = 0;
					}
				}
			}
		}

		//if line number is equal to no_lines - therefore, out of bounds, then decrement it
		if (lN == no_lines)
		{
			lN--;
		}
		
		//if this is the firstTime extract is called and it found the { to start, move to the next character
		if (firstTime && buffer[lN].at(ptr) == '{')
		{
			ptr++;
			if (ptr >= buffer[lN].length())
			{
				lN++;
				//if line number goes out of bounds, revert ptr back to the previous alue
				if (lN == no_lines)
				{
					ptr--;
				}
				else
				{
					ptr = 0;
				}
			}
		}

		//if static int z is 1 - indicating something special with the lN and incr variable has been done, OR this is not the first time the extract function is called indicating something has been done with incr (special or not) revert
		//what has been done at the bottom of the extract function to continue where the scanner left off
		if (z == 1 || !firstTime)
		{
			lN -= incr;
			z = 0;
		}
		
		//skip whitespace including blank lines
		while (lN != no_lines && (buffer[lN] == "" || isspace(buffer[lN].at(ptr))))
		{
			ptr++;
			if (ptr >= buffer[lN].length())
			{
				lN++;
				//if line number goes out of bounds, revert ptr back to the previous alue
				if (lN == no_lines)
				{
					ptr--;
				}
				else
				{
					ptr = 0;
				}
			}
		}

		//while collection is not ended(ended means either it's on the last line and/or element of it, or it didn't find the } incidicating the end of the collection
		while (lN != no_lines && (ptr != buffer[lN].length() - 1 || !(isspace(buffer[lN].at(ptr)) || buffer[lN] == "")) && buffer[lN].at(ptr) != '}')
		{
			//keep looping so long as character is whitespace
			while (lN < no_lines && ptr <= buffer[lN].length() - 1 && isspace(buffer[lN].at(ptr)))
			{
				ptr++;
				if (ptr == buffer[lN].length())
				{
					lN++;
					ptr = 0;
				}
			}

			//check if collection has more than one name-value pair
			if (ptr <= buffer[lN].length() - 1 && buffer[lN].at(ptr) == ',')
			{
				//allow it to go into the algorithm again
				pass = true;
			}
			
			//allow entry if collection is just beginning or there is another name-value pair to parse
			if (ptr <= buffer[lN].length() - 1 && buffer[lN].at(ptr) == '"' || pass)
			{
				pass = false;

				//memPtr = ptr;

				//loop until found double quote - indicating the start of a name or an object name
				while (lN < no_lines && ptr <= buffer[lN].length() - 1 && buffer[lN].at(ptr) != '"')
				{
					ptr++;
					//check if ptr is out of bounds
					if (ptr >= buffer[lN].length())
					{
						lN++;
						ptr = 0;
					}
				}
				//store where it found the start of a double quote into the static int memPtr - could be the start of an object name - if so, I want the next call to extract to start at this point
				memPtr = ptr;
				
				//keep track of line where possible name-value pair or object start is on
				tLine = lN;

				//increment to set the ptr to the first character of a name or object name
				ptr++;
				
				//check if ptr is out of bounds
				if (ptr >= buffer[lN].length())
				{
					lN++;
					ptr = 0;
				}

				//loop until found double quote - indicating the end of a name or an object name
				while (lN < no_lines && ptr <= buffer[lN].length() - 1 && buffer[lN].at(ptr) != '"')
				{
					tName += buffer[lN].at(ptr);
					ptr++;
					if (ptr == buffer[lN].length())
					{
						lN++;
						ptr = 0;
					}
				}

				//increment to set to the next character that isn't the ending quote
				ptr++;
				//check if ptr is out of bounds
				if (ptr >= buffer[lN].length())
				{
					lN++;
					ptr = 0;
				}

				//loop until found colon
				while (lN < no_lines && ptr <= buffer[lN].length() - 1 && buffer[lN].at(ptr) != ':')
				{
					ptr++;
					if (ptr == buffer[lN].length())
					{
						lN++;
						ptr = 0;
					}
				}

				//increment to set to the next character that isn't the colon
				ptr++;
				//check if ptr is out of bounds
				if (ptr >= buffer[lN].length())
				{
					lN++;
					ptr = 0;
				}

				//loop until found opening curly brace to indicate the start of an object - or a double quote indicating the start of a value
				while (lN < no_lines && ptr <= buffer[lN].length() - 1 && buffer[lN].at(ptr) != '{' && buffer[lN].at(ptr) != '"')
				{
					ptr++;
					if (ptr == buffer[lN].length())
					{
						lN++;
						ptr = 0;
					}
				}

				//check if it is value
				if (lN < no_lines && ptr <= buffer[lN].length() - 1 && buffer[lN].at(ptr) == '"')
				{
					ptr++;
					if (ptr == buffer[lN].length())
					{
						lN++;
						ptr = 0;
					}

					//loop until found double quote - indicating the end of a value
					while (lN < no_lines && ptr <= buffer[lN].length() - 1 && buffer[lN].at(ptr) != '"')
					{
						tValue += buffer[lN].at(ptr);
						ptr++;
						if (ptr == buffer[lN].length())
						{
							lN++;
							ptr = 0;
						}
					}
					c.name[nvpNext] = tName;
					c.value[nvpNext++] = tValue;
					numPairs++;
					tName = "";
					tValue = "";

					tLn = lN;
					tPtr = ptr;

					while (lN < no_lines && tPtr <= buffer[tLn].length() - 1 && (isspace(buffer[tLn].at(tPtr)) || buffer[tLn].at(tPtr) == '"'))
					{
						tPtr++;
						if (tPtr == buffer[tLn].length())
						{
							tLn++;
							tPtr = 0;
						}
					}
					
					if (buffer[tLn].at(tPtr) == '}')
					{
						lN = tLn;
						ptr = tPtr;
					}

					while (lN != no_lines && (buffer[lN] == "" || buffer[lN].at(ptr) == '}' || isspace(buffer[lN].at(ptr))))
					{
						ptr++;
						if (ptr == buffer[lN].length())
						{
							lN++;
							if (lN == no_lines || (lN == 1 && no_lines == 2))
							{
								ptr--;
								if (lN == 1 && no_lines == 2)
								{
									lN--;
									break;
								}
							}
							else
							{
								ptr = 0;
							}
						}
						if (lN == no_lines - 1 && buffer[lN] == "")
						{
							break;
						}
						if (lN != no_lines && buffer[lN] != "")
						{
							if (buffer[lN].at(ptr) == ',')
							{
								lN = tLn;
								ptr = tPtr;
								break;
							}
						}
						while (lN != no_lines && (buffer[lN] == "" || isspace(buffer[lN].at(ptr))))
						{
							ptr++;
							if (ptr >= buffer[lN].length())
							{
								lN++;
								//if line number goes out of bounds, revert ptr back to the previous alue
								if (lN == no_lines)
								{
									ptr--;
								}
								else
								{
									ptr = 0;
								}
							}
						}
					}
					if (lN == no_lines)
					{
						lN--;
					}

					//keep looping so long as character is whitespace or the value's double quote
					while (lN != no_lines - 1 && buffer[lN] != "" && lN != no_lines && ptr <= buffer[lN].length() - 1 && buffer[lN].at(ptr) != '}' && (isspace(buffer[lN].at(ptr)) || buffer[lN].at(ptr) == '"'))
					{
						ptr++;
						if (ptr == buffer[lN].length())
						{
							lN++;
							if (lN == no_lines)
							{
								ptr--;
							}
							else
							{
								ptr = 0;
							}
						}
					}
					if (lN == no_lines)
					{
						lN--;
					}
					if (lN == no_lines - 1 && buffer[lN] == "")
					{
						break;
					}
				}
				//else it is an opening curly brace of an object
				else
				{
					//if numPairs is not equal to 0, then that means a new collection was created before this object - so stop the loop
					if (numPairs != 0)
					{
						//set lN back to the line where beginning of new collection starts
						lN = tLine;
						remPtr = true;
						break;
					}

					c.objectName = tName;
					isObject = true;
					tName = "";

					//change ptr to point to next character
					ptr++;
					if (ptr == buffer[lN].length())
					{
						lN++;
						ptr = 0;
					}
				}
			}
		}

		//check if this is the end of the collection
		if (lN != no_lines - 1 && buffer[lN] != "" && buffer[lN].at(ptr) == '}')
		{
			//keep looping until the character is not a whitespace and a closing curly brace
			while (lN < no_lines && ptr <= buffer[lN].length() - 1 && (isspace(buffer[lN].at(ptr)) || buffer[lN].at(ptr) == '}'))
			{
				ptr++;
				//check if lN is going out of bounds
				if (lN > no_lines - 1)
				{
					//decrement to make sure ptr does not go out of bounds
					ptr--;
					break;
				}
				if (ptr == buffer[lN].length())
				{
					lN++;
					if (lN == no_lines || (lN == 1 && no_lines == 2))
					{
						lN--;
						ptr--;
						break;
					}
					else
					{
						ptr = 0;
					}
				}

				//check if the next character is another closing curly brace
				if (buffer[lN] != "" && buffer[lN].at(ptr) == '}')
				{
					break;
				}

				while (buffer[lN] == "")
				{
					lN++;
				}

				if (lN > no_lines - 1)
				{
					lN--;
					break;
				}
			}
		}

		incr = 2;
		//check if character is a comma - indicating another collection is next
		if (lN != no_lines - 1 && buffer[lN] != "" && buffer[lN].at(ptr) == ',')
		{
			//keep looping until it finds the double quote - indicating the name of a possible name-value pair or object name
			while (lN < no_lines && ptr <= buffer[lN].length() - 1 && (isspace(buffer[lN].at(ptr)) || buffer[lN].at(ptr) != '"'))
			{
				ptr++;
				if (ptr == buffer[lN].length())
				{
					lN++;
					ptr = 0;
				}
			}
			int diff = (lN + 2) - no_lines;
			if (diff > -1)
			{
				while (!(diff <= 0))
				{
					diff--;
				}
				if (lN + 2 >= no_lines)
				{
					incr = diff;
					z = 1;
				}
			}
			memPtr = ptr;
		}
		else
		{
			if (remPtr == false)
			{
				memPtr = 0;
			}
		}
		//increment in case this is the last collection so for loop in main doesn't call this function once more
		lN += incr;

		//set name-value pair to safe empty state for printing later on
		c.name[nvpNext][0] = '\0';
		c.value[nvpNext][0] = '\0';

		firstTime = false;

		//if the for loop condition in main is false - reset all the static variables
		if (!(lN < no_lines))
		{
			z = 0;
			firstTime = true;
			memPtr = 0;
			incr = 2;
		}
		//if collection is an object
		if (isObject)
		{
			return 1;
		}
		//set safe empty state for an anonymous collection
		c.objectName[0] = '\0';
		return 0;
	}

	int print(const Collection& collection)
	{
		int numPairs = 0;

		//check how many name-value pairs there are
		while (collection.name[numPairs][0] != '\0')
		{
			numPairs++;
		}

		//print objectName if it's not in safe empty state
		if (collection.objectName[0] != '\0')
		{
			std::cout << '"' << collection.objectName << '"' << std::endl;
		}

		for (int i = 0; i < numPairs; i++)
		{
			std::cout << '\t' << '"' << collection.name[i] << '"' << "=>" << '"' << collection.value[i] << '"' << std::endl;
		}

		//add new line to separate next collection
		std::cout << std::endl;

		return numPairs;
	}
}
