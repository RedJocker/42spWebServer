#include "../src/http/Request.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdlib>

#define EXPECT_TRUE(cond, msg) \
	do { \
		if (!(cond)) std::cerr << "FAIL: " << msg << "\n"; \
		else std::cout << "PASS: " << msg << "\n"; \
	} while(0)

#define EXPECT_EQ_STR(actual, expected, msg) \
	do { \
		if ((actual) != (expected)) \
			std::cerr << "FAIL: " << msg << " | expected=[" << expected << "] got=[" << actual << "]\n"; \
		else \
			std::cout << "PASS: " << msg << " | value=[" << actual << "]\n"; \
	} while(0)

static int makePipeWith(const std::string &payload) {
	int fds[2];
	if (pipe(fds) == -1) {
		std::cerr << "Error creating pipe\n";
		exit(1);
	}
	write(fds[1], payload.c_str(), payload.size());
	close(fds[1]);
	return fds[0];
}

void test_basic_request() {
	std::cout << "=== Test: Basic GET Request ===\n";
	std::string raw = "GET / HTTP/1.1\r\n";
	int fd = makePipeWith(raw);

	Request req;
	bool ok = req.parseFromFd(fd);
	close(fd);

	EXPECT_TRUE(ok, "parseFromFd should succeed");
	EXPECT_EQ_STR(req.getMethod(), "GET", "Method must be GET");
	EXPECT_EQ_STR(req.getPath(), "/", "Path must be '/'");
	EXPECT_EQ_STR(req.getProtocol(), "HTTP/1.1", "Protocol must be HTTP/1.1");

	std::cout << "=== End of Test ===\n\n";
}

void test_invalid_request() {
	std::cout << "=== Test: Invalid Request Line ===\n";
	std::string raw = "GARBAGE\r\n";
	int fd = makePipeWith(raw);

	Request req;
	bool ok = req.parseFromFd(fd);
	close(fd);

	EXPECT_TRUE(!ok, "parseFromFd should fail on invalid input");

	std::cout << "=== End of Test ===\n\n";
}

int main() {
	test_basic_request();
	test_invalid_request();
	std::cout << "All HTTP Request tests finished.\n";
	return 0;
}
