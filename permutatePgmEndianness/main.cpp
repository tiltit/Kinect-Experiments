#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>

#include "split.h"

using namespace std;

int main(int argc, char *argv[])
{
	if ( argc != 2 ) {
    	cout <<"usage: "<< argv[0] <<" <filename>\n";
  	} else {

  		struct stat results;
  		int size;
    
		if (stat(argv[1], &results) == 0) {
			size = results.st_size;
		} else {
			cerr << "Error reading file" << endl;
			return 1;
		}

		char buff[size];
		ifstream pgm(argv[1], ios::in | ios::binary);

		if(!pgm.read(buff, size)) {
			cerr << "Error reading file" << endl;
			pgm.close();
			return 2;
		}

		pgm.close();

		int i = 0;
		string header = "";

		do {
			header.append(1,buff[i]);
		} 
		while( (buff[++i] != '\n') || (i>=size-1) );
		

		vector<string> headerFields = split(header, ' ');

		if (headerFields.size() != 4) {
			cerr << "Wrong input file" << endl;
			return 3;
		}

		if (headerFields[0] != "P5") {
			cerr << "Wrong input file" << endl;
			return 3;
		}

		if (headerFields[3] != "65535") {
			cerr << "Wrong bit depth" << endl;
			return 3;
		}

		do {
			char tmp = buff[++i];
			buff[i] = buff[i+1];
			buff[++i] = tmp;
		} while ( i+1 < size );

		ofstream pgmOut (argv[1], ios::out | ios::binary);
		pgmOut.write (buff, size);
		pgmOut.close();

		cout << "Done converting: "  << argv[1] << endl;

  	}
	return 0;
}
