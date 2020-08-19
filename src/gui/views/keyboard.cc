#include<keyboard.h>
#include<json/json.h>

namespace nglui{
//keycode data from: https://github.com/mBut/jquery.mlkeyboard/tree/master/src/layouts
//the best code(many language):  https://github.com/EdOverride/multi-lang-keyboard/tree/master/dist/keyboards
typedef KeyboardView KV;
typedef struct{
   UINT c1;
   UINT c2;
   USHORT w;
   USHORT h;
   const char*text;
}KEYDEF;
extern KEYDEF US[];

Keyboard::Keyboard(int x,int y,int w,int h):Window(x,y,w,h,TYPE_KEYGUARD){
    kv=new KeyboardView(w,h-8);
    setBgColor(0xFF000010);
    kv->setFgColor(0xFFFFFFFF);
    kv->setBgColor(0xFF000000);
    addChildView(kv);
    for(int i=0;i<56;i++){
        KEYDEF &k=US[i];
        USHORT w=(k.c1==KV::KC_NONE)?k.w:(k.w==0?1:k.w)*40;
        USHORT h=(k.c1==KV::KC_NONE)?k.h:(k.h==0?1:k.h)*40;
        if(k.c1==KV::KC_NONE&&k.w==0)h=42;
        kv->addKey(k.c1,k.c2,w,h,k.text);
    }invalidate(nullptr);
}

void Keyboard::setBuddy(EditText*buddy){
    kv->setBuddy(buddy);
}

int Keyboard::setLanguage(const std::string&lan){
   return 0;
}

KEYDEF US[]={//English (US)
    { '`', '~' },    { '1', '!' },   { '2', '@' },   { '3', '#' },   { '4', '$' },    { '5', '%' },   { '6', '^' },    { '7', '&' },   { '8', '*' },    
    { '9', '(' },    { '0', ')' },   { '-', '_' },   { '=', '+' },   {KV::KC_NONE,KV::KC_NONE},  {KV::KC_NONE,KV::KC_NONE,20},

    { 'q', 'Q'  },   { 'w', 'W' },   { 'e', 'E' },   { 'r', 'R' },   { 't', 'T' },    { 'y', 'Y' },   { 'u', 'U' },    { 'i', 'I' },   { 'o', 'O'  },
    { 'p', 'P'  },   { '[', '{' },   { ']', '}' },   { '\\','|'  },  {KV::KC_NONE,KV::KC_NONE},

    { 'a', 'A'  },   { 's', 'S' },   { 'd', 'D' },   { 'f', 'F'  },  { 'g', 'G' },    { 'h', 'H' },   { 'j', 'J' },    { 'k', 'K' },   { 'l', 'L'  },
    { ';', ':'  },   { '\'','\"'},   {KV::KC_NONE,KV::KC_NONE},      {KV::KC_NONE,KV::KC_NONE,20},

    {'z', 'Z',  },   {'x', 'X',  },   {'c', 'C',},   {'v',  'V', },  {'b', 'B', },    {'n', 'N', },   {'m', 'M', },    {',', '<', },   {'.', '>',  },
    {'/', '?',  },   {KV::KC_NONE,KV::KC_NONE},

    {KV::KC_CAPS,KV::KC_CAPS,2,0,"CAP"},{0x20,0x20,6,0,"Space"},{KV::KC_LANS,KV::KC_LANS,4,0,"Language"}

 };

KEYDEF RU[]={//Russion
   {0xb4  ,   '~' }, {0x451 ,  0x401}, {'1'  ,    '!' }, {'2' ,    '\"' }, {'3',  0x2116 }, {'4' ,   ';' }, {'5',   '%' }, {'6',   ':'},
   {'7'  ,    '?' }, {'8'   ,   '*' }, {'9'  ,    '(' }, {'0' ,    ')'  }, {'-',  '_'    }, {'=' ,   '+' }, {0, 0},

   {0x0439, 0x0419}, {0x0446, 0x0426}, {0x0443, 0x0423}, {0x043A, 0x041A}, {0x0435,0x0415}, {0x043D, 0x041D}, {0x0433, 0x0413}, {0x0448, 0x0428},
   {0x0449, 0x0429}, {0x0437, 0x0417}, {0x0445, 0x0425}, {0x044A, 0x042A}, {'\\',   '/'  }, {0,     0 },

   {0x0444, 0x0424}, {0x044B, 0x042B}, {0x0432, 0x0412}, {0x0430, 0x0410}, {0x043F,0x041F}, {0x0440, 0x0420}, {0x043E, 0x041E}, {0x043B, 0x041B},
   {0x0434, 0x0414}, {0x0436, 0x0416}, {0x044D, 0x042D}, {KV::KC_NONE,KV::KC_NONE},

   {'/',    '|'   }, {0x044F, 0x042F}, {0x0447, 0x0427}, {0x0441, 0x0421}, {0x043C,0x041C}, {0x0438, 0x0418}, {0x0442, 0x0422}, {0x044C, 0x042C},
   {0x0431, 0x0411}, {0x044E, 0x042E}, {'.',      ',' }, {KV::KC_NONE,KV::KC_NONE}
};

KEYDEF IT[]={//Italian 
    {'\\',  '|'  },   {'1',   '!' },   {'2',   '\"'},   {'3',  0xa3 },   {'4',   '$' },   {'5',   '%' },   {'6',   '&' },   {'7',   '/' },
    {'8',   '('  },   {'9',   ')' },   {'0',   '=' },   {'\'',  '?' },   {0xec,  '^' },   {0,  0 },

    {'q',   'Q'  },   {'w',   'W' },   {'e',   'E' },   {'r',   'R' },   {'t',   'T' },   {'y',   'Y' },   {'u',   'U' },   {'i',   'I' },
    {'o',   'O'  },   {'p',   'P' },   {0xe8,  0xe9},   {'+',   '*' },   {0xf9,  0xa7},   {0,  0 },

    {'a',  'A'  },    {'s',  'S'  },   {'d',  'D'  },   {'f',  'F'  },   {'g',   'G' },   {'h',  'H'  },   {'j',  'J'  },   {'k',  'K'  },
    {'l',  'L'  },    {0xf2, 0xe7 },   {0xe0, 0xb0 },   {0, 0 },

    {'<',  '>'  },    {'z',  'Z'  },   {'x',  'X'  },   {'c',  'C'  },   {'v',   'V' },   {'b',  'B'  },   {'n',  'N'  },   {'m',  'M'  },
    {',',  ';'  },    {'.',  ':'  },   {'-',  '_'  },   {0, 0}
};

KEYDEF ES[]={//Spanish 
    {0xba, 0xaa },  {'1',    '!'},    {'2',  '\"' },    {'3',  '\'' },    {'4',   '$' },    {'5',   '%' },    {'6',   '&' },    {'7',   '/' },
    {'8',   '(' },  {'9',   ')' },    {'0',   '=' },    {'\'',   '?' },   {0xa1,  0xbf},    {0,  0    },

    {'q',   'Q' },  {'w',   'W' },    {'e',   'E' },    {'r',   'R' },    {'t',   'T' },    {'y',   'Y' },    {'u',   'U' },    {'i',   'I' },
    {'o',   'O' },  {'p',   'P' },    {'`',   '^' },    {'+',   '*' },    {0xe7, 0xc7 },    {0,    0 },

    {'a',   'A' },  {'s',   'S' },    {'d',   'D' },    {'f',   'F' },    {'g',   'G' },    {'h',   'H' },    {'j',   'J' },    {'k',   'K' },
    {'l',   'L' },  {0xf1, 0xd1 },    {0xb4, 0xa8 },    {0,  0    },

    {'<',   '>' },  {'z',   'Z' },    {'x',   'X' },    {'c',   'C' },    {'v',   'V' },    {'b',   'B' },    {'n',   'N' },    {'m',  'M'  },   
    {',',  ';' },   {'.',   ':' },   {'-',   '_' }

};

}//namespace
