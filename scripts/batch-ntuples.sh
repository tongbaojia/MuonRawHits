here=$(pwd)
pkg=$(dirname ${here})
work=/n/atlasfs/atlasdata/tuna/MuonRawHits/batch-$(date "+%Y-%m-%d-%Hh%Mm%Ss")

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

for run in 00278880 00279169 00279345 00279685 00280231 00280464 00280673 00280862 00281074 00281143 00281411 00282992 00283429 00283780 00284213 00284285; do

    for split in firsthalf secondhalf; do

        job=${work}/${run}/${split}/job-${run}.sh
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
        
        echo athena.py ${pkg}/run/input/${run}.py ${pkg}/run/input/${split}.py ${pkg}/run/muonrawhits.py >> ${job}
        
        sbatch ${job} | tee ${jobdir}/sbatch.txt

    done
done

cd ${here}
