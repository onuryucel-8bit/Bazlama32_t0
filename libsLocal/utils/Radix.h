#pragma once
//update file test 160924
//TEST
#include<iostream>
//----
#include<vector>
#include<cstdint>
#include<string>
#include<sstream>
#include<algorithm>
#include<cmath>

//FIXME stoul ve turevlerinin guvenli versiyonunu ekle 13.06.25
namespace rdx {

	/*
	* hex => dec
	* dec => bin
	* dec => hex
	*/
/*
	std::string toBinaryString(size_t dec);

	char printDec_inhexType(int dec);
	
	//BIN => X
	size_t binToDec(std::string bin);
	std::string binToHex(size_t bin);

	//HEX => X
	uint32_t hexToDec(std::string hex);
	uint8_t hexToDec(char hex);
	std::string hextoBin(std::string bin);
	
	//DEC => X
	std::string decToBin(int dec);
	std::string decToHex(size_t dec);

	//BIN => STR
	//Gets bool array translates to bin str
	std::string binToStr(std::vector<bool> arr, size_t begin, size_t end);
	std::string binToStr(bool arr[], size_t size);
	std::string binToStr(bool arr[], size_t begin, size_t size);
*/
    //----------------------------------------------------------------//
    //----------------------------------------------------------------//
    //----------------------------------------------------------------//

    

    inline std::string toBinaryString(size_t dec)
    {
        std::string str = "";
        for (size_t i = dec; dec > 0; dec /= 2)
        {
            str = std::to_string(dec % 2) + str;
        }

        return str;
    }

    //TODO !code here
    inline size_t binToDec(std::string bin)
    {
        size_t result = 0;
        int power = 0;

        for (int i = bin.length() - 1; i >= 0; i--)
        {
            char bit = (bin[i] == '1') ? 1 : 0;
            result += std::pow(2, power) * bit;
            power++;
        }
        return result;
    }
    //TODO code
    inline std::string binToHex(size_t bin)
    {
        return std::string();
    }

    inline uint32_t hexToDec(std::string hex)
    {

        //0xFE => FE

        unsigned int dec = std::stoul(hex, nullptr, 16);

        return (uint32_t)(dec);
    }


    inline char printDec_inhexType(int dec)
    {
        switch (dec) {
        case 0:
            return '0';
        case 1:
            return '1';
        case 2:
            return '2';
        case 3:
            return '3';
        case 4:
            return '4';
        case 5:
            return '5';
        case 6:
            return '6';
        case 7:
            return '7';
        case 8:
            return '8';
        case 9:
            return '9';
        case 10:
            return 'A';
        case 11:
            return 'B';
        case 12:
            return 'C';
        case 13:
            return 'D';
        case 14:
            return 'E';
        case 15:
            return 'F';
        default:
            return '?'; // or any other value indicating an error
        }
    }

    inline uint8_t hexToDec(char hex)
    {

        hex = std::toupper(hex);

        switch (hex) {
        case '0':
            return 0;
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case 'A':
            return 10;
        case 'B':
            return 11;
        case 'C':
            return 12;
        case 'D':
            return 13;
        case 'E':
            return 14;
        case 'F':
            return 15;
        default:

            return 200;
        }
    }
    //TODO code here
    inline std::string hextoBin(std::string bin)
    {
        return std::string();
    }

    inline std::string decToBin(int dec)
    {
        std::string bin = "";

        while (dec > 1) {

            bin += std::to_string(dec % 2);
            dec /= 2;
        }

        bin += std::to_string(dec);

        std::reverse(bin.begin(), bin.end());

        return bin;
    }

    inline std::string decToHex(size_t dec)
    {

        std::stringstream ss;
        ss << std::hex << std::uppercase << dec;
        return ss.str();
    }

    inline std::string binToStr(std::vector<bool> arr, size_t begin, size_t end)
    {

        std::string result;
        for (size_t i = begin; i < end; i++)
        {
            result += (arr[i] ? '1' : '0');
        }

        return result;
    }

    inline std::string binToStr(bool arr[], size_t size)
    {
        std::string result;
        for (size_t i = 0; i < size; i++)
        {

            result += (arr[i] ? '1' : '0');
        }
        if (result.empty())
        {
            result = "0";
        }
        return result;
    }

    inline std::string binToStr(bool arr[], size_t begin, size_t size)
    {
        std::string result;
        for (size_t i = begin; i < size; i++)
        {

            result += (arr[i] ? '1' : '0');
        }

        return result;
    }
   
}