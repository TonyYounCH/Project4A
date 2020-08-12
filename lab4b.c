/*
NAME: Changhui Youn
EMAIL: tonyyoun2@gmail.com
ID: 304207830
*/
#ifdef DUMMY
#define DUM 1;
#else
#define DUM 0;
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <ctype.h>
#include <mraa.h>
#include <mraa/aio.h>

#define PERIOD 'p'
#define SCALE 's'
#define LOG 'l'

int period = 1;
char scale = 'F';
int stop = 0;
time_t begin = 0;
time_t end = 0;
int log_flag = 0;
int logfd = 0;

mraa_aio_context temp;
mraa_gpio_context button;

void do_when_interrupted() {
	struct timespec ts;
	struct tm * tm;
	clock_gettime(CLOCK_REALTIME, &ts);
	tm = localtime(&(ts.tv_sec));
	fprintf(stdout, "%.2d:%.2d:%.2d SHUTDOWN\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
	exit(0);
}

void curr_temp_report(float temperature){
	struct timespec ts;
	struct tm * tm;
	clock_gettime(CLOCK_REALTIME, &ts);
	tm = localtime(&(ts.tv_sec));
	fprintf(stdout, "%.2d:%.2d:%.2d %.1f\n", tm->tm_hour, tm->tm_min, tm->tm_sec, temperature);
}

void initialize_the_sensors() {
	temp = mraa_aio_init(1);
	if (temp == NULL && !DUM) {
		fprintf(stderr, "Failed to init aio\n");
		exit(1);
	}

	button = mraa_gpio_init(60);
	if (button == NULL) {
		fprintf(stderr, "Failed to init button\n");
		exit(1);
	}

	mraa_gpio_dir(button, MRAA_GPIO_IN);
	mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &do_when_interrupted, NULL);

}

float convert_temper_reading(int reading) {
	float R = 1023.0/((float) reading) - 1.0;
	float R0 = 100000.0;
	float B = 4275;
	R = R0*R;
	//C is the temperature in Celcious
	float C = 1.0/(log(R/R0)/B + 1/298.15) - 273.15;
	//F is the temperature in Fahrenheit
	float F = (C * 9)/5 + 32;
	if(scale == 'C')
		return C;
	else
		return F;
}

void report_temp() {
	// if it is time to report temperature && !stop
	// read from temperature sensor, convert and report
	time(&end);
	if(difftime(end, begin) >= period && !stop) {
		time(&begin);
		int reading = mraa_aio_read(temp);
		if(DUM)
			reading = 650;
		float temperature = convert_temper_reading(reading);
		curr_temp_report(temperature);
	}
}

void process_stdin(char *input) {
    if(strcmp(input, "OFF") == 0){
        do_when_interrupted();
    }
    else if(strcmp(input, "START") == 0){
    	stop = 0;
    }
    else if(strcmp(input, "STOP") == 0){
    	stop = 1;
    }
    else if(strcmp(input, "SCALE=F") == 0){
        scale = 'F';
    }
    else if(strcmp(input, "SCALE=C") == 0){
        scale = 'C';
    }
    else if(strncmp(input, "PERIOD=", sizeof(char)*7) == 0){
        period = (int)atoi(input+7);
    }
    else if((strncmp(input, "LOG", sizeof(char)*3) == 0)){
        if(log_flag){
        	dprintf(logfd, "%s\n", input);
        }
    }
    else {
        fprintf(stdout, "Command not recognized\n");
        exit(1);
    }
}


int main(int argc, char* argv[]) {
	int opt = 0;


	struct option options[] = {
		{"period", 1, NULL, PERIOD},
		{"scale", 1, NULL, SCALE},
		{"log", 1, NULL, LOG},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "", options, NULL)) != -1) {
		switch (opt) {
			case PERIOD: 
				// get checking period
				period = atoi(optarg);
				break;
			case SCALE:
				// get iteration #
				if (optarg[0] == 'F' || optarg[0] == 'C') {
					scale = optarg[0];
				} else {
					fprintf(stderr, "Invalid argument(s)\n--scale option only accepts [C, F]\n");
					exit(1);
				}
				break;
			case LOG:
				// log file
				log_flag = 1;
				char* filename = optarg;
				logfd = creat(filename, 0666);
				if(logfd < 0){
					fprintf(stderr, "Failed to open log file");
					exit(1);
				}
				break;
			default:
				fprintf(stderr, "Invalid argument(s)\nYou may use --period=#, --scale=[C,F], --log=filepath\n");
				exit(1);
				break;
		}
	}

	initialize_the_sensors();

	struct pollfd pollfd;
	pollfd.fd = 0;
	pollfd.events = POLLIN;

    char commandBuff[128];
    char copyBuff[128];
    memset(commandBuff, 0, 128);
    memset(copyBuff, 0, 128);
    int copyIndex = 0;
	while (1) {
		// if it is time to report temperature && !stop
		// read from temperature sensor, convert and report
		report_temp();

		 // use poll syscalls, no or very short< 50ms timeout interval
		int ret = poll(&pollfd, 1, 0);
		if(ret < 0){
			fprintf(stderr, "Failed to read from poll\n");
		}
		if(pollfd.revents && POLLIN){
			int num = read(STDIN_FILENO, commandBuff, 128);
			if(num < 0){
				fprintf(stderr, "Failed to read from STDIN_FILENO\n");
			}
			int i;
			for(i = 0; i < num && copyIndex < 128; i++){
				if(commandBuff[i] =='\n'){
					process_stdin((char*)&copyBuff);
					copyIndex = 0;
					memset(copyBuff, 0, 128); //clear
				}
				else {
					copyBuff[copyIndex] = commandBuff[i];
					copyIndex++;
				}
			}
			
		}

		// if (push button is pressed) 
		// 	log and exit. 
	}

	mraa_aio_close(temp);
	mraa_gpio_close(button);

	return 0;



}