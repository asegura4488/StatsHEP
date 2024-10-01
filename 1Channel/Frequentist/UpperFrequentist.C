void UpperFrequentist(){

  double mu = 0.;
  double b = 5.;
  
  for(int i=0;i<=10;i++){
    mu = 0.5*TMath::ChisquareQuantile(0.95,2*(i+1)) - b;
    std::cout << " n: " << i << " upper limit: " << mu << std::endl;
  }


}

int main (){
 UpperFrequentist();
 return 0;
}
