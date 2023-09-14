/*
  Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @description : Implementation of aosp functions required
                 for sigma certification
*/
#include "gui/Surface.h"
#include <gui/ISurfaceComposer.h>
#include <gui/SurfaceComposerClient.h>
#include <ui/DisplayState.h>
#include <ui/DisplayConfig.h>
#include <ui/Rotation.h>
#include "common_log.h"
#include "sigmautils.h"

using namespace android;

static sp<IBinder> dpy = NULL;
static sp<IGraphicBufferProducer> bufferProducer = NULL;


static struct S
{
    sp<SurfaceComposerClient> client;
    sp<SurfaceControl> surfaceControl;
    sp<Surface> surface;
    ~S()
    {
        client->dispose();
    }
} *s = NULL;

void deleteSurfaceUtil()
{
    if (s != NULL)
    delete s;
    s=NULL;
}

/* Function: getSurface
 * description: Creates a surface for client
 * parameters : none
 * return value: surface handle
 */
android::Surface* getSurface()
{
    LOGE("Retrieving surface \n");
    s = new S;
    if (s == NULL)
    {
        //LOGD("Mem Alloc for Struct S failed");
    }
    if (s != NULL)
    {
        // create a client to surfaceflinger
        s->client = new SurfaceComposerClient();

        if (s->client != NULL && s->client->initCheck() == NO_ERROR)
        {
            LOGE("Retrieving surfaceComposerClient \n");
            sp<IBinder> primaryDisplay = SurfaceComposerClient::getInternalDisplayToken();
            if(primaryDisplay)
            {
                ui::DisplayState mainDpyState;
                status_t ret = s->client->getDisplayState(primaryDisplay, &mainDpyState);
                sp<IBinder> m_mainDpy = SurfaceComposerClient::getInternalDisplayToken();
                DisplayConfig mainDpyConfig;
                status_t ret1 = SurfaceComposerClient::getActiveDisplayConfig(m_mainDpy, &mainDpyConfig);
                ssize_t displayWidth = 800;
                ssize_t displayHeight = 640;
                if (ret == NO_ERROR && ret1 == NO_ERROR) {
                  if (mainDpyState.orientation == ui::ROTATION_90
                     || mainDpyState.orientation == ui::ROTATION_270) {
                     displayHeight = mainDpyConfig.resolution.width;
                     displayWidth = mainDpyConfig.resolution.height;
                  } else {
                     displayWidth = mainDpyConfig.resolution.width;
                     displayHeight = mainDpyConfig.resolution.height;
                  }
                }
               if (displayWidth == 0)
                   displayWidth = 800;
               if (displayHeight  == 0)
                   displayHeight = 480;
                s->surfaceControl = s->client->createSurface(
                                                             String8("Wfd Sink Surface"),
                                                             displayWidth,
                                                             displayHeight,
                                                             PIXEL_FORMAT_RGBA_8888,
                                                             0);
                if (s->surfaceControl == NULL || !s->surfaceControl->isValid()) {
                    // LOGE("surface control is NULL or not valid, returning NULL");
                    LOGE("Retrieving surface Control failed\n");
                    return NULL;
                }
                SurfaceComposerClient::Transaction{}
                    .setLayer(s->surfaceControl, INT_MAX-1)
                    .show(s->surfaceControl)
                    .apply();

                s->surface = s->surfaceControl->getSurface();
                if (s->surface != NULL)
                {
                    return (s->surface.get());
                }
            }
            else
            {
                LOGE("Failed to find any physcial display!\n");
            }
        }
        else
        {
           // LOGE("SurfaceComposerClient is NULL or error when initializing");
        }
    }
    return NULL;
}

void setWFDSurface(WiFiDisplayClient *gWFDClient)
{
    int ret = gWFDClient->setSurface(getSurface());
    LOGE("setsurface returned %d", ret);
}

int configureVirtualDisplay(const sp<IBinder>& m_mainDpy,const ui::DisplayState& mainDpyState,
        const sp<IGraphicBufferProducer>& bufferProducer,
        sp<IBinder>* pDisplayHandle, mmSessionInfo* pMMInfo) {

    /*-------------------------------------------------------------------------
    Input rectangle settings:

    layerStackRect defines which area of the window manager coordinate space
    will be used.

    inputRect is the region of primary that needs to be captured, which for WFD
    is the entire screen. Set the correct value based on the orientation

    ---------------------------------------------------------------------------
    */
    bool deviceRotated = mainDpyState.orientation == ui::ROTATION_90 ||
            mainDpyState.orientation == ui::ROTATION_270;
    uint32_t sourceWidth, sourceHeight;
    DisplayConfig mainDpyConfig;
    status_t ret1 = SurfaceComposerClient::getActiveDisplayConfig(m_mainDpy, &mainDpyConfig);
    if (!deviceRotated) {
        sourceWidth = mainDpyConfig.resolution.width;
        sourceHeight = mainDpyConfig.resolution.height;
    } else {
        sourceHeight = mainDpyConfig.resolution.width;
        sourceWidth = mainDpyConfig.resolution.height;
    }

    if(!sourceWidth || !sourceHeight)
    {
        fprintf(stderr,"Invalid parameters.\n");
        return -1;
    }
    Rect inputRect(sourceWidth, sourceHeight);

    fprintf(stderr,"Source Width = %d , Height = %d .\n",
        sourceWidth,sourceHeight);

    /*-------------------------------------------------------------------------
    Output rectangle calculations:

    displayRect defines where on the display will layerStackRect be mapped to.
    displayRect is specified postorientation, i.e. it uses the orientation seen
    by the end-user

    outputRect is the output area for display. It's our onus to set the correct
    offset values of the rectangle in an endeavour to retain the aspect ratio
    of the primary. The calculations are pretty self-explanatory

    ---------------------------------------------------------------------------
    */

    float aspectRatio = sourceWidth*1.0/sourceHeight;

    uint32_t videoWidth, videoHeight;
    uint32_t outWidth, outHeight;

    videoWidth  = pMMInfo->width;
    videoHeight = pMMInfo->height;

    if (videoWidth > (uint32_t)(videoHeight* aspectRatio)) {
        // limited by short height; trim width
        outHeight= videoHeight;
        outWidth= (uint32_t)(outHeight * aspectRatio);
    } else {
        // limited by narrow width; reduce height
        outWidth = videoWidth;
        outHeight= (uint32_t)(outWidth / aspectRatio);
    }
    uint32_t offX, offY;
    offX = (videoWidth - outWidth) / 2;
    offY = (videoHeight - outHeight) / 2;
    Rect outputRect(offX, offY, offX + outWidth, offY + outHeight);

    LOGE("Destination Width = %d , Height = %d ", outWidth, outHeight);
    LOGE("with X offset = %d , Y offset = %d\n", offX, offY);

    sp<IBinder> dpy = SurfaceComposerClient::createDisplay(
            String8("venc-widi"), static_cast<bool>(pMMInfo->hdcp));
    if(dpy.get() == NULL)
    {
        fprintf(stderr,"Unable to create Virtual Display");
        return -1;
    }

    SurfaceComposerClient::Transaction t;
    t.setDisplaySurface(dpy, bufferProducer);
    t.setDisplayProjection(dpy, ui::ROTATION_0, inputRect, outputRect);
    t.setDisplayLayerStack(dpy, 0);    // default stack
    t.apply();
    *pDisplayHandle = dpy;
    LOGE("Done with display configuration \n");
    return 0;
}

int createVDS(void* pInfo)
{
    int err = 0;
    mmSessionInfo *pMMInfo = reinterpret_cast<mmSessionInfo *>(pInfo);
    if(pMMInfo && pMMInfo->producer)
    {
        LOGE("Source module provided surface\n");
        bufferProducer = reinterpret_cast
        <IGraphicBufferProducer*>(pMMInfo->producer);
        sp<IBinder> mainDpy = SurfaceComposerClient::getInternalDisplayToken();
        if(mainDpy)
        {
            ui::DisplayState mainDpyState;
            err = SurfaceComposerClient::getDisplayState(mainDpy,
                                                        &mainDpyState);
            if (err)
            {
                LOGE("Can't get display state %d\n", err);
                return err;
            }
            err = configureVirtualDisplay(mainDpy,mainDpyState, bufferProducer, &dpy, pMMInfo);
            if(err)
            {
                LOGE("Can't configure VDS err = %d",err);
                return err;
            }
        }
        else
        {
            LOGE("Failed to find any physcial display!\n");
        }
    }
    else
    {
        LOGE("\n Invalid argument passed to createVDS!");
        return -1;
    }
    return 0;
}

void releaseVDS()
{

    if(dpy != NULL)
    {
        /*---------------------------------------------------------
        Stop the producer before destroying the consumer. Surface
        Media Source hijacks the display destroyed event realizing that
        prodcuer is disconnecting from the BufferQueue and that it should stop
        the recording. This will prevent calling stop on SMS from hanging.
        -----------------------------------------------------------
        */
        bufferProducer = NULL;
        SurfaceComposerClient::destroyDisplay(dpy);
        LOGE("Done with display destruction");
        dpy = NULL;
    }
}
