#ifndef __NGL_UIEVENTS_H__
#define __NGL_UIEVENTS_H__
#include <string>
#include <eventcodes.h>
#include <bitset.h>
#include <stdint.h>
#include <vector>
#include <queue>

typedef __int64_t nsecs_t;

namespace nglui{

struct PointerCoords {
    enum { MAX_AXES = 30 }; // 30 so that sizeof(PointerCoords) == 128

    // Bitfield of axes that are present in this structure.
    uint64_t bits __attribute__((aligned(8)));

    // Values of axes that are stored in this structure packed in order by axis id
    // for each axis that is present in the structure according to 'bits'.
    float values[MAX_AXES];

    inline void clear() {
        BitSet64::clear(bits);
    }

    bool isEmpty() const {
        return BitSet64::isEmpty(bits);
    }

    float getAxisValue(int32_t axis) const;
    int setAxisValue(int32_t axis, float value);

    void scale(float scale);
    void applyOffset(float xOffset, float yOffset);

    inline float getX() const {
        return getAxisValue(ABS_X);
    }

    inline float getY() const {
        return getAxisValue(ABS_Y);
    }

    bool operator==(const PointerCoords& other) const;
    inline bool operator!=(const PointerCoords& other) const {
        return !(*this == other);
    }

    void copyFrom(const PointerCoords& other);

private:
    void tooManyAxes(int axis);
};

/*
 * Pointer property data.
 */
struct PointerProperties {
    // The id of the pointer.
    int32_t id;

    // The pointer tool type.
    int32_t toolType;

    inline void clear() {
        id = -1;
        toolType = 0;
    }

    bool operator==(const PointerProperties& other) const;
    inline bool operator!=(const PointerProperties& other) const {
        return !(*this == other);
    }

    void copyFrom(const PointerProperties& other);
};
enum {
   INPUTSOURCE_KEY=1,
   INPUTSOURCE_TOUCH=2,
};
class InputEvent{
protected:
   int mDeviceId;
   int mSource;
public:   
   enum{
     EVENT_TYPE_KEY = 1,
     EVENT_TYPE_MOTION = 2,
     EVENT_TYPE_FOCUS = 3
   };
   virtual int getType()=0;
   void initialize(int32_t deviceId, int32_t source);
   void initialize(const InputEvent& from);
   void setSource(int source){mSource=source;}
};

class KeyEvent:public InputEvent{
private:
    int32_t mAction;
    int32_t mFlags;
    int32_t mKeyCode;
    int32_t mScanCode;
    int32_t mMetaState;
    int32_t mRepeatCount;
    nsecs_t mDownTime;
    nsecs_t mEventTime;
public:
   void initialize(
            int32_t deviceId,
            int32_t source,
            int32_t action,
            int32_t flags,
            int32_t keyCode,
            int32_t scanCode,
            int32_t metaState,
            int32_t repeatCount,
            nsecs_t downTime,
            nsecs_t eventTime);
   void initialize(const KeyEvent& from);
   virtual int getType(){return EV_KEY;}
   int getKeyCode() {return mKeyCode;}
   void setKeyCode(int k){mKeyCode=k;}
   int getFlags(){return mFlags;}
   inline int32_t getScanCode() const { return mScanCode; }
   inline int32_t getMetaState() const { return mMetaState; } 
   int getAction(){return mAction;}//key up-->0 down-->1
   int getRepeatCount(){return mRepeatCount;}
   inline nsecs_t getDownTime() const { return mDownTime; }
   inline nsecs_t getEventTime() const { return mEventTime; }
   static const char*getLabel(int key);
   static int getKeyCodeFromLabel(const char*label);
public:
   enum{
     ACTION_DOWN = 0,
     ACTION_UP = 1,
     ACTION_MULTIPLE = 2
   };
   enum{
     AKEY_STATE_UNKNOWN = -1,
     AKEY_STATE_UP = 0,
     AKEY_STATE_DOWN = 1,
     AKEY_STATE_VIRTUAL = 2
   };
};

class MotionEvent:public InputEvent{
protected:
   int32_t mAction;
   int32_t mActionButton;
   int32_t mFlags;
   int32_t mEdgeFlags;
   int32_t mMetaState;
   int32_t mButtonState;
   float mXOffset;
   float mYOffset;
   float mXPrecision;
   float mYPrecision;
   nsecs_t mDownTime;
   std::vector<PointerProperties> mPointerProperties;
   std::vector< nsecs_t > mSampleEventTimes;
   std::vector<PointerCoords> mSamplePointerCoords;
public:
   MotionEvent(){}
   MotionEvent(const MotionEvent&m);
   void initialize(int deviceId,int source,int action,int actionButton,
            int flags, int edgeFlags,int metaState,   int buttonState,
            float xOffset, float yOffset, float xPrecision, float yPrecision,
            nsecs_t downTime, nsecs_t eventTime, size_t pointerCount,
            const PointerProperties* pointerProperties,const PointerCoords* pointerCoords);

   void copyFrom(const MotionEvent* other, bool keepHistory);
   virtual int getType(){return EV_ABS;}
   inline int32_t getAction() const { return mAction;}  
   inline void setAction(int32_t action) { mAction = action; }
   inline int32_t getActionMasked() const { return mAction &ACTION_MASK; }
   inline int32_t getFlags() const { return mFlags; }
   inline void setFlags(int32_t flags) { mFlags = flags; }
   inline int32_t getEdgeFlags() const { return mEdgeFlags; }
   inline void setEdgeFlags(int32_t edgeFlags) { mEdgeFlags = edgeFlags; }
   inline int32_t getMetaState() const { return mMetaState; }
   inline void setMetaState(int32_t metaState) { mMetaState = metaState; }
   inline int32_t getButtonState() const { return mButtonState; }
   inline void setButtonState(int32_t buttonState) { mButtonState = buttonState; }
   inline int32_t getActionButton() const { return mActionButton; }
   inline void setActionButton(int32_t button) { mActionButton = button; }
   inline float getXOffset() const { return mXOffset; }
   inline float getYOffset() const { return mYOffset; }
   inline float getXPrecision() const { return mXPrecision; }
   inline float getYPrecision() const { return mYPrecision; }
   inline size_t getHistorySize() const { return mSampleEventTimes.size() - 1; }
   inline nsecs_t getHistoricalEventTime(size_t historicalIndex) const {
       return mSampleEventTimes[historicalIndex];
   }
   const PointerCoords* getHistoricalRawPointerCoords(size_t pointerIndex, size_t historicalIndex) const;
   float getHistoricalRawAxisValue(int32_t axis, size_t pointerIndex,size_t historicalIndex) const;

   inline size_t getPointerCount() const { return mPointerProperties.size(); }
   inline const PointerProperties* getPointerProperties(size_t pointerIndex) const {
        return &mPointerProperties[pointerIndex];
   }
   void addSample(nsecs_t eventTime, const PointerCoords* pointerCoords);
   void offsetLocation(float xOffset, float yOffset);
   void scale(float scaleFactor);

    // Apply 3x3 perspective matrix transformation.
    // Matrix is in row-major form and compatible with SkMatrix.
   void transform(const float matrix[9]);
   inline int32_t getPointerId(size_t pointerIndex) const {
        return mPointerProperties[pointerIndex].id;
   }

   inline int32_t getToolType(size_t pointerIndex) const {
        return mPointerProperties[pointerIndex].toolType;
   }
   inline nsecs_t getEventTime() const { return mSampleEventTimes[getHistorySize()]; }
   const PointerCoords* getRawPointerCoords(size_t pointerIndex) const;
   float getRawAxisValue(int32_t axis, size_t pointerIndex) const;
   inline float getRawX(size_t pointerIndex) const {
       return getRawAxisValue(ABS_X, pointerIndex);
   }

   inline float getRawY(size_t pointerIndex) const {
       return getRawAxisValue(ABS_Y, pointerIndex);
   }
   float getAxisValue(int32_t axis, size_t pointerIndex)const;
   float getX(size_t pointer=0){return getAxisValue(ABS_X,pointer);}
   float getY(size_t pointer=0){return getAxisValue(ABS_Y,pointer);}
   inline float getPressure(size_t pointerIndex) const {
       return getAxisValue(AXIS_PRESSURE, pointerIndex);
   }
   inline float getSize(size_t pointerIndex) const {
       return getAxisValue(AXIS_SIZE, pointerIndex);
   }
   inline float getTouchMajor(size_t pointerIndex) const {
       return getAxisValue(AXIS_TOUCH_MAJOR, pointerIndex);
   }
   inline float getTouchMinor(size_t pointerIndex) const {
       return getAxisValue(AXIS_TOUCH_MINOR, pointerIndex);
   }
   inline float getToolMajor(size_t pointerIndex) const {
       return getAxisValue(AXIS_TOOL_MAJOR, pointerIndex);
   }
   inline float getToolMinor(size_t pointerIndex) const {
       return getAxisValue(AXIS_TOOL_MINOR, pointerIndex);
   }
   inline float getOrientation(size_t pointerIndex) const {
       return getAxisValue(AXIS_ORIENTATION, pointerIndex);
   }
   ssize_t findPointerIndex(int32_t pointerId) const;
public:
   enum{
     ACTION_MASK = 0xff,
     ACTION_POINTER_INDEX_MASK = 0xff00,
     ACTION_DOWN = 0,
     ACTION_UP = 1,
     ACTION_MOVE = 2,
     ACTION_CANCEL = 3,
     ACTION_OUTSIDE = 4,
     ACTION_POINTER_DOWN = 5,
     ACTION_POINTER_UP = 6,
     ACTION_HOVER_MOVE = 7,
     ACTION_SCROLL = 8,
     ACTION_HOVER_ENTER = 9,
     ACTION_HOVER_EXIT = 10,
     ACTION_BUTTON_PRESS = 11,
     ACTION_BUTTON_RELEASE = 12
   };
   enum{
     AXIS_X = 0,
     AXIS_Y = 1,
     AXIS_PRESSURE = 2,
     AXIS_SIZE = 3,
     AXIS_TOUCH_MAJOR = 4,
     AXIS_TOUCH_MINOR = 5,
     AXIS_TOOL_MAJOR = 6,
     AXIS_TOOL_MINOR = 7,
     AXIS_ORIENTATION = 8,
     AXIS_VSCROLL = 9,
     AXIS_HSCROLL = 10,
     AXIS_Z = 11,
     AXIS_RX = 12,
     AXIS_RY = 13,
     AXIS_RZ = 14,
     AXIS_HAT_X = 15,
     AXIS_HAT_Y = 16,
     AXIS_LTRIGGER = 17,
     AXIS_RTRIGGER = 18,
     AXIS_THROTTLE = 19,
     AXIS_RUDDER = 20,
     AXIS_WHEEL = 21,
     AXIS_GAS = 22,
     AXIS_BRAKE = 23,
     AXIS_DISTANCE = 24,
     AXIS_TILT = 25,
     AXIS_SCROLL = 26,
     AXIS_RELATIVE_X = 27,
     AXIS_RELATIVE_Y = 28,
     AXIS_GENERIC_1 = 32,
     AXIS_GENERIC_2 = 33,
     AXIS_GENERIC_3 = 34,
     AXIS_GENERIC_4 = 35,
     AXIS_GENERIC_5 = 36,
     AXIS_GENERIC_6 = 37,
     AXIS_GENERIC_7 = 38,
     AXIS_GENERIC_8 = 39,
     AXIS_GENERIC_9 = 40,
     AXIS_GENERIC_10 = 41,
     AXIS_GENERIC_11 = 42,
     AXIS_GENERIC_12 = 43,
     AXIS_GENERIC_13 = 44,
     AXIS_GENERIC_14 = 45,
     AXIS_GENERIC_15 = 46,
     AXIS_GENERIC_16 = 47
   };
   enum{
     BUTTON_PRIMARY = 1 << 0,
     BUTTON_SECONDARY = 1 << 1,
     BUTTON_TERTIARY = 1 << 2,
     BUTTON_BACK = 1 << 3,
     BUTTON_FORWARD = 1 << 4,
     BUTTON_STYLUS_PRIMARY = 1 << 5,
     BUTTON_STYLUS_SECONDARY = 1 << 6
   };
};
/*
 * Input event factory.
 */
class InputEventFactoryInterface {
protected:
    virtual ~InputEventFactoryInterface() { }

public:
    InputEventFactoryInterface() { }

    virtual KeyEvent* createKeyEvent() = 0;
    virtual MotionEvent* createMotionEvent() = 0;
};

/*
 * A simple input event factory implementation that uses a single preallocated instance
 * of each type of input event that are reused for each request.
 */
class PreallocatedInputEventFactory : public InputEventFactoryInterface {
public:
    PreallocatedInputEventFactory() { }
    virtual ~PreallocatedInputEventFactory() { }

    virtual KeyEvent* createKeyEvent() { return & mKeyEvent; }
    virtual MotionEvent* createMotionEvent() { return & mMotionEvent; }

private:
    KeyEvent mKeyEvent;
    MotionEvent mMotionEvent;
};

/*
 * An input event factory implementation that maintains a pool of input events.
 */
class PooledInputEventFactory : public InputEventFactoryInterface {
public:
    PooledInputEventFactory(size_t maxPoolSize = 20);
    virtual ~PooledInputEventFactory();

    virtual KeyEvent* createKeyEvent();
    virtual MotionEvent* createMotionEvent();

    void recycle(InputEvent* event);

private:
    const size_t mMaxPoolSize;

    std::queue<KeyEvent*> mKeyEventPool;
    std::queue<MotionEvent*> mMotionEventPool;
};
}
#endif
