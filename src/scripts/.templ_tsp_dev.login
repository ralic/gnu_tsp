#!/bin/csh

##############################
# Find host type
##############################

set HOST_UNAME=`uname -a | awk '{ print $1 }'`

#Linux
if ( "$HOST_UNAME" == "Linux" )  then
	setenv HOST_TARGET "linux"

#Sun & Sun64
else if ( "$HOST_UNAME" == "SunOS") then
    #try to find isalist util
    set SPARC_TYPE=` isalist | awk '{ print $1 }' | grep sparcv9`
    if( $? == 0 ) then
	if( $SPARC_TYPE != "" ) then
	    setenv HOST_TARGET "sun64"
	else
	    setenv HOST_TARGET "sun"
	endif	
    else
	setenv HOST_TARGET "sun"
    endif

#dec
else if ( "$HOST_UNAME" == "OSF1") then
      setenv HOST_TARGET "dec"

#???
else
     echo "ERROR : Unknown system : $HOST_UNAME"
     exit -1
endif


##############################
# HOME_EXEC_BASE / HOME_EXEC_CURRENT
##############################
# %%tsp_base%% will be set by the configure script 
setenv TSP_BASE %%tsp_base%%
echo "Using TSP_BASE=$TSP_BASE"
setenv HOME_EXEC_BASE $TSP_BASE/exec
setenv HOME_EXEC_CURRENT $HOME_EXEC_BASE/current


##############################
# Paths
##############################
setenv PATH ${PATH}:${TSP_BASE}/src/scripts:${HOME_EXEC_CURRENT}/bin.consumer.debug:${HOME_EXEC_CURRENT}/bin.provider.debug:${JAVA_HOME}/bin


##############################
# DEVBASE
##############################
setenv DEVBASE $TSP_BASE
echo "Using DEVBASE=$DEVBASE"

if ( ! $?STRACE_DEBUG ) then 
	setenv STRACE_DEBUG 1
endif
echo "Using STRACE_DEBUG=$STRACE_DEBUG"


##############################
# ALIAS
##############################
alias go_ec 'cd ${HOME_EXEC_CURRENT}/bin.consumer'
alias go_ecd 'cd ${HOME_EXEC_CURRENT}/bin.consumer.debug'
alias go_ep 'cd ${HOME_EXEC_CURRENT}/bin.provider'
alias go_epd 'cd ${HOME_EXEC_CURRENT}/bin.provider.debug'
alias go_tsp 'cd ${TSP_BASE}'
alias go_scripts 'cd ${TSP_BASE}/src/scripts'
alias go_tests 'cd ${TSP_BASE}/src/core/tests'
