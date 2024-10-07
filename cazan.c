/* See LICENSE file for copyright and license details. */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "util.h"

/* macros */
#define PI 3.14159265358979323846
#define JUL1970 2440587.5
#define SEC_INDAY 86400.0
#define SEC_INHOUR 3600.0
#define OFFSET 2451545.0

/* typedef */
/* function declarations */

/* global variables */
static const long double p1 = 0.066666666666666666;
static const long double to_rad = 0.017453292519943295;
static const long double to_deg = 57.295779513082322865;
static long double p2, p3;

/* function implementations */
static void
usage(void)
{
	die("usage: cazan -[AaUuv] [-lo longitude] [-la latitude]\n\
              [-al altitude] [-tz timezone] [-fa fajr_angle]\n\
              [-ia isha_angle] [-am asr_mazhab] [-r ramadan]\n\
options:\n    \
-A  print all prayers time, 12-hour clock format.\n    \
-a  print all prayers time.\n    \
-U  print all prayers time, unix-time format.\n    \
-u  print next prayer time, unix-time format.\n    \
-v  print version.");
}

static void
convert_sec_double_to_min_hour(double sec, int *hours, int *min)
{
	*hours = sec / SEC_INHOUR;
	*min = (sec - (*hours * SEC_INHOUR)) / 60;
}

static long double
T(long double t)
{
	long double p4, p5;
	p4 = -1.0 * sin(to_rad * t);
	p5 = to_deg * (acos((p4 - p3) / p2));
	return p1 * p5;
}

int
main(int argc, char *argv[])
{
	int hours, min;
	char pray_char;
	time_t now, start_of_day;
	long double duhr_t, asr_t, maghrib_t, isha_t;
	long double julian, d, g, e, q, sing, sin2g, sine, cose, L, sinL, cosL;
	long double RA, D, EqT, duhr, fajr, asr, maghrib, isha;
	long double fajr_t, p4, p5, p6, p7, A;
	long double next_prayer;


	/*
	if (argc == 1)
		goto start;

	if (argc == 2) {
		if (strncmp("-v", argv[1], 2) == 0)
			die("cazan-" VERSION);
		if (strncmp("-a", argv[1], 2) == 0)
			opt = 'a';
		if (strncmp("-A", argv[1], 2) == 0)
			opt = 'A';
		if (strncmp("-U", argv[1], 2) == 0)
			opt = 'U';
		if (strncmp("-u", argv[1], 2) == 0)
			opt = 'u';
		else
			usage();
		goto start;
	}

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-lo")) {
			longitude = strtold(argv[++i], NULL);
		} else if (!strcmp(argv[i], "-la")) {
			latitude = strtold(argv[++i], NULL);
		} else if (!strcmp(argv[i], "-al")) {
			altitude = strtold(argv[++i], NULL);
		} else if (!strcmp(argv[i], "-tz")) {
			timezone = strtold(argv[++i], NULL);
		} else if (!strcmp(argv[i], "-fa")) {
			fajr_angle = strtold(argv[++i], NULL);
		} else if (!strcmp(argv[i], "-ia")) {
			isha_angle = strtold(argv[++i], NULL);
		} else if (!strcmp(argv[i], "-am")) {
			asr_mazhab = atoi(argv[++i]);
			FAIL_IF((asr_mazhab > 2 || asr_mazhab < 1),
				"asr_mazhab must be 1 or 2");
		} else if (!strcmp(argv[i], "-r"))  {
			ramadan = atoi(argv[++i]);
		} else {
			usage();
		}
	}

	*/
	if (argc == 1)
		goto start;
	else if (argc == 2 && strncmp("-v", argv[1], 2) == 0)
		die("cazan-" VERSION);
	else
		usage();

start:
	time(&now);
	start_of_day = now - (now % 86400);
	julian = (now / SEC_INDAY) + JUL1970;
	d = julian - OFFSET;
	g = to_rad * ((d * 0.98560028) + 357.529);
	e = to_rad * (23.439 - (d * 0.00000036));
	q = (d * 0.98564736) + 280.459;
	sing = 1.915 * sin(g);
	sin2g = 0.020 * sin(2.0 * g);
	sine = sin(e);
	cose = cos(e);
	L = to_rad * (q + sing + sin2g);
	sinL = sin(L);
	cosL = cos(L);
	RA = to_deg * (atan2(cose * sinL, cosL) / 15.0);
	D = to_deg * (asin(sine * sinL));
	EqT = q / 15.0 - RA;
	p2 = cos(to_rad * latitude) * cos(to_rad * D);
	p3 = sin(to_rad * latitude) * sin(to_rad * D);

	/* duhr */
	duhr = 12.0 + timezone - EqT - (longitude / 15.0);
	duhr = duhr - (24 * floor(duhr / 24));
	duhr_t = start_of_day + (duhr * 3600);

	/* fajr */
	fajr_t = T(fajr_angle);
	fajr = duhr - fajr_t;
	fajr_t = start_of_day + (fajr * 3600);

	/* asr */
	p4 = tan(to_rad * ((latitude - D)));
	p5 = atan2(1.0, p4 + asr_mazhab);
	p6 = sin(p5);
	p7 = to_deg * (acos((p6 - p3) / p2));
	A = p1 * p7;
	asr = duhr + A;
	asr = asr - (24 * floor(asr / 24));
	asr_t = start_of_day + (asr * 3600);

	/* maghrib */
	maghrib = duhr + T(0.8333 + 0.0347 * sqrt(altitude));
	maghrib = maghrib - (24 * floor(maghrib / 24));
	maghrib_t = start_of_day + (maghrib * 3600);

	/* isha */
	if (fajr_angle == 18.5) { /* Umm Al-Qura */
		isha = maghrib + (90.0 * 0.016666666666666666);
		if (ramadan == 1) /* Ramadan Umm Al-Qura */
			isha = maghrib + (120.0 * 0.016666666666666666);
	} else { /* Use isha_angle */
		isha = duhr + T(isha_angle);
	}
	isha = isha - (24 * floor(isha / 24));
	isha_t = start_of_day + (isha * 3600);

	next_prayer = fajr_t - now;

	if (fajr_t > now) {
		pray_char = 'F';
	} else if (duhr_t > now) {
		pray_char = 'D';
		next_prayer = duhr_t - now;
	} else if (asr_t > now) {
		pray_char = 'A';
		next_prayer = asr_t - now;
	} else if (maghrib_t > now) {
		pray_char = 'M';
		next_prayer = maghrib_t - now;
	} else if (isha_t > now) {
		pray_char = 'I';
		next_prayer = isha_t - now;
	} else {
		pray_char = 'F';
		next_prayer = fajr_t + SEC_INDAY - now;
	}

	/*
	printf("now\t\t%ld\n", now);
	printf("fajr_t\t\t%.18Lf\n", fajr_t);
	printf("duhr_t\t\t%.18Lf\n", duhr_t);
	printf("asr_t\t\t%.18Lf\n", asr_t);
	printf("maghrib_t\t%.18Lf\n", maghrib_t);
	printf("isha_t\t\t%.18Lf\n", isha_t);
	*/

	convert_sec_double_to_min_hour(next_prayer, &hours, &min);
	printf("%c %.2d:%.2d\n", pray_char, hours, min);

	return 0;
}
