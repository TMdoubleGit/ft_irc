#include "File.hpp"


File::File(std::string filename, std::string filepath, std::string sender, std::string receiver)
{
    _filename = filename;
    _filepath = filepath;
    _sender = sender;
    _receiver = receiver;
}

File::~File()
{
}

std::string File::getFileName() const { return _filename; }

std::string File::getFilePath() const { return _filepath; }

std::string File::getSender() const { return _sender; }

std::string File::getReceiver() const { return _receiver; }

File::File()
{ 
}

File::File(const File &copy)
{
    *this = copy;
}

File& File::operator=(const File &obj)
{
    _filename = obj._filename;
    _filepath = obj._filepath;
    _sender = obj._sender;
    _receiver = obj._receiver;
    return (*this);
}
