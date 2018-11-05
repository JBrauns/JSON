#if !defined(JSON_PARSER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joshua Brauns $
   $Notice: $
   ======================================================================== */

#define JSON_IDENTIFIER_COMPONENTS 0

#define FormatString(...) _snprintf(__VA_ARGS__);

class JSONParser :
    public Tokenizer
{
public:
    JSONParser(const char *fileName) :
            Tokenizer(fileName, JSON_IDENTIFIER_COMPONENTS) {}
    ~JSONParser() {}

    //! Parse the complete json file into a json root object
    JSONObject *Parse(const char *fileName);

    //! Parse a JSON object
    JSONObject *ParseObject(char *key, size_t keyLength);

    //! Parse a JSON array
    JSONArray *ParseArray(char *key, size_t keyLength);
    
    //! Parse a string
    JSONString *ParseString(Token startToken, char *key, size_t keyLength);
    
    //! Parse a string
    JSONNumber *ParseNumber(Token token, char *key, size_t keyLength);
    
    //! Parse a string
    JSONLiteral *ParseLiteral(Token token, char *key, size_t keyLength);
};

#define JSON_PARSER_H
#endif
