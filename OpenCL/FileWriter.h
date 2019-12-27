#pragma once

class FileWriter {
private:
	std::ifstream loadFile;
	std::ofstream writeFile;
	std::string* iBuffer;
	std::vector<std::vector<std::string>> content;
	std::string filename;

	void loadOldData(const std::string& path);
	void openFile(const std::string& path);
	void writeContent();
public:
	FileWriter(const std::string& path, const bool oldFile);
	FileWriter(const std::string& path, const std::string& filename, const bool oldFile);
	~FileWriter();

	void resizeRows(const int number);
	void resizeColumns(const int number);
	void closeFile();
	void writeSingleValueToColumn(const int row, const int column, const double value);
	void writeTextToColumn(const int row, const int column, const std::string& data);
	void writeHeadline(const std::vector<std::string>& headline);
};