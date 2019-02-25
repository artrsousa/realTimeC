#include <time.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

//#include "get_num.c"
#include "tlpi_hdr.h"
#include "error_functions.c"

int sigCnt = 0;
struct sigaction act;

static void alarmHandler();
static void displayTimes(const char *msg, Boolean includeTimer);

int main() {
	struct itimerval itv;
	clock_t prevClock;

	/* set the alarm routine */
	
	act.sa_handler = alarmHandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	
	if (sigaction(SIGALRM, &act, NULL) == -1) 
		printf("sigaction failed. %s\n", strerror(errno)); 
	
	/* set the time val and interval */
	
	itv.it_value.tv_sec     = 2;
	itv.it_value.tv_usec    = 0;
	itv.it_interval.tv_sec  = 2;
	itv.it_interval.tv_usec = 0;
	
	displayTimes("Starting: ", FALSE);
	if (setitimer(ITIMER_REAL, &itv, NULL) == -1)
		errExit("setitimer");
	
	prevClock = clock();
	
	for (;;) {
		
		/* inner loop consumes at least 0.5 seconds CPU time */
		displayTimes("Working...", TRUE); 
		fflush(stdout);
		
		while (((clock() - prevClock) * 10 / CLOCKS_PER_SEC) < 5) {;} 
		
		prevClock = clock();
		displayTimes("Pausing...", TRUE);
		pause();
	}
	
	return 0;	
}

static void displayTimes(const char *msg, Boolean includeTimer) {
    struct itimerval itv;
    static struct timeval start;
    struct timeval curr;
    static int callNum = 0;             /* Number of calls to this function */

    if (callNum == 0)                   /* Initialize elapsed time meter */
        if (gettimeofday(&start, NULL) == -1)
            errExit("gettimeofday");

    if (callNum % 50 == 0)              /* Print header every 20 lines */
        printf("       Elapsed   Value Interval\n");

    if (gettimeofday(&curr, NULL) == -1)
        errExit("gettimeofday");
    printf("%-7s %6.2f", msg, curr.tv_sec - start.tv_sec +
                        (curr.tv_usec - start.tv_usec) / 1000000.0);

    if (includeTimer) {
        if (getitimer(ITIMER_REAL, &itv) == -1)
            errExit("getitimer");
        printf("  %6.2f  %6.2f ",
                itv.it_value.tv_sec + itv.it_value.tv_usec / 1000000.0,
                itv.it_interval.tv_sec + itv.it_interval.tv_usec / 1000000.0);
    }

    printf("\n");
    fflush(stdout);
    callNum++;	
}

static void alarmHandler() {
	displayTimes("\nALARM: \t   ", TRUE);
}



