# Copyright (c) 2022 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

#! /bin/bash

RPM_DIR="`pwd`/rpmbuild"
SOURCE_DIR="`pwd`"
RPMBUILDOPTS="-bb"
DIR="ramdump-parser"
VERSION="1.0"
TARBALL="${DIR}-${VERSION}.tar.gz"

clean_rpmbuild()
{
	rm -rf ${RPM_DIR}
}

clean_rpms()
{
	rm -rf *.rpm
}

clean_rpmbuild
clean_rpms
mkdir -p ${RPM_DIR}/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
git archive --prefix="${DIR}-${VERSION}"/ --format=tar HEAD | gzip -v > ${TARBALL}
mv ${TARBALL} ${RPM_DIR}/SOURCES
cp -f *.spec ${RPM_DIR}/SPECS
rpmbuild --define "_sourcedir ${RPM_DIR}/SOURCES" --define "_builddir ${RPM_DIR}/BUILD" --define "_srcrpmdir ${RPM_DIR}/SRPMS" --define "_rpmdir ${RPM_DIR}/RPMS" --define "_specdir ${RPM_DIR}/SPECS"  ${RPMBUILDOPTS} ${RPM_DIR}/SPECS/*.spec
if [ $? -eq 0 ]; then
	cp -f ${RPM_DIR}/RPMS/x86_64/*.rpm .
	clean_rpmbuild
	echo
	echo "------------------------------------------------------------"
	echo "Generated RPM: `ls *.rpm`"
	echo "------------------------------------------------------------"
	echo
else
	exit 1
fi
