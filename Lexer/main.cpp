#include <cassert>
#include <iostream>
#include <unordered_set>
#include "Lexer.hpp"

struct Rule {
    std::string regex;
    std::string type;
};

int main () {

    std::vector<Rule> rules = {
        {"auto", "1"}, {"break", "2"}, {"case", "3"}, {"char", "4"},
        {"const", "5"}, {"continue", "6"}, {"default", "7"}, {"do", "8"},
        {"double", "9"}, {"else", "10"}, {"enum", "11"}, {"extern", "12"},
        {"float", "13"}, {"for", "14"}, {"goto", "15"}, {"if", "16"},
        {"int", "17"}, {"long", "18"}, {"register", "19"}, {"return", "20"},
        {"short", "21"}, {"signed", "22"}, {"sizeof", "23"}, {"static", "24"},
        {"struct", "25"}, {"switch", "26"}, {"typedef", "27"}, {"union", "28"},
        {"unsigned", "29"}, {"void", "30"}, {"volatile", "31"}, {"while", "32"},
        {"-", "33"}, {"--", "34"}, {"-=", "35"}, {"->", "36"},
        {"!", "37"}, {"!=", "38"}, {"%", "39"}, {"%=", "40"}, {"&", "41"},
        {"&&", "42"}, {"&=", "43"}, {"\\(", "44"}, {"\\)", "45"},
        {"\\*", "46"}, {"\\*=", "47"}, {",", "48"}, {"\\.", "49"},
        {"/", "50"}, {"/=", "51"}, {":", "52"}, {";", "53"},
        {"\\?", "54"}, {"\\[", "55"}, {"\\]", "56"}, {"^", "57"},
        {"\\^=", "58"}, {"{", "59"}, {"\\|", "60"}, {"\\|\\|", "61"},
        {"\\|=", "62"}, {"}", "63"}, {"~", "64"}, {"\\+", "65"},
        {"\\+\\+", "66"}, {"\\+=", "67"}, {"<", "68"}, {"<<", "69"},
        {"<<=", "70"}, {"<=", "71"}, {"=", "72"}, {"==", "73"},
        {">", "74"}, {">=", "75"}, {">>", "76"}, {">>=", "77"},
        {"\"", "78"}, {"\\/\\*@.*\\*\\/", "79"}, {"\\/\\/@l*", "79"}, 
        {"@d+(L|l)?|0(x|X)@h+", "80"}, {"@d+.@d+((e|E)(+|-)?@d+)?", "80"}, {"(@a|_)@w+", "81"},
        {"\\%(-)?(.@d+)?(d|ld|lld|f|lf|c|s|p|x)","81"},
        {"@a@d?","81"}, {"#", "82"}, {"'", "82"},{"@s+", "space/ctrl"}
    };

    RegexParser* parser = new RegexParser();
    for (auto [regex, type] : rules) {
        parser->registerRegex(regex, type);
    }
    DFAConverter* converter = new DFAConverter(parser);
    std::string sourceCode = "int main()\n{\n\nprintf(\"HelloWorld\");\n\n return 0;\n\n}";
    sourceCode = "int main()\n\n{\n\nint i = 0;// comment test\n for (i = 0; i != 10; ++i)\n {\n\tprintf(\"%d\",i);\n\t}\n\treturn 0;\n}";
    sourceCode = "int main(){char c = \"h\";/* comment 12313test */if (c==\"h\")printf(\"%c\",c);else print(\"k\");return 0;}";
    sourceCode = "auto break case char const continue default do double else enum extern float for goto if int long register return short signed sizeof static struct switch typedef union unsigned void volatile while - -- -= -> ! != % %= & && &= ( ) * *= , . / /= : ; ? [ ] ^ ^= { | || |= } ~ + ++ += < << <<= <= = == > >= >> >>= \" /*comment1*/ -1.2e+10 auto_ //asdfa";
    sourceCode = "auto123break break_case case%char3const_continue _default 0do1.23double else_enum ex!tern float4.5for6goto if+int/long_register r56eturn -short+signed-sizo_fstatic+struct++switch typedef union&unsigned ?void<<=volatilewhile-+-->%=%&&&=(123)*,./:/;/<<<<====>>=>>=\"/*comment2\n中文*/ \"string\"";
    sourceCode = "#include <stdio.h>\n\nint main() {\n    int x = 10;\n    double pi = 3.14159265;\n    char letter = 'A';\n  \"hfjkasdhfjaaaaaaaaaaaaaaakasdhf\"  char message[] = \"Hello, world!\";\n\n    printf(\"The value of x is %d\n\", x);\n    printf(\"The value of pi is %.2lf\n\", pi);\n\n    // This is a single-line comment.\n    printf(\"The letter is %c\n\", letter);\n\n    /*\n    This is a multi-line comment.\n    It can span multiple lines.\n  中文也可以\n  */\n\n    printf(\"Message: %s\n\", message);\n\n    // Keywords\n    int if_variable = 5;\n    while (if_variable > 0) {\n        if (if_variable % 2 == 0) {\n            printf(\"Even: %d\n\", if_variable);\n        } else {\n            printf(\"Odd: %d\n\", if_variable);\n        }\n        if_variable--;\n    }\n\n    return 0;\n}\n";
    Lexer lexer(*converter, sourceCode);

    int token_count = 1;
    for (Token token = lexer.getNextToken(); token.type != "" && token.type != "EOF"; token = lexer.getNextToken()) {
        // format: count:<value,type>
        if (token.type != "space/ctrl") {
            std::cout << token_count++ << ":<" << token.value << "," << token.type << ">" << std::endl;
        }
    }
}
