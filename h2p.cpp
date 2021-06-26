// H2P (HTML to PDF)
// Developed by Jay (jay14nath@gmail.com)
// compile: g++ -o h2p.exe h2p.cpp
// run: h2p

#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;

int getValueFromFile(string fileName) {
	ifstream fileptr(fileName);
	if (fileptr.is_open()) {
		int value;
		while (fileptr >> value) {}
		fileptr.close();
		return value;	
	}
	else {
		cout << "Could not read file " << fileName << endl;
		return -1;
	}
}

vector<string> readFileLines(string filename) {
	vector<string> fileLines;
	string str;
	ifstream fileptr(filename);
	if (fileptr.is_open()) {
		while (fileptr >> str) {
			fileLines.push_back(str);
		}
		fileptr.close();
		return fileLines;	
	}
	else {
		cout << "Could not read file " << filename << endl;
		return fileLines;
	}
}

void calculateHeightOfPage(string htmlPageName) {
	string cmd = "python geth.py " + htmlPageName + " > height.txt";
	system(cmd.c_str());
}

void listAllHtmlFileNames(string programName) {
	string cmd = programName;
	system(cmd.c_str());
}

string getFirstToken(string str) {
	string token = "";
	for (auto ch : str) {
        if (ch == '.') {
            break;
        }
        token += ch;
    }
    return token;
}

void housekeeping() {
	string delete_cmd;
	cout << "Deleting files.txt...";
	try{
		delete_cmd = "del files.txt";
		system(delete_cmd.c_str());
	} catch (...) {
		cout << "Error" << endl;
	}
	cout << endl;

	cout << "Deleting ghostdriver.log...";
	try{
		delete_cmd = "del ghostdriver.log";
		system(delete_cmd.c_str());
	} catch (...) {
		cout << "Error" << endl;
	}
	cout << endl;

	cout << "Deleting height.txt...";
	try{
		delete_cmd = "del height.txt";
		system(delete_cmd.c_str());
	} catch (...) {
		cout << "Error" << endl;
	}
	cout << endl;
	

	cout << "Deleting pagecount.txt...";
	try{
		delete_cmd = "del pagecount.txt";
		system(delete_cmd.c_str());
	} catch (...) {
		cout << "Error" << endl;
	}
	cout << endl;
}

int main() {
	cout << "H2P v1.0" << endl;
	cout << "This is simple CPP program to convert HTML to PDF (one single long page)" << endl;
	cout << "Devleoped by: Jay" << endl;
	cout << "Contact: jay14nath@gmail.com" << endl << endl;

	int variance = 20;
	cout << "Here the variance is set to 20\nIf the program failed to generate single page pdf, then increase the variance." << endl;

	// Prepare a list of all html file names
	listAllHtmlFileNames("listHTML_files.exe");

	vector<string> htmlFileNames;
	// get all html file names in the present directory
	if (readFileLines("files.txt").empty()) {
		cout << "Error reading the html files!" << endl;
		return 0;
	}
	else {
		htmlFileNames = readFileLines("files.txt");
		// del this loop on production
		for (int i=0; i<htmlFileNames.size(); i++) {
			cout << htmlFileNames[i] << endl;
		}
	}

	int total_successfull_conversion = 0;
	// convert all html files into pdfs
	for (int i=0; i<htmlFileNames.size(); i++) {

		calculateHeightOfPage(htmlFileNames[i]);

		if (getValueFromFile("height.txt") == -1) {
			cout << "Error reading the height!" << endl;
			return 0;
		}
		else {
			int height = int((double)getValueFromFile("height.txt") * 0.2645833333); // convert to mm
			int testHeight = height - variance;
			vector<int> heights;
			for (int j=height; j>=height-variance; j--) {
				heights.push_back(j - 168); // 168 is the htmlClientHeight
			}

			sort(heights.begin(), heights.end());

			// print all the pdfs
			string wkhtmltopdf_cmdStr = "";
			string wkhtmltopdf_cmdStrPrefix = "wkhtmltopdf -L 0mm -R 0mm -T 0mm -B 0mm --page-width 208mm --page-height ";
			string wkhtmltopdf_cmdStrSuffix1 = "mm " + htmlFileNames[i] + " ";
			string wkhtmltopdf_cmdStrSuffix2 = ".pdf";

			vector<string> pdfToBeDeleted;
			string pdfToBeRenamed; // upon succesion
			bool flag_successfull_conversion = false;

			// find the pdf with single page
			for (int j=0; j<heights.size(); j++) {
				wkhtmltopdf_cmdStr = "";
				wkhtmltopdf_cmdStr += wkhtmltopdf_cmdStrPrefix;
				wkhtmltopdf_cmdStr += to_string(heights[j]);
				wkhtmltopdf_cmdStr += wkhtmltopdf_cmdStrSuffix1;
				wkhtmltopdf_cmdStr += to_string(heights[j]);
				wkhtmltopdf_cmdStr += wkhtmltopdf_cmdStrSuffix2;
				system(wkhtmltopdf_cmdStr.c_str());

				cout << "Sample PDF is generated" << endl;
				cout << "Single page detection begins..." << endl;

				string pagecount_cmd = "cpdf.exe -pages " + to_string(heights[j]) + ".pdf > pagecount.txt";
				system(pagecount_cmd.c_str());

				flag_successfull_conversion = false;

				if (getValueFromFile("pagecount.txt") == -1) {
					cout << "Error reading the pagecount!" << endl;
					return 0;
				}
				else {
					int pagecount = getValueFromFile("pagecount.txt"); 
					if (pagecount == 1) {
						// found the single page pdf
						pdfToBeRenamed = to_string(heights[j]) + ".pdf"; // dummy name
						total_successfull_conversion++;
						flag_successfull_conversion = true;
						break;
					}
					else {
						// put the current pdf name in the to be deleted list
						pdfToBeDeleted.push_back(to_string(heights[j]) + ".pdf");
					}

				}

			} // end of for of pdf creation	

			cout << endl << "Deleting all redundant PDFs" << endl;
			// if there any
			if (!pdfToBeDeleted.empty()) {
				// delete previously created all pdfs
				for (int k=0; k<pdfToBeDeleted.size(); k++) {
					string delete_cmd = "del " + pdfToBeDeleted[k];
					system(delete_cmd.c_str());
				}
			}
			
			// rename pdf to original name
			cout << endl << "Renaming PDF...";

			string renameTo = getFirstToken(htmlFileNames[i]) + ".pdf";
			string rename_cmd = "rename " + pdfToBeRenamed + " " + renameTo;
			system(rename_cmd.c_str());
			cout << "Completed" << endl;

			cout << endl << "Housekeeping..." << endl;
			housekeeping();

		} // end of else
	} // end of for (i)

	cout << endl << "Successfully converted " << total_successfull_conversion << " html files" << endl;

	return 0;
}