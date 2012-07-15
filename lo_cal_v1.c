/*
 * Loginov Oleg (loginoleg@gmil.com)
 *
 * COMPILE
 * OSX compile: 	gcc ./lo_cal.c -o lo_cal -framework CoreFoundation [-DUSECOLORS]
 * Linix compile: 	gcc ./lo_cal.c -o lo_cal [-DUSECOLORS]
 * add -DUSECOLORS for color output
 *
 * RUN
 * "./lo_cal [month.year]" for 
 * "./lo_cal" for current month
 *
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#ifdef __MACH__
	#include <CoreFoundation/CoreFoundation.h>
#endif


#ifdef USECOLORS
	#define RED "\33[22;31m"
	#define NC "\33[0m"
#else
    #define RED ""
    #define NC ""
#endif

#define LEFT_DAYSELECT 	'['
#define RIGHT_DAYSELECT ']'

#define ERR_USAGE 				"for usage lo_cal month.year (example: lo_cal 02.2014 for feb 2014)"
#define ERR_INCORRECT_MONTH 	"incorrect format of the month"
#define ERR_INCORRECT_YEAR 		"incorrect format of the year"

#define CELL_SIZE 4 
#define DAYS_A_WEEK 7
#define BUF_SIZE 200


void error_exit(char *);
char *wkday(const int);
int get_first_day_of_week(void);
int print_header(int);
int print_week(char *, int);
int calendar(int, int, char *, int, int, int);
int get_first_day_of_year(int, int);
int cur_date(int, int);
int cur_year(void);
int cur_month(void);


char month_days[] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
char result[600];

int 
main(int argc, char *argv[])
{
	char *in_month_str, *in_year_str;
	int in_month, in_year, date;
	size_t y, i, j, first_day_of_week;

	if (argc == 2) { 
		/* check args */
		if (!(in_month_str = strtok(argv[1], ".")))
		{
			error_exit(ERR_USAGE);
		}
		
		if (!(in_year_str = strtok(NULL, ".")))
		{
			error_exit(ERR_USAGE);
		}

		in_month = atoi(in_month_str);
		in_year = atoi(in_year_str);

		if (!((in_month >= 1) && (in_month <= 12)))
		{
			error_exit(ERR_INCORRECT_MONTH);
		}

		if (!((in_year >= 1) && (in_year < 9999)))
		{
			error_exit(ERR_INCORRECT_YEAR);
		}
	}
	else if (argc == 1)
	{
		/* without args, print current month calendar */
		in_year = cur_year();
		in_month = cur_month();
	}
	else 
	{
		/* wrong number of args */
		error_exit(ERR_USAGE);
	}

	setlocale(LC_TIME, "");
	first_day_of_week = get_first_day_of_week();

	if (date = cur_date(in_month, in_year))
	{
		calendar(in_month, in_year, result, CELL_SIZE * DAYS_A_WEEK, date, first_day_of_week);
	}
	else
	{
		calendar(in_month, in_year, result, CELL_SIZE * DAYS_A_WEEK, 0, first_day_of_week);
	}

	// print header
	print_header(first_day_of_week);

	// print weeks
	for(i=0; i < 6 * DAYS_A_WEEK * CELL_SIZE; i = i + DAYS_A_WEEK * CELL_SIZE)
	{
		print_week(result + i, CELL_SIZE * DAYS_A_WEEK);
	}

	exit(EXIT_SUCCESS);
}

int 
get_first_day_of_week(void)
{
	// for OSX
	#ifdef __MACH__
		CFIndex first_day_of_week;
		CFCalendarRef currentCalendar = CFCalendarCopyCurrent();
		first_day_of_week = CFCalendarGetFirstWeekday(currentCalendar);
		CFRelease(currentCalendar);
		if (first_day_of_week && first_day_of_week >= 1 && first_day_of_week <= 7)
		{
			return first_day_of_week;
		}
	#endif

	// for linux
	#ifdef __linux__
	    const char *const first_day_of_week = nl_langinfo(_NL_TIME_FIRST_WORKDAY);
	    if (first_day_of_week && *first_day_of_week >= 1 && *first_day_of_week <= 7)
	    {
	        return (int)*first_day_of_week;
	    }
	#endif

    // default Sun for other
    return 1;
}

/* returned abbreviated day of the week */
char *
wkday(const int d)
{
    switch (d) {
    case 1: return nl_langinfo(ABDAY_1); /* Sun */
    case 2: return nl_langinfo(ABDAY_2); /* Mon */
    case 3: return nl_langinfo(ABDAY_3); 
    case 4: return nl_langinfo(ABDAY_4); 
    case 5: return nl_langinfo(ABDAY_5); 
    case 6: return nl_langinfo(ABDAY_6); 
    case 7: return nl_langinfo(ABDAY_7); 
    default: return "";
    }
}

/* quits the program and print error */
void
error_exit(char *error)
{
	// printf("Error: %s\n", error);
	fprintf(stderr, "Error: %s\n", error);
	exit(EXIT_FAILURE);
}

/* print the string of days of the week to stdout */
int 
print_header(int first_day_of_week)
{
	char *wkd;
	int i, len = 0;

	i = first_day_of_week;

	wkd = wkday(i);
	len = strlen(wkd);

	/* print header of firts day */
    if (first_day_of_week == 1)
    {
 		printf((len / 2 % 2 == 0) ? " %s%s%s  " : " %s%s%s ", RED, wkd, NC);
	}
	else if (first_day_of_week == 2)
	{
		printf((len / 2 % 2 == 0) ? " %s  " : " %s ", wkd);
	}

	/* print header of 2-5 days */
    for (i = first_day_of_week + 1; i < first_day_of_week + 5; i++)
    {
        wkd = wkday(i);
        printf((len / 2 % 2 == 0) ? "%s  " : "%s ", wkd);
    }

    /* print header of 6 day */
    wkd = wkday(i);
    if (first_day_of_week == 1)
    {
 		printf((len / 2 % 2 == 0) ? "%s  " : "%s ", wkd);
	}
	else if (first_day_of_week == 2)
	{
		printf((len / 2 % 2 == 0) ? "%s%s%s  " : "%s%s%s ", RED, wkd, NC);	
	}

	/* print header of 7 day */
    wkd = wkday((first_day_of_week == 1) ? 7 : 1);
    printf("%s%s%s\n", RED, wkd, NC);

	return 1;
}

/* print the one week to stdout */
int 
print_week(char *str, int n)
{
	size_t i;
	char *s;

	s = str;
	i = DAYS_A_WEEK * CELL_SIZE;
	while(i--)
	{
		if(*s++ == '\0')
		{
			s[-1] = ' ';
		}
	}

	i = n + 1;
	while(i--)
	{
		if(*--s != ' ')
		{
			break;
		}
	}

	s[1] = '\0';
	printf("%s\n", str);
}

/* creates a monthly calendar */
int 
calendar(int m, int y, char *p, int w, int market_date, int first_day_of_week)
{
	size_t d, i;
	char *s;

	s = p;
	d = get_first_day_of_year(y, first_day_of_week);
	month_days[2] = 29;
	month_days[9] = 30;

	switch((get_first_day_of_year(y+1, first_day_of_week)+7-d)%7) {

		/* non leap year*/
		case 1:
			month_days[2] = 28;
			break;
		/* 1752 */
		default:
			month_days[9] = 19;
			break;
		/* leap year */
		case 2:
		;
	}

	for(i=1; i<m; i++)
	{
		d += month_days[i];
	}
	d %= 7;
	s += 4*d;
	// s++;
	for(i=1; i<=month_days[m]; i++) 
	{
		s++;
		if(i==3 && month_days[m]==19) 
		{
			i += 11;
			month_days[m] += 11;
		}

		if(i <= 9)
		{
			if (i == market_date)
			{
				s[0] = LEFT_DAYSELECT;
				*s++;
				*s = i%10+'0';
				s[+1] = RIGHT_DAYSELECT;
			}
			else
			{
				*s++;
				*s = i%10+'0';
			}
		}
		else
		{
			if (i == market_date)
			{
				s[-1] = LEFT_DAYSELECT;
				*s++ = i/10+'0';
				*s = i%10+'0';
				s[+1] = RIGHT_DAYSELECT;
			}
			else
			{
				*s++ = i/10+'0';
				*s = i%10+'0';
			}
		}
		s++;
		s++;
		if(++d == DAYS_A_WEEK) 
		{
			d = 0;
			s = p + CELL_SIZE * DAYS_A_WEEK; /* goto next week */
			p = s;
		}
	}
}

/* return day of the week of jan 1 of given year*/
int
get_first_day_of_year(int year, int first_day_of_week)
{
	int y, d;

	/* one extra day for 4 years */
	y = year;
	d = 5 - first_day_of_week + y + (y + 3) / 4; 

	/* less three days per 400 */
	if (y > 1800) 
	{
		d -= (y - 1701) / 100;
		d += (y - 1601) / 400;
	}

	/* great calendar changeover instant */
	if (y > 1752) 
	{
		d += 3;
	}

	return(d%7);
}

/* returns the current date if it is in the current month and year, else returned 0 */
int 
cur_date(int in_month, int in_year)
{
	char outstr [BUF_SIZE];
	time_t t;
	struct tm *tmp;

	t = time(NULL);
	tmp = localtime(&t);
	if (tmp == NULL) {
		error_exit("localtime");
	}

	if (strftime(outstr, sizeof(outstr), "%d", tmp) == 0) {
		error_exit("strftime returned 0");
	}

	return ((cur_year() == in_year) && (cur_month() == in_month)) ? atoi(outstr) : 0;
}

/* returned current year */
int 
cur_year(void)
{
	char outstr [BUF_SIZE];
	time_t t;
	struct tm *tmp;

	t = time(NULL);
	tmp = localtime(&t);

	if (tmp == NULL) {
		error_exit("localtime");
	}

	if (strftime(outstr, sizeof(outstr), "%Y", tmp) == 0) {
		error_exit("strftime returned 0");
	}

	return atoi(outstr);
}

/* returned current month */
int 
cur_month(void)
{
	char outstr [BUF_SIZE];
	time_t t;
	struct tm *tmp;

	t = time(NULL);
	tmp = localtime(&t);

	if (tmp == NULL) {
		error_exit("localtime");
	}

	if (strftime(outstr, sizeof(outstr), "%m", tmp) == 0) {
		error_exit("strftime returned 0");
	}

	return atoi(outstr);
}
