

IMS_RTP := vendor.qti.imsrtpservice@3.0
IMS_RTP += vendor.qti.imsrtpservice@3.0_product
IMS_RTP += vendor.qti.imsrtpservice@3.0_vendor


CNE := com.quicinc.cne.api@1.0
CNE += com.quicinc.cne.api@1.1
CNE += com.quicinc.cne.server@1.0
CNE += com.quicinc.cne.server@2.0
CNE += com.quicinc.cne.server@2.1
CNE += com.quicinc.cne.server@2.2
CNE += com.quicinc.cne.constants@1.0
CNE += com.quicinc.cne.constants@2.0
CNE += com.quicinc.cne.constants@2.1
CNE += vendor.qti.hardware.data.cne.internal.api@1.0
CNE += vendor.qti.hardware.data.cne.internal.server@1.0
CNE += vendor.qti.hardware.data.cne.internal.constants@1.0
CNE += vendor.qti.data.factory@1.0
CNE += vendor.qti.data.factory@2.0
CNE += vendor.qti.data.factory@2.1
CNE += vendor.qti.data.factory@2.2
CNE += vendor.qti.data.factory@2.3
CNE += vendor.qti.hardware.data.qmi@1.0
CNE += com.quicinc.cne.api-V1.0-java
CNE += com.quicinc.cne.api-V1.1-java
CNE += com.quicinc.cne.constants-V1.0-java
CNE += com.quicinc.cne.constants-V2.0-java
CNE += com.quicinc.cne.constants-V2.1-java
CNE += com.quicinc.cne.server-V1.0-java
CNE += com.quicinc.cne.server-V2.0-java
CNE += com.quicinc.cne.server-V2.1-java
CNE += com.quicinc.cne.server-V2.2-java
CNE += vendor.qti.hardware.data.cne.internal.api-V1.0-java
CNE += vendor.qti.hardware.data.cne.internal.server-V1.0-java
CNE += vendor.qti.hardware.data.cne.internal.constants-V1.0-java
CNE += vendor.qti.data.factory-V1.0-java
CNE += vendor.qti.data.factory-V2.0-java
CNE += vendor.qti.data.factory-V2.1-java
CNE += vendor.qti.data.factory-V2.2-java
CNE += vendor.qti.hardware.data.qmi-V1.0-java
CNE += vendor.qti.hardware.slmadapter@1.0
CNE += vendor.qti.hardware.slmadapter-V1.0-java
CNE += vendor.qti.hardware.mwqemadapter@1.0
CNE += vendor.qti.hardware.mwqemadapter-V1.0-java

CNE += vendor.qti.data.factory-V2.2-java
CNE += vendor.qti.data.factory-V2.3-java
CNE += vendor.qti.hardware.data.qmi-V1.0-java
CNE += vendor.qti.hardware.slmadapter@1.0
CNE += vendor.qti.hardware.slmadapter-V1.0-java
CNE += vendor.qti.hardware.mwqemadapter@1.0
CNE += vendor.qti.hardware.mwqemadapter-V1.0-java
CNE += vendor.qti.hardware.data.lce@1.0
CNE += vendor.qti.hardware.data.lce-V1.0-java

CACERT := vendor.qti.hardware.cacert@1.0

DPM := com.qualcomm.qti.dpm.api@1.0
DPM += com.qualcomm.qti.dpm.api@1.0_system

IMS := vendor.qti.ims.factory@1.0
IMS += vendor.qti.ims.factory-V1.0-java
IMS += vendor.qti.ims.factory@1.1
IMS += vendor.qti.ims.factory-V1.1-java
IMS += vendor.qti.ims.factory@2.0
IMS += vendor.qti.ims.factory-V2.0-java
IMS += vendor.qti.ims.factory@2.1
IMS += vendor.qti.ims.factory-V2.1-java
IMS += vendor.qti.ims.factory@2.2
IMS += vendor.qti.ims.factory-V2.2-java
IMS += vendor.qti.ims.connection@1.0
IMS += vendor.qti.ims.connection-V1.0-java
IMS += vendor.qti.ims.rcsuce@1.0
IMS += vendor.qti.ims.rcsuce-V1.0-java
IMS += vendor.qti.ims.rcsuce@1.1
IMS += vendor.qti.ims.rcsuce-V1.1-java
IMS += vendor.qti.ims.rcsuce@1.2
IMS += vendor.qti.ims.rcsuce-V1.2-java
IMS += vendor.qti.ims.rcssip@1.0
IMS += vendor.qti.ims.rcssip-V1.0-java
IMS += vendor.qti.ims.rcssip@1.1
IMS += vendor.qti.ims.rcssip-V1.1-java
IMS += vendor.qti.ims.rcssip@1.2
IMS += vendor.qti.ims.rcssip-V1.2-java


PRODUCT_PACKAGES += $(IMS_RTP)
PRODUCT_PACKAGES += $(CNE)
PRODUCT_PACKAGES += $(CACERT)
PRODUCT_PACKAGES += $(DPM)
PRODUCT_PACKAGES += $(IMS)
