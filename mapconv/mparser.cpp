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

#include "mparser.h"

mcMapParser::mcMapParser(iObjectRegistry *object_reg)
{
  m_empty_token = false;

  m_current_line = 0;

  m_peek_char = '\0';
  m_char_peeked = false;

  m_object_reg = object_reg;
}


mcMapParser::~mcMapParser()
{
}


bool mcMapParser::Open(const char* filename)
{
  csRef<iVFS> vfs = csQueryRegistry<iVFS>(m_object_reg);

  m_file = vfs->Open(filename, VFS_FILE_READ);

  if (!m_file.IsValid())
  {
    ReportError("Error opening map \"%s\"", filename);
    return false;
  }

  m_current_line = 1;

  if ( !ReadNextToken(m_next_token) )
  {
    m_next_token.Clear();
  }

  return true;
}


bool mcMapParser::GetChar(char &c)
{
  if(m_char_peeked)
  {
    c = m_peek_char;
    m_char_peeked = false;
    return true;
  }

  if(m_file->Read(&c, 1) != 1)
  {
    return false;
  }

  return true;
}

bool mcMapParser::PeekChar(char &c)
{
  if(!m_char_peeked)
  {
    if(m_file->Read(&m_peek_char, 1) != 1)
    {
      return false;
    }
  }

  m_char_peeked = true;
  c = m_peek_char;
  return true;
}


bool mcMapParser::SkipWhitespace()
{
  char c;

  for(;;)
  {
    if( !PeekChar(c) )
    {
      return false;
    }

    if(c != ' ' && c != '\t' && c != '\r')
    {
      break;
    }

    if( !GetChar(c) )
    {
      return false;
    }
  }

  return true;
}


bool mcMapParser::SkipToNextLine()
{
  char c;

  for(;;)
  {
    if( !GetChar(c) )
    {
      return false;
    }

    if( c == '\n' )
    {
      break;
    }
  }

  return true;
}


bool mcMapParser::GetNextToken (csString& str)
{
  str.Replace (m_next_token);

  if (m_next_token.IsEmpty() && !m_empty_token)
  {
    return false;
  }

  ReadNextToken (m_next_token);
  return true;
}


bool mcMapParser::ReadNextToken (csString& str)
{
  char c, next_c;

  str.Clear();
  m_empty_token = false;

  /* Skip commented and empty lines */
  for(;;)
  {
    if( !SkipWhitespace() ) 
    {
      return false;
    }

    if( !GetChar(c) )
    {
      return false;
    }

    if(c == '\n')
    {
      /* An empty line */
      ++m_current_line;
      continue;
    }

    if( PeekChar(next_c) )
    {
      if( c == '/' && next_c == '/')
      {
        if( !SkipToNextLine() )
        {
          return false;
        }
      }
      else
      {
        break;
      }
    }
    else
    {
      break;
    }
  }

  /* Quoted strings are returned directly */
  if (c == '\"')
  {
    for(;;)
    {
      if ( !GetChar(c) )
      {
        /* If we fail now, the token is not complete! */
        return false; 
      }

      if (c == '\"')
      {
        /* The end of the string has been reached */
	m_empty_token = str.IsEmpty();
        return true;
      }

      str << c;
    }
  }

  /* Check for special single character tokens */
  if ( c == '{'  || c == '}' ||
       c == '('  || c == ')' ||
       c == '\'' || c ==':'  )
  {
    str << c;
    return true;
  }

  /* Parse a regular word */
  for(;;)
  {
    str << c;

    if (!PeekChar(next_c))
    {
      break;
    }

    if ( next_c == '{'  || next_c == '}'  ||
         next_c == '('  || next_c == ')'  ||
         next_c == '\'' || next_c ==':'   ||
         next_c == ' '  || next_c == '\t' || 
         next_c == '\r' || next_c == '\n' )
    {
      break;
    }

    if(!GetChar(c))
    {
      break;
    }
  }

  return true;
}


bool mcMapParser::GetIntToken  (int &val)
{
  csString buffer;

  if (!GetNextToken(buffer))
  {
    ReportError("Unexpected end of file. (expected an int number)");
    return false;
  }

  char dummy;
  if (sscanf(buffer, "%d%c", &val, &dummy) != 1)
  {
    ReportError("Invalid Numeric format. Expected int, found \"%s\"",
                buffer.GetData());
    return false;
  }

  return true;
}


bool mcMapParser::GetFloatToken(double &val)
{
  csString buffer;
  if (!GetNextToken(buffer))
  {
    ReportError("Unexpected end of file. (expected a double number)");
    return false;
  }

  char dummy;
  if (sscanf(buffer, "%lf%c", &val, &dummy) != 1)
  {
    ReportError("Invalid Numeric format. Expected double, found \"%s\"", 
                buffer.GetData());
    return false;
  }

  return true;
}


bool mcMapParser::GetTextToken (csString &val)
{
  if (!GetNextToken(val))
  {
    ReportError("Unexpected end of file. (expected a text)");
    return false;
  }

  return true;
}


int  mcMapParser::GetCurrentLine() 
{
  return m_current_line;
}


void mcMapParser::ReportError(const char* message, ...)
{
  csPrintf("Error in line %d: ", m_current_line);
  va_list args;
  va_start (args, message);
  csPrintfV (message, args);
  va_end (args);
  csPrintf("\n");
}

