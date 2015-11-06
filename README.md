# MuonRawHits

### make ntuples of raw hits in run/

    athena.py input/00280614.py muonrawhits.py
    
    # or
    
    source scripts/batch-ntuples.sh

### break up the ntuples for faster processing in run/

    python divide_and_randomize.py --input=input.root --output=divided/ --chunks=20
    
    # or
    
    for run in $(ls -1 ${batch_dir}); do
        python divide_and_randomize.py --input=${batch_dir}/${run}/ntuple*.root --output=${divide_dir}/${run}/ --chunks=20
    done

### make histograms (may move this to C++) in plots/
    python plots.py --hist --run=${run}

### make plots in plots/
    python plots.py --plot

### some handy one-liners

    athena.py input/00281411.py muonrawhits.py | grep "strip\|chamber" | tee csc_everything.txt
    python -c "for line in sorted(list(set(open('csc_everything.txt').readlines()))): print line.strip()" > csc_everything_disambig.txt
    cat csc_everything_disambig.txt | grep strip   > ../data/geometry/csc_strips.txt
    cat csc_everything_disambig.txt | grep chamber > ../data/geometry/csc_chambers.txt

    for run in 00*; do 
      python -c "import ROOT; tr = ROOT.TChain('physics'); tr.Add('${run}/*'); print '${run}', tr.GetEntries()"
    done

