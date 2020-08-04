#ifndef __FAV_GROUP_H__
#define __FAV_GROUP_H__
#include <ngl_types.h>
#include <ngl_log.h>
#include <descriptors.h>
NGL_BEGIN_DECLS

/**
    @defgroup dtvapi Favorite Group
    @brief This section describes how to grouping service by favorite API
    @{
*/
/**
    @defgroup Favorite Constansts
    @brief This section describes defined the system favorite id
    @{
*/

#define GROUP_SYSTEM 0x8000     /**<Prefix of SystemGroup >*/
#define GROUP_CUSTOM 0x4000     /**<Prefix of Customized Group>*/
#define GROUP_BAT    0x2000     /**<Prefix of Bouquet Group which is groupped by bouquet id>*/
#define GROUP_FAV    0x0000     /**<Prefix of User defined favorite>*/

#define FAV_GROUP_ALL   ((GROUP_SYSTEM<<16)|0)  /**<System favorite group which include all of searched service>*/
#define FAV_GROUP_AUDIO ((GROUP_SYSTEM<<16)|1)  /**<System favorite group which include all of audio service>*/
#define FAV_GROUP_VIDEO ((GROUP_SYSTEM<<16)|2)  /**<System favorite group which include all of video service>*/
#define FAV_GROUP_AV    ((GROUP_SYSTEM<<16)|3)  /**<System favorite group which include all of audio and video service>*/
/** @} */ /*endof constants*/



/**
  @defgroup favoriteFunc Favorite Group  Functions
  @brief The Favorite functions used to manager the service's groupping
  @{
*/

/**
  This function init favorite's data ,create system favorite groups and bouquet group
  @param [in] favpath favpath is an json file which used to save userdefined favorites
  [{"id":1,"name":"foo","services":[{"netid":2,"tsid":2,"sid":3}]}]
  @retval     Favorites count @ref FavLoadData @ref FavSaveData
*/
int FavInit(const char*favpath);

/**
  The FavLoadData function used to load user defined favorites from a external json file
  @param [in] fname file path used to load data.
  @retval return Favorite group count loaded or NGL_ERROR
*/
int FavLoadData(const char*fname);

/**
  The FavSaveData function used to save user defined favorites to a external json file
  @param [in] fname file path used to save data
  @retval return Favorite group count saved 
*/
int FavSaveData(const char*fname);

/**
  The FavGetGroupCount function get favorite group's count 
  @retval favorite group's count
*/
int FavGetGroupCount();

/**
  The FavGetGroupInfo function get the favorite's ID and name
  @param [in] idx  Index of favoritegroup which is from 0 to @ref FavGetGroupCount
  @param [out]id   return favorite's id
  @param [out]grpname return favorite's name 
  @retval zero if success 
*/
int FavGetGroupInfo(int idx,UINT*id,char*grpname);

/**
  This function get favorite's name according favid
  @param [in] favid   Id of favorite group
  @param [out]name return favorite's name
  @retval zero if success
*/
int FavGetGroupName(UINT favid,char*name);

/**
   The FavAddGroup function create an new favorite group by given name.only for custom favorite return favid
   @param [in]name favorite group name to created
   @retval return favoriteid 
*/
int FavAddGroup(const char*name);

/**
   The FavAddGroupWithID function create an new favorite group by given favid and name,it's only used by this module
   @param [in]favid fravorite group id
   @param [in]name favorite name
   @retval zero id add success,non zero if favid has exists.
*/
int FavAddGroupWithID(UINT favid,const char*name);

/**
   The FavRemoveGroup function remove group from database
   @param [in]favid  the favid which you want to removed
   @retval zero if removed success,otherwise nonzero 
*/
int FavRemoveGroup(int favid);

/**
   The FavGetServiceCount function get service count of specific favorite 
   @param [in] favid 
   @retval service count
*/
UINT FavGetServiceCount(UINT favid);

/**
   The FavGetServices function used to get service from favorite
   @param[in]favid
   @param[out]svcs used return service data which is allocated by caller.
   @param[in]count svcs buffer size
   @retval  return favorite's service count
*/
int FavGetServices(UINT favid,SERVICELOCATOR*svcs,int count);
int FavGetService(UINT favid,SERVICELOCATOR*svcs,int idx);

/**
   The FavAddService function add a service to favorite group
   @param[in]favid which the service you wanto added
   @param[in]svc service
   @retval zero if add success,nonzero if addfailed or service has exists in the group 
*/
int FavAddService(UINT favid,const SERVICELOCATOR*svc);

/**
   The FavRemoveService function remove a service from favorite group
   @param[in]favid which the service you wanto added
   @param[in]svc service
   @retval zero if remove success,nonzero if  service is not included in the group
*/
int FavRemoveService(UINT favid,const SERVICELOCATOR*svc);

/**
   The FavClearService function remove all service of the group
   @param[in] favorite's id
   @retval zero if remove success,nonzero if  favid is not exists
   @ref FavRemoveService @FavAddService
*/
int FavClearService(UINT favidgrp);

/** @} */ /*end of favoriteFunc*/
/** @} */ /*endof dtvapi*/

NGL_END_DECLS
#endif
