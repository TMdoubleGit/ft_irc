#include "Bot.hpp"
#include "Server.hpp"

Bot::Bot(): _name("IRCSavvy"), _blue("\033[34m"), _green("\033[32m"), _reset("\033[0m"), _yellow("\033[33m")
{
}

Bot::Bot(const Bot &copy)
{
    *this = copy;
}

Bot& Bot::operator=(const Bot &obj)
{
    _name = obj._name;
    return (*this);
}

Bot::~Bot()
{
}

std::string Bot::getName() const { return _name; }

std::string Bot::Introduction() const
{

    std::string s = _blue + "I, my name is IRCSavvy and i'm your friendly neighborhood irc bot.\n" + _reset;
    s += "You can call me with " + _green + "/msg IRCSavvy [number choice]!" + _reset + " Here is what i can do for your:\n";
    s += _blue + "[1]" + _reset + " Server Information. I can tell you all i know about this server!\n";
    s += _blue + "[2]" + _reset + " Random trivia. Do you want some joke or maybe an inspiring quote ?\n";
    s += _blue + "[3] + [Euros]" + _reset + " Euro converter. I love money, do you ? See how much dollars you can get for your euros.\n";
    s += _blue + "[4] + [NumberofRoll] + [NumberOfFace]" + _reset + " Dice Roll. Always usefull for games or crucial life decision-making.";

    return s;
}

std::string Bot::convertEuro(float euro, std::string s_euros) const
{
    float dollar = euro * 1.06;
    float yen = euro * 159.07;
    float pound_sterling = euro * 0.87;
    float franc_swiss = euro * 0.95;
    float dollar_canadian = euro * 1.46;

    std::string s_dollar, s_yen, s_pound, s_franc, s_canadian;
    std::stringstream ss;

    ss << dollar;
    s_dollar = ss.str();
    ss.str("");
    ss << yen;
    s_yen = ss.str();
    ss.str("");
    ss << pound_sterling;
    s_pound = ss.str();
    ss.str("");
    ss << franc_swiss;
    s_franc = ss.str();
    ss.str("");
    ss << dollar_canadian;
    s_canadian = ss.str();

    std::string s = _blue + "Here is your " + s_euros + " Euros, converted in the most popular currency in the world\n" + _reset;
    s += _yellow + "US Dollars: " + _reset + s_dollar + "\n";
    s += _yellow + "Japenese Yens: " + _reset + s_yen + "\n";
    s += _yellow + "Pound Sterlings: " + _reset + s_pound + "\n";
    s += _yellow + "Swiss Francs: " + _reset + s_franc + "\n";
    s += _yellow + "Canadian Dollars: " + _reset + s_canadian;

    return s;
}

std::string Bot::getTrivia() const
{
    std::srand(std::time(0));
    int rng = std::rand() % 20;

    switch (rng) {
        case 0:
            return "Why don't programmers like nature?\n" + _blue + "It has too many bugs." + _reset;
        case 1:
            return "How many programmers does it take to change a light bulb?\n" + _blue + "None. That's a hardware problem." + _reset;
        case 2:
            return "Why do C++ programmers wear glasses ?\n" + _blue + "Because they don't C#." + _reset;
        case 3:
            return "What does the C++ programmer say to the C programmer ?\n" + _blue + "You have no class." + _reset;
        case 4:
            return "I told my dad that I was taking a C programming course.\n" + _blue + "He offered to give me some pointers." + _reset;
        case 5:
            return "Why are blind people bad at programming?\n" + _blue + "Because they can't C." + _reset;
        case 6:
            return "Did you know ?\n" + _blue + "The first computer game was created in 1961." + _reset;
        case 7:
            return "Did you know ?\n" + _blue + "The world’s first computer programmer was a woman named Ada Lovelace." + _reset;
        case 8:
            return "Did you know?\n" + _blue + "The first computer covered 1,800 square feet." + _reset;
        case 9:
            return "Did you know?\n" + _blue + "While we may all know Python as a giant snake, the name of the Python programming language actually comes from an old BBC comedy sketch series called Monty Python’s Flying Circus." + _reset;
        case 10:
            return "Did you know?\n" + _blue + "There are over 700 different programming languages." + _reset;
        case 11:
            return "Did you know?\n" + _blue + "According to many online studies, the most disliked programming languages are Perl, Delphi, and VBA." + _reset;
        case 12:
            return "Did you know ?\n" + _blue + "The first programming language was called FORTRAN." + _reset;
        case 13:
            return "Did you know ?\n" + _blue + "NASA still uses programs from the 70s in their spacecraft, called ADA and HAL/S." + _reset;
        case 14:
            return "Did you know ?\n" + _blue + "C programming language has a predecessor known as “B,” which was conceived and written by Ken Thompson in 1969." + _reset;
        case 15:
            return "Code is like humor. When you have to explain it, it’s bad." + _blue + " – Cory House" + _reset;
        case 16:
            return "Software is like sex: it’s better when it’s free." + _blue + " – Linus Torvalds" + _reset;
        case 17:
            return "You might not think that programmers are artists, but programming is an extremely creative profession. It’s logic-based creativity." + _blue + " – John Romero" + _reset;
        case 18:
            return "If debugging is the process of removing bugs, then programming must be the process of putting them in." + _blue + " – Sam Redwine" + _reset;
        case 19:
            return "If, at first, you do not succeed, call it version 1.0." + _blue + " ― Khayri R.R. Woulfe" + _reset;
        default:
            return "";
    }
}

std::string Bot::getRollDice(std::vector<std::string> &options)
{
    int faces_number;
    int dice_number;
    std::srand(std::time(0));
    std::vector<int> roll_dice;
    std::string ret = "";

    if (options.size() < 2) {
        faces_number = 20;
        dice_number = 1;
    }
    else if (options.size() < 3) {
        faces_number = 20;
        if (!isUnsignedInt(options[1]))
            dice_number = 1;
        else {
            std::istringstream iss(options[1]);
            iss >> dice_number;
        }
    }
    else {
        if (!isUnsignedInt(options[1]))
            dice_number = 1;
        else {
            std::istringstream iss(options[1]);
            iss >> dice_number;
        }
        if (!isUnsignedInt(options[2]))
            faces_number = 20;
        else {
            std::istringstream iss2(options[2]);
            iss2 >> faces_number;
        }
    }
    for(int i = 0; i < dice_number; i++)
        roll_dice.push_back((std::rand() % faces_number) + 1);
    ret += _blue + "Here is your " + intToString(dice_number) + "D" + intToString(faces_number) + ": " + _reset;
    for (size_t i = 0; i < roll_dice.size(); i++) {
        ret += intToString(roll_dice[i]) + ", ";
    }
    ret.erase(ret.length() - 2);
    return ret;
}

bool Bot::isFloat(const char* s)
{
    char* endPtr;
    strtod(s, &endPtr);
    if (*endPtr != '\0' && *endPtr != '\n') {
        return false;
    }
    return true;
}

bool Bot::isUnsignedInt(const std::string &str)
{
    for (size_t i = 0; i < str.length(); ++i) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return !str.empty();
}

std::string Bot::intToString(int value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str(); 
}