here=$(pwd)
pkg=/n/home05/tuna/_sami/MuonRawHits
work=/n/atlasfs/atlasdata/tuna/MuonRawHits/batch-$(date "+%Y-%m-%d-%Hh%Mm%Ss")

echo
echo "output directory: ${work}"
echo

for run in 00280862 00281143 00281381 00281411 00282992 00283429 00283780 00284213; do

    job=${work}/${run}/job-${run}.sh
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

    echo athena.py ${pkg}/run/input/${run}.py ${pkg}/run/muonrawhits.py >> ${job}

    sbatch ${job} | tee ${jobdir}/sbatch.txt

done

cd ${here}
