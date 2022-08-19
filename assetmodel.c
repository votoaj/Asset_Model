#include <stdlib.h>
#include <stdio.h>


float dateDiff(int day1, int mon1, int year1, int day2, int mon2, int year2)
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
}

int valid_date(int day, int mon, int year)
{
	int 
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



float bond_mkt_value(float coupon, float prin, int mode, float periods,float treasury_rate, float mkt_sprd)
{
	float mkt_value = 0.0;
	float v = 1.0;
	float mkt_rate = treasury_rate + mkt_sprd;
	
	for (int i = 1; i <= periods; i++)
	{
		v /= (1+mkt_rate); 
		
		if (i < periods)
		{
			mkt_value += prin * coupon * v;
		}
		else 
		{
			mkt_value += prin * (1 + coupon) * v;
		}
	}

	return mkt_value;
	
}



int main()
{

	float periods = 21.0;
	float mkt_rate = 0.0545;
	float treasury = 0.0;
	float mkt_value = 0.0;

	
	float proj_yrs = 5;
	
	float yc_tenors[10] = { 0.25, 0.5, 1, 2, 3, 5, 7, 10, 20, 30 };
	
	float scn[6][10] = {
					{0.0006,	0.0006,	0.0019,	0.0039,	0.0073,	0.0097,	0.0126,	0.0144,	0.0152,	0.0194},
					{0.0044,	0.0046,	0.0059,	0.0079,	0.0112,	0.0134,	0.0163,	0.0181,	0.0189,	0.023},
					{0.0082,	0.0085,	0.01,	0.0119,	0.015,	0.0172,	0.0199,	0.0217,	0.0226,	0.0265},
					{0.0121,	0.0125,	0.014,	0.0159,	0.0189,	0.0209,	0.0236,	0.0254,	0.0263,	0.0301},
					{0.0159,	0.0165,	0.018,	0.0199,	0.0227,	0.0246,	0.0272,	0.029,	0.03,	0.0336},
					{0.0197,	0.0205,	0.0221,	0.024,	0.0266,	0.0284,	0.0309,	0.0327,	0.0337,	0.0372}
					};
					
					
	float rates[10] = {0.0006,	0.0006,	0.0019,	0.0039,	0.0073,	0.0097,	0.0126,	0.0144,	0.0152,	0.0194};
	
	

	int len = sizeof(yc_tenors) / sizeof(yc_tenors[0]);
	
	mkt_value = bond_mkt_value(	0.053, 2000000.0, 1, 31.0, 0.03, 0.0245);
	
	
	
	//pos = bisectionSearch(yc_tenors, 0, 9, 25);
	treasury = naiveInterp(yc_tenors,scn[0], 25.0, len);
	
	printf("%f\n", rates[len-1]);
	
	

	
	
	printf("%f\n", mkt_value);
	printf("%f", treasury);
	
	
}






/*
issues:
dealing with dates in C
interpolation
solver functionality

*/