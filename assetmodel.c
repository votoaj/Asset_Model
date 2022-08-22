#include <stdlib.h>
#include <stdio.h>
#include <math.h>

struct Date {
	int day;
	int month;
	int year;
};




int monthDiff(struct Date date1, struct Date date2)
{
	int result = 0;
	
	result = date2.month - date1.month + 12 * (date2.year - date1.year);
	
	return result;
}





float dateDiff(struct Date date1, struct Date date2)
{
	float result = 0.0;
	
	// Convert dates to Julian Day Number
	
	int JDN1 = 367 * date1.year - (7 * (date1.year + 5001 + (date1.month - 9) / 7)) / 4 + (275 * date1.month) / 9 + date1.day + 1729777;
	int JDN2 = 367 * date2.year - (7 * (date2.year + 5001 + (date2.month - 9) / 7)) / 4 + (275 * date2.month) / 9 + date2.day + 1729777;

	result = ((float)JDN2 - (float)JDN1) / 365.25;
	
	return result;
}


/*
float dateDiff(int day1, int mon1, int year1, int day2, int mon2, int year2)
{
	float result = 0.0;
	
	// Convert dates to Julian Day Number
	
	int JDN1 = 367 * year1 - (7 * (year1 + 5001 + (mon1 - 9) / 7)) / 4 + (275 * mon1) / 9 + day1 + 1729777;
	int JDN2 = 367 * year2 - (7 * (year2 + 5001 + (mon2 - 9) / 7)) / 4 + (275 * mon2) / 9 + day2 + 1729777;

	result = ((float)JDN2 - (float)JDN1) / 365.25;
	
	return result;
}

*/


float dateDiff2(int day1, int mon1, int year1, int day2, int mon2, int year2)
{
	int day_diff, mon_diff, year_diff;
	
	if (!valid_date(day1, mon1, year1))
	{
		printf("First date is invalid.\n");
	}
	
	if (!valid_date(day2, mon2, year2))
	{
		printf("Second date is invalid.\n");
		exit(0);
	}
	
	if (day2 > day1)
	{
		// borrow days from february?
		
		if (mon2 == 3)
		{
			// check if its a leap year
			if ((year2 % 4 == 0 && year2 % 100 != 0) || (year2 % 400 == 0))
			{
				day2 += 29;
			}
			else
			{
				day2 += 28;
			}
		}
		
		// borrow days from april or june or september or november
		else if (mon2 == 5 || mon2 == 7 || mon2 == 10 || mon2 == 12)
		{
			day2 += 30;
		}
		
		//borrow days from jan or mar or july or aug or oct or dec
		else
		{
			day2 += 31;
		}
		
		mon2 = mon2 - 1;
		
	}
	
	if (mon2 < mon1)
	{
		mon2 += 12;
		year2 -= 1;
	}
}

int valid_date(int day, int mon, int year)
{
	int is_valid = 1, is_leap = 0;
	
	if (year >= 1800 && year <= 9999)
	{
		// check if it is a leap year
		if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
		{
			is_leap = 1;
		}
		
		// check if mon is between 1 and 12
		
		if (mon >= 1 && mon < 12)
		{
			// check for days in feb
			if (mon == 2)
			{
				if (is_leap && day == 29)
				{
					is_valid = 1;
				}
				else if (day > 28)
				{
					is_valid = 0;
				}
			}
			
			// check for days in april, june, september, and november
			
			else if (mon == 4 || mon == 6 || mon == 9 || mon == 11)
			{
				if (day > 30)
				{
					is_valid = 0;
				}
			}
			
			// check for days in rest of months
			else if (day > 31)
			{
				is_valid = 0;
			}
		}
		
		else
		{
			is_valid = 0;
		}
		
	}
	
	else
	{
		is_valid = 0;
	}	
	
	return is_valid;	

}





float naiveInterp(float x[], float y[], float xp, int len)
{
	
	float result = 0.0;
			
	if (xp < x[0])
	{
		result = y[0];
	}
	else if (xp > x[len-1])
	{
		result = y[len-1];
	}
	else
	{	
		for (int i = 0; i < (len-1); i++)
		{
			if (xp >= x[i] && xp <= x[i+1])
			{
				result = y[i] + (y[i+1] - y[i]) * (xp - x[i]) / (x[i+1] - x[i]);
			}
			
		}
	}
	
	return result;
}





int bisectionSearch(float arr[], int lo, int hi, float x)
{
	int pos; 
	
	if (lo <= hi && x >= arr[lo] && x <= arr[hi])
	{
		pos = lo + (((float)(hi - lo) / (arr[hi] - arr[lo])) * ( x - arr[lo] ));
		
		if (arr[pos] == x)
			return pos;
		
		if (arr[pos] < x)
			return bisectionSearch(arr, pos + 1, hi, x);
		
		if (arr[pos] > x)
			return bisectionSearch(arr, lo, pos - 1, x);
		
	}
	
	return -1;
}



float market_value_bond(float coupon, float prin, int pmt_freq, int periods,float treasury_rate, float mkt_sprd)
{
	float mkt_value = 0.0;
	float v = 1.0;
	float mkt_rate = (treasury_rate + mkt_sprd) / 12.0;
	coupon /= pmt_freq;
	
	while (periods > 0)
	{
			
		if (periods % (12 / pmt_freq) == 0)
		{
			mkt_value += prin * coupon * v * powf(1.0 / (1 + mkt_rate), 0.5);  // powf is to account for timings, we assume payments are mid month
		}
		
		v /= (1 + mkt_rate);
		
		periods--;
		
	}
	
	
	mkt_value += prin * (1 + coupon) * v * powf(1.0 / (1 + mkt_rate), 0.5);
	
	return mkt_value;
	
	
}

void cashflow_bond(float buf[1000],float scn[361][10], float coupon, float prin, int pmt_freq, int periods)
{
	int time_step = 0;
	coupon /= pmt_freq;
		
	while (periods > 0)
	{
		if (periods % (12 / pmt_freq) == 0)
		{
			buf[time_step] = prin * coupon;
		}
		
		time_step++;
		periods--;
		
	}
	
	buf[time_step] = prin * (1 + coupon);
}


float market_value_commercial_mortgage(float coupon, float prin, int pmt_freq, int periods,float treasury_rate, float mkt_sprd, int amort_term)
{
	float mkt_value = 0.0;
	float v = 1.0;
	float mkt_rate = (treasury_rate + mkt_sprd) / 12.0;
	coupon /= pmt_freq;
	float interest = 0.0;
	float payment = prin / ((1.0 - powf( 1.0 / ( 1.0 + coupon) , amort_term)) / coupon);
	
	while (periods > 0)
	{
		
		if (periods % (12 / pmt_freq) == 0)
		{
			
			interest = prin * coupon;
			
			mkt_value += payment * v * powf(1.0 / (1 + mkt_rate), 0.5);  // powf is to account for timings, we assume payments are mid month
			
			prin = prin - payment + interest;
		}
		
		v /= (1 + mkt_rate);
		
		periods--;
	}
	
	
	mkt_value += prin * v * powf(1.0 / (1 + mkt_rate), 0.5);  // any leftover principal.....shouldnt be any
	
	return mkt_value;
	
}






int main()
{
	
	int row = 0;
	float scn[361][10] = {{0,0}};
	char line[1000];
	char* token = NULL;
	FILE* fp = fopen("C:\\work\\C\\assetmodel\\scn.csv", "r");
	
	float temp = 0.0;
	
	for (int i = 0; i < 361; i++)
	{
		int col = 0;
		
		while ((fscanf(fp, "%f", &scn[i][col++]) == 1) && col < 10)
		{
			fscanf(fp, ",");
		}
	}
	
		
	int periods = 21;
	float mkt_rate = 0.0545;
	float treasury = 0.0;
	float mkt_value = 0.0;

	
	float proj_yrs = 5;
	
	float yc_tenors[10] = { 0.25, 0.5, 1, 2, 3, 5, 7, 10, 20, 30 };
	
	/*float scn[6][10] = {
					{0.0006,	0.0006,	0.0019,	0.0039,	0.0073,	0.0097,	0.0126,	0.0144,	0.0152,	0.0194},
					{0.0044,	0.0046,	0.0059,	0.0079,	0.0112,	0.0134,	0.0163,	0.0181,	0.0189,	0.023},
					{0.0082,	0.0085,	0.01,	0.0119,	0.015,	0.0172,	0.0199,	0.0217,	0.0226,	0.0265},
					{0.0121,	0.0125,	0.014,	0.0159,	0.0189,	0.0209,	0.0236,	0.0254,	0.0263,	0.0301},
					{0.0159,	0.0165,	0.018,	0.0199,	0.0227,	0.0246,	0.0272,	0.029,	0.03,	0.0336},
					{0.0197,	0.0205,	0.0221,	0.024,	0.0266,	0.0284,	0.0309,	0.0327,	0.0337,	0.0372}
					};*/
	
	
	
	float rates[10] = {0.0006,	0.0006,	0.0019,	0.0039,	0.0073,	0.0097,	0.0126,	0.0144,	0.0152,	0.0194};
	
	

	int len = sizeof(yc_tenors) / sizeof(yc_tenors[0]);
	
	
	struct Date maturity_date = {15, 12, 2049};
	struct Date projection_date = {30, 9, 2019};

	
	
	
	periods = monthDiff(projection_date, maturity_date);
	
	//pos = bisectionSearch(yc_tenors, 0, 9, 25);
	treasury = naiveInterp(yc_tenors,scn[0], 25.0, len);
	printf("%f\n", treasury);
	
	printf("%d\n", periods);
	
	float cashflows[1000] = {0};
	
	//cashflow_bond(cashflows, scn, 0.053, 2000000.0, 2, periods);
	
	/*for (int i = 0; i < 364; i++)
	{
		printf("%f\n", cashflows[i]);
	}*/

	mkt_value = market_value_bond(	0.053, 2000000.0, 1,periods, 0.03, 0.0245);
	
	printf("mkt value of bond: %f\n", mkt_value);
	
	struct Date maturity_date1 = {1, 6, 2028};
	struct Date projection_date1 = {1, 5, 2018};

	
	periods = monthDiff(projection_date1, maturity_date1);
	

	
	mkt_value = market_value_commercial_mortgage(0.04524, 3300000.0, 12, 120, 0.0287, 0.0175, 240);
	printf("mkt value of commercial mtg: %f\n", mkt_value);
	
	float payment = 3300000.0 / ((1.0 - powf( 1.0 / ( 1.0 + 0.04524/12) , 240)) / (0.04524 / 12));
	printf("payment of commercial mtg: %f\n", payment);

}






/*
issues:
dealing with dates in C
interpolation
solver functionality

*/





/*
date logic: overiq.com

*/
