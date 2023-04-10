/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2018 Piotr Gawlowicz
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Piotr Gawlowicz <gawlowicz.p@gmail.com>
 *
以下是OpenGymInterface类中可供Python调用的接口：

SetGetActionSpaceCb(Callback< Ptr<OpenGymSpace> > cb)：设置回调函数以获取动作空间的描述。
SetGetObservationSpaceCb(Callback< Ptr<OpenGymSpace> > cb)：设置回调函数以获取观测空间的描述。
SetGetGameOverCb(Callback< bool > cb)：设置回调函数以获取游戏是否结束的信息。
SetGetObservationCb(Callback< Ptr<OpenGymDataContainer> > cb)：设置回调函数以获取当前观测值。
SetGetRewardCb(Callback<float> cb)：设置回调函数以获取当前奖励值。
SetGetExtraInfoCb(Callbackstd::string cb)：设置回调函数以获取额外信息。
SetExecuteActionsCb(Callback<bool, Ptr<OpenGymDataContainer> > cb)：设置回调函数以执行动作。
NotifyCurrentState()：通知仿真环境当前状态。
WaitForStop()：等待仿真环境停止。


这个函数并没有被注册到Python中。
OpenGymInterface类是在C++中实现的，其中包含了一些可供Python调用的接口。
这些接口可以通过在Python中调用相应的函数来使用。具体来说，Python代码可以通过调用OpenGymInterface类中的GetTypeId()函数来获取一个类型ID，然后使用这个类型ID来创建一个OpenGymInterface对象。在创建对象之后，Python代码可以使用OpenGymInterface对象中的各种接口来与仿真环境进行交互。

 */

#include <sys/types.h>
#include <unistd.h>
#include "ns3/log.h"
#include "ns3/config.h"
#include "ns3/simulator.h"
#include "opengym_interface.h"
#include "opengym_env.h"
#include "container.h"
#include "spaces.h"
#include "messages.pb.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("OpenGymInterface");

NS_OBJECT_ENSURE_REGISTERED (OpenGymInterface);


TypeId
OpenGymInterface::GetTypeId (void)
{
  static TypeId tid = TypeId ("OpenGymInterface")
    .SetParent<Object> ()
    .SetGroupName ("OpenGym")
    .AddConstructor<OpenGymInterface> ()
    ;
  return tid;
}

Ptr<OpenGymInterface>
OpenGymInterface::Get (uint32_t port)
{
  NS_LOG_FUNCTION_NOARGS ();
  return *DoGet (port);
}

Ptr<OpenGymInterface> *
OpenGymInterface::DoGet (uint32_t port)
{
  NS_LOG_FUNCTION_NOARGS ();
  static Ptr<OpenGymInterface> ptr = 0;
  if (ptr == 0)
    {
      ptr = CreateObject<OpenGymInterface> (port);
      Config::RegisterRootNamespaceObject (ptr);
      Simulator::ScheduleDestroy (&OpenGymInterface::Delete);
    }
  return &ptr;
}

void
OpenGymInterface::Delete (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Config::UnregisterRootNamespaceObject (Get ());
  (*DoGet ()) = 0;
}

OpenGymInterface::OpenGymInterface(uint32_t port):
   m_port(port), m_zmq_context(1), m_zmq_socket(m_zmq_context, ZMQ_REQ),m_zmq_socket_mutipart(),
  m_simEnd(false), m_stopEnvRequested(false), m_initSimMsgSent(false), m_fedLearning(false)
{
  NS_LOG_FUNCTION (this);
}

OpenGymInterface::OpenGymInterface(bool isFedLearning, uint32_t port):
   m_port(port), m_zmq_context(1), m_zmq_socket(m_zmq_context, ZMQ_REQ),m_zmq_socket_mutipart(),
  m_simEnd(false), m_stopEnvRequested(false), m_initSimMsgSent(false), m_fedLearning(isFedLearning)
{
  NS_LOG_FUNCTION (this);
}


OpenGymInterface::~OpenGymInterface ()
{
  NS_LOG_FUNCTION (this);
}

void
OpenGymInterface::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
}

void
OpenGymInterface::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
}

void
OpenGymInterface::SetGetActionSpaceCb(Callback< Ptr<OpenGymSpace> > cb)
{
  NS_LOG_FUNCTION (this);
  m_actionSpaceCb = cb;
}

void
OpenGymInterface::SetGetObservationSpaceCb(Callback< Ptr<OpenGymSpace> > cb)
{
  NS_LOG_FUNCTION (this);
  m_observationSpaceCb = cb;
}

void 
OpenGymInterface::SetGetModelSpaceCb(Callback< Ptr<OpenGymSpace> > cb)
{
  NS_LOG_FUNCTION(this);
  m_ModelSpaceCb = cb;
}

void
OpenGymInterface::SetGetGameOverCb(Callback< bool > cb)
{
  NS_LOG_FUNCTION (this);
  m_gameOverCb = cb;
}

void
OpenGymInterface::SetGetObservationCb(Callback< Ptr<OpenGymDataContainer> > cb)
{
  NS_LOG_FUNCTION (this);
  m_obsCb = cb;
}

void
OpenGymInterface::SetGetRewardCb(Callback<float> cb)
{
  NS_LOG_FUNCTION (this);
  m_rewardCb = cb;
}

void
OpenGymInterface::SetGetExtraInfoCb(Callback<std::string> cb)
{
  NS_LOG_FUNCTION (this);
  m_extraInfoCb = cb;
}

void
OpenGymInterface::SetExecuteActionsCb(Callback<bool, Ptr<OpenGymDataContainer> > cb)
{
  NS_LOG_FUNCTION (this);
  m_actionCb = cb;
}

void 
OpenGymInterface::SetExecuteModelcb(Callback<void, Ptr<OpenGymDataContainer> > cb)
{
  NS_LOG_FUNCTION(this);
  m_modelactionCb = cb;
}

void
OpenGymInterface::RecvModel(std::vector<double> model)
{
  m_model = model;
  NotifyCurrentState();
}

void 
OpenGymInterface::Init()
{
  NS_LOG_FUNCTION (this);
  // do not send init msg twice
  if (m_initSimMsgSent) {
    return;
  }
  m_initSimMsgSent = true;

  std::string connectAddr = "tcp://localhost:" + std::to_string(m_port);
  zmq_connect ((void*)m_zmq_socket, connectAddr.c_str());

  Ptr<OpenGymSpace> obsSpace = GetObservationSpace();
  Ptr<OpenGymSpace> actionSpace = GetActionSpace();
  Ptr<OpenGymSpace> modelSpace = GetModelSpace();

  NS_LOG_UNCOND("Simulation process id: " << ::getpid() << " (parent (waf shell) id: " << ::getppid() << ")");
  NS_LOG_UNCOND("Waiting for Python process to connect on port: "<< connectAddr);
  NS_LOG_UNCOND("Please start proper Python Gym Agent");

  ns3opengym::SimInitMsg simInitMsg;
  simInitMsg.set_simprocessid(::getpid());
  simInitMsg.set_wafshellprocessid(::getppid());

  if (obsSpace) {
    ns3opengym::SpaceDescription spaceDesc;
    spaceDesc = obsSpace->GetSpaceDescription();
    simInitMsg.mutable_obsspace()->CopyFrom(spaceDesc);
  }

  if (actionSpace) {
    ns3opengym::SpaceDescription spaceDesc;
    spaceDesc = actionSpace->GetSpaceDescription();
    simInitMsg.mutable_actspace()->CopyFrom(spaceDesc);
  }

  if(modelSpace) {
    ns3opengym::SpaceDescription spaceDesc;
    spaceDesc = modelSpace->GetSpaceDescription();
    simInitMsg.mutable_modspace()->CopyFrom(spaceDesc);
  }
  
  // send init msg to python
  zmq::message_t request(simInitMsg.ByteSize());;
  simInitMsg.SerializeToArray(request.data(), simInitMsg.ByteSize());
  m_zmq_socket.send (request, zmq::send_flags::none);

  // receive init ack msg form python
  ns3opengym::SimInitAck simInitAck;
  zmq::message_t reply;
  (void) m_zmq_socket.recv (reply, zmq::recv_flags::none);
  simInitAck.ParseFromArray(reply.data(), reply.size());

  bool done = simInitAck.done();
  NS_LOG_DEBUG("Sim Init Ack: " << done);

  bool stopSim = simInitAck.stopsimreq();
  if (stopSim) {
    NS_LOG_DEBUG("---Stop requested: " << stopSim);
    m_stopEnvRequested = true;
    Simulator::Stop();
    Simulator::Destroy ();
    std::exit(0);
  }
}

void
OpenGymInterface::NotifyCurrentState()
{
  NS_LOG_FUNCTION (this);
  if (!m_initSimMsgSent) {
    Init();
  }

  if (m_stopEnvRequested) {
    return;
  }

  if(GetExtraInfo() == "RecvModel")
  {

    Ptr<OpenGymBoxContainer<double>> m_modelBox = CreateObject<OpenGymBoxContainer<double> >();
    for(auto it : m_model)
    {
      m_modelBox->AddValue(it);
    }
    ns3opengym::EnvModelMsg envModelMsg;
    envModelMsg.mutable_modedata()->CopyFrom(m_modelBox->GetDataContainerPbMsg());

    zmq::message_t request(envModelMsg.ByteSize());;
    envModelMsg.SerializeToArray(request.data(), envModelMsg.ByteSize());
    m_zmq_socket.send (request, zmq::send_flags::none);

    return;
  }
  
  // collect current env state
  Ptr<OpenGymDataContainer> obsDataContainer = GetObservation();
  float reward = GetReward();
  bool isGameOver = IsGameOver();
  std::string extraInfo = GetExtraInfo();

  ns3opengym::EnvStateMsg envStateMsg;
  // observation
  ns3opengym::DataContainer obsDataContainerPbMsg;
  if (obsDataContainer) {
    obsDataContainerPbMsg = obsDataContainer->GetDataContainerPbMsg();
    envStateMsg.mutable_obsdata()->CopyFrom(obsDataContainerPbMsg);
  }
  // reward
  envStateMsg.set_reward(reward);
  // game over
  envStateMsg.set_isgameover(false);
  if (isGameOver)
  {
    envStateMsg.set_isgameover(true);
    if (m_simEnd) {
      envStateMsg.set_reason(ns3opengym::EnvStateMsg::SimulationEnd);
    } else {
      envStateMsg.set_reason(ns3opengym::EnvStateMsg::GameOver);
    }
  }

  // extra info
  envStateMsg.set_info(extraInfo);

  // send env state msg to python
  zmq::message_t request(envStateMsg.ByteSize());;
  envStateMsg.SerializeToArray(request.data(), envStateMsg.ByteSize());
  m_zmq_socket.send (request, zmq::send_flags::none);

  NS_LOG_FUNCTION (this << "------------------------------------------------------------------------\n");
  NS_LOG_FUNCTION (this << "-----------------------Waiting for responed-----------------------------\n");
  NS_LOG_FUNCTION (this << "------------------------------------------------------------------------\n");
 

  NS_LOG_FUNCTION (this << " m_f " << m_fedLearning << " extrainfo " << GetExtraInfo());
  if(!m_fedLearning || GetExtraInfo() == "Training") 
  // if(1)
  {
    // receive act msg form python
    ns3opengym::EnvActMsg envActMsg;
    zmq::message_t reply;
    (void) m_zmq_socket.recv (reply, zmq::recv_flags::none);
    envActMsg.ParseFromArray(reply.data(), reply.size());

    if (m_simEnd) 
    {
      // if sim end only rx ms and quit
      return;
    }

    bool stopSim = envActMsg.stopsimreq();
    if (stopSim) 
    {
      NS_LOG_DEBUG("---Stop requested: " << stopSim);
      m_stopEnvRequested = true;
      Simulator::Stop();
      Simulator::Destroy ();
      std::exit(0);
    }

    // first step after reset is called without actions, just to get current state
    ns3opengym::DataContainer actDataContainerPbMsg = envActMsg.actdata();
    Ptr<OpenGymDataContainer> actDataContainer = OpenGymDataContainer::CreateFromDataContainerPbMsg(actDataContainerPbMsg);
    ExecuteActions(actDataContainer);
  } 
  else if(GetExtraInfo() == "GetModel")
  {
    // 使用 zmq::recv_multipart() 方法一次性读取所有帧
    // m_zmq_socket_mutipart.recv(m_zmq_socket);
    // // 创建 Protobuf 消息对象
    // ns3opengym::EnvModelMsg envModelMsg;
    // // 处理消息的多个帧
    // for (size_t i = 0; i < m_zmq_socket_mutipart.size(); ++i) 
    // {
    //     // 使用 frame.data() 和 frame.size() 来访问帧的数据
    //     // 将数据追加复制到 Protobuf 消息对象中
    //     zmq::message_t frame = m_zmq_socket_mutipart.pop();
    //     ns3opengym::EnvModelMsg subModel;
    //     subModel.ParseFromArray(frame.data(), frame.size());
    //     NS_LOG_DEBUG(" Model recv =>>> " << static_cast<char*>(frame.data()) << frame.size());
    //     envModelMsg.MergeFrom(subModel);
    // }
    
    ns3opengym::EnvModelMsg envModelMsg;
    zmq::message_t reply;
    (void) m_zmq_socket.recv (reply, zmq::recv_flags::none);
    envModelMsg.ParseFromArray(reply.data(), reply.size());
    NS_LOG_FUNCTION(this << "i recv model !!!!");
    NS_LOG_FUNCTION(this << "i recv model !!!! " << reply.size() << (char*)reply.data());
    if (m_simEnd) 
    {
      return;
    }

    bool stopSim = envModelMsg.stopsimreq();
    NS_LOG_FUNCTION(this << " stop Sim is " << stopSim);
    if (stopSim) 
    // if(1)
    { 
      NS_LOG_DEBUG("---Stop requested: " << stopSim);
      m_stopEnvRequested = true;
      Simulator::Stop();
      Simulator::Destroy ();
      std::exit(0);
    }

    ns3opengym::DataContainer modelDataContainerPbMsg = envModelMsg.modedata();
    Ptr<OpenGymDataContainer> modelDataContainer = OpenGymDataContainer::CreateFromDataContainerPbMsg(modelDataContainerPbMsg);
    ExecuteModel(modelDataContainer);
  }
  return;
}

void
OpenGymInterface::WaitForStop()
{
  NS_LOG_FUNCTION (this);
  NS_LOG_UNCOND("Wait for stop message");
  NotifyCurrentState();
}

void
OpenGymInterface::NotifySimulationEnd()
{
  NS_LOG_FUNCTION (this);
  m_simEnd = true;
  if (m_initSimMsgSent) {
    WaitForStop();
  }
}

bool
OpenGymInterface::IsGameOver()
{
  NS_LOG_FUNCTION (this);
  bool gameOver = false;
  if (!m_gameOverCb.IsNull())
  {
    gameOver = m_gameOverCb();
  }
  return (gameOver || m_simEnd);
}

bool
OpenGymInterface::IsFedLearning()
{
  NS_LOG_FUNCTION(this);
  return m_fedLearning;
}

void 
OpenGymInterface::SetIsFedLearning(bool isFedLearning)
{
  NS_LOG_FUNCTION(this);
  m_fedLearning = isFedLearning;
}

Ptr<OpenGymSpace>
OpenGymInterface::GetActionSpace()
{
  NS_LOG_FUNCTION (this);
  Ptr<OpenGymSpace> actionSpace;
  if (!m_actionSpaceCb.IsNull())
  {
    actionSpace = m_actionSpaceCb();
  }
  return actionSpace;
}

Ptr<OpenGymSpace>
OpenGymInterface::GetObservationSpace()
{
  NS_LOG_FUNCTION (this);
  Ptr<OpenGymSpace> obsSpace;
  if (!m_observationSpaceCb.IsNull())
  {
    obsSpace = m_observationSpaceCb();
  }
  return obsSpace;
}

Ptr<OpenGymSpace>
OpenGymInterface::GetModelSpace()
{
  NS_LOG_FUNCTION(this);
  Ptr<OpenGymSpace> modeSpace;
  if(!m_ModelSpaceCb.IsNull())
  {
    modeSpace = m_ModelSpaceCb();
  }
  return modeSpace;
}

Ptr<OpenGymDataContainer>
OpenGymInterface::GetObservation()
{
  NS_LOG_FUNCTION (this);
  Ptr<OpenGymDataContainer>  obs;
  if (!m_obsCb.IsNull())
  {
    obs = m_obsCb();
  }
  return obs;
}

float
OpenGymInterface::GetReward()
{
  NS_LOG_FUNCTION (this);
  float reward = 0.0;
  if (!m_rewardCb.IsNull())
  {
    reward = m_rewardCb();
  }
  return reward;
}

std::string
OpenGymInterface::GetExtraInfo()
{
  NS_LOG_FUNCTION (this);
  std::string info;
  if (!m_extraInfoCb.IsNull())
  {
    info = m_extraInfoCb();
  }
  return info;
}

bool
OpenGymInterface::ExecuteActions(Ptr<OpenGymDataContainer> action)
{
  NS_LOG_FUNCTION (this);
  bool reply = false;
  if (!m_actionCb.IsNull())
  {
    reply = m_actionCb(action);
  }
  return reply;
}

void
OpenGymInterface::ExecuteModel(Ptr<OpenGymDataContainer> model)
{
  NS_LOG_FUNCTION (this);
  if(!m_modelactionCb.IsNull())
  {
    m_modelactionCb(model);
  }
  return;
}

void
OpenGymInterface::Notify(Ptr<OpenGymEnv> entity)
{
  NS_LOG_FUNCTION (this);

  SetGetGameOverCb( MakeCallback (&OpenGymEnv::GetGameOver, entity) );
  SetGetObservationCb( MakeCallback (&OpenGymEnv::GetObservation, entity) );
  SetGetRewardCb( MakeCallback (&OpenGymEnv::GetReward, entity) );
  SetGetExtraInfoCb( MakeCallback (&OpenGymEnv::GetExtraInfo, entity) );
  SetExecuteActionsCb( MakeCallback (&OpenGymEnv::ExecuteActions, entity) );
  if(m_fedLearning)
  {
    SetExecuteModelcb( MakeCallback (&OpenGymEnv::ExecuteModel, entity) );
  }
  
  NotifyCurrentState();
}

}

