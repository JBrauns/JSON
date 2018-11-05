#if !defined(TOKENIZER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joshua Brauns $
   $Notice: $
   ======================================================================== */

#define ASCII_CHAR_SPACE 0x20
#define ASCII_CHAR_TAB 0x09
#define ASCII_CHAR_LINE_FEED 0x0A
#define ASCII_CHAR_CARRIAGE_RETURN 0x0D

// NOTE(joshua): By default an identifier consists at least of characters
enum IdentifierComponent
{
    IdentifierComponent_Number     = (1 << 0),
    IdentifierComponent_Underscore = (1 << 1)

    // TODO(joshua): Insert additional component types
};

enum TokenType
{
    TokenType_Undefined,

    TokenType_Identifier,
    TokenType_String,
    
    TokenType_SquareBracketOpen,
    TokenType_SquareBracketClose,
    TokenType_RoundBracketOpen,
    TokenType_RoundBracketClose,
    TokenType_CurlyBracketOpen,
    TokenType_CurlyBracketClose,
    TokenType_AngleBracketOpen,
    TokenType_AngleBracketClose,

    TokenType_Period,
    TokenType_Comma,
    TokenType_Colon,
    TokenType_Semicolon,

    TokenType_Number,
    TokenType_Plus,
    TokenType_Minus,

    TokenType_EndOfStream,
    
    TokenType_Count
};
struct Token
{
    TokenType Type;

    char *Text;
    size_t Length;
};

class Tokenizer
{
public:
    char *TextStart;
    char *TextAt;
    unsigned int validComponent;
    
    //! Constructor
    Tokenizer(const char *fileName, unsigned int identifierComponentFlags);
    
    //! Destructor
    ~Tokenizer();

    //! TODO(joshua): Documentation missing
    Token GetNextToken();

    //! TODO(joshua): Documentation missing
    size_t StringBetweenToken(Token tokenStart, char **stringStart);
    
    //! TODO(joshua): Documentation missing
    bool IsAlpha(char letter);

    //! TODO(joshua): Documentation missing
    bool IsNumber(char letter);

    //! TODO(joshua): Documentation missing
    char *SkipWhitespace(char *at);
};

#define TOKENIZER_H
#endif
