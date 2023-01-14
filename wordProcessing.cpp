#include "wordProcessing.hpp"
#include <fstream>
#include <string>
#include <iterator>
#include <vector>
#include <sstream>
#include <algorithm>
#include <stdlib.h>
#include <time.h>

namespace wordProcessing {
    namespace {
        // Private section dedicated to verification and file reading
        std::array<char, 6> vowels = {'a', 'e', 'i', 'o', 'u', 'y'};
        std::array<char, 21> consonants = {'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k',
                                           'm', 'n', 'p', 'q', 'r', 's', 't', 'v',
                                           'w', 'x', 'z'};
        std::ifstream wordListStream("res/wordList.txt");
        std::vector<std::string> getContents() {
            std::string newWord;
            std::vector<std::string> contents;
            while (wordListStream >> newWord) {
                contents.push_back(newWord);
            }
            return contents;
        }

        bool isInList(std::string& answer) {
            auto contents = getContents();
            auto wordIt = std::lower_bound(contents.begin(), contents.end(), answer);
            if (wordIt != contents.end() && *wordIt == answer) {
                return true;
            }
            return false;
        }

        bool canBeFormed(std::string& word, std::vector<char> charsetVector) {
            // Remove char if it already appears in the word
            // (!) This functions assumes the charsetVector is sorted
            for (char character: word) {
                auto charIt = std::lower_bound(charsetVector.begin(), charsetVector.end(), character);
                if (charIt == charsetVector.end() || *charIt != character) {
                    return false;
                }
                charsetVector.erase(charIt);
            }
            return true;
        }

        bool validCharset(std::array<char, 16>& charset) {
            std::vector<std::string> wordList = getContents();
            int formableWordCount = 0;
            
            for (std::string word : wordList) {
                // Create a vector, which copies the contents of the charset
                if (canBeFormed(word, std::vector<char>(std::begin(charset), std::end(charset)))) {
                    formableWordCount++;
                    if (formableWordCount == 5) {
                        return true;
                    }
                }
            }
            return false;
        }
    }

    bool validateAnswer(std::array<char, 16>& charset, std::string& answer) {
        // The wordList file is sorted alphabetically. This allows for the usage of binary search
        if (canBeFormed(answer, std::vector<char>(charset.begin(), charset.end())) && isInList(answer)) {
            return true;
        }
        return false;
    }

    std::array<char, 16> generateCharset() {
        srand(time(NULL));
        bool isValid = false;
        std::array<char, 16> resultCharset;
        std::vector<char> combinedCharacters;
        combinedCharacters.reserve(vowels.size() + consonants.size());
        combinedCharacters.insert(combinedCharacters.end(), vowels.begin(), vowels.end());
        combinedCharacters.insert(combinedCharacters.end(), consonants.begin(), consonants.end());
        while (!isValid) {
            
            int counter = 0;
            // Randomize 6 vowels
            for (; counter < 6; counter++) {
                resultCharset[counter] = vowels[rand() % vowels.size()];
            }

            // Randomize 4 consonants
            for (; counter < 10; counter++) {
                resultCharset[counter] = consonants[rand() % vowels.size()];
            }

            // Randomize the final 6 letters
            for (; counter < 16; counter++) {
                resultCharset[counter] = combinedCharacters[rand() % combinedCharacters.size()];
            }
            isValid = validCharset(resultCharset);
        }  
        // Sort the charset. This allows the use of binary search algorithms in the process of answer verification
        std::sort(resultCharset.begin(), resultCharset.end());
        return resultCharset;
    }
}
