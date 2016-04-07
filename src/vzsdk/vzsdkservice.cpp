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

#include "vzsdk/vzsdkservice.h"
#include "vzsdk/task.h"
#include "base/logging.h"
#include "vzsdk/vzsdkbase.h"
#include "vzsdk/commandanalysis.h"
#include "vzconnectdev.h"
#include "vzrecognition.h"
#include "vzmaintendev.h"
#include "vzwlistdev.h"
#include "vzserialdev.h"
#include "vziodev.h"

namespace vzsdk {

/************************************************************************/
// ����:     VzsdkServices
// ����ֵ:
// ˵��:     Ĭ�Ϲ��캯������ʼ��
/************************************************************************/
VzsdkService::VzsdkService()
    : push_manager_task_(NULL)
    , connect_dev(NULL) {

}

VzsdkService::~VzsdkService() {

}

int VzsdkService::GetSessionID() {
    return connect_dev->GetSessionID();
}

/************************************************************************/
// ����:     Start
// ����ֵ:   bool
// ˵��:     ��ʼ����Դ����
/************************************************************************/
bool VzsdkService::Start() {

    // Init logging system
    LogMessage::LogTimestamps(true);
    LogMessage::LogContext(vzsdk::LS_INFO);
    LogMessage::LogThreads(true);
    queue_layer_.reset(new QueueLayer());
    ASSERT(push_manager_task_ == NULL);
    push_thread_.reset(new Thread());
    push_manager_task_ = new PushManagerTask(queue_layer_.get(),
            push_thread_.get());

    // For push manager life live
    Task::Ptr push_task(push_manager_task_);
    if (queue_layer_->Start()) {
        push_manager_task_->SyncProcessTask();
        initModule();
        return true;
    }
    return false;
}

/************************************************************************/
// ����:     Stop
// ����ֵ:   bool
// ˵��:     ֹͣ����
/************************************************************************/
bool VzsdkService::Stop() {
    //GetConnectDev()->DisconnectServer();

    return queue_layer_->Stop();
}

void VzsdkService::initModule() {
    VzConnectDevPtr _conncet_dev_ptr(new VzConnectDev(this));
    connect_dev = _conncet_dev_ptr;

    VzRecognitionPtr _recognition_ptr(new VzRecognition(this));
    recongition = _recognition_ptr;

    VzMaintenDevPtr _mainten_dev_ptr(new VzMaintenDev(this));
    maintenDev = _mainten_dev_ptr;

    VzWlistDevPtr _whist_dev_ptr(new VzWlistDev(this));
    whist_dev = _whist_dev_ptr;

    VzSerialDevPtr _serial_dev_ptr(new VzSerialDev(this));
    serial_dev = _serial_dev_ptr;

    VzIODevPtr _io_dev_ptr(new VzIODev(this));
    io_dev = _io_dev_ptr;
}

/************************************************************************/
// ����:     AddPushHandle
// ����:     PushHandle::Ptr handle
// ����ֵ:   void
// ˵��:     ��Ϣ�������
/************************************************************************/
void VzsdkService::AddPushHandle(PushHandle::Ptr handle) {
    if (push_manager_task_) {
        push_manager_task_->AddPushHandle(handle);
    }
}

/************************************************************************/
// ����:     RemovePushHandle
// ����:     PushHandle::Ptr handle
// ����ֵ:   void
// ˵��:     �Ƴ���Ϣ
/************************************************************************/
void VzsdkService::RemovePushHandle(PushHandle::Ptr handle) {
    if (push_manager_task_) {
        push_manager_task_->RemovePushHandle(handle);
    }
}

vzsdk::QueueLayerPtr VzsdkService::GetQueueLayer() {
    return queue_layer_;
}

vzsdk::VzConnectDevPtr VzsdkService::GetConnectDev() {
    return connect_dev;
}

vzsdk::VzRecognitionPtr VzsdkService::GetRecongintion() {
    return recongition;
}

vzsdk::VzMaintenDevPtr VzsdkService::GetMaintenDev() {
    return maintenDev;
}

vzsdk::VzWlistDevPtr VzsdkService::GetWlistDev() {
    return whist_dev;
}

vzsdk::VzSerialDevPtr VzsdkService::GetSerialDev() {
    return serial_dev;
}

vzsdk::VzIODevPtr VzsdkService::GetIODev() {
    return io_dev;
}
}
