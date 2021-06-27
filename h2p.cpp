// H2P (HTML 2 PDF)
// Developed by Jay (jay14nath@gmail.com)
// compile: g++ -o h2p.exe h2p.cpp
// run: h2p

#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;

string tempDirectoryName = "h2p_tempDir";

int getValueFromFile(string fileNameWithPath) {
	ifstream fileptr(fileNameWithPath);
	if (fileptr.is_open()) {
		int value;
		while (fileptr >> value) {}
		fileptr.close();
		return value;	
	}
	else {
		cout << "Could not read file " << fileNameWithPath << endl;
		return -1;
	}
}

vector<string> readFileLines(string fileNameWithPath) {
	vector<string> fileLines;
	string str;
	ifstream fileptr(fileNameWithPath);
	if (fileptr.is_open()) {
		while (getline(fileptr, str)) {
			fileLines.push_back(str);
		}
		fileptr.close();
		return fileLines;	
	}
	else {
		cout << "Could not read file " << fileNameWithPath << endl;
		return fileLines;
	}
}

int calculateHeightOfPage(string htmlPageName) {
	string genretedFileName = tempDirectoryName + "\\" + "height.txt";
	string cmd = "python geth.py \"" + htmlPageName + "\" > " + genretedFileName;
	try{
		system(cmd.c_str());
	} catch (...) {
		cout << endl << "Error executing the python script" << endl;
		return 0;
	}
	return 1;
}

void listAllHtmlFileNames(string programName) {
	string cmd = programName;
	system(cmd.c_str());
}

string getFirstToken(string str, char delimiter) {
	string token = "";
	for (auto ch : str) {
        if (ch == delimiter) {
            break;
        }
        token += ch;
    }
    return token;
}

void housekeeping() {
	string cmd;
	cout << "Deleting temporary directory and files ... ";
	try{
		cmd = "rmdir /q/s h2p_tempDir";
		system(cmd.c_str());
		cmd = "del ghostdriver.log";
		system(cmd.c_str());
	} catch (...) {
		cout << endl << "Error deleting temporary directory and files" << endl;
	}
	cout << endl;
}

int createHiddenDirectory (string dirName) {
	string cmd;
	cout << "Creating temporary directory ... ";
	try{
		cmd = "md \"" + dirName + "\"";
		system(cmd.c_str());
		// cmd for make the dir hidden
		cmd = "attrib +h \"" + dirName + "\"";
		system(cmd.c_str());
	} catch (...) {
		cout << endl << "Error Creating temporary directory and files" << endl;
		return 0;
	}
	return 1;
}

int main() {
	cout << "H2P v1.0" << endl;
	cout << "[HTML2PDF]" << endl;
	cout << "This is simple CPP program to convert HTML to PDF (one single long pdf page)" << endl;
	cout << "Developed by: Jay" << endl;
	cout << "Contact: jay14nath@gmail.com" << endl << endl;

	cout << "Enter the variance (suggested 20, bigger is better but slower): ";
	int variance;
	cin >> variance;
	cout << "Here the variance is set to " << variance << "\nIf the program failed to generate single page pdf, then increase the variance." << endl;

	// all temp files will be stored here which will be deleted later
	if (createHiddenDirectory(tempDirectoryName)) { // parameter is the temp dir name
		cout << "Completed" << endl;
	}
	else {
		cout << endl << "Error creating temp dir" << endl;
		cout << endl << "Program is terminating...\nHousekeeping..." << endl;
		housekeeping();
		return 0;
	}

	// Prepare a list of all html file names
	listAllHtmlFileNames("listHTML_files.exe");

	// Move the file.txt to the temp dir
	try{
		string cmd = "move files.txt \"" + tempDirectoryName + "\"";
		system(cmd.c_str());
	} catch (...) {
		cout << endl << "Error moving files.txt" << endl;
		cout << endl << "Program is terminating...\nHousekeeping..." << endl;
		housekeeping();
		return 0;
	}

	vector<string> htmlFileNames;
	string fullPathOf_filesDotTxt = tempDirectoryName + "\\" + "files.txt";
	// get all html file names in the present directory
	if (readFileLines(fullPathOf_filesDotTxt).empty()) {
		cout << endl << "Error reading the html files!" << endl;
		cout << endl << "Program is terminating...\nHousekeeping..." << endl;
		housekeeping();
		return 0;
	}
	else {
		htmlFileNames = readFileLines(fullPathOf_filesDotTxt);
		// Display all HTML file names
		cout << endl << "HTML files to work on:" << endl;
		for (int i=0; i<htmlFileNames.size(); i++) {
			cout << i+1 << ". " << htmlFileNames[i] << endl;
		}
	}

	int total_successfull_conversion = 0;
	// convert all html files into pdfs
	for (int i=0; i<htmlFileNames.size(); i++) {

		if(calculateHeightOfPage(htmlFileNames[i])) {
			cout << "Successfully calculated the height of HTML page " << i+1 << ". " << htmlFileNames[i] << endl;
		}
		else {
			cout << endl << "Error calculating the height of of HTML page " << i+1 << ". " << htmlFileNames[i] << endl;
			continue; // move on to next html page
		}

		string fullPathOf_heightDotTxt = tempDirectoryName + "\\" + "height.txt";

		if (getValueFromFile(fullPathOf_heightDotTxt) == -1) {
			cout << endl << "Error reading the height!" << endl;
			continue; // move on to next html page
		}
		else {
			int height = int((double)getValueFromFile(fullPathOf_heightDotTxt) * 0.2645833333); // convert to mm
			vector<int> heights;
			// here variance is added because sometimes the correct height is range between (height - 168 - variance)
			// to height instead of  (height - 168) to height
			int startingHeight = height - 168 - variance; // 168 is the htmlClientHeight

			for (int j=startingHeight; j <= height; j++) {
				heights.push_back(j);
			}

			sort(heights.begin(), heights.end());

			// print all the pdfs
			string wkhtmltopdf_cmdStr = "";
			string wkhtmltopdf_cmdStrPrefix = "wkhtmltopdf -L 0mm -R 0mm -T 0mm -B 0mm --page-width 208mm --page-height ";
			string wkhtmltopdf_cmdStrSuffix1 = "mm \"" + htmlFileNames[i] + "\" ";

			vector<string> pdfToBeDeleted;
			string pdfToBeRenamed; // upon succesion
			bool flag_successfull_conversion = false;

			// find the pdf with single page
			for (int j=0; j<heights.size(); j++) {
				string tempPdfName = to_string(heights[j]) + ".pdf";
				string tempPdfNameFullPath = tempDirectoryName + "\\" + tempPdfName;
				wkhtmltopdf_cmdStr = "";
				wkhtmltopdf_cmdStr += wkhtmltopdf_cmdStrPrefix;
				wkhtmltopdf_cmdStr += to_string(heights[j]);
				wkhtmltopdf_cmdStr += wkhtmltopdf_cmdStrSuffix1;
				wkhtmltopdf_cmdStr += tempPdfNameFullPath;

				cout << "\nprint pdf cmd: " << wkhtmltopdf_cmdStr << endl;
				try {
					system(wkhtmltopdf_cmdStr.c_str());
				} catch (...) {
					cout << endl << "Error executing wkhtmltopdf" << endl;
					continue; // move on to create next pdf
				}

				cout << "Sample PDF is generated" << endl;
				cout << "Single page detection begins..." << endl;

				string fullPathOf_pagecountDotTxt = tempDirectoryName + "\\" + "pagecount.txt";
				string pagecount_cmd = "cpdf.exe -pages " + tempPdfNameFullPath + " > " + fullPathOf_pagecountDotTxt;
				try {
					system(pagecount_cmd.c_str());
				} catch (...) {
					cout << endl << "Error executing cpdf.exe" << endl;
					continue; // move on to create next pdf
				}

				flag_successfull_conversion = false;

				if (getValueFromFile(fullPathOf_pagecountDotTxt) == -1) {
					cout << "Error reading the pagecount!" << endl;
					continue; // move on to create next pdf
				}
				else {
					int pagecount = getValueFromFile(fullPathOf_pagecountDotTxt); 
					if (pagecount == 1) {
						// found the single page pdf
						pdfToBeRenamed = tempPdfName; // dummy name
						total_successfull_conversion++;
						flag_successfull_conversion = true;
						break;
					}
					else {
						// put the current pdf name in the to be deleted list
						pdfToBeDeleted.push_back(tempPdfName);
					}

				}

			} // end of for of pdf creation	

			cout << endl << "Deleting all redundant PDFs" << endl;
			// if there any
			if (!pdfToBeDeleted.empty()) {
				// delete previously created all pdfs
				string pdfToBeDeletedFullPath;
				for (int k=0; k<pdfToBeDeleted.size(); k++) {
					pdfToBeDeletedFullPath = tempDirectoryName + "\\" + pdfToBeDeleted[k];
					string delete_cmd = "del " + pdfToBeDeletedFullPath;
					try {
						system(delete_cmd.c_str());
					} catch (...) {
						cout << endl << "Error deleting pdfs" << endl;
						continue; // move on to next pdf
					}
				}
			}
			
			// rename pdf to original name
			if (flag_successfull_conversion) {
				cout << endl << "Renaming and moving the PDF...";
				string pdfToBeRenamedFullPath = "\"" + tempDirectoryName + "\\" + pdfToBeRenamed + "\"";
				string renameTo = "\"" + getFirstToken(htmlFileNames[i], '.') + ".pdf\"";
				string rename_cmd = "rename " + pdfToBeRenamedFullPath + " " + renameTo;
				try {
					system(rename_cmd.c_str());
				} catch (...) {
						cout << endl << "Error renaming the pdf" << endl;
						continue; // move on to next html page
				}
				// move the renamed pdf to the pwd
				try {
					string resultantPdfName = getFirstToken(htmlFileNames[i], '.') + ".pdf";
					string pdfToBeMovedFullPath = "\"" + tempDirectoryName + "\\" + resultantPdfName + "\"";
					string move_cmd = "move " + pdfToBeMovedFullPath + " \"" + resultantPdfName + "\"";
					system(move_cmd.c_str());
				} catch (...) {
						cout << endl << "Error moving the pdf" << endl;
						continue; // move on to next html page
				}
			}
			else {
				cout << endl << "Could not find single page pdf!" << endl;
			}

		} // end of else
	} // end of for (i); Move on to next html page

	// delete the temp directory
	cout << endl << "Housekeeping..." << endl;
	housekeeping();

	cout << endl << "Successfully converted " << total_successfull_conversion << " html files" << endl;

	return 0;
}
