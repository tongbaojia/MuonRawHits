# echo "setup MuonRawHits MuonRawHits-00-00-00 in /n/atlasfs/atlascode/backedup/btong/SegRate"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.8/CMT/v1r25p20140131
endif
source ${CMTROOT}/mgr/setup.csh
set cmtMuonRawHitstempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if $status != 0 then
  set cmtMuonRawHitstempfile=/tmp/cmt.$$
endif
${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=MuonRawHits -version=MuonRawHits-00-00-00 -path=/n/atlasfs/atlascode/backedup/btong/SegRate  -no_cleanup $* >${cmtMuonRawHitstempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=MuonRawHits -version=MuonRawHits-00-00-00 -path=/n/atlasfs/atlascode/backedup/btong/SegRate  -no_cleanup $* >${cmtMuonRawHitstempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmtMuonRawHitstempfile}
  unset cmtMuonRawHitstempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmtMuonRawHitstempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmtMuonRawHitstempfile}
unset cmtMuonRawHitstempfile
exit $cmtsetupstatus

