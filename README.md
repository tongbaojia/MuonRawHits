# MuonRawHits

### make ntuples of raw hits in run/

    athena.py input/00280614.py muonrawhits.py
    
    # or
    
    source scripts/batch-ntuples.sh

### break up the ntuples for faster processing in run/, as desired

    python divide_and_randomize.py --input=input.root --output=divided/ --chunks=20
    
    # or
    
    for run in $(ls -1 ${batch_dir}); do
        python divide_and_randomize.py --input=${batch_dir}/${run}/*/ntuple*.root --output=${divide_dir}/${run}/ --chunks=30
    done

### some handy one-liners

    athena.py input/00281411.py muonrawhits.py | grep "strip\|chamber" | grep "CSL\|CSS" | tee csc_everything.txt
    python -c "for line in sorted(list(set(open('csc_everything.txt').readlines()))): print line.strip()" > csc_everything_disambig.txt
    cat csc_everything_disambig.txt | grep strip   > ../data/geometry/csc_strips.txt
    cat csc_everything_disambig.txt | grep chamber > ../data/geometry/csc_chambers.txt

    // 150 events, plz
    athena.py input/00281411.py muonrawhits.py | grep "tube\|chamber" | grep -v "CSL" | grep -v "CSS" | tee mdt_everything.txt
    python -c "for line in sorted(list(set(open('mdt_everything.txt').readlines()))): print line.strip()" > mdt_everything_disambig.txt
    cat mdt_everything_disambig.txt | grep tube | grep EI > ../data/geometry/mdt_tubes_EI.txt
    cat mdt_everything_disambig.txt | grep tube | grep EM > ../data/geometry/mdt_tubes_EM.txt
    cat mdt_everything_disambig.txt | grep chamber        > ../data/geometry/mdt_chambers.txt

    for run in 00*; do 
      python -c "import ROOT; tr = ROOT.TChain('physics'); tr.Add('${run}/*'); print '${run}', tr.GetEntries()"
    done

    for ds in $(dq2-ls data15_13TeV.002*.physics_ZeroBias.recon.ESD.f* | sort); do 
      echo ${ds} $(dq2-list-dataset-replicas ${ds} | grep NET2)
    done
