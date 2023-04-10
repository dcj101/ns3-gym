#include "wsn-rl-env.h"
#include <ns3/simulator.h>

namespace ns3
{
NS_LOG_COMPONENT_DEFINE ("ns3::WsnRlGymEnv");

NS_OBJECT_ENSURE_REGISTERED (WsnRlGymEnv);

WsnRlGymEnv::WsnRlGymEnv () : OpenGymEnv()
{
  NS_LOG_FUNCTION (this);
}

WsnRlGymEnv::~WsnRlGymEnv ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
WsnRlGymEnv::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WsnRlGymEnv")
    .SetParent<OpenGymEnv> ()
    .AddConstructor<WsnRlGymEnv> ()
  ;
  return tid;
}


void 
WsnRlGymEnv::SetReward(float value)
{
    NS_LOG_FUNCTION(this);
    m_reward = value;
}

void 
WsnRlGymEnv::SetPenalty(float value)
{
    NS_LOG_FUNCTION(this);
    m_penalty = value;
}

Ptr<OpenGymSpace> 
WsnRlGymEnv::GetActionSpace()
{
    uint32_t parameterNum = 1;
    float low = 0.0;
    float high = 1024.0;
    std::vector<uint32_t> shape = {parameterNum,};
    std::string dtype = TypeNameGet<uint32_t> ();  
    Ptr<OpenGymBoxSpace> box = CreateObject<OpenGymBoxSpace> (low, high, shape, dtype);
    NS_LOG_INFO("MyGetActionSpace: " << box);
    return box;
}


Ptr<OpenGymSpace> 
WsnRlGymEnv::GetObservationSpace()
{
    // 丢包率
    // 发送速率
    // 延迟抖动
    uint32_t parameterNum = 3;
    float low = 0.0;
    float hight = 1000000.0;
    std::vector<uint32_t> shape = {parameterNum,};
    std::string dtype = TypeNameGet<uint64_t> ();

    Ptr<OpenGymBoxSpace> box = CreateObject<OpenGymBoxSpace> (low, hight, shape, dtype);
    NS_LOG_INFO ("MyGetObservationSpace: " << box);
    return box;
}


Ptr<OpenGymSpace> 
WsnRlGymEnv::GetModelSpace()
{
    uint32_t parameterNum = 200;
    float low = 0.0;
    float hight = 1000000.0;
    std::vector<uint32_t> shape = {parameterNum,};
    std::string dtype = TypeNameGet<double> ();

    Ptr<OpenGymBoxSpace> box = CreateObject<OpenGymBoxSpace> (low, hight, shape, dtype);
    NS_LOG_INFO ("MyGetObservationSpace: " << box);
    return box;
}

bool
WsnRlGymEnv::GetGameOver()
{
  m_isGameOver = false;
  bool test = false;
  static float stepCounter = 0.0;
  stepCounter += 1;
  if (stepCounter == 10 && test) {
      m_isGameOver = true;
  }
  NS_LOG_INFO ("MyGetGameOver: " << m_isGameOver);
  return m_isGameOver;
}

Ptr<OpenGymDataContainer> 
WsnRlGymEnv::GetObservation()
{
    uint32_t parameterNum = 3;
    std::vector<uint32_t> shape = {parameterNum,};

    Ptr<OpenGymBoxContainer<uint64_t> > box = CreateObject<OpenGymBoxContainer<uint64_t> > (shape);

    box->AddValue(m_lostPacketRadio);
    box->AddValue(m_sendRate*100);
    box->AddValue(m_Delay*100);

    NS_LOG_INFO ("MyGetObservation: " << box);
    return box;
}


float
WsnRlGymEnv::GetReward()
{
    return m_reward;
}

std::string 
WsnRlGymEnv::GetExtraInfo()
{
    return m_info;
}

uint32_t 
WsnRlGymEnv::GetBackoffRl(uint32_t lostPacketRadio, float sendRate, float Delay)
{
    m_lostPacketRadio = lostPacketRadio;
    m_sendRate = sendRate;
    m_Delay = Delay;
    if(isFedlearning)
    {
        return -1;    
    }
    NS_LOG_FUNCTION(this << lostPacketRadio << " " << sendRate << " " << Delay);
    isTraining = true;
    m_info = "Training";
    Notify();
    isTraining = false;
    return m_backoff;
}

std::vector<double> 
WsnRlGymEnv::GetMyModel()
{
    if(isTraining)
    {
        return std::vector<double>();
    }
    isFedlearning = true;
    m_info = "GetModel";
    Notify();
    return m_model;
}

void 
WsnRlGymEnv::RecvModel(std::vector<double> mode)
{
    m_model = mode;
    m_info = "RecvModel";
    if(m_openGymInterface)
    {
        m_openGymInterface->RecvModel(mode);
    }
    else 
    {
        NS_LOG_FUNCTION(this << "m_openGymInterface is zero point");
    }
    isFedlearning = false;
}


bool 
WsnRlGymEnv::ExecuteActions(Ptr<OpenGymDataContainer> action)
{
    Ptr<OpenGymBoxContainer<uint32_t> > box = DynamicCast<OpenGymBoxContainer<uint32_t> >(action);
    m_backoff = box->GetValue(0);
    NS_LOG_INFO ("MyExecuteActions: " << action);
    return true;
}

void
WsnRlGymEnv::ExecuteModel(Ptr<OpenGymDataContainer> action)
{
    Ptr<OpenGymBoxContainer<double> > box = DynamicCast<OpenGymBoxContainer<double> >(action);
    for(int i = 0 ; i < 200; ++ i)
    {
        m_model.push_back(box->GetValue(i));
    }
    NS_LOG_INFO ("MyExecuteActions: " << action);
    return;
}



}