#include "pch.h"
#include "FileWriter.h"

FileWriter::FileWriter(const std::string& path) {
	filename = "measurements.csv";
	iBuffer = new std::string();
	this->loadOldData(path);
	this->openFile(path);
}

FileWriter::FileWriter(const std::string& path, const std::string& filename): filename(filename) {
	iBuffer = new std::string();
	this->loadOldData(path);
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

void FileWriter::closeFile() {
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

void FileWriter::writeSingleLine(const double value) {
	writeFile << std::to_string(value) + "\n";
}

void FileWriter::writeSingleLine(const std::vector<double>& values) {
	for (double elem : values) {
		writeFile << std::to_string(elem) + ";";
	}
	writeFile << "\n";
}

// lineBreak -> after which number of values a line break should occur
void FileWriter::writeMultipleLines(const std::vector<double>& values, const int lineBreak) {
	for (int i = 0; i < values.size(); i++) {
		if (i % (lineBreak - 1) == 0) {
			writeFile << "\n";
		}
		writeFile << std::to_string(values[i]) + ";";
	}
	writeFile << "\n";
}

void FileWriter::writeText(const std::string& data) {
	writeFile << data;
}
