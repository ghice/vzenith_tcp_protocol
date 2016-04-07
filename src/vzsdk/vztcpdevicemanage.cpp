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
    for (VzsdkServicesMap::iterator it = vzsdk_service_map.begin();
            it != vzsdk_service_map.end();
            ++it) {
        //it->second->GetConnectDev()->DisconnectServer();
    }
    vzsdk_service_map.clear();
}

/************************************************************************/
// ����:     ExistService
// ����:     int _session_id
// ����ֵ:   bool
// ˵��:     �Ƿ����session_id��Ӧ�ķ���
/************************************************************************/
bool vzsdk::VzTcpDeviceManage::ExistService(int _session_id) {
    bool bExist = true;
    VzsdkServicesMap::iterator it = vzsdk_service_map.find(_session_id);
    if (it == vzsdk_service_map.end())
        bExist = false;
    return bExist;
}

/************************************************************************/
// ����:     GetService
// ����:     int _session_id
// ����ֵ:   const VzsdkServicesPtr
// ˵��:     ����session_id��ȡ������
/************************************************************************/
const VzsdkServicesPtr vzsdk::VzTcpDeviceManage::GetService(int _session_id) {
    VzsdkServicesPtr _vzsdk_service;
    if (ExistService(_session_id)) {
        VzsdkServicesMap::iterator it = vzsdk_service_map.find(_session_id);
        _vzsdk_service = it->second;
    }
    return _vzsdk_service;
}

const VzsdkServicesPtr vzsdk::VzTcpDeviceManage::GetService(const std::string& _ip) {
    VzsdkServicesPtr _vzsdk_service;
    for (VzsdkServicesMap::iterator it = vzsdk_service_map.begin();
            it != vzsdk_service_map.end();
            ++it) {
        VzConnectDevPtr _connect_ptr = it->second->GetConnectDev();
        if (_ip == _connect_ptr->GetIP()) {
            _vzsdk_service = it->second;
            break;
        }
    }

    return _vzsdk_service;
}

/************************************************************************/
// ����:     RemoveService
// ����:     int _session_id
// ����ֵ:   bool
// ˵��:     ��ӳ������Ƴ�����
/************************************************************************/
bool vzsdk::VzTcpDeviceManage::RemoveService(int _session_id) {
    bool bRet = false;
    VzsdkServicesMap::iterator it = vzsdk_service_map.find(_session_id);
    if (it != vzsdk_service_map.end()) {
        vzsdk_service_map.erase(it);
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
int vzsdk::VzTcpDeviceManage::CreateNewService(const std::string& _ip, const int _port, const std::string& _user_name, const std::string& _user_pwd) {
    VzsdkServicesPtr _sdk_service(new VzsdkService);
    _sdk_service->Start();
    int _session_id = _sdk_service->GetConnectDev()->ConnectServer(_ip, _port);
    if (_session_id != SESSION_ID_INVALUE) {
        vzsdk_service_map.insert(std::make_pair(_session_id, _sdk_service));
    }
    return _session_id;
}

/************************************************************************/
// ����:     CloseService
// ����:     int _session_id
// ����ֵ:   bool
// ˵��:     �رշ���
/************************************************************************/
bool vzsdk::VzTcpDeviceManage::CloseService(int _session_id) {
    if (!ExistService(_session_id))
        return false;

    VzsdkServicesPtr _sdk_service = GetService(_session_id);
    if (_sdk_service) {
        _sdk_service->Stop();
        RemoveService(_session_id);
    }
    return true;
}
