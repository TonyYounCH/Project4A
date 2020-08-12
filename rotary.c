#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <mraa.h>

sig_atomic_t volatile run_flag = 1;

void do_when_interrupted(int sig) {
	if (sig == SIGINT)
		run_flag = 0;
}

int main() {
	uint16_t value;
	mraa_aio_context rotary;
	rotary = mraa_aio_init(1);
	printf("%d\n", rotary);

	signal(SIGINT, do_when_interrupted);

	while (run_flag) {
		value = mraa_aio_read(rotary);
		printf("%d\n", value);
		usleep(100000);
	}

	mraa_aio_close(rotary);
	return 0;
}
