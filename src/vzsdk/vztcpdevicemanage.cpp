#include "VzTcpDeviceManage.h"
#include "VzConnectDev.h"

const int _c_success = 0;
const int _c_failed = -1;

vzsdk::VzTcpDeviceManage::VzTcpDeviceManage() {
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

    VzsdkServicesPtr sdk_service = GetService(session_id);
    if (sdk_service) {
        sdk_service->Stop();
        RemoveService(session_id);
    }
    return true;
}
