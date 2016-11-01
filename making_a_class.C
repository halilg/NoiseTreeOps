{
    TFile f("HCALTree_999.root", "READ");
    TDirectoryFile* df = (TDirectoryFile*)f.Get("ExportTree");
    TTree* t = (TTree*)df->Get("HcalTree");
    t->MakeClass("HcalNoiseTree");
}
