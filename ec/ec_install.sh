#!/usr/bin/bash


#Commands
SCRIPTDIR="`cd \`/usr/bin/dirname $0\`; pwd`"
PKGADD=/usr/sbin/pkgadd
BASENAME=/usr/bin/basename
GREP=/usr/bin/grep
NAWK=/usr/bin/nawk
PKGINFO=/usr/bin/pkginfo
SCRIPTNAME="`/usr/bin/basename $0`"
CP=/usr/bin/cp
RM=/usr/bin/rm
TOUCH=/usr/bin/touch
CHOWN=/usr/bin/chown
CHMOD=/usr/bin/chmod
CRLE=/usr/bin/crle
ECHO=/usr/bin/echo

#variables
PAM_DEST=/usr/lib/security/
SSL_PKG_NAME=SMCossl
SSL_PKG=openssl-1.0.0j-sol10-x86-local
PAM_FILE=libpam_eric_sso.so
#PAM file with standard UAS configuration plus the SSO module
PAM_UPDATE_FILE=pam.conf.updated
MASTER_KEY_FILE=masterkey
COUNTER_FILE=counter
SILENT_INST_FILE=admfile.adm


###################################MAIN######################################



if [ `id | $NAWK -F'[()]' '{print $2}'` != "root" ]; then
    printf "You must be root to run this script"
    exit 1
fi

# Server type definition
SERVER_CONFIG_TYPE_FILE=/ericsson/config/ericsson_use_config

# Check server config type file exists
if [ ! -f "$SERVER_CONFIG_TYPE_FILE" ]; then
        printf "ERROR: Failed to locate $SERVER_CONFIG_TYPE_FILE\n"
        exit 1
fi


# Check for supported server config type
server_type=$($NAWK -F"=" '/config/{print $2}' $SERVER_CONFIG_TYPE_FILE)
if [ "$server_type" != "appserv" ]; then
        printf "ERROR: $HOSTNAME has unsupported server configuration type: [$server_type].\n"
        printf "ERROR: $SCRIPTNAME can only be run on server type: [appserv]\n"
        exit 1
fi



printf "Updating openssl \n"
$PKGADD -G -n -a ${SCRIPTDIR}/${SILENT_INST_FILE} -d ${SCRIPTDIR}/${SSL_PKG} ${SSL_PKG_NAME} > /dev/null 2>&1

$PKGINFO -q  ${SSL_PKG_NAME}

if [ $? -eq 0 ]; then
        printf "openssl updated correctly \n"
else
        printf "Failed to update openssl \n"
        exit 1
fi


#Copy master key
printf "Copying master key file \n"

$CP ${SCRIPTDIR}/${MASTER_KEY_FILE} /var/tmp/

if [ ! -f /var/tmp/${MASTER_KEY_FILE} ]; then
	printf "Failed to copy master key to /var/tmp/${MASTER_KEY_FILE} \n"
	exit 1
fi

#Create counter file

printf "Creating counter file \n" 

$TOUCH /var/tmp/${COUNTER_FILE}

#Add a dummy value to the counter. Make sure that its not empty
$ECHO "22684080" >>  /var/tmp/${COUNTER_FILE}

if [ ! -f /var/tmp/${COUNTER_FILE} ]; then
        printf "Failed to create counter file  /var/tmp/${COUNTER_FILE} \n"
        exit 1
fi

printf "Setting permission on counter and master key \n"
$CHOWN root:root /var/tmp/${MASTER_KEY_FILE} /var/tmp/${COUNTER_FILE}

$CHMOD 600 /var/tmp/${MASTER_KEY_FILE} /var/tmp/${COUNTER_FILE}

printf "Copying PAM file \n"


$CP ${SCRIPTDIR}/${PAM_FILE} ${PAM_DEST}

if [ ! -f ${PAM_DEST}/${PAM_FILE} ]; then
        printf "Failed to copy PAM file to ${PAM_DEST} \n" 
        exit 1
fi

$CHOWN root:root ${PAM_DEST}/${PAM_FILE}

$CHMOD 755 ${PAM_DEST}/${PAM_FILE}

$CRLE | $GREP /usr/sfw/lib > /dev/null 2>&1

if [ $? -ne 0 ]; then
	#Update the linking environment to pick up new binaries
	printf "Updating runtime linking environment\n"
	$CRLE -u -l /usr/sfw/lib
fi


$GREP ${PAM_FILE} /etc/pam.conf > /dev/null 2>&1

if [ $? -eq 1 ]; then
	printf "Updating PAM configuration file \n"
	$CP /etc/pam.conf /etc/pam.orig
	$CP ${SCRIPTDIR}/${PAM_UPDATE_FILE} /etc/pam.conf
fi

$GREP ${PAM_FILE} /etc/pam.conf > /dev/null 2>&1

if [ $? -eq 1 ]; then
	printf "Failed to add entry for ${PAM_FILE} to /etc/pam.conf \n"
fi

printf "Updating inet services to detect new runtime linking environment \n"
pkill -HUP inetd


printf "Script has finsihed updating PAM configuration. These updates should be performed on each UAS \n"
exit 0
