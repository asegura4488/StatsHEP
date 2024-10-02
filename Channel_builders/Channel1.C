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

void SetHistoStyle(TH1D *H, string Xname, string Yname, int Id = 0){


	H->SetTitle("");

	// Axis
	H->GetXaxis()->SetTitle(Xname.c_str());
	H->GetXaxis()->SetTitleOffset(0.8);
	H->GetXaxis()->SetTitleSize(0.05);

	H->GetYaxis()->SetTitle(Yname.c_str());
	H->GetYaxis()->SetTitleOffset(1.0);
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

void Channel1(){

	SetTemplateStyle();

	TCanvas *c = new TCanvas("c","1 Channel", 200,200,800,800);
	//c->SetGrid();

	// Data
	TH1D *HData = new TH1D("HData","HData",3,0.,3.);
	SetHistoStyle(HData,"m(#rho)[GeV]","Events",0);
	HData->SetBinContent(2,105.0);
	HData->Sumw2();


	// Background
	TH1D *HBkg = new TH1D("HBkg","HBkg",3,0.,3.);
	SetHistoStyle(HBkg,"m(#rho)[GeV]","Events",1);
	HBkg->SetBinContent(2,100.0);
	HBkg->Sumw2();
	//Limits
	SetHistoLimits(HBkg,0.,3.,0.,160.0);

	// Signal 
	TH1D *HSignal = new TH1D("HSignal","HSignal",3,0.,3.);
	SetHistoStyle(HSignal,"m(#rho)[GeV]","Events",2);
	HSignal->SetBinContent(2,100.+10.);
	HSignal->Sumw2();
	HSignal->SetBinError(2,0.02);


	// Drawing
	HBkg->Draw("Hist");
	HSignal->Draw("e1psame");
	HData->Draw("e1psame");

	// Re drawing x-axis
	gPad->RedrawAxis();


	TLegend *leg = new TLegend(0.414787,0.6829897,0.8934837,0.8956186,NULL,"brNDC");
	leg->SetHeader("1-Channel","C");
	leg->AddEntry(HBkg,"Background","f");
	leg->AddEntry(HSignal,"Background + Signal","l");
	leg->AddEntry(HData,"Data","p");
	leg->Draw();

     c->SaveAs("1ToyChannel.pdf");

}

int main(){

	
	
	Channel1();
	return 0;
}
