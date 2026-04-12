
#include <iostream>
#include <string>
#include <stdio.h>

class IOBase {
public:
	virtual bool is_open() const = 0;
	virtual ~IOBase() = default;
};

class Reader: virtual public IOBase {
public:
	bool is_eof_reached = false;
	virtual const char* read(int count) = 0;
};

class Writer: virtual public IOBase {
public:
	virtual void write(char* data, int count) = 0;
};

class ReaderWriter: public Reader, public Writer {};

class StringReaderWriter: public ReaderWriter {
public:
	std::string source;

	StringReaderWriter(std::string src) : source(src) {
		if (src.size() == 0) {
			is_eof_reached = false;
		}
	}

	StringReaderWriter(): source("") {}

	bool is_open() const override {
		return true;
	}

	const char* read(int count) override {
		const char* res = source.substr(0, count).c_str();

		source = source.substr(count);
		if (source.size() == 0) {
			is_eof_reached = true;
		}

		return res;
	}

	void write(char* data, int count) override {
		if (is_eof_reached && count > 0)
			is_eof_reached = false;

		for (int i = 0; i < count; i++) {
			source += data[i];
		}
	}

	~StringReaderWriter() = default;
};

class FILEReaderWriter: public ReaderWriter {
private:
	long read_pos;
public:
	FILE *fstream;

	bool is_open() const override {
		return ftell(fstream) >= 0;
	}

	FILEReaderWriter(FILE* file): fstream(file), read_pos(ftell(file)) {
		is_eof_reached = static_cast<bool>(feof(fstream));
	}

	FILEReaderWriter(const char* fname): fstream(fopen(fname, "a+")), read_pos(0) {
		is_eof_reached = static_cast<bool>(feof(fstream));
	}

	const char* read(int count) override {
		char *res = new char[count + 1];
		fseek(fstream, read_pos, SEEK_SET);
		fread(res, sizeof(char), count, fstream);

		is_eof_reached = static_cast<bool>(feof(fstream));
		read_pos = ftell(fstream);

		return res;
	}

	void write(char* data, int count) override {
		fseek(fstream, 0, SEEK_END);
		fwrite(data, sizeof(char), count, fstream);

		is_eof_reached = true;
	}

	~FILEReaderWriter() {
		fclose(fstream);
	};
};


int main() {
	// StringReaderWriter rw_str("");

	// const char* data = "hello, world";
	// rw_str.write(data, 5);

	// std::cout << rw_str.source << '\n';

	FILEReaderWriter rw_file("test.txt");

	rw_file.write("Hello, World!", 13);
	const char *res = rw_file.read(5);

	for (int i = 0; i < 5; ++i) {
		std::cout << res[i];
	}
	std::cout << '\n';

	delete[] res;
	// std::cout << std::string(rw_file.read(5)) << '\n';

	return 0;
}
