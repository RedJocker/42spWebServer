#include "http/Request.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

static int g_failures = 0;

#define PASS(msg) std::cout << "PASS: " << msg << "\n"
#define FAIL(msg) do { std::cerr << "FAIL: " << msg << "\n"; ++g_failures; } while(0)

#define EXPECT_TRUE(cond, msg) \
	do { if (cond) PASS(msg); else FAIL(msg); } while(0)

#define EXPECT_EQ_STR(actual, expected, msg) \
	do { \
		if ((actual) == (expected)) { \
			std::cout << "PASS: " << msg << " | value=[" << (actual) << "]\n"; \
		} else { \
			std::cerr << "FAIL: " << msg << " | expected=[" << (expected) \
					  << "] got=[" << (actual) << "]\n"; \
			++g_failures; \
		} \
	} while(0)

static int makePipeWith(const std::string &payload) {
	int fds[2];
	if (pipe(fds) == -1) {
		std::cerr << "ERROR: pipe() failed\n";
		return -1;
	}
	ssize_t w = write(fds[1], payload.c_str(), payload.size());
	(void)w;
	close(fds[1]);
	return fds[0];
}

// GET request tests
static void test_basic_request_crlf() {
	std::cout << "=== Test: Basic GET (CRLF) ===\n";
	const std::string raw = "GET / HTTP/1.1\r\n";
	int fd = makePipeWith(raw);
	if (fd == -1) { FAIL("pipe setup failed"); return; }

	http::Request req;
	bool ok = req.readFromFd(fd);
	close(fd);

	EXPECT_TRUE(ok, "readFromFd should succeed for CRLF line");
	EXPECT_EQ_STR(req.getMethod(), "GET", "Method is GET");
	EXPECT_EQ_STR(req.getPath(), "/", "Path is '/'");
	EXPECT_EQ_STR(req.getProtocol(), "HTTP/1.1", "Protocol is HTTP/1.1");
	std::cout << "=== End ===\n\n";
}

static void test_basic_request_lf() {
	std::cout << "=== Test: Basic GET (LF-only) ===\n";
	const std::string raw = "GET / HTTP/1.1\n";
	int fd = makePipeWith(raw);
	if (fd == -1) { FAIL("pipe setup failed"); return; }

	http::Request req;
	bool ok = req.readFromFd(fd);
	close(fd);

	EXPECT_TRUE(ok, "readFromFd should succeed for LF-only line");
	EXPECT_EQ_STR(req.getMethod(), "GET", "Method is GET");
	EXPECT_EQ_STR(req.getPath(), "/", "Path is '/'");
	EXPECT_EQ_STR(req.getProtocol(), "HTTP/1.1", "Protocol is HTTP/1.1");
	std::cout << "=== End ===\n\n";
}

static void test_invalid_request_line() {
	std::cout << "=== Test: Invalid request line ===\n";
	const std::string raw = "INVALID\r\n";
	int fd = makePipeWith(raw);
	if (fd == -1) { FAIL("pipe setup failed"); return; }

	http::Request req;
	bool ok = req.readFromFd(fd);
	close(fd);

	EXPECT_TRUE(!ok, "readFromFd should fail for malformed line");
	std::cout << "=== End ===\n\n";
}

// POST request tests
static void test_post_request_with_body() {
	std::cout << "=== Test: POST with headers and body ===\n";
	const std::string raw =
		"POST /submit HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Length: 11\r\n"
		"\r\n"
		"hello world";

	int fd = makePipeWith(raw);
	if (fd == -1) { FAIL("pipe setup failed"); return; }

	http::Request req;
	bool ok = req.readFromFd(fd);
	close(fd);

	EXPECT_TRUE(ok, "readFromFd should succeed for POST request");
	EXPECT_EQ_STR(req.getMethod(), "POST", "Method is POST");
	EXPECT_EQ_STR(req.getPath(), "/submit", "Path is /submit");
	EXPECT_EQ_STR(req.getProtocol(), "HTTP/1.1", "Protocol is HTTP/1.1");
	EXPECT_EQ_STR(req.getHeader("Host"), "localhost", "Host header is localhost");
	EXPECT_EQ_STR(req.getBody(), "hello world", "Body is 'hello world'");
	std::cout << "=== End ===\n\n";
}

int main() {
	test_basic_request_crlf();
	test_basic_request_lf();
	test_invalid_request_line();
	test_post_request_with_body();

	if (g_failures == 0) {
		std::cout << "All Request tests passed.\n";
		return 0;
	}
	std::cerr << g_failures << " test(s) FAILED.\n";
	return 1;
}

