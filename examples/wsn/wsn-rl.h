#ifndef WSN_RL_H
#define WSN_RL_H

#include "ns3/opengym-module.h"
#include "wsn-rl-env.h"
#include "ns3/double.h"

namespace ns3
{

class WsnRl : public Object
{

public:
    static TypeId GetTypeId (void);

    WsnRl ();
    ~WsnRl ();
    
    uint32_t GetBackoffRl(uint32_t lostPacketRadio, float sendRate, float Delay);

private:

    void CreateGymEnv();
    
    float m_reward {1.0};
    float m_penalty {-100.0};

    Ptr<WsnRlGymEnv> m_wsnRlGymEnv;
};

}

#endif