#include "pch.h"
#include "FileWriter.h"
#include <algorithm>

FileWriter::FileWriter(const std::string& path, const bool oldFile) {
	filename = "measurements.csv";
	iBuffer = new std::string();
	if (oldFile) {
		this->loadOldData(path);
	}
	this->openFile(path);
}

FileWriter::FileWriter(const std::string& path, const std::string& filename, const bool oldFile): filename(filename) {
	iBuffer = new std::string();
	if (oldFile) {
		this->loadOldData(path);
	}
	this->openFile(path);
}

FileWriter::~FileWriter() {
	this->closeFile();
	delete iBuffer;
}

void FileWriter::openFile(const std::string& path) {
	writeFile.open(path + filename);
	if (!writeFile.is_open()) {
		std::cout << "Couldn't open csv file\n";
		std::exit(1);
	}
	writeFile << *iBuffer;
}

void FileWriter::resizeRows(const int number) {
	content.resize(number);
}

void FileWriter::resizeColumns(const int number) {
	for (int i = 0; i < content.size(); i++) {
		content[i].resize(number);
	}
}

void FileWriter::closeFile() {
	this->writeContent();
	writeFile.close();
}

void FileWriter::loadOldData(const std::string& path) {
	std::string lineBuffer;
	loadFile.open(path + filename);
	if (!loadFile.is_open()) {
		std::cout << "Couldn't load csv file\n";
		std::exit(1);
	}
	while (std::getline(loadFile, lineBuffer)) {
		iBuffer->append(lineBuffer + "\n");
	}
	loadFile.close();
}

void FileWriter::writeSingleValueToColumn(const int row, const int column, const double value) {
	std::string buffer = std::to_string(value);
	std::replace(buffer.begin(), buffer.end(), '.', ',');
	content[row][column] = buffer;
}

void FileWriter::writeTextToColumn(const int row, const int column, const std::string& data) {
	content[row][column] = data;
}

void FileWriter::writeHeadline(const std::vector<std::string>& headline) {
	for (int i = 0; i < headline.size(); i++) {
		content[0][i] = headline[i];
	}
}

void FileWriter::writeContent() {
	for (auto row : content) {
		for (auto col : row) {
			writeFile << col + ";";
		}
		writeFile << "\n";
	}
}

bool FileWriter::is_not_digit(char c)
{
	return !std::isdigit(c);
}

bool FileWriter::row_compare(const std::vector<std::string>& row1, const std::vector<std::string>& row2) {
	// sorted by first column
	std::string column1 = row1[0], column2 = row2[0];

	std::string::iterator it1 = column1.begin(), it2 = column2.begin();

	if (std::isdigit(column1[0]) && std::isdigit(column2[0])) {
		int n1, n2;
		std::stringstream ss(column1);
		ss >> n1;
		ss.clear();
		ss.str(column2);
		ss >> n2;

		if (n1 != n2) return n1 < n2;

		it1 = std::find_if(column1.begin(), column1.end(), &FileWriter::is_not_digit);
		it2 = std::find_if(column2.begin(), column2.end(), &FileWriter::is_not_digit);
	}

	return std::lexicographical_compare(it1, column1.end(), it2, column2.end());
}

void FileWriter::sortByFirstColumn() {
	std::sort(content.begin(), content.end(), &FileWriter::row_compare);
}