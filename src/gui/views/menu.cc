#include <menu.h>
namespace nglui{

MenuItem::MenuItem(){
   mEnabled=true;
   mVisible=true;
   mSubMenu=nullptr;
   mGroupId=-1;
   mItemId=-1;
   mOrder=-1;
}

MenuItem& MenuItem::setEnabled(bool v){
    mEnabled=v;
    return *this;
}

MenuItem& MenuItem::setTitle(const std::string&txt){
    mTitle=txt;
    return *this;
}

MenuItem& MenuItem::setVisible(bool v){
    mVisible=v;
    return *this;
}

MenuItem& MenuItem::setTooltipText(const std::string&tip){
    mTooltip=tip;
    return *this;
}

SubMenu* MenuItem::getSubMenu()const{
    return mSubMenu.get();
}

bool MenuItem::hasSubMenu()const{
    return mSubMenu&&mSubMenu->size();
}

//###############################

Menu::Menu(){
}

Menu::~Menu(){
   clear();
}

MenuItem& Menu::add(const std::string&title){
   return add(-1,-1,-1,title);
}

MenuItem& Menu::add(int groupId, int itemId, int order,const std::string&title){
   MenuItem*itm=new MenuItem(); 
   itm->setTitle(title);
   itm->mGroupId=groupId;
   itm->mItemId=itemId;
   itm->mOrder=order;
   mMenuItems.push_back(itm);
   return *itm;
}

SubMenu& Menu::addSubMenu(const std::string&title){
   return addSubMenu(-1,-1,-1,title);
}

SubMenu& Menu::addSubMenu(int groupId,int itemId,int order,const std::string& title){
   MenuItem&itm=add(groupId,itemId,order,title);
   itm.mSubMenu=std::make_shared<Menu>();
   return *itm.mSubMenu.get();
}

void Menu::removeItem(int id){
}

void Menu::removeGroup(int groupId){
}

void Menu::clear(){
}

void Menu::setGroupCheckable(int group, bool checkable, bool exclusive){
}

void Menu::setGroupEnabled(int group, bool enabled){
}

bool Menu::hasVisibleItems()const{
}

MenuItem*Menu::findItem(int id)const{
}

int Menu::size()const{
}

MenuItem* Menu::getItem(int index)const{
}

void Menu::close(){
}

}//end namespace


