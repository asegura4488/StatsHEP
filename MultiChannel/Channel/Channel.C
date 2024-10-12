#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <TCanvas.h>
#include <TLegend.h>
#include <TH1D.h>
#include <TStyle.h>
#include <TROOT.h>



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


void Channel(){

	SetTemplateStyle();

	// Rutina de lectura de datos
	std::vector<double> n_values, b_values, s_values;

	std::ifstream infile("Data/NData_NBkg_Signal124.0.csv"); // Archivo de entrada
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
		double n,b,s;

		// Crear un stream a partir de la linea para leer n,b,s

		std::istringstream ss(line);
		if (!(ss >> n >> comma >> b >> comma >> s)){
			std::cerr << "Error al leer los valores en la linea: " << line << std::endl;
			continue;
		}

		n_values.push_back(n);
		b_values.push_back(b);
		s_values.push_back(s);

	}

	infile.close(); // Cerrar archivo de datos


	// Rutina para hacer es histograma

	TCanvas *c = new TCanvas("c","1 Channel", 200,200,800,800);
	//c->SetGrid();

	double xmin = 100.;
	double xmax = 160.;
	double ymin = 0.;
	double ymax = 400;


	// Data
	TH1D *HData = new TH1D("HData","HData",n_values.size(),xmin,xmax);
	SetHistoStyle(HData,"m(H)[GeV]","Events",0);
	
	// Background
	TH1D *HBkg = new TH1D("HBkg","HBkg",b_values.size(),xmin,xmax);
	SetHistoStyle(HBkg,"m(H)[GeV]","Events",1);
	

	TH1D *HSignal = new TH1D("HSignal","HSignal",s_values.size(),xmin,xmax);
	SetHistoStyle(HSignal,"m(H)[GeV]","Events",2);
	

	// Filling channels
	for (size_t i = 0; i < n_values.size(); ++i){

		HData->SetBinContent(i+1,n_values[i]); // Llenar datos n
		HBkg->SetBinContent(i+1,b_values[i]); // Llenar datos b
		HSignal->SetBinContent(i+1,s_values[i]);

		HBkg->SetBinError(i+1,1.);
		HSignal->SetBinError(i+1,1);

	}

	//HData->Sumw2();
	//HBkg->Sumw2();
	//HSignal->Sumw2();
		
	//for (size_t i = 0; i < n_values.size(); ++i){
	//	std::cout << HSignal->GetBinError(i+1) << std::endl;
	//}


	SetHistoLimits(HBkg,xmin,xmax,ymin,ymax);
	SetHistoLimits(HSignal,xmin,xmax,ymin,ymax);
	SetHistoLimits(HData,xmin,xmax,ymin,ymax);

	// Crear un nuevo histograma que sea la suma de los dos
	TH1D *HSignalBkg = (TH1D*)HSignal->Clone("HSignalBkg");
	HSignalBkg->Add(HBkg);
	HSignalBkg->SetTitle("Signal + Background");
	SetHistoStyle(HSignalBkg,"m(H)[GeV]","Events",3);


	// Drawing histos
	HBkg->Draw("Hist");
	HSignalBkg->Draw("e1psame");
	//HSignal->Draw("e1psame");
	HData->Draw("e1psame");

	// Re drawing x-axis
	gPad->RedrawAxis();


	TLegend *leg = new TLegend(0.414787,0.6829897,0.8934837,0.8956186,NULL,"brNDC");
	leg->SetHeader("Invariant mass","C");
	leg->AddEntry(HBkg,"Background","f");
	leg->AddEntry(HSignal,"Signal + background","l");
	leg->AddEntry(HData,"Data","p");
	leg->Draw();

    c->SaveAs("MultiChannel.pdf");

}

int main(){
	Channel();
	return 0;
}
