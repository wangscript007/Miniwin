#include <config.h>
#include <gif_lib.h>
#include <malloc.h>
#include <string.h>

#define  argb(a, r, g, b) ( ((a) & 0xff) << 24 ) | ( ((r) & 0xff) << 16 ) | ( ((g) & 0xff) << 8 ) | ((b) & 0xff)

#define  dispose(ext) (((ext)->Bytes[0] & 0x1c) >> 2)
#define  trans_index(ext) ((ext)->Bytes[3])
#define  transparency(ext) ((ext)->Bytes[0] & 1)
#define  delay(ext) (10*((ext)->Bytes[2] << 8 | (ext)->Bytes[1]))

//https://github.com/wangchao0837/GifPlayer

typedef struct GifBean {
    int current_frame;
    int total_frame;
    int *delays;
} GifBean;

int gifDrawFrame(void*gifhandle,size_t pxstride, void *pixels,bool force_dispose_1) {
    GifColorType *bg;
    GifColorType *color;
    SavedImage *frame;
    ExtensionBlock *ext = 0;
    GifImageDesc *frameInfo;
    ColorMapObject *colorMap;
    int *line;
    int width, height, x, y, j, loc, n, inc, p;
    void *px;
    GifFileType*gif=(GifFileType*)gifhandle;
    GifBean*bean=(GifBean*)gif->UserData;
    width = gif->SWidth;
    height = gif->SHeight;
    frame = &(gif->SavedImages[bean->current_frame]);
    frameInfo = &(frame->ImageDesc);
    if (frameInfo->ColorMap) {
        colorMap = frameInfo->ColorMap;
    } else {
        colorMap = gif->SColorMap;
    }

    bg = &colorMap->Colors[gif->SBackGroundColor];
    for (j = 0; j < frame->ExtensionBlockCount; j++) {
        if (frame->ExtensionBlocks[j].Function == GRAPHICS_EXT_FUNC_CODE) {
            ext = &(frame->ExtensionBlocks[j]);
            break;
        }
    }
    // For dispose = 1, we assume its been drawn
    px = pixels;
    if (ext && dispose(ext) == 1 && force_dispose_1 && bean->current_frame > 0) {
        bean->current_frame = bean->current_frame - 1,
                 gifDrawFrame(gif,pxstride, pixels, true);
    } else if (ext && dispose(ext) == 2 && bg) {
        for (y = 0; y < height; y++) {
            line = (int *) px;
            for (x = 0; x < width; x++) {
                line[x] = argb(255, bg->Red, bg->Green, bg->Blue);
            }
            px = (int *) ((char *) px + pxstride);
        }
        bean->current_frame=(bean->current_frame+1)%gif->ImageCount;
    } else if (ext && dispose(ext) == 3 && bean->current_frame > 1) {
        bean->current_frame = bean->current_frame - 2,
                 gifDrawFrame(gif,pxstride, pixels, true);
    }else bean->current_frame=(bean->current_frame+1)%gif->ImageCount;
    px = pixels;
    if (frameInfo->Interlace) {
        n = 0;
        inc = 8;
        p = 0;
        px = (int *) ((char *) px + pxstride * frameInfo->Top);
        for (y = frameInfo->Top; y < frameInfo->Top + frameInfo->Height; y++) {
            for (x = frameInfo->Left; x < frameInfo->Left + frameInfo->Width; x++) {
                loc = (y - frameInfo->Top) * frameInfo->Width + (x - frameInfo->Left);
                if (ext && frame->RasterBits[loc] == trans_index(ext) && transparency(ext)) {
                    continue;
                }
                color = (ext && frame->RasterBits[loc] == trans_index(ext)) ? bg
                        : &colorMap->Colors[frame->RasterBits[loc]];
                if (color)
                    line[x] = argb(255, color->Red, color->Green, color->Blue);
            }
            px = (int *) ((char *) px + pxstride * inc);
            n += inc;
            if (n >= frameInfo->Height) {
                n = 0;
                switch (p) {
                case 0:
                    px = (int *) ((char *) pixels + pxstride * (4 + frameInfo->Top));
                    inc = 8;     p++;     break;
                case 1:
                    px = (int *) ((char *) pixels + pxstride * (2 + frameInfo->Top));
                    inc = 4;     p++;     break;
                case 2:
                    px = (int *) ((char *) pixels + pxstride * (1 + frameInfo->Top));
                    inc = 2;     p++;     break;
                }
            }
        }
    } else {
        px = (int *) ((char *) px + pxstride * frameInfo->Top);
        for (y = frameInfo->Top; y < frameInfo->Top + frameInfo->Height; y++) {
            line = (int *) px;
            for (x = frameInfo->Left; x < frameInfo->Left + frameInfo->Width; x++) {
                loc = (y - frameInfo->Top) * frameInfo->Width + (x - frameInfo->Left);
                if (ext && frame->RasterBits[loc] == trans_index(ext) && transparency(ext)) {
                    continue;
                }
                color = (ext && frame->RasterBits[loc] == trans_index(ext)) ? bg
                        : &colorMap->Colors[frame->RasterBits[loc]];
                if (color)
                    line[x] = argb(255, color->Red, color->Green, color->Blue);
            }
            px = (int *) ((char *) px + pxstride);
        }
    }
    return delay(ext);
}

void *gifload(const char*path) {
    int i,j,err;
    GifFileType *gifFileType = DGifOpenFileName(path, &err);
    if(NULL==gifFileType)return NULL;
    GifBean *gifBean = (GifBean *) malloc(sizeof(GifBean));
    DGifSlurp(gifFileType);

    memset((void*)gifBean, 0, sizeof(GifBean));
    gifFileType->UserData = gifBean;

    gifBean->delays = (int *) malloc(sizeof(int) * gifFileType->ImageCount);
    memset(gifBean->delays, 0, sizeof(int) * gifFileType->ImageCount);
    gifBean->total_frame = gifFileType->ImageCount;
    ExtensionBlock *ext;

    for (i = 0; i < gifFileType->ImageCount; ++i) {
        SavedImage frame = gifFileType->SavedImages[i];
        for (j = 0; j < frame.ExtensionBlockCount; ++j) {
            if (frame.ExtensionBlocks[j].Function == GRAPHICS_EXT_FUNC_CODE) {
                ext = &frame.ExtensionBlocks[j];
                break;
            }
        }
        if (ext) {
            int frame_delay = 10 * (ext->Bytes[2] << 8 | ext->Bytes[1]);
            gifBean->delays[i] = frame_delay;
        }
    }
    return gifFileType;
}

void gifunload(void*handle){
    GifFileType *gifFileType =(GifFileType *)handle;
    GifBean*bean=(GifBean*)gifFileType->UserData;
    free(bean->delays);
    free(bean);
    DGifCloseFile(gifFileType);     
}

void gifgetinfo(void*handle,int*w,int*h,int*frames,int*delays){
    GifFileType *gif =(GifFileType *)handle;
    GifBean*bean=(GifBean*)gif->UserData;
    *w=gif->SWidth;
    *h=gif->SHeight;
    *frames=gif->ImageCount;
    if(delays)memcpy(delays,bean->delays,sizeof(int)*gif->ImageCount);
}
void gifgetnextframe(void*handle,int*idx){
    GifFileType *gif =(GifFileType *)handle;
    GifBean*bean=(GifBean*)gif->UserData;
    *idx=bean->current_frame;
}
#if 0
extern "C"
JNIEXPORT jint JNICALL
Java_com_dongnao_ndkplayer_GifHandler_getWidth__J(JNIEnv *env, jobject instance, jlong ndkGif) {

    GifFileType *gifFileType = (GifFileType *) ndkGif;
    return gifFileType->SWidth;

}
extern "C"
JNIEXPORT jint JNICALL
Java_com_dongnao_ndkplayer_GifHandler_getHeight__J(JNIEnv *env, jobject instance, jlong ndkGif) {
    GifFileType *gifFileType = (GifFileType *) ndkGif;
    return gifFileType->SHeight;

}
extern "C"
JNIEXPORT jint JNICALL
Java_com_dongnao_ndkplayer_GifHandler_updateFrame__JLandroid_graphics_Bitmap_2(JNIEnv *env,
        jobject instance,
        jlong ndkGif,
        jobject bitmap) {

    //强转代表gif图片的结构体
    GifFileType *gifFileType = (GifFileType *) ndkGif;
    GifBean *gifBean = (GifBean *) gifFileType->UserData;
    AndroidBitmapInfo info;
    //代表一幅图片的像素数组
    void *pixels;
    AndroidBitmap_getInfo(env, bitmap, &info);
    //锁定bitmap  一幅图片--》二维 数组   ===一个二维数组
    AndroidBitmap_lockPixels(env, bitmap, &pixels);

    // TODO
    drawFrame(gifFileType, gifBean, info, pixels, false);

    //播放完成之后   循环到下一帧
    gifBean->current_frame += 1;
    LOGE("当前帧  %d  ", gifBean->current_frame);
    if (gifBean->current_frame >= gifBean->total_frame - 1) {
        gifBean->current_frame = 0;
        LOGE("重新过来  %d  ", gifBean->current_frame);
    }
    AndroidBitmap_unlockPixels(env, bitmap);
    return gifBean->delays[gifBean->current_frame];

}
#endif
