#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>


/* GLOBALVARIABLES */
float yieldcurve_tenors[10] = { 0.25, 0.5, 1, 2, 3, 5, 7, 10, 20, 30 };
float rates[10] = {0.0172, 0.0251, 0.028, 0.0292, 0.0299, 0.0301, 0.0304, 0.0298, 0.0338, 0.0314};
int yc_len = 10;

const float call_rates[11] = {-0.05,-0.04,-0.03,-0.02,-0.01,0.0,0.01,0.02,0.03,0.04,0.05}
const float call_probs[11] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0}
int call_len = 11;

////////////////////






struct Date {
	int day;
	int month;
	int year;
};

struct Date _projDate =  {30, 6, 2022};

struct Bond {
	float coupon;
	float prin;
	int pmt_freq;
	struct Date maturityDate;
	struct Date callDate;	
	float mkt_spread;
	float mkt_value_input;
};






/*
float fmax(float x, float y)
{
	if (x > y)
	{
		return x;
	} 
	else
	{
		return y;
	}
}


float fmin(float x, float y)
{
	if (x < y)
	{
		return x;
	}
	else
	{
		return y;
	}
}
*/

int monthDiff(struct Date date1, struct Date date2)
{
	int result = 0;
	
	result = date2.month - date1.month + 12 * (date2.year - date1.year);
	
	return result;
}


float r_base(float ten)
{
	if (ten < yieldcurve_tenors[0])
	{
		return rates[0];
	}
	else if (ten > yieldcurve_tenors[(yc_len - 1)])
	{
		return rates[(yc_len - 1)];
	}
	
	else
	{
		for (int i = 0; i < (yc_len - 1); i++)
		{
			if (yieldcurve_tenors[i] <= ten && ten <= yieldcurve_tenors[i+1])
			{
				return rates[i] + (rates[i+1] - rates[i]) * (ten - yieldcurve_tenors[i]) / (yieldcurve_tenors[i+1] - yieldcurve_tenors[i]);
			} 
			
		}
	}

	printf("Tenor not found.\n");
	return 0.0;
	
}

float r(float t, float T)
{
	if (t = T)
		return r_base(T);
	else
		return (r_base(T)*T - r_base(t)*t) / (T - t);
}



float Z(t, T)
{
	return exp(-1.0 * r(t, t+T) * (T-t));
}




float market_value_bond_struct(struct Bond bond1, struct Date projDate) /* pass array or pointer as current yield curve */
{
	float mkt_value = 0.0;
	float v = 1.0;
	int periods = monthDiff(projDate, bond1.maturityDate);
	
	float treasury_rate = r(0.0, periods / 12.0);
	
	float mkt_rate = (treasury_rate + bond1.mkt_spread) / 12.0;
	bond1.coupon /= bond1.pmt_freq;
	
	while (periods > 0)
	{
			
		if (periods % (12 / bond1.pmt_freq) == 0)
		{
			mkt_value += bond1.prin * bond1.coupon * v * powf(1.0 / (1 + mkt_rate), 0.5);  // powf is to account for timings, we assume payments are mid month
		}
		
		v /= (1 + mkt_rate);
		
		periods--;
		
	}
	
	
	mkt_value += bond1.prin * (1 + bond1.coupon) * v * powf(1.0 / (1 + mkt_rate), 0.5);
	
	return mkt_value;
	
	
}

void set_mktSpread_bond(struct Bond *bond1)
{
	int itr = 0;
	int maxmitr = 40;
	float allerr = 1.0;
	float a = 0.0;
	float b = 0.2;
	
	float mkt_diff_a = 0.0;	
	float mkt_diff_mid = 0.0;
	
	
	while ( itr < maxmitr )
	{
		bond1->mkt_spread = a;
		
		mkt_diff_a = bond1->mkt_value_input - market_value_bond_struct(*bond1, _projDate);
			
		//using bisection method here
		//bisectionMethod(&mkt_spread, a, b, &itr);		
		bond1->mkt_spread = (a + b) / 2;
		
		mkt_diff_mid = bond1->mkt_value_input - market_value_bond_struct(*bond1, _projDate);
		
		if (abs(mkt_diff_mid) < allerr)
		{
			break;
		}
		
		
		if ((mkt_diff_mid < 0.0 && mkt_diff_a < 0.0) || (mkt_diff_mid > 0.0 && mkt_diff_a > 0.0))
		{
			a = bond1->mkt_spread;
			b = b;
		}
		else
		{
			a = a;
			b = bond1->mkt_spread;
		}			
		itr++;		
	}	
}




float naiveInterp(float *x, float *y, float xp, int len)
{
	
	float result = 0.0;
			
	if (xp < *x)
	{
		result = *y;
	}
	else if (xp > *(x + len - 1))
	{
		result = *(y + len - 1);
	}
	else
	{	
		for (int i = 0; i < (len-1); i++)
		{
			if (xp >= *(x + i) && xp <= *(x + i + 1))
			{
				result = *(y + i) + (*(y + i + 1) - *(y + i)) * (xp - *(x + i)) / (*(x + i + 1) - *(x + i));
			}
			
		}
	}
	
	return result;
}



void bisectionMethod(float *x, float a, float b, int *itr)
{

	*x = (a + b) / 2;
	++(*itr);
	
	printf("Iteration no. %3d X = %7.5f\n", *itr, *x);

}


float cashflow_bond(struct Bond bond1, struct Date date1)
{
	
	float cashflow = 0.0;
	int periods = monthDiff(projDate, bond1.maturityDate);
	float treasury_rate = r(0.0, periods / 12.0);
	float call_margin = 0.01;
	
	float call_rate = treasury_rate + bond1.mkt_spread + call_margin - bond1.coupon; // current rate plus margin minus actual coupon
	
	float call_prob = naiveInterp(call_rates, call_probs, call_rate, call_len);
	
	bond.coupon /= bond.pmt_freq;

	if (periods % (12 / bond.pmt_freq) == 0 )
	{
		cashflow += bond.prin * bond.coupon;
		cashflow += bond.prin * call_prob;
	}
	
	if (periods == 0)
	{
		cashflow += bond.prin;
	}	
	
	return cashflow;
}


float market_value_mortgage(float coupon, float prin, int pmt_freq, int periods,float treasury_rate, float mkt_sprd, int amort_term)
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


float cashflow_mortgage(float scn[361][10], float coupon, float prin, int pmt_freq, int periods, int amort_term)
{
	int time_step = 0;
	coupon /= pmt_freq;
	float payment = prin / ((1.0 - powf( 1.0 / ( 1.0 + coupon) , amort_term)) / coupon);
	
	float cashflow = 0.0;
		

	if (periods % (12 / pmt_freq) == 0 && periods > 0)
	{
		cashflow = payment;
	}
	else if (periods == 0)
	{
		cashflow = prin;
	}

	
	return cashflow;

}



float market_value_cmo(float cmoAPortion, float cmoASpread, float cmoBPortion, float cmoBSpread, float cmoCSpread, float underlying_coupon, float underlying_prin, int pmt_freq, int periods,float treasury_rate, float mkt_sprd, float default_rate)
{
	float mkt_value = 0.0;
	float v = 1.0;
	float mkt_rate = (treasury_rate + mkt_sprd) / 12.0;
	underlying_coupon /= pmt_freq;
	float interest = 0.0;
	float payment = 0.0;
	
	float cmoAPrin = underlying_prin * cmoAPortion;
	float cmoBPrin = underlying_prin * cmoBPortion;
	float cmoCPrin = underlying_prin * (1.0 - cmoAPortion - cmoBPortion);

	float cmoAIntEarned = 0.0;
	float cmoBIntEarned = 0.0;
	float cmoCIntEarned = 0.0;

	float cmoAIntPaid = 0.0;
	float cmoBIntPaid = 0.0;
	float cmoCIntPaid = 0.0;

	float cmoAPrinPaid = 0.0;
	float cmoBPrinPaid = 0.0;
	float cmoCPrinPaid = 0.0;
	
	while (periods > 0)
	{
		
		if (periods % (12 / pmt_freq) == 0)
		{
			
			underlying_prin *= powf((1 - default_rate), 1.0 / pmt_freq);
			
			payment = underlying_prin / ((1.0 - powf( 1.0 / ( 1.0 + underlying_coupon) , periods)) / underlying_coupon);
			
			interest = underlying_prin * underlying_coupon;
			
			cmoAIntEarned = cmoAPrin * (underlying_coupon + cmoASpread / pmt_freq);
			cmoBIntEarned = cmoBPrin * (underlying_coupon + cmoBSpread / pmt_freq);
			cmoCIntEarned = cmoCPrin * (underlying_coupon + cmoCSpread / pmt_freq);
			
			cmoAIntPaid = fmin(cmoAIntEarned, payment);
			cmoBIntPaid = fmin(cmoBIntEarned, payment - cmoAIntPaid);
			cmoCIntPaid = fmin(cmoCIntEarned, payment - cmoAIntPaid - cmoBIntPaid);
			
			
			cmoAPrinPaid = fmax(fmin(cmoAPrin, payment - cmoAIntPaid - cmoBIntPaid - cmoCIntPaid), 0.0);
			cmoAPrin -= cmoAPrinPaid;
			
			cmoBPrinPaid = fmax(fmin(cmoBPrin, payment - cmoAIntPaid - cmoBIntPaid - cmoCIntPaid - cmoAPrinPaid), 0.0);
			cmoBPrin -= cmoBPrinPaid;
			
			cmoCPrinPaid = fmax(fmin(cmoCPrin, payment - cmoAIntPaid - cmoBIntPaid - cmoCIntPaid - cmoAPrinPaid - cmoBPrinPaid), 0.0);
			cmoCPrin -= cmoCPrinPaid;
			
			//printf("Prin & Interest paid at period %d: %f, %f, %f, %f, %f\n", periods, cmoBPrinPaid, cmoBIntPaid,cmoAPrinPaid, cmoAIntPaid, cmoCIntPaid);
			
			mkt_value += (cmoBPrinPaid + cmoBIntPaid) * v * powf(1.0 / (1 + mkt_rate), 0.5);  // powf is to account for timings, we assume payments are mid month
			
			underlying_prin = underlying_prin - payment + interest;
		}
		
		v /= (1 + mkt_rate);
		
		periods--;
	}
	
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

		
	struct Date maturity_date = {30, 1, 2032};
	struct Date projection_date = {30, 6, 2022};
	
	
	periods = monthDiff(projection_date, maturity_date);
	float mkt_value = 0.0;
	
	/*
	float mkt_value_input = 1997875.0;
	float mkt_spread = 0.0;
	
	
	int itr = 0, maxmitr = 40;
	float allerr = 0.1;
	float a = 0.0, b = 0.2;
	
	
	float mkt_diff_a = 0.0;
	
	mkt_value = market_value_bond(0.033, 2500000.0, 2, periods, 0.0, yc_tenors, scn[0]);
	
	float mkt_diff_mid = mkt_value_input - mkt_value;
	
	
	while ( fabs(mkt_diff_mid) > allerr && itr < maxmitr )
	{
		
		mkt_value = market_value_bond(0.033, 2500000.0, 2, periods, a, yc_tenors, scn[0]);
		
		mkt_diff_a = mkt_value_input - mkt_value;
		
		bisectionMethod(&mkt_spread, a, b, &itr);
		
		mkt_value = market_value_bond(0.033, 2500000.0, 2, periods, mkt_spread, yc_tenors, scn[0]);
		
		mkt_diff_mid = mkt_value_input - mkt_value;
		
		if ((mkt_diff_mid < 0.0 && mkt_diff_a < 0.0) || (mkt_diff_mid > 0.0 && mkt_diff_a > 0.0))
		{
			a = mkt_spread;
			b = b;
		}
		else
		{
			a = a;
			b = mkt_spread;
		}
		
		
	}
	
	mkt_value = market_value_bond(0.033, 2500000.0, 2, periods, mkt_spread, yc_tenors, scn[0]);
	
	printf("Market Value: %f, Spread: %f\n",mkt_value, mkt_spread);
	*/
	struct Bond bond1 = {0.033, 2500000.0, 2, maturity_date, maturity_date, 0.0, 1997875.0};
	
	set_mktSpread_bond(&bond1);

	/*
	
	float coupon;
	float prin;
	int pmt_freq;
	struct Date maturityDate;
	struct Date callDate;
	
	float mkt_sprd;
	float periods;
	
	market_value_bond_struct(struct Bond bond1, struct Date projDate, float yc_tenors[], float scn[])
	
	*/
	
	mkt_value = market_value_bond_struct(bond1, projection_date);
	
	printf("Market Value: %f, Spread: %f",mkt_value, bond1.mkt_spread);
	
	/*
	float cashInt = 0.0;
	
	
	int time_step = 0;
	
	mkt_value = market_value_bond(0.053, 2000000.0, 2, periods, 0.0252, yc_tenors, scn[0]);
	printf("%f",mkt_value);
	
	
	while (time_step < 361 && periods >= 0)
	{
		mkt_value = market_value_bond(0.053, 2000000.0, 2, periods, 0.0252, yc_tenors, scn[time_step]);
		cashInt = cashflow_bond(scn[time_step], 0.053, 2000000.0, 2, periods);
		
		printf("Market Value, Cashflow at time %d: %f, %f\n", time_step, mkt_value, cashInt);
		
		periods--;
		time_step++;
	
	}


	*/
	

	
	
}



