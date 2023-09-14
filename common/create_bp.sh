#Copyright (c) 2019 Qualcomm Technologies, Inc.
#All Rights Reserved.
#Confidential and Proprietary - Qualcomm Technologies, Inc.
#!/bin/bash
function create_symlinks(){
echo $1
for i in $(/bin/ls $VAR_DIR/target/product/$1/BP_INCLUSION --hide bin_prjs);do
j="$(echo $i | sed 's/^_//g')"
j="$(echo $j | sed 's/_/\//g')"
DIR="vendor/qcom/proprietary/$j"
if [ ! -d $DIR ];then
if [ -f $TOP_DIR/$VAR_DIR/target/product/$1/BP_INCLUSION/$i/inactivebp ];then
  ln -sf $TOP_DIR/$VAR_DIR/target/product/$1/BP_INCLUSION/$i/inactivebp $TOP_DIR/$VAR_DIR/target/product/$1/BP_INCLUSION/$i/Android.bp
fi
fi
done
if [ -f $TOP_DIR/$VAR_DIR/target/product/$1/BP_INCLUSION/bin_prjs/inactivebp ];then
  ln -sf $TOP_DIR/$VAR_DIR/target/product/$1/BP_INCLUSION/bin_prjs/inactivebp $TOP_DIR/$VAR_DIR/target/product/$1/BP_INCLUSION/bin_prjs/Android.bp
fi
if [ -f $TOP_DIR/$VAR_DIR/target/product/$1/inactivebp ];then
  ln -sf $TOP_DIR/$VAR_DIR/target/product/$1/inactivebp $TOP_DIR/$VAR_DIR/target/product/$1/Android.bp
fi
}
VAR_DIR=$(echo $2 | sed 's/.*vendor\/qcom/vendor\/qcom/g')
TOP_DIR=$(pwd)
pushd $VAR_DIR
find . -type l | grep "Android.bp" | xargs rm -f
popd
TARGET="$1"
create_symlinks $TARGET
tbname=`echo $TARGET | awk -F"_" '{print $1}'`
tbsuffix=`echo $TARGET | awk -F"$tbname" '{print $2}'`
create_symlinks "qssi$tbsuffix"
