#include "http/Request.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>

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

// Helper: create pipe with payload
static int makePipeWith(const std::string &payload) {
	int fds[2];
	if (pipe(fds) == -1) { perror("pipe"); return -1; }
	ssize_t w = write(fds[1], payload.c_str(), payload.size());
	(void)w;
	close(fds[1]);
	int flags = fcntl(fds[0], F_GETFL, 0);
	fcntl(fds[0], F_SETFL, flags | O_NONBLOCK);
	return fds[0];
}

// Test: simple GET request line only
static void test_request_line_only() {
	std::cout << "=== Test: Request line only ===\n";
	const std::string raw = "GET /index.html HTTP/1.1\r\n";
	int fd = makePipeWith(raw);
	if (fd == -1) { FAIL("pipe setup failed"); return; }

	http::Request req;
	bool ok = req.readFromFd(fd);
	close(fd);

	EXPECT_TRUE(ok, "readFromFd should succeed for request line");
	EXPECT_EQ_STR(req.getMethod(), "GET", "Method is GET");
	EXPECT_EQ_STR(req.getPath(), "/index.html", "Path is /index.html");
	EXPECT_EQ_STR(req.getProtocol(), "HTTP/1.1", "Protocol is HTTP/1.1");
	std::cout << "=== End ===\n\n";
}

// Test: GET request with headers, sent in chunks
static void test_request_with_headers() {
	std::cout << "=== Test: GET with headers (chunked) ===\n";
	const std::string part1 = "GET /home HTTP/1.1\r\nHost: loc";
	const std::string part2 = "alhost\r\nUser-Agent: TestClient\r\n\r\n";

	int pipefd[2];
	if (pipe(pipefd) == -1) { FAIL("pipe setup failed"); return; }
	int flags = fcntl(pipefd[0], F_GETFL, 0);
	fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);

	http::Request req;

	write(pipefd[1], part1.c_str(), part1.size());
	EXPECT_TRUE(!req.readFromFd(pipefd[0]), "Request incomplete after first chunk");

	write(pipefd[1], part2.c_str(), part2.size());
	EXPECT_TRUE(req.readFromFd(pipefd[0]), "Request complete after second chunk");

	EXPECT_EQ_STR(req.getMethod(), "GET", "Method is GET");
	EXPECT_EQ_STR(req.getPath(), "/home", "Path is /home");
	EXPECT_EQ_STR(req.getProtocol(), "HTTP/1.1", "Protocol is HTTP/1.1");
	EXPECT_EQ_STR(req.getHeader("Host"), "localhost", "Host header is localhost");
	EXPECT_EQ_STR(req.getHeader("User-Agent"), "TestClient", "User-Agent header is TestClient");
	EXPECT_EQ_STR(req.getHeader("Non-Existent"), "", "Non-existent header returns empty string");

	close(pipefd[0]);
	close(pipefd[1]);
	std::cout << "=== End ===\n\n";
}

int main() {
	test_request_line_only();
	test_request_with_headers();

	if (g_failures == 0) {
		std::cout << "All Request tests passed ✅\n";
		return 0;
	}
	std::cerr << g_failures << " test(s) FAILED ❌\n";
	return 1;
}

