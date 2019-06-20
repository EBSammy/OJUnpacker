#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>

struct HeaderInfo
{
    char version;
    char flags;
    char granulePoisiton[8];
    char serialNumber[4];
    char sequenceNumber[4];
    char checksum[4];
    char totalSegments;
};


char oggHeader[7] = {0x4F, 0x67, 0x67, 0x53, 0x00, 0x02, '\0'};

using namespace std;

void handlePak(fstream&, string);
streampos findHeader(fstream&);
int main()
{
    fstream pak;
    string filename;
    cout << "Give a .pak file." << endl;
    cout << oggHeader << endl;
    cin >> filename;
    pak.open(filename, ios::binary|ios::in);
    handlePak(pak, filename);

    return 0;
}

void handlePak(fstream &pak, string name)
{
    //step 1. determine what we're looking at
    char buffer[7]= "";
    pak.read(buffer, 6);
    pak.seekg(pak.beg);
    if (memcmp(buffer, oggHeader, 6) == 0)
    {
        //we are working with an audio file
        int base = 1;
        streampos lastLocation1;
        streampos lastLocation2;
        while(!(pak.eof()))
        {
            char *stuff;
            ofstream song;
            ostringstream tempName;
            tempName << name << base << ".ogg";
            song.open(tempName.str(), ios::out|ios::binary);
            lastLocation1 = findHeader(pak);
            lastLocation2 = findHeader(pak);
            stuff = new char[lastLocation2-lastLocation1+1];
            cout << lastLocation2-lastLocation1+1;
            pak.seekg(lastLocation1);
            pak.read(stuff, lastLocation2-lastLocation1-1);
            song.put(0x4F);
            if (lastLocation1 == lastLocation2)
            {
                pak.close();
                pak.open(name, ios::binary|ios::in);
                pak.seekg(lastLocation1);

                char shet;
                while(!pak.eof())
                {
                    //cout << pak.tellg() << " ";
                    pak.get(shet);
                    song.put(shet);
                }
                delete[] stuff;
                song.close();
                break;
            }
            song.write(stuff, lastLocation2-lastLocation1+1);
            /*if (strcmp(stuff, ""))
            {
                delete[] stuff;
                break;
            }*/
            delete[] stuff;
            song.close();
            base++;
        }
    }
    //fread(buffer, 4, 1, pak);
}

streampos findHeader(fstream &pak)
{

    //current 4 bytes will be held here
    char buffer[7];
    while(pak.read(buffer, 6))
    {
        pak.seekg(-5, pak.cur);
        if (memcmp(buffer, oggHeader, 6) == 0)
        {
            if (pak.eof())
            {
                pak.seekg(0, ios_base::end);
            }
            return pak.tellg();
        }


    }
}
