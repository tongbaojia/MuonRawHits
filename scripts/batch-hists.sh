here=$(pwd)
pkg=$(dirname ${here})

tag=2015-11-08-09h00m45s

ntups=/n/atlasfs/atlasdata/tuna/MuonRawHits/ntuples-${tag}
work=${pkg}/scripts/hists-${tag}
script=${pkg}/plots/hists.py

if [ "$(basename ${pkg})" != "MuonRawHits" ]; then
    echo
    echo "FATAL: Package directory is not MuonRawHits."
    echo "       Please run from MuonRawHits/scripts/"
    echo
    return
fi

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
        echo "#SBATCH --mem-per-cpu 2048"   >> ${job}
        echo "#SBATCH --workdir ${jobdir}"  >> ${job}
        echo "#"                            >> ${job}
        
        echo python ${script} --input=${ntups}/${run}/${file} --output=histograms-${run}-${file}.root >> ${job}
        
        sbatch ${job} | tee ${jobdir}/sbatch.txt

    done

done

cd ${here}
