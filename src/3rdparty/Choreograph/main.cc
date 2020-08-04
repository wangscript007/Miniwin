#include "choreograph/Choreograph.h"

using namespace choreograph;
int main(int argc,char*argv[]){
    Timeline timeline;
    Output<float> target = 0.0f;

    PhraseRef<float> l2r=ch::makeRamp(.0f,2.f,1.0f,EaseInOutQuad());
    {
    timeline.apply( &target,l2r)
    .updateFn([&target](){printf("target=%.3f\r\n",target.value());});
    }
    printf("timeline.size=%d\r\n",timeline.size());
    for(int i=1;i<=101;i++){
       timeline.jumpTo( 0.01*i);
    }
    printf("timeline.size=%d\r\n",timeline.size());
    return 0;
}

