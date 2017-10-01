#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>

#include <unistd.h>
#include <signal.h>
#include <poll.h>

#include <sys/signalfd.h>

#include "terminal.h"

int main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;

	int ret = 0;

	sigset_t ss;
	sigset_t oss;

	int fd = -1;

	if (sigfillset(&ss) != 0) {
		perror("sigfillset");
		goto fail;
	}

	if (sigprocmask(SIG_BLOCK, &ss, &oss) < 0) {
		perror("sigprocmask");
		goto fail;
	}

	fd = signalfd(-1, &ss, SFD_NONBLOCK);

	if (fd < 0) {
		perror("signalfd");
		goto fail;
	}

	struct signalfd_siginfo ssi;

	struct pollfd pfd[2];
	memset(pfd, 0, sizeof(pfd));
	pfd[0].fd = STDIN_FILENO;
	pfd[0].events = POLLIN;
	pfd[1].fd = fd;
	pfd[1].events = POLLIN;

	if (termios_stdin_char_mode(true) < 0) {
		perror("termios_stdin_char_mode");
		goto fail;
	}
	printf("Logging signals, press <q> to quit\n");
	printf("\n");

	while (1) {
		if (poll(pfd, sizeof(pfd) / sizeof(pfd[0]), -1) < 0) {
			perror("poll");
			goto fail;
		}
		if (pfd[0].revents & POLLIN) {
			char c;
			if (read(STDIN_FILENO, &c, sizeof(c)) != sizeof(c)) {
				perror("read");
				goto fail;
			}
			if (tolower(c) == 'q') {
				break;
			}
		}
		if (pfd[1].revents & POLLIN) {
			if (read(fd, &ssi, sizeof(ssi)) != sizeof(ssi)) {
				perror("read");
				goto fail;
			}
			printf("Signal: %s\n", strsignal(ssi.ssi_signo));
			printf("Signo: %d\n", ssi.ssi_signo);
			printf("Code: %" PRId32 "\n", ssi.ssi_code);
			printf("\n");
		}
	}

	goto done;
fail:
	ret = -1;
done:
	termios_reset();
	close(fd);
	return ret;
}
