# echo "cleanup MuonRawHits MuonRawHits-00-00-00 in /n/atlasfs/atlascode/backedup/btong/SegRate"

if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.8/CMT/v1r25p20140131; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtMuonRawHitstempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if test ! $? = 0 ; then cmtMuonRawHitstempfile=/tmp/cmt.$$; fi
${CMTROOT}/${CMTBIN}/cmt.exe cleanup -sh -pack=MuonRawHits -version=MuonRawHits-00-00-00 -path=/n/atlasfs/atlascode/backedup/btong/SegRate  $* >${cmtMuonRawHitstempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/${CMTBIN}/cmt.exe cleanup -sh -pack=MuonRawHits -version=MuonRawHits-00-00-00 -path=/n/atlasfs/atlascode/backedup/btong/SegRate  $* >${cmtMuonRawHitstempfile}"
  cmtcleanupstatus=2
  /bin/rm -f ${cmtMuonRawHitstempfile}
  unset cmtMuonRawHitstempfile
  return $cmtcleanupstatus
fi
cmtcleanupstatus=0
. ${cmtMuonRawHitstempfile}
if test $? != 0 ; then
  cmtcleanupstatus=2
fi
/bin/rm -f ${cmtMuonRawHitstempfile}
unset cmtMuonRawHitstempfile
return $cmtcleanupstatus

