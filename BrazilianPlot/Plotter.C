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
	g->GetYaxis()->SetTitleOffset(1.1);
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
	std::vector<double> Vmass, Vmsigma2, Vmsigma1, Vexpected, Vsigma1, Vsigma2, Vobserved;

	std::ifstream infile("UpperLimitMassScanqm.csv"); // Archivo de entrada
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


		// Variables para almacenar temporalemente la informacion
		double mass_,msigma2_,msigma1_,expected_,sigma1_,sigma2_,observed_;

		// Crear un stream a partir de la linea para leer n,b,s

		std::istringstream ss(line);
		if (!(ss >> mass_ >> comma >> msigma2_ >> comma >> msigma1_ >> comma >> expected_ >> comma >> sigma1_ >> comma >> sigma2_ >>  comma >> observed_)){
			std::cerr << "Error al leer los valores en la linea: " << line << std::endl;
			continue;
		}

		Vmass.push_back(mass_);
		Vmsigma2.push_back(msigma2_);
		Vmsigma1.push_back(msigma1_);
		Vexpected.push_back(expected_);
		Vsigma1.push_back(sigma1_);
		Vsigma2.push_back(sigma2_);
		Vobserved.push_back(observed_);

	}

	infile.close(); // Cerrar archivo de datos


	// Tamaño del vector
	Int_t N = Vmass.size();

	// Dynamic Arrays
	Double_t *DMass = new Double_t[N];
	Double_t *DMsigma2 = new Double_t[N];
	Double_t *DMsigma1 = new Double_t[N];
	Double_t *DExpected = new Double_t[N];
	Double_t *DSigma1 = new Double_t[N];
	Double_t *DSigma2 = new Double_t[N];
	Double_t *DObserved = new Double_t[N];
	
	
	// Filling Dynamic Arrays
	for (size_t i = 0; i < N; ++i){
	     DMass[i] = Vmass[i];
	     DMsigma2[i] = Vmsigma2[i];
	     DMsigma1[i] = Vmsigma1[i];
	     DExpected[i] = Vexpected[i];
	     DSigma1[i] = Vsigma1[i];
	     DSigma2[i] = Vsigma2[i];
	     DObserved[i] = Vobserved[i];
	}

	// TVectorD for plotting
	TVectorD *Mass = new TVectorD();
	Mass->Use(N,DMass);

	TVectorD *Msigma2 = new TVectorD();
	Msigma2->Use(N,DMsigma2);
	TVectorD *Msigma1 = new TVectorD();
	Msigma1->Use(N,DMsigma1);

	TVectorD *Expected = new TVectorD();
	Expected->Use(N,DExpected);

	TVectorD *Sigma1 = new TVectorD();
	Sigma1->Use(N,DSigma1);
	
	TVectorD *Sigma2 = new TVectorD();
	Sigma2->Use(N,DSigma2);
	
	TVectorD *Observed = new TVectorD();
	Observed->Use(N,DObserved);
	
	

	// Rutina para hacer es histograma

	TCanvas *c = new TCanvas("c","Upper Limit scan", 200,200,900,500);
	//c->SetGrid();

	// Solo importan los errores en Y

	TGraphAsymmErrors *Expected1Sigma = new TGraphAsymmErrors(*Mass,*Expected,*Expected,*Expected,*Msigma1,*Sigma1);
	TGraphAsymmErrors *Expected2Sigma = new TGraphAsymmErrors(*Mass,*Expected,*Expected,*Expected,*Msigma2,*Sigma2);
	
	TGraph *ExpectedGraph = new TGraph(*Mass,*Expected);
	TGraph *ObservedGraph = new TGraph(*Mass,*Observed);
	
	
	Expected1Sigma->SetLineStyle(2); // Dash line        
    Expected2Sigma->SetLineStyle(2); // Dash line
    ExpectedGraph->SetLineStyle(2);  // Dash line     
        
    ExpectedGraph->SetLineWidth(3);
    ObservedGraph->SetLineWidth(3);

	Expected1Sigma->SetFillColor(kGreen);	
	Expected2Sigma->SetFillColor(kYellow);

        
    SetLimits(Expected1Sigma,100.,160.,0.,1.6);
    SetLimits(Expected2Sigma,100.,160.,0.,1.6);

    SetStyle(Expected2Sigma,"m(H)[GeV]","95% CL limits on #mu");

 
 	// plotting
    Expected2Sigma->Draw("APL31");
	Expected1Sigma->Draw("same31");
    ExpectedGraph->Draw("same"); 
    ObservedGraph->Draw("same");


	// Re drawing x-axis
	gPad->RedrawAxis();

	/*
	Podria Servir para manipular el TAxis
	TAxis *eje = ExpectedGraph2->GetXaxis();
	eje->SetRangeUser(100,160);
     */

	TLegend *leg = new TLegend(0.6357955,0.6823394,0.8960227,0.8944954,NULL,"brNDC");
	//leg->SetHeader("Upper limits for #mu","C");
	leg->AddEntry(ObservedGraph,"Observed","l");
	leg->AddEntry(ExpectedGraph,"Expected","l");
	leg->AddEntry(Expected1Sigma,"Expected #pm 1#sigma","lf");
	leg->AddEntry(Expected2Sigma,"Expected #pm 2#sigma","lf");
	leg->Draw();

	c->Modified();
	c->SaveAs("UpperLimitMass.pdf");

}

int main(){
	Plotter();
	return 0;
}
