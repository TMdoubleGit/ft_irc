#ifndef FILE_HPP
#define FILE_HPP

#include "Server.hpp"

class File
{
    private:
        std::string _filename;
        std::string _filepath;
        std::string _sender;
        std::string _receiver;
    
    public:
        File(std::string filename, std::string filepath, std::string sender, std::string receiver);
        File(const File &copy);
		File& operator=(const File &obj);
        ~File();

        //getter
        std::string getFileName() const;
        std::string getFilePath() const;
        std::string getSender() const;
        std::string getReceiver() const;
        
    private:
        File();
};

#endif