#include<fstream>
#include<istream>
#include<string>
#include<memory>
#include<chrono>
#include<iostream>
#include "./ModernDictionary/ModernDictionary.hpp"
#include "./utils/utils.hpp"


void loadDictionary(const std::string &filePath, const std::unique_ptr<dictionary::ModernDictionary> &dict) {

    std::ifstream inputFileStream(filePath);
    std::string word;

    if(inputFileStream.is_open()) {

        while(std::getline(inputFileStream, word)) {
            util::preProcessText(word);
            dict->insertWord(word);
        }

        dict->calculateAndCacheCounts();
        inputFileStream.close();
    }

    else {
        std::cout<<"Error opening the file. \n";
    }
}

void loadandProcessOperations(const std::string &filePath, const std::unique_ptr<dictionary::ModernDictionary> &dict) {

    std::ifstream inputFileStream(filePath);
    std::string operation;
    std::string argument;
    std::string outputFilePath = "output.txt";

    if(inputFileStream.is_open()) {

        std::ofstream outputFileStream(outputFilePath);
        
        if(outputFileStream.is_open()) {

            while(inputFileStream>>operation>>argument) {
                std::string outputString = dict->processOperations(operation, argument);
                outputString+= "\n";
                outputFileStream<<outputString;
            }

            outputFileStream.close();
        }

        else {
            std::cout<<"op file not open \n";
        }

        inputFileStream.close();

    }

    else {
        std::cout<<"File not open\n";
    }

}


int main(int argc, char* argv[]) {

    if(argc == 1) {
        std::cout<<"Please pass the input file name and operations instructions file name. \n";
    }

    std::string inputFile = argv[1];
    std::string operationsFile = argv[2];


    inputFile = "./" + inputFile;
    operationsFile = "./" + operationsFile;

    std::unique_ptr<dictionary::ModernDictionary> dict = std::make_unique<dictionary::ModernDictionary>();
    util::Timer timeCalculator;

    timeCalculator.start();
    loadDictionary(inputFile, dict);
    int64_t timeTaken = timeCalculator.msPassed();

    std::cout<<"Dictionary loaded and counts cached in: "<<timeTaken<<" milliseconds. \n";

    timeCalculator.start();
    loadandProcessOperations(operationsFile, dict);
    timeTaken = timeCalculator.msPassed();

    std::cout<<"Processed all operations in: "<<timeTaken<<" milliseconds. \n";

    return 0;

}
