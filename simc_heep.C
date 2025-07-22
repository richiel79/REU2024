#include<iostream>
#include<sstream>
#include<filesystem>
#include "TStopwatch.h"
/*#include "TH1F.h"
#include "TFile.h"
#include "TString.h" Include if want to compile before running in root */
#include <string>
#include <cctype>
#include<algorithm>
#include "TMath.h"
using namespace std;
namespace fs = std::filesystem;

//Initializing the functions
void simc_analysis(TString filename);
bool check_file(TString file, string a);
string removeSpaces(string str);



void simc_heep(){

    //Takes the user's directory path and converts into a string
    string directory = fs::current_path().string();
    string input;

    cout << "............................................................." << endl;
    cout << "Enter the ROOTFile to be analyzed:  ";
    cin >> input;

    input = removeSpaces(input);
    //This line can be modified if the filename pattern changes but still has a unique #  
    TString filename_pattern = input;

     if(check_file(filename_pattern, directory) == true) {
         simc_analysis(filename_pattern);
         exit(0);
        } else {
          cout << "File not found." << endl;
     } 
}





void simc_analysis(TString filename){

    TStopwatch t; //Keeps time
    Double_t  normfac, em, q2, w, weight, pmx, pmy, pmz, nu, hdelta, hxptar, hyptar, hytar, edelta, exptar, eyptar, eytar, e_angle, p_angle;
    Double_t dtr = TMath::Pi()/180;
    const double p_mass = 0.938; //Mass of proton in GeV

    //Open the file 
    TFile *file = new TFile(filename,"READ");
    if (!file || file->IsZombie()){
        cerr << "Error opening file" << endl;
        exit(0);
    }
    auto tree = file->Get<TTree>("SNT");

    if(!tree) {
        cerr << "Error getting TTree from file" << endl;
        exit(0);
    }

    

    Long64_t nentries = tree->GetEntries();

    cout << endl <<  "Using " << filename << " for analysis!" << endl;
    cout << "Total Entries = " << nentries << endl;
    cout << "Please wait, the  analysis for " + filename + " has started! " << endl; 

    // Create an output filename based on the input filename
    TString outputFilename = filename;
    outputFilename.ReplaceAll(".root", "_analyzed.root");
    outputFilename = "analyzed/" + outputFilename;
    TFile *output = new TFile(outputFilename, "recreate");


    //Setting the address of variables within the tree  
    tree->SetBranchAddress("Normfac", &normfac);
    tree->SetBranchAddress("Em",&em);
    tree->SetBranchAddress("Q2", &q2);
    tree->SetBranchAddress("W", &w);
    tree->SetBranchAddress("Weight", &weight);
    tree->SetBranchAddress("Pmx", &pmx);
    tree->SetBranchAddress("Pmy", &pmy);
    tree->SetBranchAddress("Pmz", &pmz);
    tree->SetBranchAddress("nu", &nu);
    tree->SetBranchAddress("h_delta",&hdelta);
    tree->SetBranchAddress("h_xptar",&hxptar);
    tree->SetBranchAddress("h_yptar",&hyptar);
    tree->SetBranchAddress("h_ytar",&hytar);
    tree->SetBranchAddress("e_delta",&edelta);
    tree->SetBranchAddress("e_xptar",&exptar);
    tree->SetBranchAddress("e_yptar",&eyptar);
    tree->SetBranchAddress("e_ytar", &eytar);
    tree->SetBranchAddress("theta_e", &e_angle);
    tree->SetBranchAddress("theta_p", &p_angle);
  

    //Initiating the histograms of kinematic quantities
    TH1F *Em = new TH1F("Em", "Missing Energy;Em [GeV];",1000,tree->GetMinimum("Em"),tree->GetMaximum("Em"));
    TH1F *Q2 = new TH1F("Q2", "4 Momentum-Transfer; #font[12]{Q2} [GeV^2]",100,tree->GetMinimum("Q2"),tree->GetMaximum("Q2"));
    TH1F *W = new TH1F("W", "Invariant Mass",1000,tree->GetMinimum("W"),tree->GetMaximum("W"));
    TH1F *Pmx = new TH1F("Pmx", "Missing Momentum x; Pmx [GeV]",1000,tree->GetMinimum("Pmx"),tree->GetMaximum("Pmx"));
    TH1F *Pmy = new TH1F("Pmy", "Missing Momentum y; Pmy [GeV]",1000,tree->GetMinimum("Pmy"),tree->GetMaximum("Pmy"));
    TH1F *Pmz = new TH1F("Pmz", "Missing Momentum z; Pmz [GeV]",1000,tree->GetMinimum("Pmz"),tree->GetMinimum("Pmz"));			 
    TH1F *Nu = new TH1F("Nu", "Energy Transferred;[GeV]", 100, tree->GetMinimum("nu"), tree->GetMaximum("nu"));

    //Initiating the histograms for the hadron kinematics
    TH1F *hDel = new TH1F("h_delta", "HMS Momentum Acceptance; #delta_{HMS} [%]", 100, tree->GetMinimum("h_delta"), tree->GetMaximum("h_delta"));
    TH1F *hXP = new TH1F("h_xptar", "HMS X'tar (relative Out-of-Plane); X'tar [rad]", 100, tree->GetMinimum("h_xptar"), tree->GetMaximum("h_xptar"));
    TH1F *hYP = new TH1F("h_yptar", "HMS Y'tar (relative-In-Plane)", 100, tree->GetMinimum("h_yptar"), tree->GetMaximum("h_yptar"));
    TH1F *hY = new TH1F("h_ytar", "HMS Y'tar; Ytar [cm]", 100, tree->GetMinimum("h_ytar"), tree->GetMaximum("h_ytar"));

    //Initiating histograms for the electron kinematics
    TH1F *eDel = new TH1F("e_delta", "SHMS Momentum Acceptance; #delta_{SHMS} [%]", 100, tree->GetMinimum("e_delta"), tree->GetMaximum("e_delta"));
    TH1F *eXP = new TH1F("e_xptar","SHMS X'tar (relative Out-of-Plane); X'tar [rad]", 100, tree->GetMinimum("e_xptar"),tree->GetMaximum("e_xptar"));
    TH1F *eYP = new TH1F("e_yptar", "SHMS In-plane Scattering Angle; Y'tar [rad]", 100, tree->GetMinimum("e_yptar"), tree->GetMaximum("e_yptar"));
    TH1F *eY = new TH1F("e_ytar", "SHMS Ytar; Ytar [cm]", 100, tree->GetMinimum("e_ytar"), tree->GetMaximum("e_ytar"));
			 
    TH1F *xbj = new TH1F("x_bj", "x-Bjorken", 1000, tree->GetMinimum("Q2/(2*nu*p_mass)"), tree->GetMaximum("Q2/(2*nu*p_mass)"));
    TH1F *eTHETA = new TH1F("theta_e", "Electron angle; angle [deg]", 100, tree->GetMinimum("theta_e/dtr"),tree->GetMaximum("theta_e/dtr"));
    TH1F *pTHETA = new TH1F("theta_p", "Proton angle; angle [deg]", 100, tree->GetMinimum("theta_p/dtr"), tree->GetMaximum("theta_p/dtr"));
    
    //Comment out when finished//Prevent ROOT from displaying graphs while computing
    gROOT->SetBatch(1);

    //Initializing the Canvases for the different parameters
    TCanvas *c1_e = new TCanvas();
    TCanvas *c2_q = new TCanvas();
    TCanvas *c3_w = new TCanvas();
    TCanvas *c4_px = new TCanvas();
    TCanvas *c5_py = new TCanvas();
    TCanvas *c6_pz = new TCanvas();
    TCanvas *c7_nu = new TCanvas();
    TCanvas *hd = new TCanvas();
    TCanvas *hxp = new TCanvas();
    TCanvas *hyp = new TCanvas();
    TCanvas *hy = new TCanvas();
    TCanvas *ed = new TCanvas();
    TCanvas *exp = new TCanvas();
    TCanvas *eyp = new TCanvas();
    TCanvas *ey = new TCanvas();
    TCanvas *x_bj = new TCanvas();
    TCanvas *EA = new TCanvas();
    TCanvas *PA = new TCanvas();
			 
    //Loop over each entry to fill in the histogram
    for(Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);
        Double_t test = normfac * weight / nentries;
	Double_t x = q2/(2*p_mass*nu);
	
        //Fill the histograms with their values 
        Em->Fill(em, test);
        Q2->Fill(q2, test);
        W->Fill(w, test);
        Pmx->Fill(pmx, test);
        Pmy->Fill(pmy, test);
        Pmz->Fill(pmz, test);
	Nu->Fill(nu, test);
	hDel->Fill(hdelta, test);
	hXP->Fill(hxptar, test);
	hYP->Fill(hyptar, test);
	hY->Fill(hytar, test);
	eDel->Fill(edelta, test);
	eXP->Fill(exptar, test);
	eYP->Fill(eyptar, test);
	eY->Fill(eytar, test);
	xbj->Fill(x, test);
	eTHETA->Fill(e_angle/dtr, test);
	pTHETA->Fill(p_angle/dtr, test);
    }
    //Normalize histograms
    if (Em->Integral() > 0) Em->Scale(1.0 / Em->Integral());
    if (Q2->Integral() > 0) Q2->Scale(1.0 / Q2->Integral());
    if (W->Integral() > 0) W->Scale(1.0 / W->Integral());
    if (Pmx->Integral() > 0) Pmx->Scale(1.0 / Pmx->Integral());
    if (Pmy->Integral() > 0) Pmy->Scale(1.0 / Pmy->Integral());
    if (Pmz->Integral() > 0) Pmz->Scale(1.0 / Pmz->Integral());
    if (Nu->Integral() > 0) Nu->Scale(1.0 / Nu->Integral());
    if (hDel->Integral() > 0) hDel->Scale(1.0 / hDel->Integral());
    if (hXP->Integral() > 0) hXP->Scale(1.0 / hXP->Integral());
    if (hYP->Integral() > 0) hYP->Scale(1.0 / hYP->Integral());
    if (hY->Integral() > 0) hY->Scale(1.0 / hY->Integral());
    if (eDel->Integral() > 0) eDel->Scale(1.0 / eDel->Integral());
    if (eXP->Integral() > 0) eXP->Scale(1.0 / eXP->Integral());
    if (eYP->Integral() > 0) eYP->Scale(1.0 / eYP->Integral());
    if (eY->Integral() > 0) eY->Scale(1.0 / eY->Integral());
    if (xbj->Integral() > 0) xbj->Scale(1.0 / xbj->Integral());
    if (eTHETA->Integral() > 0) eTHETA->Scale(1.0 / eTHETA->Integral());
    if (pTHETA->Integral() > 0) pTHETA->Scale(1.0 / pTHETA->Integral());

    //Draws the weighted histograms on their respective canvas  
    c1_e->cd();
    Em->Draw("norm");
    Em->GetXaxis()->SetRangeUser(-0.1,0.1);
    Em->SetLineWidth(2);
    c1_e->Update();

    c2_q->cd();
    Q2->Draw();
    Q2->GetXaxis()->SetRangeUser(1.6,2.6);
    Q2->SetLineWidth(2);
    c2_q->Update();

    c3_w->cd();
    W->Draw();
    W->GetXaxis()->SetRangeUser(0.8,1.1);
    W->SetLineWidth(2);
    c3_w->Update();

    c4_px->cd();
    Pmx->Draw();
    Pmx->GetXaxis()->SetRangeUser(-0.1,0.1);
    Pmx->SetLineWidth(2);
    c4_px->Update();

    c5_py->cd();
    Pmy->Draw();
    Pmy->GetXaxis()->SetRangeUser(-0.1,0.1);
    Pmy->SetLineWidth(2);
    c5_py->Update();

    c6_pz->cd();
    Pmz->Draw();
    Pmz->GetXaxis()->SetRangeUser(-0.1,0.1);
    Pmz->SetLineWidth(2);
    c6_pz->Update();

    c7_nu->cd();
    Nu->Draw();
    Nu->SetLineWidth(2);
    c7_nu->Update();

    hd->cd();
    hDel->Draw();
    hDel->GetXaxis()->SetRangeUser(-15,15);
    hDel->SetLineWidth(2);
    hd->Update();

    hxp->cd();
    hXP->Draw();
    hXP->GetXaxis()->SetRangeUser(-0.1,0.1);
    hXP->SetLineWidth(2);
    hxp->Update();

    hyp->cd();
    hYP->Draw();
    hYP->GetXaxis()->SetRangeUser(-0.1,0.1);
    hYP->SetLineWidth(2);
    hyp->Update();

    hy->cd();
    hY->Draw();
    hY->GetXaxis()->SetRangeUser(-6,6);
    hY->SetLineWidth(2);
    hy->Update();

    ed->cd();
    eDel->Draw();
    eDel->SetLineWidth(2);
    ed->Update();

    exp->cd();
    eXP->Draw();
    eXP->GetXaxis()->SetRangeUser(-0.05,0.05);
    eXP->SetLineWidth(2);
    exp->Update();

    eyp->cd();
    eYP->Draw();
    eYP->GetXaxis()->SetRangeUser(-0.05,0.05);
    eYP->SetLineWidth(2);
    eyp->Update();

    ey->cd();
    eY->Draw();
    eY->GetXaxis()->SetRangeUser(-2,2);
    eY->SetLineWidth(2);
    ey->Update();

    x_bj->cd();
    xbj->Draw();
    xbj->GetXaxis()->SetRangeUser(0.9,1.1);
    xbj->SetLineWidth(2);
    x_bj->Update();

    EA->cd();
    eTHETA->Draw();
    eTHETA->SetLineWidth(2);
    EA->Update();

    PA->cd();
    pTHETA->Draw();
    pTHETA->SetLineWidth(2);
    PA->Update();

    //Writes all these Histograms to the output files
    output->Write();
    output->Close();
    file->Close();

    //Formatting text
    cout << "............................................................." << endl;
    cout << "Analysis completed! Output file created in worksim/analyzed directory" << endl;
    t.Print();
    cout << "............................................................." << endl;
}


bool check_file(TString file, string a){

  bool file_found = false;
    for(const auto& entry : fs::directory_iterator(a)){
        if(entry.is_regular_file() && entry.path().filename() == file){
            return file_found = true;
        }
    }
    return file_found;
}

string removeSpaces(string str){
  str.erase(remove(str.begin(), str.end(), ' '), str.end());
  return str;
}

