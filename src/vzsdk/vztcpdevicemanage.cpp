/*
* vzsdk
* Copyright 2013 - 2016, Vzenith Inc.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  1. Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*  3. The name of the author may not be used to endorse or promote products
*     derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "vzsdk/VzTcpDeviceManage.h"
#include "vzsdk/VzConnectDev.h"

const int _c_success = 0;
const int _c_failed = -1;

vzsdk::VzTcpDeviceManage::VzTcpDeviceManage() 
            : conn_callback_(NULL)
            , user_data_(NULL){
}

/************************************************************************/
// ����:     ~VzTcpDeviceManage
// ����ֵ:
// ˵��:     �������ͷ���Դ
/************************************************************************/
vzsdk::VzTcpDeviceManage::~VzTcpDeviceManage() {
}

/************************************************************************/
// ����:     ExistService
// ����:     int _session_id
// ����ֵ:   bool
// ˵��:     �Ƿ����session_id��Ӧ�ķ���
/************************************************************************/
bool vzsdk::VzTcpDeviceManage::ExistService(int session_id) {
    bool bExist = true;
    VzsdkServicesMap::iterator it = vzsdk_service_map_.find(session_id);
    if (it == vzsdk_service_map_.end())
        bExist = false;
    return bExist;
}

/************************************************************************/
// ����:     GetService
// ����:     int _session_id
// ����ֵ:   const VzsdkServicesPtr
// ˵��:     ����session_id��ȡ������
/************************************************************************/
const VzsdkServicesPtr vzsdk::VzTcpDeviceManage::GetService(int session_id) {
    VzsdkServicesPtr vzsdk_service;
    if (ExistService(session_id)) {
        VzsdkServicesMap::iterator it = vzsdk_service_map_.find(session_id);
        vzsdk_service = it->second;
    }
    return vzsdk_service;
}

const VzsdkServicesPtr vzsdk::VzTcpDeviceManage::GetService(const std::string& ip) {
    VzsdkServicesPtr vzsdk_service;
    for (VzsdkServicesMap::iterator it = vzsdk_service_map_.begin();
            it != vzsdk_service_map_.end();
            ++it) {
        VzConnectDevPtr _connect_ptr = it->second->GetConnectDev();
        if (ip == _connect_ptr->GetIP()) {
            vzsdk_service = it->second;
            break;
        }
    }

    return vzsdk_service;
}

/************************************************************************/
// ����:     RemoveService
// ����:     int _session_id
// ����ֵ:   bool
// ˵��:     ��ӳ������Ƴ�����
/************************************************************************/
bool vzsdk::VzTcpDeviceManage::RemoveService(int session_id) {
    bool bRet = false;
    VzsdkServicesMap::iterator it = vzsdk_service_map_.find(session_id);
    if (it != vzsdk_service_map_.end()) {
        vzsdk_service_map_.erase(it);
        bRet = true;
    }

    return bRet;
}

/************************************************************************/
// ����:     CreateNewService
// ����:     const std::string & _ip
// ����:     const int _port
// ����:     const std::string & _user_name
// ����:     const std::string & _user_pwd
// ����ֵ:   int
// ˵��:     �����µķ���
/************************************************************************/
int vzsdk::VzTcpDeviceManage::CreateNewService(const std::string& ip, const int port, const std::string& user_name, const std::string& user_pwd) {
    VzsdkServicesPtr sdk_service(new VzsdkService);
    sdk_service->Start();
    int session_id = sdk_service->GetConnectDev()->ConnectServer(ip, port);
    if (session_id != SESSION_ID_INVALUE) {
        sdk_service->SetCommonNotifyCallBack(conn_callback_, user_data_);;
        vzsdk_service_map_.insert(std::make_pair(session_id, sdk_service));
    }
    return session_id;
}

/************************************************************************/
// ����:     CloseService
// ����:     int _session_id
// ����ֵ:   bool
// ˵��:     �رշ���
/************************************************************************/
bool vzsdk::VzTcpDeviceManage::CloseService(int session_id) {
    if (!ExistService(session_id))
        return false;

    GetService(session_id)->Stop();
    RemoveService(session_id);
    return true;
}

void vzsdk::VzTcpDeviceManage::SetCommonNotifyCallBack(VZLPRC_TCP_COMMON_NOTIFY_CALLBACK func
        , void *user_Data_) {
    conn_callback_ = func;
    user_data_ = user_Data_;
    for (VzsdkServicesMap::iterator it = vzsdk_service_map_.begin();
            it != vzsdk_service_map_.end();
            ++it) {
        it->second->SetCommonNotifyCallBack(func, user_Data_);
        it->second->GetConnectDev()->SetCommonNotifyCallBack(func, user_Data_);
    }
}

void vzsdk::VzTcpDeviceManage::GetCommonNotifyCallBack(VZLPRC_TCP_COMMON_NOTIFY_CALLBACK func, void *user_data) {
    func = conn_callback_;
    user_data = user_data_;
}
