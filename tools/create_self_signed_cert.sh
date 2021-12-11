#/bin/bash

DNS_LIST=()
IP_LIST=()

### Set your envirioment variables
#CA
CA=myCA
CA_C="BR"
CA_ST="ES"
CA_L="Vitoria"
CA_O="Agro CA"
CA_OU="TI"
CA_CN="agroca.com"

#Domain
DOMAIN=mydomain.com 
DOMAIN_C="BR"
DOMAIN_ST="ES"
DOMAIN_L="Vitoria"
DOMAIN_O="Agro Telemetry"
DOMAIN_OU="Engineering"
DOMAIN_CN="agrotelemetry.com"

#Certicate parameters
DAYS=825
KEY_SIZE=2048

OUTPUT_DIR=certs

usage()
{
	echo "$(basename $0) [-o <output_directory>] "
	echo "               [-c <ca_prefix>] [-m <domain_prefix>] "
	echo "               [-s <key_size>] [-a <valid_days>]"
	echo "               [-i <IP>]... [-d <domain name>]..."
}

exit_if_not_number()
{
	re='^[0-9]+$'
	if ! [[ "$1" =~ $re ]] ; then
	   echo "$2: '$1' is not a number"
	   usage
	   exit 1
	fi
}

while getopts i:d:o:c:m:s:a: op; do
	case $op in 
		i)
			IP_LIST+=($OPTARG)
		;;
		d)
			DNS_LIST+=($OPTARG)
		;;
		o)
			OUTPUT_DIR=$OPTARG
		;;
		c)
			CA=$OPTARG
		;;
		m)
			DOMAIN=$OPTARG
		;;
		s)
			KEY_SIZE=$OPTARG
			exit_if_not_number $KEY_SIZE "-s"
		;;
		a)
			DAYS=$OPTARG
			exit_if_not_number $DAYS "-a"
		;;
		*)
			echo "Option '$options' not supported"
		;;
	esac
done

if [ ${#IP_LIST} -eq 0 -a ${#DNS_LIST} -eq 0 ]
then
	echo "You need to provide a IP or domain name"
	usage
	exit 1
fi

CA_SUBJ="/C=$CA_C/ST=$CA_ST/L=$CA_L/O=$CA_O/OU=$CA_OU/CN=$CA_CN"
DOMAIN_SUBJ="/C=$DOMAIN_C/ST=$DOMAIN_ST/L=$DOMAIN_L/O=$DOMAIN_O/OU=$DOMAIN_OU/CN=$DOMAIN_CN"

OPENSSL=openssl

mkdir -p $OUTPUT_DIR
cd $OUTPUT_DIR

create_ca_key_certificate()
{
	#Generate CA private key (type a passphrase as asked)
	#$OPENSSL genrsa -des3 -out $CA.key $KEY_SIZE
	$OPENSSL genrsa -out $CA.key $KEY_SIZE
	
	#Removing encryptation of key
	#cp $CA.key $CA.org
	#openssl rsa -in $CA.org -out $CA.key
	
	#Generate root certificate (it will be asked the passphrase just created, a some other info)
	$OPENSSL req -x509 -new -nodes -key $CA.key -sha256 -days $DAYS -subj "$CA_SUBJ" -out $CA.pem
	
	# Coverting CA from PEM format to CRT
	$OPENSSL x509 -outform der -in $CA.pem -out $CA.crt
}

create_domain_key_certificate()
{
	#Generate a private key to your domain
	$OPENSSL genrsa -out $DOMAIN.key $KEY_SIZE
	
	#Create a certificate-signing request (answer all questions)
	$OPENSSL req -new -key $DOMAIN.key -subj "$DOMAIN_SUBJ" -out $DOMAIN.csr
	
	# Create a config file for the extensions (paste the following command)
	>$DOMAIN.ext cat <<-EOF
	authorityKeyIdentifier=keyid,issuer
	basicConstraints=CA:FALSE
	keyUsage = digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment
	subjectAltName = @alt_names
	[alt_names]
	EOF
	
	declare -i i=1
	
	for dns in "${DNS_LIST[@]}"
	do
	     echo "DNS.${i} = ${dns}" >> $DOMAIN.ext
	     i=$i+1 
	done
	
	i=1
	for ip in "${IP_LIST[@]}"
	do
	     echo "IP.${i} = ${ip}" >> $DOMAIN.ext
	     i=$i+1 
	done
	
	# Finally, create the signed certificate
	$OPENSSL x509 -req -in $DOMAIN.csr -CA $CA.pem -CAkey $CA.key -CAcreateserial \
			-out $DOMAIN.crt -days $DAYS -sha256 -extfile $DOMAIN.ext
}

create_ca_key_certificate
create_domain_key_certificate
