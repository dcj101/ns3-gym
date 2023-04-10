#ifndef WSN_RL_ENV_H
#define WSN_RL_ENV_H

#include "ns3/opengym-module.h"
#include "ns3/double.h"
#include <vector>

namespace ns3
{

class WsnRlGymEnv : public OpenGymEnv
{
public:
    WsnRlGymEnv ();
    virtual ~WsnRlGymEnv ();

    static TypeId GetTypeId ();

    void SetReward(float value);
    void SetPenalty(float value);

    uint32_t GetBackoffRl(uint32_t lostPacketRadio, float sendRate, float Delay);
    std::vector<double> GetMyModel();
    virtual void RecvModel(std::vector<double> mode);

    virtual Ptr<OpenGymSpace> GetActionSpace();
    virtual Ptr<OpenGymSpace> GetObservationSpace();
    virtual Ptr<OpenGymSpace> GetModelSpace();
    // virtual Ptr<OpenGymSpace> GetModelSpace();
    // TODO:  get all in one function like below, do we need it?
    //virtual void GetEnvState(Ptr<OpenGymDataContainer>  &obs, float &reward, bool &done, std::string &info);
    virtual bool GetGameOver();
    virtual Ptr<OpenGymDataContainer> GetObservation();
    
    virtual float GetReward();
    virtual std::string GetExtraInfo();
    virtual bool ExecuteActions(Ptr<OpenGymDataContainer> action);
    virtual void ExecuteModel(Ptr<OpenGymDataContainer> model);
    // virtual Ptr<OpenGymDataContainer> ExecuteModel(Ptr<OpenGymDataContainer> model);

    // void SetOpenGymInterface(Ptr<OpenGymInterface> openGymInterface);
    // void Notify();
    // void NotifySimulationEnd();

    bool m_isGameOver;

    // reward
    float m_reward;
    float m_penalty;

    // extra info
    std::string m_info;

    // 丢包率
    uint32_t m_lostPacketRadio;
    // 发包速度
    float m_sendRate;
    // 延迟
    float m_Delay;

    uint32_t m_backoff;

    std::vector<double> m_model;

    bool isFedlearning {false};

    bool isTraining {true};
};

}
#endif