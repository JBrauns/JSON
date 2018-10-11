#if !defined(JSON_PARSER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joshua Brauns $
   $Notice: $
   ======================================================================== */

#define JSON_IDENTIFIER_COMPONENTS 0

class JSONParser :
    public Tokenizer
{
public:
    JSONParser(const char *fileName) :
            Tokenizer(fileName, JSON_IDENTIFIER_COMPONENTS) {}
    ~JSONParser() {}

    //! Parse the complete json file into a json root object
    bool Parse(JSONObject **parent);

    //! Expecially for string handling
    Token GetNextJsonToken();
    
    //! Parse a string
    char *ParseString(Token token);

    //! Get a root json object
    JSONObject *GetRoot();
};

#define JSON_PARSER_H
#endif
