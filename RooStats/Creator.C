void Creator(bool Toy = true){

   TH1D *HData;
   TH1D *HBkg;  
   TH1D *HSignal;

   if (Toy){

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


   double xmin = 100.;
   double xmax = 160.;
   double ymin = 0.;
   double ymax = 300;


 
   HData = new TH1D("HData","HData",n_values.size(),xmin,xmax);
   HBkg = new TH1D("HBkg","HBkg",b_values.size(),xmin,xmax);
   HSignal = new TH1D("HSignal","HSignal",s_values.size(),xmin,xmax);
 
   // Filling channels
	for (size_t i = 0; i < n_values.size(); ++i){

	//	HData->SetBinContent(i+1,int(b_values[i]+s_values[i])); // Llenar para significancia n = s+b
		HData->SetBinContent(i+1,int(n_values[i])); // Llenar datos n
		HBkg->SetBinContent(i+1,int(b_values[i])); // Llenar datos b
		HSignal->SetBinContent(i+1,int(s_values[i]));

		HBkg->SetBinError(i+1,1.);
		HSignal->SetBinError(i+1,1);

   }


   HData->Sumw2();
   HBkg->Sumw2();
   HSignal->Sumw2();
 

   TFile *OutFile = new TFile("Channel1.root","RECREATE");
   TDirectory *Directory = OutFile->mkdir("Channel1"); 
   Directory->cd();  

   HData->Write();
   HBkg->Write();
   HSignal->Write();
   OutFile->cd();
   OutFile->Close(); 

   }
   else{
     // Higgs toy model
     TFile *File = TFile::Open("HistoHiggs.root","READ");    
     HData = (TH1D*)File->Get("HData");
     
    // HData = (TH1D*)File->Get("HBkg"); // for significance
    
     HBkg = (TH1D*)File->Get("HBkg");
     HSignal = (TH1D*)File->Get("HSignal");

   }

 
   // WorkSpace

   RooStats::HistFactory::Measurement *Measure = new RooStats::HistFactory::Measurement("Measure","Measure");
   Measure->SetOutputFilePrefix( "Measure" );
   Measure->SetPOI("mu");	
   Measure->SetLumi(1.0);
  // Measure->SetLumiRelErr(0.1);
   Measure->AddConstantParam("Lumi");

   RooStats::HistFactory::Data *Data = new RooStats::HistFactory::Data();
   Data->SetHisto(HData);

   RooStats::HistFactory::Sample *Bkg = new RooStats::HistFactory::Sample("Bkg");
   Bkg->SetHisto(HBkg);
   Bkg->AddOverallSys("BkgSys", 0.8, 1.2); 

   RooStats::HistFactory::Sample *Signal = new RooStats::HistFactory::Sample("Signal");
   Signal->AddNormFactor("mu",1.,0.,10.);
 //  Signal->AddOverallSys("MuSys", 0.99, 1.01);
   Signal->SetHisto(HSignal);


    RooStats::HistFactory::Channel *Channel = new RooStats::HistFactory::Channel("Channel1");
    Channel->SetData(*Data) ;   // Add the DATA to the channel
    Channel->AddSample(*Bkg) ;    // Add the BACKGROUND to the channel
    Channel->AddSample(*Signal) ; // Add the SIGNAL to the channel
    //Channel->SetStatErrorConfig(0.01, "Poisson");
    Measure->AddChannel(*Channel);

    RooStats::HistFactory::HistoToWorkspaceFactoryFast *HtoW 
    = new RooStats::HistFactory::HistoToWorkspaceFactoryFast(*Measure);
    
    RooWorkspace *w = HtoW->MakeCombinedModel(*Measure);
    w->writeToFile("Workspace.root");
    w->Print("t");


    

}

int main(){

	Creator();
	return 0;

}
