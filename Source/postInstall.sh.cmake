#! /bin/sh

# Copy the LCTimeDate.dat file from the previous version
if [[ -e $USR_ROOT/Configuration/Machine/LossCompensationTimeDate.dat ]]; then
	cp $USR_ROOT/Configuration/Machine/LossCompensationTimeDate.dat /home/Ccrt${CUSTOMER_RELEASE_VER}/Rewrite/Configuration/Machine
fi

# Copy the previous machine losses
if [[ -e $USR_ROOT/Configuration/Servers/MachineLosses.xml ]]; then
	cp $(fullpath $USR_ROOT/Configuration/Servers/MachineLosses.xml) /home/Ccrt${CUSTOMER_RELEASE_VER}/Rewrite/Configuration/Servers
fi

# Copy the previous test log index file
if [[ -e $USR_ROOT/Configuration/Machine/NextTestLogIndex.dat ]]; then
	cp $(fullpath $USR_ROOT/Configuration/Machine/NextTestLogIndex.dat) /home/Ccrt${CUSTOMER_RELEASE_VER}/Rewrite/Configuration/Machine
fi

# Make sure the new CCRT install is owned by CCRT
chown -R CCRT:devel $(fullpath /home/Ccrt${CUSTOMER_RELEASE_VER})

echo Installation finished for CCRT Release ${CUSTOMER_RELEASE_VER} @ $(date)
