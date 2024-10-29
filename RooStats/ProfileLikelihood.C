void ProfileLikelihood(bool is_inverter = true){


  TFile *file = TFile::Open("Workspace.root");


  RooWorkspace *w = (RooWorkspace *)file->Get("combined");
   if (!w) {
      cout << "workspace not found" << endl;
     // return -1.0;
   }

   RooAbsData *data = w->data("obsData");
   RooStats::ModelConfig *mc = (RooStats::ModelConfig *)w->obj("ModelConfig");

   if (!data || !mc) {
      w->Print();
      cout << "data or ModelConfig was not found" << endl;
     // return -1.0;
   }

   // ajustar el POI para la hipothesis alternativa
   std::cout << " POI value: " << w->var("mu")->getVal() << endl; 
   w->var("mu")->setVal(1.);
   
   RooRealVar* POI1 = (RooRealVar*) mc->GetParametersOfInterest()->first();
   mc->SetSnapshot( *POI1  );
   // ajustar el POI para la hipothesis null
   RooStats::ModelConfig* bModel = (RooStats::ModelConfig*) mc->Clone("BModel");
   RooRealVar* POI = (RooRealVar*) bModel->GetParametersOfInterest()->first();
   POI->setVal(0.);
   bModel->SetSnapshot( *POI  );
   

   // Profile Estimator
   RooStats::ProfileLikelihoodTestStat *ProfileLike = new RooStats::ProfileLikelihoodTestStat(*mc->GetPdf());
   // for exclusion
   ProfileLike->SetOneSided(true);
   // Only for discovery
   // ProfileLike->SetOneSidedDiscovery(true);
   ProfileLike->SetVarName("q_{0}/2");


   RooStats::FrequentistCalculator *freqCalc = new RooStats::FrequentistCalculator(*data, *bModel, *mc);
   freqCalc->SetToys(2000, 2000);

   RooStats::ToyMCSampler *ToyMC = (RooStats::ToyMCSampler*)freqCalc->GetTestStatSampler();
   ToyMC->SetTestStatistic(ProfileLike);

   if (!mc->GetPdf()->canBeExtended()) {
    ToyMC->SetNEventsPerToy(1);
   }
/*
   // Correr toys en paralelo
   RooStats::ProofConfig *pc = NULL; 
   pc = new RooStats::ProofConfig(*w, 6, "workers=6", kFALSE);

   if (pc)
       ToyMC->SetProofConfig(pc);
*/


   RooStats::HypoTestResult *Result = freqCalc->GetHypoTest();
   Result->GetNullDistribution()->SetTitle("b only");
   Result->GetAltDistribution()->SetTitle("s+b");
   Result->Print();
   double NullPvalue = Result->NullPValue();
   double AltPvalue = Result->AlternatePValue();
   std::cout << " Hypo - Null Pvalue: " << NullPvalue << " Significance: " << TMath::NormQuantile(1.- NullPvalue) << std::endl;
   std::cout << " Hypo - Alt Pvalue: " << AltPvalue << " Significance: " << TMath::NormQuantile(1.- AltPvalue) << std::endl;

   TCanvas *c1 = new TCanvas("c1","c1");
   RooStats::HypoTestPlot *plot = new RooStats::HypoTestPlot(*Result, 100, -1.0, 15.);
  
   int nPOI = 1;
   TF1 *f = new TF1("f", TString::Format("1*ROOT::Math::chisquared_pdf(2*x,%d,0)", nPOI), 0, 20);
   f->SetLineColor(kBlack);
   f->SetLineStyle(7);
   plot->AddTF1(f, TString::Format("#chi^{2}(2x,%d)", nPOI));
   plot->SetLogYaxis(true);
   plot->Draw();
   c1->SaveAs("Dist_ProfileLike.pdf");

   if(is_inverter){

   RooStats::HypoTestInverter *Inverter = new RooStats::HypoTestInverter(*freqCalc);  
   Inverter->SetConfidenceLevel(0.95);
   Inverter->UseCLs(true);
   Inverter->SetVerbose(true); // don't show details
   Inverter->SetFixedScan(5,0.1,2.0); // POI scan

   RooStats::HypoTestInverterResult *IResult =  Inverter->GetInterval();
   RooStats::HypoTestInverterPlot* IPlot = new RooStats::HypoTestInverterPlot("IPlot","Pvalue Scan",IResult);

    TCanvas *c2 = new TCanvas("c2","c2");
    c2->SetLogy(false);
    IPlot->Draw("CLb 2CL");   

    c2->SaveAs("UpperLimit_ProfileLike.pdf");

    double ExpectedLimit = IResult->GetExpectedUpperLimit(0);
    double ObsLimit = IResult->UpperLimit();
    std::cout << " Expected Limit: " << ExpectedLimit << std::endl;
    std::cout << " Observed Limit: " << ObsLimit << std::endl;


   }

}
