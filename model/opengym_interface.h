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
 */

#ifndef OPENGYM_INTERFACE_H
#define OPENGYM_INTERFACE_H

#include "ns3/object.h"
#include <zmq.hpp>
#include <zmq_addon.hpp>

namespace ns3 {

class OpenGymSpace;
class OpenGymDataContainer;
class OpenGymEnv;

class OpenGymInterface : public Object
{
public:
  static Ptr<OpenGymInterface> Get (uint32_t port=5555);

  OpenGymInterface (uint32_t port=5555);
  OpenGymInterface (bool isFedLearning, uint32_t port=5555);
  virtual ~OpenGymInterface ();

  static TypeId GetTypeId ();

  void Init();
  void NotifyCurrentState();
  void WaitForStop();

  void NotifySimulationEnd();

  Ptr<OpenGymSpace> GetActionSpace();
  Ptr<OpenGymSpace> GetObservationSpace();
  Ptr<OpenGymSpace> GetModelSpace();

  Ptr<OpenGymDataContainer> GetObservation();
  float GetReward();
  bool IsGameOver();

  bool IsFedLearning();

  std::string GetExtraInfo();
  bool ExecuteActions(Ptr<OpenGymDataContainer> action);
  Ptr<OpenGymDataContainer>  ExecuteModel(Ptr<OpenGymDataContainer> model);

  void SetGetActionSpaceCb(Callback< Ptr<OpenGymSpace> > cb);
  void SetGetObservationSpaceCb(Callback< Ptr<OpenGymSpace> > cb);
  //设置模型的空间类型
  void SetGetModelSpaceCb(Callback< Ptr<OpenGymSpace> > cb);


  void SetGetObservationCb(Callback< Ptr<OpenGymDataContainer> > cb);
  //设置获取 中心服务器 传回来的模型回调
  void SetGetModelCb(Callback< Ptr<OpenGymDataContainer> > cb);

  void SetGetRewardCb(Callback<float> cb);
  void SetGetGameOverCb(Callback< bool > cb);
  void SetGetExtraInfoCb(Callback<std::string> cb);


  void SetExecuteActionsCb(Callback<bool, Ptr<OpenGymDataContainer> > cb);
  // 设置模型接收到的回调动作
  void SetExecuteModelcb(Callback<Ptr<OpenGymDataContainer>, Ptr<OpenGymDataContainer> > cb);


  void Notify(Ptr<OpenGymEnv> entity);

protected:
  // Inherited
  virtual void DoInitialize (void);
  virtual void DoDispose (void);

private:
  static Ptr<OpenGymInterface> *DoGet (uint32_t port=5555);
  static void Delete (void);

  
  uint32_t m_port;
  zmq::context_t m_zmq_context;
  zmq::socket_t m_zmq_socket;
  zmq::multipart_t m_zmq_socket_mutipart;

  bool m_simEnd;
  bool m_stopEnvRequested;
  bool m_initSimMsgSent;
  bool m_fedLearning;

  Callback< Ptr<OpenGymSpace> > m_actionSpaceCb;
  Callback< Ptr<OpenGymSpace> > m_observationSpaceCb;
  Callback< Ptr<OpenGymSpace> > m_ModelSpaceCb;

  Callback< bool > m_gameOverCb;
  
  Callback< Ptr<OpenGymDataContainer> > m_obsCb;

  Callback<float> m_rewardCb;
  Callback<std::string> m_extraInfoCb;
  
  Callback<bool, Ptr<OpenGymDataContainer> > m_actionCb;
  Callback<Ptr<OpenGymDataContainer>, Ptr<OpenGymDataContainer> > m_modelactionCb;

};

} // end of namespace ns3

#endif /* OPENGYM_INTERFACE_H */

