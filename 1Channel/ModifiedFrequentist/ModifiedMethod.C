#include<stdio.h> 
#include<iostream>
#include<TMath.h>
#include<TCanvas.h>
#include<TGraph.h> 
#include<TAxis.h>

// Channels
double n = 0.;
double b = 0.;
double s = 1.;


double GetCLs(double mu, double n, double b, double s){

  double CLb  =  TMath::Prob(2.*b,2*(1+n));
  double CLsb =  TMath::Prob(2.*(mu*s+b),2*(1+n));

  return CLsb/CLb;

}

double GetUpperLimit(int N, double *Mu, double *pvalue, double alpha=0.05){

	int index = 0;
	bool found = false;

	double upper = 0.;

	for ( int i = 0; i < N; i++ ){

		if ((pvalue[i] < alpha) && (!found)){
			index = i;
			std::cout << " Index found: " << index  << std::endl;
			break;  
		}

	}

	//1D interpolation
	
	double x0 = Mu[index-1];
	double x1 = Mu[index];
	double fx0 = pvalue[index-1];
	double fx1 = pvalue[index];

	// Slope
	double m = (fx1-fx0)/(x1-x0);

	if (m==0){
		std::cout << " - No calculable en ese intervalo - " << std::endl;
		return 0;
	}

	upper = (alpha - fx0)/m + x0;

	return upper;
}

void ModifiedMethod(double alpha=0.05){


	double b = 8.;
	double a = 0.;
	int N = 20; // Number of points

	double *Mu = new double[N];
	double *pvalue = new double[N];
	double *Critical = new double[N];

	double Dmu = (b - a)/(N-1);
	double mu = 0.;

	for (int i = 0; i < N; i++){
		Mu[i] = mu;
		pvalue[i] = GetCLs(mu,n,b,s);
		Critical[i] = alpha;
		mu += Dmu;
		//std::cout << Mu[i] << " " << pvalue[i] << std::endl;
	}

	double UpperLimit = GetUpperLimit(N,Mu,pvalue,alpha);
	std::cout << "The upper limit is: " << UpperLimit << std::endl;

	TCanvas *c = new TCanvas("c","Canvas",200,200,800,800);
	TGraph *gr1 = new TGraph(N,Mu,pvalue);
	TGraph *gr2 = new TGraph(N,Mu,Critical);


	gr1->SetTitle("Modified frequentist pvalue scan");
 	gr1->SetLineColor(1);
  	gr1->SetLineWidth(3);

  	gr1->GetXaxis()->SetTitle("#mu");
  	gr1->GetXaxis()->SetTitleSize(0.06);
  	gr1->GetXaxis()->SetTitleOffset(0.7);
  
  	gr1->GetYaxis()->SetTitle("p-value");
  	gr1->GetYaxis()->SetTitleSize(0.06);
  	gr1->GetYaxis()->SetTitleOffset(0.7);

  	gr2->SetLineColor(2);
  	gr2->SetLineWidth(3);

	gr1->Draw("AL*");
	gr2->Draw("same");

	c->SaveAs("ModifiedUpperLimit.pdf");

}


int main(){

	
	ModifiedMethod();
	

	return 0;
}