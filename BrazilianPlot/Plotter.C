#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <TCanvas.h>
#include <TAxis.h>
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

void SetLimits(TGraphAsymmErrors *g, double xmin, double xmax, double ymin, double ymax){
	g->GetXaxis()->SetRangeUser(xmin,xmax);
	g->SetMinimum(ymin);
	g->SetMaximum(ymax);
}

void SetStyle(TGraphAsymmErrors *g, std::string Xname, std::string Yname, int Id = 0){


	g->SetTitle("");

	// Axis
	g->GetXaxis()->SetTitle(Xname.c_str());
	g->GetXaxis()->SetTitleOffset(0.9);
	g->GetXaxis()->SetTitleSize(0.05);

	g->GetYaxis()->SetTitle(Yname.c_str());
	g->GetYaxis()->SetTitleOffset(1.2);
	g->GetYaxis()->SetTitleSize(0.05);

	//Labels
	g->GetXaxis()->SetLabelOffset(0.01);
	g->GetXaxis()->SetLabelSize(0.04);
	g->GetXaxis()->SetLabelColor(1);

	g->GetYaxis()->SetLabelOffset(0.01);
	g->GetYaxis()->SetLabelSize(0.04);
	g->GetYaxis()->SetLabelColor(1);

	// Ticks
	g->GetXaxis()->SetTickLength(0.04);
	g->GetYaxis()->SetTickLength(0.04);
	
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

	TCanvas *c = new TCanvas("c","Upper Limit scan", 200,200,1000,500);
	//c->SetGrid();

	// Solo importan los errores en Y
	TGraphAsymmErrors *ExpectedGraph1 = new TGraphAsymmErrors(*Mass,*Expected,*Expected,*Expected,*Msigma1,*Sigma1);
	TGraphAsymmErrors *ExpectedGraph2 = new TGraphAsymmErrors(*Mass,*Expected,*Expected,*Expected,*Msigma2,*Sigma2);
	
	TGraph *ExpectedGraph = new TGraph(*Mass,*Expected);
	TGraph *ObservedGraph = new TGraph(*Mass,*Observed);
	
	
	ExpectedGraph1->SetLineStyle(2); // Dash line        
        ExpectedGraph2->SetLineStyle(2); // Dash line
        ExpectedGraph->SetLineStyle(2);  // Dash line     
        
        ExpectedGraph->SetLineWidth(3);
        ObservedGraph->SetLineWidth(3);

	ExpectedGraph1->SetFillColor(kGreen);	
	ExpectedGraph2->SetFillColor(kYellow);

        
        SetLimits(ExpectedGraph1,100.,160.,0.,1.6);
        SetLimits(ExpectedGraph2,100.,160.,0.,1.6);

        SetStyle(ExpectedGraph2,"m(H)[GeV]","95% CL limits on #mu");

 
        ExpectedGraph2->Draw("APL31");
	ExpectedGraph1->Draw("same31");
        ExpectedGraph->Draw("same"); 
        ObservedGraph->Draw("same");


	// Re drawing x-axis
	gPad->RedrawAxis();

	/*
	Podria Servir
	TAxis *eje = ExpectedGraph2->GetXaxis();
	eje->SetRangeUser(100,160);
        */

	TLegend *leg = new TLegend(0.6357955,0.6823394,0.8960227,0.8944954,NULL,"brNDC");
	//leg->SetHeader("Upper limits for #mu","C");
	leg->AddEntry(ObservedGraph,"Observed","l");
	leg->AddEntry(ExpectedGraph,"Expected","l");
	leg->AddEntry(ExpectedGraph1,"Expected #pm 1#sigma","lf");
	leg->AddEntry(ExpectedGraph2,"Expected #pm 2#sigma","lf");
	leg->Draw();

	c->Modified();
	c->SaveAs("UpperLimitMass.pdf");

}

int main(){
	Plotter();
	return 0;
}
