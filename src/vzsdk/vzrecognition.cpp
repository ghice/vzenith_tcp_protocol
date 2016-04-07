#include "VzRecognition.h"
#include "vzsdk\vzsdkdefines.h"
#include "commandanalysis.h"
#include "base\logging.h"
#include "vzsdkpushhandle.h"

vzsdk::VzRecognition::VzRecognition(VzsdkService* _service)
    : VZModuleBase(_service)
    , ivs_handle(new vzsdk::IvsPushHandle("ivs_result")) {
}


vzsdk::VzRecognition::~VzRecognition() {
}

/************************************************************************/
// ����:     GetRecord
// ����:     int _record_id
// ����:     bool _need_image
// ����:     TH_PlateResult & oPlateResult
// ����ֵ:   int
// ˵��:     ���ݼ�¼��ID����ȡ����ʶ���¼
/************************************************************************/
int vzsdk::VzRecognition::GetRecord(int _record_id, bool _need_image, TH_PlateResult& oPlateResult) {
    Json::Value req_json;
    int _session_id = sdk_service->GetSessionID();
    commandanalysis::GeneratGetRecordByIdCmd(_record_id, _need_image, req_json);
    std::string result;
    if (_session_id == 0) {
        LOG(LS_WARNING) << "The session is is zero, is not a right session";
        return SESSION_ID_INVALUE;
    }

    Task::Ptr req_task(new ReqRecordTask(sdk_service->GetQueueLayer().get(),
                                         DEFAULT_TIMEOUT,
                                         _session_id,
                                         req_json));

    Message::Ptr msg = req_task->SyncProcessTask();
    if (!msg || msg->phandler == NULL) {
        return REQ_FAILED;
    }

    //��������
    ResponseData *response = static_cast<ResponseData *>(msg->pdata.get());
    Json::Value _value = response->res_json();
    TH_PlateResult _plate_result;
    int nFullImgSize = 0, nClipImgSize = 0;
    commandanalysis::ParsePlateResultResponse(_value, _plate_result, nFullImgSize, nClipImgSize);

    void *pImage = NULL;
    void *pClipImage = NULL;

    int len = strlen(response->res_data().c_str());
    pImage = (void *)(response->res_data().c_str() + len + 1);
    pClipImage = (void *)(response->res_data().c_str() + len + nFullImgSize + 1);

    return REQ_SUCCEED;
}

/************************************************************************/
// ����:     GetImage
// ����:     int _image_id
// ����:     char * _image_ata
// ����:     int & _image_size
// ����ֵ:   int
// ˵��:     ����ͼƬID����ȡͼƬ
/************************************************************************/
int vzsdk::VzRecognition::GetImage(int _image_id, char* _image_data, int& _image_size) {
    Json::Value req_json;
    commandanalysis::GeneratGetImageByIdCmd(_image_id, req_json);

    Message::Ptr _msg = SyncProcessReqTask(req_json);
    if (!_msg || _msg->phandler == NULL) {
        return REQ_FAILED;
    }

    //����ͼƬ
    ResponseData *response = static_cast<ResponseData *>(_msg->pdata.get());
    Json::Value result = response->res_json();

    RECORD_RESPONSE _responese;
    commandanalysis::ParseRecordResponse(result, &_responese);

    if (_responese.id != _image_id)
        return REQ_FAILED;

    int _len = strlen(response->res_data().c_str());
    const char* res_data = response->res_data().c_str();
    const char* _get_image_data = res_data + _len + 1;
    _image_size = _responese.size;
    memcpy(_image_data, _get_image_data, _responese.size);
    return REQ_SUCCEED;
}

/************************************************************************/
// ����:     ForceTrigger
// ����ֵ:   int
// ˵��:     ǿ�ƴ���
/************************************************************************/
int vzsdk::VzRecognition::ForceTrigger() {
    Json::Value req_json;
    commandanalysis::GeneratForceTrigger(req_json);

    if (!PostReqTask(req_json)) {
        return REQ_FAILED;
    }

    return REQ_SUCCEED;
}

int vzsdk::VzRecognition::setReciveIvsResultCallback(VZLPRC_TCP_PLATE_INFO_CALLBACK _func, void* _UserData, int bEnableImage) {
    static_cast<IvsPushHandle*>(ivs_handle.get())->SetPlateCallBack(_func, _UserData);
    sdk_service->AddPushHandle(ivs_handle);
    ReciveIvsResult(sdk_service->GetSessionID()
                    , ivs_handle
                    , true
                    , vzsdk::FORMAT_JSON
                    , bEnableImage
                    , vzsdk::FULL_IMG);
    return REQ_SUCCEED;
}

/************************************************************************/
// ����:     ReciveIvsResult
// ����:     uint32 session_id
// ����:     PushHandle::Ptr handle
// ����:     bool enable_result
// ����:     IvsFormat format
// ����:     bool enable_img
// ����:     IvsImgType img_type
// ����ֵ:   int
// ˵��:     ����ʶ����
/************************************************************************/
int vzsdk::VzRecognition::ReciveIvsResult(uint32 session_id,
        PushHandle::Ptr handle,
        bool enable_result,
        IvsFormat format,
        bool enable_img,
        IvsImgType img_type) {

    if (session_id == 0) {
        LOG(LS_WARNING) << "The session is is zero, is not a right session";
        return SESSION_ID_INVALUE;
    }

    Json::Value _req_json;
    commandanalysis::GeneratIVSResult(enable_result, format, enable_img, img_type, _req_json);
    sdk_service->AddPushHandle(handle);
    Task::Ptr req_task(new ReqPushTask(sdk_service->GetQueueLayer().get(),
                                       DEFAULT_TIMEOUT,
                                       session_id,
                                       _req_json));

    Message::Ptr msg = req_task->SyncProcessTask();
    if (!msg || msg->phandler == NULL) {
        return DEFAULT_RESULT_TIMEOUT;
    }
    return REQ_SUCCEED;
}