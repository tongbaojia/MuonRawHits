here=$(pwd)

tag=2015-11-04-04h33m50s
ntups=/n/atlasfs/atlasdata/tuna/MuonRawHits/ntuples-${tag}
work=/n/home05/tuna/_sami/MuonRawHits/scripts/hists-${tag}
script=/n/home05/tuna/_sami/MuonRawHits/plots/hists.py

echo
echo "output directory: ${work}"
echo

for run in $(ls ${ntups}); do

    for file in $(ls ${ntups}/${run}); do

        job=${work}/${run}/${file}/job-${run}.sh
        jobdir=$(dirname ${job})

        mkdir -p ${jobdir}
        cd       ${jobdir}
        touch    ${job}
        
        echo "#!/bin/bash"                  >> ${job}
        echo "#"                            >> ${job}
        echo "#SBATCH -p pleiades"          >> ${job}
        echo "#SBATCH -t 1-0:0:0"           >> ${job}
        echo "#SBATCH --mem-per-cpu 10000"  >> ${job}
        echo "#SBATCH --workdir ${jobdir}"  >> ${job}
        echo "#"                            >> ${job}
        
        echo python ${script} --input=${ntups}/${run}/${file} --output=histograms-${run}-${file}.root >> ${job}
        
        sbatch ${job} | tee ${jobdir}/sbatch.txt

    done

done

cd ${here}
