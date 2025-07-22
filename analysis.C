#include <iostream>
#include<sstream>
#include "TStopwatch.h"
#include "TMath.h"
#include <string>
#include <cctype>
#include <filesystem>
#include <algorithm>
using namespace std;
namespace fs = std::filesystem;


void hist_compare(TString file1, TString file2);
string removeSpaces(string a);
bool find_file_recursive(const fs::path& directory, const string& file);

void analysis(){
  string directory = fs::current_path().string();
   string input1;
   string input2;

  cout << "Enter the data replayed ROOTFile: ";
  cin >> input1;
  input1 = removeSpaces(input1);

  cout << "Enter the analyzed simulated data ROOTFile: ";
  cin >> input2;
  input2 = removeSpaces(input2);

  TString file1 = input1;
  TString file2 = input2;
  
  if (find_file_recursive(directory, input1) == true) cout << input1 + " Exists" << endl;
  else {
    cout << "Error opening File 1" << endl;
    exit(0);
  }
  
  if (find_file_recursive(directory, input2) == true) cout << input2 + " Exists" << endl;
   else {
     cout << "Error opening File 2" << endl;
     exit(0);
   }

  hist_compare(file1, file2);
  
}

void hist_compare(TString file_1, TString file_2){
  TStopwatch time;
  
  TFile *file1 = new TFile(file_1, "READ");
  if (!file1 || file1->IsZombie()){
    cerr << "Error opening file 1" << endl;
    exit(0);
  }

  TFile *file2 = new TFile(file_2, "READ");
  if (!file2 || file2->IsZombie()){
    cerr << "Error opening file 2" << endl;
    exit(0);
  }

  cout << "Using file: " + file_1 + "and file: " + file_2 + " for the data analysis comparison." << endl;
  cout << "Please wait..... " << endl;

  TString outputFilename = file_1;
  outputFilename.ReplaceAll("deut_prod_LH2", "compare_analysis");
  //outputFilename = "analyzed/" + outputFilename;
  TFile *output = new TFile(outputFilename, "recreate");

  TH1F *Pmx_simc = (TH1F*)file1->Get("H_Pmx_Lab;1");
  TH1F *Pmx_data = (TH1F*)file2->Get("Pmx;1");

  TCanvas *c1 = new TCanvas();

  // if (Pmx_simc->Integral()>0) Pmx_simc->Scale(1.0/ Pmx_simc->Integral());

  c1->cd();
  Pmx_simc->Draw();
  Pmx_data->Draw();
  c1->Update();

  output->Write();
  file1->Close();
  file2->Close();
  output->Close();

  time.Print();
  
}

bool find_file_recursive(const fs::path& directory, const std::string& file) {
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_symlink() || entry.is_directory()) {
            if (find_file_recursive(entry, file)) {
                return true;
            }
        } else if (entry.is_regular_file() && entry.path().filename() == file) {
            std::cout << file << " found!" << std::endl;
            return true;
        }
    }
    return false;
}
   

string removeSpaces(string str){
  str.erase(remove(str.begin(), str.end(), ' '), str.end());
  return str;
}


