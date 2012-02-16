#ifndef __MC_MAPPARSER_H__
#define __MC_MAPPARSER_H__

#include "crystalspace.h"


class mcMapParser
{
  public:
    mcMapParser(iObjectRegistry *object_reg);
    ~mcMapParser();
 
    bool Open(const char* filename);
    
    bool GetNextToken(csString& str);
    bool GetIntToken(int &val);
    bool GetFloatToken(double &val);
    bool GetTextToken(csString &str);
 
    int  GetCurrentLine();
 
    void ReportError (const char* message, ...);

  protected:
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

#endif // __MC_MAPPARSER_H__

