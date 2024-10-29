void AsymtoticCalculator(bool is_inverter = true){

  TFile *file = TFile::Open("Workspace.root");


  RooWorkspace *w = (RooWorkspace *)file->Get("combined");
   if (!w) {
      cout << "workspace not found" << endl;
   }

   RooAbsData *data = w->data("obsData");
   data->Print("v");

   RooStats::ModelConfig *mc = (RooStats::ModelConfig*)w->obj("ModelConfig");

   if (!data || !mc) {
      w->Print();
      cout << "data or ModelConfig was not found" << endl;
   }

   // POI alternative hypo
   std::cout << " Alt-POI value: " << w->var("mu")->getVal() << endl;  
   w->var("mu")->setVal(1.0);

   RooRealVar *POI1 = (RooRealVar*) mc->GetParametersOfInterest()->first();
   mc->SetSnapshot( *POI1 );
   // Null-Hypo
   RooStats::ModelConfig *bModel = (RooStats::ModelConfig*) mc->Clone("BModel");
   RooRealVar *POI = (RooRealVar*) bModel->GetParametersOfInterest()->first();
   POI->setVal(0.);
   std::cout << " Null-POI value: " << POI->getVal() << endl; 
   bModel->SetSnapshot( *POI );


   // Asymptotic
   RooStats::AsymptoticCalculator *MyAsymptotic = new RooStats::AsymptoticCalculator(*data, *bModel, *mc);
   MyAsymptotic->SetOneSided(true);

   // Hypo-result
   RooStats::HypoTestResult *Result = MyAsymptotic->GetHypoTest(); 
   Result->Print();

    // Results for the HypoTest
   double NullPvalue = Result->NullPValue();
   double AltPvalue = Result->AlternatePValue();
   double Significance = Result->Significance();
   double CLs = Result->CLs();
   double CLsplusb = Result->CLsplusb();
   double CLb = Result->CLb();

   // Recordar que la hypothetsis alternativa es b-only hypo
   std::cout << " Hypo - Null Pvalue: " << NullPvalue << " Significance: " << TMath::NormQuantile(1.- NullPvalue) << std::endl;
   std::cout << " Hypo - Alt Pvalue: " << AltPvalue << " Significance: " << TMath::NormQuantile(1.- AltPvalue) << std::endl;
   std::cout << " Hypo - Significance: " << Significance << std::endl;
   std::cout << " Hypo - CLs: " << CLs << std::endl;
   std::cout << " Hypo - CLsb: " << CLsplusb << std::endl;
   std::cout << " Hypo - CLb: " << CLb << std::endl;
   std::cout << "Significance" << TMath::NormQuantile(CLb) << std::endl;

   if(is_inverter){

	// Inverter 
   RooStats::HypoTestInverter *Inverter = new RooStats::HypoTestInverter(*MyAsymptotic);  
   Inverter->SetConfidenceLevel(0.95);
    Inverter->UseCLs(true);
    Inverter->SetVerbose(true); // don't show details
    Inverter->SetFixedScan(20,0.,2.); // POI scan

    RooStats::HypoTestInverterResult *IResult =  Inverter->GetInterval();
    RooStats::HypoTestInverterPlot* IPlot = new RooStats::HypoTestInverterPlot("IPlot","Pvalue Scan",IResult);
    
    TCanvas *c1 = new TCanvas();

    IPlot->Draw("CLb 2CL"); 	  
    
    c1->SaveAs("UpperLimit_Asymptotic.pdf");

    double ExpectedLimit = IResult->GetExpectedUpperLimit(0);
    double ObsLimit = IResult->UpperLimit();
    // El indice representa el valor en ese punto mirar para mu=1
    double Clsplusb = IResult->CLsplusb(10);
    double Clb = IResult->CLb(10);
    double Cls = IResult->CLs(10);
    std::cout << " Clb " << Clb << std::endl;
    std::cout << " Clsb " << Clsplusb << std::endl;
    std::cout << " Cls " << Cls << std::endl;
    // Pvaluo
    double p0 = 1 - Clb;
    double p1 = Clsplusb;
   	// Este camino so sirve para describir la significanca estadisitca. 
   	// Se necesita ajusta el numero de observaciones a s+b para que todo cuadre
    std::cout << " Pvalue H0 " << p0 << std::endl; 
    std::cout << " Pvalue H1 " << p1 << std::endl; 
    std::cout << " Significance of signal H0 " << -TMath::NormQuantile(p0) << std::endl; 	
    
    std::cout << " Significance of signal H1 " << TMath::NormQuantile(p1) << std::endl; 
    std::cout << " Expected Limit: " << ExpectedLimit << std::endl;
    std::cout << " Observed Limit: " << ObsLimit << std::endl;

    // Si mu = 1, podemos calcular la significancia

   }

  
}

int main(){

	AsymtoticCalculator();
	return 0;
}
