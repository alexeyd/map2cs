/*
    Map2cs: a convertor to convert the frequently used MAP format, into
    something, that can be directly understood by Crystal Space.

    Copyright (C) 1999 Thomas Hieber (thieber@gmx.net)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __MPARSER_H__
#define __MPARSER_H__

#include "crystalspace.h"


/**
  * This class will parse the map file. It will translate the ASCII
  * stream into Tokens, and will also keep track of the current
  * line numbers.
  * It also contains simple error checking and reporting abilities,
  * so if you request a number and the map contains a text, you
  * will get a descriptive error message.
  * The parser is pretty general, so it could also be used to parse
  * a configuration file as well.
  */
class mcMapParser
{
public:
  mcMapParser(iObjectRegistry *object_reg);
  ~mcMapParser();

  bool Open(const char* filename);
  
  bool GetIntToken(int &val);
  bool GetFloatToken(double &val);
  bool GetTextToken(csString &str);

  int  GetCurrentLine();

  void ReportError (const char* message, ...);

protected:
  bool GetNextToken(csString& str);

  bool GetChar(char &c);
  bool PeekChar(char &c);

  bool ReadNextToken (csString& str);

  bool  SkipWhitespace ();
  bool  SkipToNextLine();

  csString m_next_token;
  bool m_empty_token;

  int   m_current_line;

  char m_peek_char;
  bool m_char_peeked;

  iObjectRegistry *m_object_reg;
  csRef<iFile> m_file;
};

#endif // __MPARSER_H__

