#include "VzConnectDev.h"
#include "vzsdkdefines.h"
#include "base/logging.h"

vzsdk::VzConnectDev::VzConnectDev(VzsdkService* _service)
    : VZModuleBase(_service) {
}

vzsdk::VzConnectDev::~VzConnectDev() {
    DisconnectServer();
}

/************************************************************************/
// ����:     ConnectServer
// ����:     const std::string & _ip_addr
// ����:     uint16 _port
// ����ֵ:   int
// ˵��:     ���ӵ��豸
/************************************************************************/
int vzsdk::VzConnectDev::ConnectServer(const std::string& _ip_addr, uint16 _port) {
    int _session_id = SESSION_ID_INVALUE;
    SocketAddress remote_address(_ip_addr, _port);
    Task::Ptr connect_task(new ConnectTask(sdk_service_->GetQueueLayer().get(),
                                           FOREVER_TIMEOUT,
                                           remote_address));
    Message::Ptr msg = connect_task->SyncProcessTask();

    if (!msg || msg->phandler == NULL) {
        return DEFAULT_RESULT_TIMEOUT;
    }

    ReqConnectData* _req_connect_data = static_cast<ReqConnectData*>(connect_task->message_data().get());
    Stanza* _stanze = static_cast<Stanza*>(msg->pdata.get());
    if (_stanze) {
        int _new_session_id = _stanze->session_id();
        req_connect_data_ptr = ReqConnectDataPtr(new ReqConnectData(*_req_connect_data));
        req_connect_data_ptr->set_session_id(_new_session_id);
        if (GetConnState() == Socket::CS_CONNECTED) {
            ReConnectServer();
            _session_id = _new_session_id;
        }

    }

    return _session_id;
}

/************************************************************************/
// ����:     DisconnectServer
// ����:     uint32 session_id
// ����ֵ:   int
// ˵��:     �ر�����
/************************************************************************/
int vzsdk::VzConnectDev::DisconnectServer() {
    int _session_id = req_connect_data_ptr->session_id();
    if (_session_id == 0) {
        LOG(LS_WARNING) << "The session is is zero, is not a right session";
        return SESSION_ID_INVALUE;
    }
    Task::Ptr disconnect_task(new DisconnectTask(sdk_service_->GetQueueLayer().get(),
                              DEFAULT_TIMEOUT,
                              _session_id));
    Message::Ptr msg = disconnect_task->SyncProcessTask();
    if (!msg || msg->phandler == NULL) {
        return DEFAULT_RESULT_TIMEOUT;
    }
    Stanza *stanza = static_cast<Stanza*>(msg->pdata.get());
    if (stanza && stanza->stanza_type() == RES_DISCONNECTED_EVENT_SUCCEED) {
        LOG(LS_INFO) << "Disconnect succeed";
        return REQ_SUCCEED;
    } else if (stanza->stanza_type() == RES_DISCONNECTED_EVENT_FAILURE) {
        return SESSION_NOT_FOUND;
    } else {
        return UNKOWN_ERROR;
    }
    return REQ_SUCCEED;
}

/************************************************************************/
// ����:     GetIP
// ����ֵ:   std::string
// ˵��:     ��ȡ�����豸��IP
/************************************************************************/
std::string vzsdk::VzConnectDev::GetIP() {
    return req_connect_data_ptr->address().ipaddr().ToString();
}

int vzsdk::VzConnectDev::GetSessionID() {
    int session_id = SESSION_ID_INVALUE;
    if (req_connect_data_ptr)
        session_id = req_connect_data_ptr->session_id();
    return session_id;
}

vzsdk::Socket::ConnState vzsdk::VzConnectDev::GetConnState() {
    int _session_id = GetSessionID();
    SessionManager::Ptr session_mana_ = sdk_service_->GetQueueLayer()->GetSessionManamger();
    vzsdk::Session::Ptr _session_ptr = session_mana_->FindSession(_session_id);
    if (_session_ptr) {
        return _session_ptr->GetState();
    }
    return Socket::CS_CLOSED;
}

/************************************************************************/
// ����:     ReConnectServer
// ����ֵ:   int
// ˵��:     ��������
/************************************************************************/
int vzsdk::VzConnectDev::ReConnectServer() {
    if (!req_connect_data_ptr)
        return SESSION_ID_INVALUE;

    req_connect_data_ptr->set_stanza_type(RES_RECONNECT_SERVER);
    sdk_service_->GetQueueLayer()->Post(RES_RECONNECT_SERVER, req_connect_data_ptr);
    return REQ_SUCCEED;
}
