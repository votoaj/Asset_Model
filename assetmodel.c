#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

/* GLOBALVARIABLES */
float yieldcurve_tenors[10] = { 0.25, 0.5, 1, 2, 3, 5, 7, 10, 20, 30 };
float rates[10] = {0.0172, 0.0251, 0.028, 0.0292, 0.0299, 0.0301, 0.0304, 0.0298, 0.0338, 0.0314};
int yc_len = 10;

float call_rates[11] = {-0.05,-0.04,-0.03,-0.02,-0.01,0.0,0.01,0.02,0.03,0.04,0.05};
float call_probs[11] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0};
int call_len = 11;

float rate_data[10][1000][361] = {0.0};
float cashflows_total[1000][361] = {0.0};
float market_value_total[1000][361] = {0.0};


/////////////////////////////////////

struct Date {
	int day;
	int month;
	int year;
};

struct Date _projDate =  {30, 6, 2022};



//need array of structures to read in all assets
struct Bond {
	float coupon;
	float prin;
	int pmt_freq;
	struct Date maturityDate;
	struct Date callDate;	
	float mkt_spread;
	float mkt_value_input;
	float default_rate;
};

struct Mortgage {
	float coupon;
	float prin;
	int pmt_freq;
	struct Date maturityDate;
	float mkt_spread;
	float mkt_value_input;
	float default_rate;
	int amort_term;
	float service_fee;
};


int monthDiff(struct Date date1, struct Date date2)
{
	int result = 0;
	
	result = date2.month - date1.month + 12 * (date2.year - date1.year);
	
	return result;
}

void set_rates(int scn, int period)
{
	rates[0] = rate_data[0][scn][period];
	rates[1] = rate_data[1][scn][period];
	rates[2] = rate_data[2][scn][period];
	rates[3] = rate_data[3][scn][period];
	rates[4] = rate_data[4][scn][period];
	rates[5] = rate_data[5][scn][period];
	rates[6] = rate_data[6][scn][period];
	rates[7] = rate_data[7][scn][period];
	rates[8] = rate_data[8][scn][period];
	rates[9] = rate_data[9][scn][period];
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

float market_value_bond(struct Bond bond1, struct Date projDate) 
{
	float mkt_value = 0.0;
	float v = 1.0;
	int periods = monthDiff(projDate, bond1.maturityDate);
	
	if (periods <= 0)
	{
		return 0.0;
	}
	
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

float psa_model(float factor, float period)
{
	return 0.0;
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
		
		mkt_diff_a = bond1->mkt_value_input - market_value_bond(*bond1, _projDate);
			
		//using bisection method here
		//bisectionMethod(&mkt_spread, a, b, &itr);		
		bond1->mkt_spread = (a + b) / 2;
		
		mkt_diff_mid = bond1->mkt_value_input - market_value_bond(*bond1, _projDate);
		
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


float cashflow_bond(struct Bond *bond1, struct Date date1)
{
	
	float interest_paid = 0.0;
	float prin_paid = 0.0;
	
	int periods = monthDiff(date1, bond1->maturityDate);
	int periods_to_call = monthDiff(date1, bond1->callDate);
	
	if (periods < 0)
	{
		return 0.0;
	}
		
	float treasury_rate = r(0.0, periods / 12.0);
	float call_prob = 0.0;
	
	if (periods_to_call <= 0)
	{	
		float call_margin = 0.01;	
		float call_rate = bond1->coupon - (treasury_rate + bond1->mkt_spread + call_margin); // current rate plus margin minus actual coupon
		call_prob = naiveInterp(call_rates, call_probs, call_rate, call_len);
	}

	
	bond1->prin *= powf(1.0 - bond1->default_rate, 1.0 / 12.0);
	
	float coupon_pmt = bond1->coupon / bond1->pmt_freq;

	if (periods % (12 / bond1->pmt_freq) == 0 )
	{
		interest_paid += bond1->prin * coupon_pmt;
		prin_paid += bond1->prin * call_prob;
	}
	
	if (periods == 0)
	{
		prin_paid += bond1->prin;
	}	
	
	
	bond1->prin -= prin_paid;	
	
	return prin_paid + interest_paid;
}


float market_value_mortgage(struct Mortgage cm1, struct Date projDate)
{
	float mkt_value = 0.0;
	float v = 1.0;
	
	int periods = monthDiff(projDate, cm1.maturityDate);	
	
	if (periods <= 0)
	{
		return 0.0;
	}
	
	float treasury_rate = r(0.0, periods / 12.0);
	
	float mkt_rate = (treasury_rate + cm1.mkt_spread) / 12.0;	
	
	cm1.coupon /= cm1.pmt_freq;
		
	float interest = 0.0;
	float payment = cm1.prin / ((1.0 - powf( 1.0 / ( 1.0 + cm1.coupon) , cm1.amort_term)) / cm1.coupon);
	
	while (periods > 0)
	{
		
		if (periods % (12 / cm1.pmt_freq) == 0)
		{
			
			interest = cm1.prin * cm1.coupon;
			
			mkt_value += payment * v * powf(1.0 / (1 + mkt_rate), 0.5);  // powf is to account for timings, we assume payments are mid month
			
			cm1.prin = cm1.prin - payment + interest;
		}
		
		v /= (1 + mkt_rate);
		
		periods--;
	}
	
	
	mkt_value += cm1.prin * v * powf(1.0 / (1 + mkt_rate), 0.5);  // any leftover principal.....shouldnt be any
	
	return mkt_value;
	
}


float cashflow_mortgage(struct Mortgage *cm1, struct Date projDate)
{
	int periods = monthDiff(projDate, cm1->maturityDate);
	
	if (periods < 0)
	{
		return 0.0;
	}	
	
	
	float coupon_pmt = cm1->coupon / cm1->pmt_freq;
	
	cm1->prin *= powf(1.0 - cm1->default_rate, 1.0 / 12.0);
	
	float payment = cm1->prin / ((1.0 - powf( 1.0 / ( 1.0 + coupon_pmt) , cm1->amort_term)) / coupon_pmt);
	
	float interest = 0.0;
	float fees = 0.0;
	
	float cashflow = 0.0;
		

	if (periods % (12 / cm1->pmt_freq) == 0 && periods > 0)
	{
		cashflow = payment;
		interest = cm1->prin * coupon_pmt;
	}
	else if (periods == 0)
	{
		cashflow = cm1->prin * (1 + coupon_pmt);
		interest = cm1->prin * coupon_pmt;		
	}
	
	fees = cm1->service_fee / cm1->pmt_freq * cm1->prin;
	
	cm1->prin = cm1->prin - cashflow + interest;
	cm1->amort_term -= 1;
	
	return cashflow - fees;

}


void set_mktSpread_mortgage(struct Mortgage *cm1)
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
		cm1->mkt_spread = a;
		
		mkt_diff_a = cm1->mkt_value_input - market_value_mortgage(*cm1, _projDate);
			
		//using bisection method here
		//bisectionMethod(&mkt_spread, a, b, &itr);		
		cm1->mkt_spread = (a + b) / 2;
		
		mkt_diff_mid = cm1->mkt_value_input - market_value_mortgage(*cm1, _projDate);
		
		if (abs(mkt_diff_mid) < allerr)
		{
			break;
		}
		
		
		if ((mkt_diff_mid < 0.0 && mkt_diff_a < 0.0) || (mkt_diff_mid > 0.0 && mkt_diff_a > 0.0))
		{
			a = cm1->mkt_spread;
			b = b;
		}
		else
		{
			a = a;
			b = cm1->mkt_spread;
		}			
		itr++;		
	}	
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
	
	
	// read in current bonds
	struct Bond bonds[500];
	FILE* bond_input = fopen("C:\\work\\C\\assetmodel\\bonds.csv", "r");
	
	int bond_read = 0;
	int bond_records = 0;
	do
	{
		bond_read = fscanf(bond_input,
					  "%f,%f,%d,%d,%d,%d,%d,%f,%f,%f\n",
					  &bonds[bond_records].coupon,
					  &bonds[bond_records].prin,
					  &bonds[bond_records].pmt_freq,
					  &bonds[bond_records].maturityDate.year,
					  &bonds[bond_records].maturityDate.month,
					  &bonds[bond_records].callDate.year,
					  &bonds[bond_records].callDate.month,
					  &bonds[bond_records].mkt_spread,
					  &bonds[bond_records].mkt_value_input,
					  &bonds[bond_records].default_rate);
		if (bond_read == 10)
		{
			bond_records++;
		}
		
	} while (!feof(bond_input));
	
	fclose(bond_input);
	
	// read in commercial mortgages
	struct Mortgage comm_morts[100];
	FILE* cm_input = fopen("C:\\work\\C\\assetmodel\\commercial_mortgages.csv", "r");
	
	int cm_read = 0;
	int cm_records = 0;
	do
	{
		cm_read = fscanf(cm_input,
					  "%f,%f,%d,%d,%d,%f,%f,%f,%d,%f\n",
					  &comm_morts[cm_records].coupon,
					  &comm_morts[cm_records].prin,
					  &comm_morts[cm_records].pmt_freq,
					  &comm_morts[cm_records].maturityDate.year,
					  &comm_morts[cm_records].maturityDate.month,
					  &comm_morts[cm_records].mkt_spread,
					  &comm_morts[cm_records].mkt_value_input,
					  &comm_morts[cm_records].default_rate,
					  &comm_morts[cm_records].amort_term,
					  &comm_morts[cm_records].service_fee);
		if (cm_read == 10)
		{
			cm_records++;
		}
		
	} while (!feof(cm_input));
	
	fclose(cm_input);	
	
	FILE* fp = fopen("C:\\work\\C\\assetmodel\\scenarios\\rate_data.csv", "r");
	
	
	for (int r = 0; r < 10; r++)
	{
		for (int i = 0; i < 1000; i++)
		{
			int col = 0;
			
			while ((fscanf(fp, "%f", &rate_data[r][i][col++]) == 1) && col < 361)
			{
				fscanf(fp, ",");
			}
		}
	}
	
	fclose(fp);

	
	struct Date maturity_date = {30, 1, 2032};
	struct Date projection_date = {30, 6, 2022};
	


	float cashflow = 0.0;
	float mkt_value = 0.0;
	
	fp = fopen("C:\\work\\C\\assetmodel\\output\\cashflows.csv", "w+");
	FILE* fp2 = fopen("C:\\work\\C\\assetmodel\\output\\marketvalue.csv", "w+");
	
	clock_t begin = clock();
	
	
	set_rates(0,0);
	for (int k = 0; k < bond_records; k++)
	{
		set_mktSpread_bond(&bonds[k]);
	}
	
	for (int k = 0; k < cm_records; k++)
	{
		set_mktSpread_mortgage(&comm_morts[k]);
	}
	
	
	
	for (int i = 0; i < 1000; i++)
	{
		
		struct Date projection_date = {30, 6, 2022};
		struct Bond bonds_copy[500];			
		struct Mortgage cm_copy[100];
		
		for (int b = 0; b < bond_records; b++)
		{
			bonds_copy[b] = bonds[b];
			
		}	
		
		
		for (int b = 0; b < cm_records; b++)
		{
			cm_copy[b] = comm_morts[b];
		}
		
	
		for (int j = 0; j < 361; j++)
		{
			

			set_rates(i,j);
			
			float cashflow = 0.0;
			float mkt_value = 0.0;
			
			for (int b = 0; b < bond_records; b++)
			{
				mkt_value += market_value_bond(bonds_copy[b], projection_date );
				cashflow += cashflow_bond(&bonds_copy[b], projection_date);					
			}	
			
			
			for (int b = 0; b < cm_records; b++)
			{
				mkt_value += market_value_mortgage(cm_copy[b], projection_date );
				cashflow += cashflow_mortgage(&cm_copy[b], projection_date);	
			}
			
			if (projection_date.month == 12)
			{
				projection_date.month = 1;
				projection_date.year += 1;
			}
			else
			{
				projection_date.month += 1;
			}
			
			fprintf(fp, "%f,", cashflow);
			fprintf(fp2, "%f,", mkt_value);
		}
		
		fprintf(fp, "\n");
		fprintf(fp2, "\n");
		
		
	}
	
	fclose(fp);
	fclose(fp2);
	
	clock_t end = clock();
	
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	
	printf("%f\n", time_spent);
	

	
}




