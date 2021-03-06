/* Modified demo code from Dr. Murphy's 615 website */
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

void get_current_time(int);
void timeout();

/* Global variable to set the timeout. */
int done;

int main() {
    int i;
    int n;
    struct itimerval value;
    struct itimerval ovalue;

    /* Executes every second for 5 minutes. */
    for(i = 1; i <= 300; i++) {
        if((signal(SIGALRM, timeout)) == SIG_ERR) {
            printf("Error in setting signal handler.\n");
            exit(0);
        }

        timerclear(&value.it_interval);
        timerclear(&value.it_value);
        timerclear(&ovalue.it_interval);
        timerclear(&ovalue.it_value);

        n = setitimer(ITIMER_REAL, &value, &ovalue);

        if(n < 0 ) {
            printf("Error in clearing timer\n");
            exit(0);
        }

        /* Sets the alarm to wake up in one second. */
        value.it_value.tv_sec = 1;

        n = setitimer(ITIMER_REAL, &value, &ovalue);

        if(n < 0 ) {
            printf("Error in clearing timer\n");
            exit(0);
        }

        get_current_time(i);

        done = 0;

        while(!done);
    }

    return 0;
}

void get_current_time(int num) {
    char clock[32];
    char* date;
    char* day;
    char* day_num;
    char* month;
    char* ms;
    char* year;
    int clock_fd;
    int current_time;
    int hours;
    int microseconds;
    int minutes;
    int seconds;
    struct timeval time;

    clock_fd = open("/proc/myclock", O_RDONLY);

    gettimeofday(&time, NULL);

    if(clock_fd < 0) {
        perror("Bad read file open");
        exit(-1);
    }

    memset(clock, 0, sizeof(clock));
    read(clock_fd, clock, sizeof(clock));

    date = ctime(&time.tv_sec);

    day = strtok(date, " ");
    month = strtok(NULL, " ");
    day_num = strtok(NULL, " ");
    strtok(NULL, " ");
    year = strtok(NULL, " ");

    ms = strtok(clock, " ");
    ms = strtok(NULL, " ");

    current_time = atoi(clock);

    /* Changes the current hour to PST. */
    hours = (current_time-25200)/3600%24;
    minutes = current_time/60%60;
    seconds = current_time%60;
    microseconds = atoi(ms);

    printf("%s %s %s %d:%02d:%02d:%d %s", day, month, day_num, hours, minutes, seconds, microseconds, year);

    /* Prints the current time to check the time drift. */
    if(num == 300) {
        printf("%s", ctime(&time.tv_sec));
    }

    close(clock_fd);
}

/* Timeout for signal processes. */
void timeout(int arg) {
    done = 1;
}
