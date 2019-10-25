#pragma once

class FileWriter {
private:
	std::ifstream loadFile;
	std::ofstream writeFile;
	std::string* iBuffer;
	std::string filename;

	void loadOldData(const std::string& path);
	void openFile(const std::string& path);
public:
	FileWriter(const std::string& path);
	FileWriter(const std::string& path, const std::string& filename);
	~FileWriter();

	void closeFile();
	void writeSingleLine(const double value);
	void writeSingleLine(const std::vector<double>& values);
	void writeMultipleLines(const std::vector<double>& values, const int lineBreak);
	void writeText(const std::string& data);
};