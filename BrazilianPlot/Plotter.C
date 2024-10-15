#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <TCanvas.h>
#include <TLegend.h>
#include <TH1D.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TVectorD.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>



void SetTemplateStyle(){


	gStyle->SetPadTopMargin(0.1);
	gStyle->SetPadBottomMargin(0.1);
	gStyle->SetPadLeftMargin(0.13);
	gStyle->SetPadRightMargin(0.1);

   	 // Stats and title
    	gStyle->SetOptStat(0);
	gStyle->SetOptTitle(0);

	// Log-scale
	gStyle->SetOptLogx(0);
	gStyle->SetOptLogy(0);

	gROOT->ForceStyle();

}

void SetHistoLimits(TH1D *H, double xmin, double xmax, double ymin, double ymax){
	H->GetXaxis()->SetLimits(xmin,xmax);
	H->SetMinimum(ymin);
	H->SetMaximum(ymax);
}

void SetHistoStyle(TH1D *H, std::string Xname, std::string Yname, int Id = 0){


	H->SetTitle("");

	// Axis
	H->GetXaxis()->SetTitle(Xname.c_str());
	H->GetXaxis()->SetTitleOffset(0.9);
	H->GetXaxis()->SetTitleSize(0.05);

	H->GetYaxis()->SetTitle(Yname.c_str());
	H->GetYaxis()->SetTitleOffset(1.2);
	H->GetYaxis()->SetTitleSize(0.05);

	//Labels
	H->GetXaxis()->SetLabelOffset(0.01);
	H->GetXaxis()->SetLabelSize(0.04);
	H->GetXaxis()->SetLabelColor(1);

	H->GetYaxis()->SetLabelOffset(0.01);
	H->GetYaxis()->SetLabelSize(0.04);
	H->GetYaxis()->SetLabelColor(1);

	// Ticks
	H->GetXaxis()->SetTickLength(0.04);
	H->GetYaxis()->SetTickLength(0.04);

	if (Id == 0){
		H->SetLineColor(kBlack);
		H->SetMarkerColor(kBlack);
		H->SetMarkerStyle(20);
		H->SetMarkerSize(1.3);
	}
	else if (Id == 1){
		H->SetFillStyle(3002);
		H->SetFillColor(kBlue);
		H->SetLineColor(kBlue);
	}
	else if (Id == 2){
		//H->SetFillStyle(3002);
		//H->SetFillColor(kRed);

		H->SetLineColor(kRed);
		H->SetLineStyle(7);
		H->SetLineWidth(2);
	}
	
}


void Plotter(){

	SetTemplateStyle();

	// Rutina de lectura de datos
	std::vector<double> mass, msigma2, msigma1, expected, sigma1, sigma2, observed;

	std::ifstream infile("UpperLimitMassScan.csv"); // Archivo de entrada
	if (!infile.is_open()){
		std::cerr << " Error al abrir el archivo de datos " << std::endl;
		return; 
	}

	std::string line;
	char comma;

	// Leer el archivo linea por linea saltando encabezado
	bool first_line = true;
	while (std::getline(infile,line)){


		if (first_line){
			first_line = false; 
			continue;
		}


		// Variables para almacenar temporalemente n,b,s
		double mass_,msigma2_,msigma1_,expected_,sigma1_,sigma2_,observed_;

		// Crear un stream a partir de la linea para leer n,b,s

		std::istringstream ss(line);
		if (!(ss >> mass_ >> comma >> msigma2_ >> comma >> msigma1_ >> comma >> expected_ >> comma >> sigma1_ >> comma >> sigma2_ >>  comma >> observed_)){
			std::cerr << "Error al leer los valores en la linea: " << line << std::endl;
			continue;
		}

		mass.push_back(mass_);
		msigma2.push_back(msigma2_);
		msigma1.push_back(msigma1_);
		expected.push_back(expected_);
		sigma1.push_back(sigma1_);
		sigma2.push_back(sigma2_);
		observed.push_back(observed_);

	}

	infile.close(); // Cerrar archivo de datos

	Int_t N = mass.size();

	// Dynamic Arrays
	Double_t *Mass_ = new Double_t[N];
	Double_t *Expected_ = new Double_t[N];
	Double_t *Observed_ = new Double_t[N];
	
	
	Double_t *Msigma1_ = new Double_t[N];
	Double_t *Sigma1_ = new Double_t[N];
	Double_t *Msigma2_ = new Double_t[N];
	Double_t *Sigma2_ = new Double_t[N];
	
	
	// Filling Array
	for (size_t i = 0; i < mass.size(); ++i){
	     Mass_[i] = mass[i];
	     Expected_[i] = expected[i];
	     Observed_[i] = observed[i];
	     
	     Msigma1_[i] = msigma1[i];
	     Sigma1_[i] = sigma1[i];
	     
	     Msigma2_[i] = msigma2[i];
	     Sigma2_[i] = sigma2[i];
	     //std::cout << mass[i] << " " << observed[i] << std::endl;
	}
	// TVectorD for plotting
	TVectorD *Mass = new TVectorD();
	Mass->Use(N,Mass_);
	TVectorD *Expected = new TVectorD();
	Expected->Use(N,Expected_);
	TVectorD *Observed = new TVectorD();
	Observed->Use(N,Observed_);
	
	TVectorD *Msigma1 = new TVectorD();
	Msigma1->Use(N,Msigma1_);
	TVectorD *Sigma1 = new TVectorD();
	Sigma1->Use(N,Sigma1_);
	
	
	TVectorD *Msigma2 = new TVectorD();
	Msigma2->Use(N,Msigma2_);
	TVectorD *Sigma2 = new TVectorD();
	Sigma2->Use(N,Sigma2_);
	

	// Rutina para hacer es histograma

	TCanvas *c = new TCanvas("c","Upper Limit scan", 200,200,800,800);
	//c->SetGrid();

	
	TGraphAsymmErrors *ExpectedGraph1 = new TGraphAsymmErrors(*Mass,*Expected,*Expected,*Expected,*Msigma1,*Sigma1);
	TGraphAsymmErrors *ExpectedGraph2 = new TGraphAsymmErrors(*Mass,*Expected,*Expected,*Expected,*Msigma2,*Sigma2);
	
	TGraph *ExpectedGraph = new TGraph(*Mass,*Expected);
	TGraph *ObservedGraph = new TGraph(*Mass,*Observed);
	
	ExpectedGraph1->SetFillColor(kGreen);	
	ExpectedGraph2->SetFillColor(kYellow);
	

	ExpectedGraph1->SetLineStyle(2); // Dash line        
        ExpectedGraph2->SetLineStyle(2); // Dash line
        ExpectedGraph->SetLineStyle(2);       
        
        ExpectedGraph->SetLineWidth(3);
        ObservedGraph->SetLineWidth(3);
        
        ExpectedGraph2->GetXaxis()->SetLimits(100.,160.);
        ExpectedGraph1->GetXaxis()->SetLimits(100.,160.);
        ExpectedGraph->GetXaxis()->SetLimits(100.,160.);
        ObservedGraph->GetXaxis()->SetLimits(100.,160.);

       
        ExpectedGraph2->Draw("APL31");
	ExpectedGraph1->Draw("same31");
        ExpectedGraph->Draw("same"); 
        ObservedGraph->Draw("same");


	// Re drawing x-axis
	gPad->RedrawAxis();


	TLegend *leg = new TLegend(0.7397727,0.6823394,0.8954545,0.8944954,NULL,"brNDC");
	leg->SetHeader("Upper limits","C");
	leg->AddEntry(ObservedGraph,"Observed","l");
	leg->AddEntry(ExpectedGraph,"Expected","l");
	leg->AddEntry(ExpectedGraph1,"1#sigma","Cf");
	leg->AddEntry(ExpectedGraph2,"2#sigma","Cf");
	leg->Draw();

	 c->SaveAs("UpperLimitMass.pdf");

}

int main(){
	Plotter();
	return 0;
}
